/*--------------------------------------------------------
| "So what is it?"
|				- The Cat, Red Dwarf
| --------------------------------------------------------
 An FunctionBinder can be used to bind any function to a string name.
 The function can later be invoked by passing a string with parameters.
 
 Here is a simple example:
 
	void VoidFunction();
	int IntFunction();

 	FunctionBinder binder;
	binder.RegisterVoid("VoidFunction",bind(&VoidFunction));
	binder.RegisterVoid("IntFunction",bind(&IntFunction));
 
 Now the functions can be called using a string in the following manner.
 
	if(binder.Call("VoidFunction()"))
	{
		//Success
	}

	int returnValue=0;
	if(binder.Call("IntFunction()",&returnValue))
	{
		//Success - use returnValue
	}

 You can use boost bind to bind functions with parameters as well.

	int MyFunction(f32 a, std::string b);

	binder.Register("MyFunction",bind(&MyFunction,placeholders::_1,placeholders::_2),true,
					boost::fusion::vector<f32,std::string>(1.23f,"Hello World"));
 
 The boolean after bind indicates whether or not default parameters are allowed.
 The boost fusion vector defines the parameter list for the call, so for all registrations of functions
 with parameters you need to provide the allowsDefaults flag and the fusion vector.
 The fusion vector does not need to have default values defined, but the parameter list is essential.
 
 The function can be called as follows:
 
	int returnValue=0;
	if(binder.Call("MyFunction(123.465,Golly!)",&returnValue))
	{
		//Success - use returnValue
	}

 You can specify the regular expressions used to extract the function and parameters block from the
 call string. The defaults are function(parameters) and where parameters takes the form param1,param2,...
 the latter allows whitespace either side of the commas.
 */
#ifndef _FunctionBinder_H_
#define _FunctionBinder_H_

#include <echo/Util/Utils.h>
#include <echo/Util/StringUtils.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/Vector3.h>
#include <echo/Graphics/Colour.h>
#include <map>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/any.hpp>
#include <echo/cpp/functional>
#include <boost/algorithm/string.hpp>

namespace Echo
{
	//Base abstract object for moving return objects around.
	class AbstractTypeTransport
	{
	public:
		AbstractTypeTransport(){};
		virtual ~AbstractTypeTransport(){};
		virtual const std::type_info& GetTypeInfo() const = 0;
		virtual optional<std::string> GetValueAsString() const = 0;
	};

	template<typename T>
	class ReturnTypeTransport : public AbstractTypeTransport
	{
	private:
		T mValue;
	public:
		ReturnTypeTransport(){}
		ReturnTypeTransport(T value) : mValue(std::move(value)){}
		~ReturnTypeTransport(){}
		virtual const std::type_info& GetTypeInfo() const override
		{
			return typeid(T);
		}
		// Reference the value
		inline const T& GetValue() const
		{
			return mValue;
		}
		inline T& GetValue()
		{
			return mValue;
		}
		// Copy the value
		inline T CopyValue() const
		{
			return mValue;
		}
		// Move the value
		T MoveValue()
		{
			return std::move(mValue);
		}
		optional<std::string> GetValueAsString() const override
		{
			return Utils::String::ToString<T>(mValue);
		}
	};

	//Specialisation for void transport objects - no data to transport
	template<>
	class ReturnTypeTransport<void> : public AbstractTypeTransport
	{
	public:
		ReturnTypeTransport(){}
		~ReturnTypeTransport(){}
		virtual const std::type_info& GetTypeInfo() const override
		{
			return typeid(void);
		}
		optional<std::string> GetValueAsString() const override
		{
			return std::string("void");
		}
	};

	class FunctionBinder
	{
	public:
		struct CallStatuses
		{
			enum _
			{
				SUCCESS = 0,
				FUNCTION_NOT_FOUND,
				NOT_ENOUGH_PARAMETERS,
				PARAMETER_CONVERSION_FAILED,
				RETURN_CONVERSION_FAILED,
				UNKNOWN
			};
		};
		typedef CallStatuses::_ CallStatus;

		struct CallResult
		{
			CallResult() : mStatus(CallStatuses::UNKNOWN){}
			CallResult(CallStatus status, shared_ptr<AbstractTypeTransport> value) : mStatus(status), mValue(value){}
			CallResult(CallStatus status) : mStatus(status){}
			CallStatus mStatus;
			shared_ptr<AbstractTypeTransport> mValue;

			template<typename T>
			optional<T> GetValueAs() const
			{
				//Attempt to match the return type.
				const ReturnTypeTransport<T>* castTransport=dynamic_cast<const ReturnTypeTransport<T>*>(mValue.get());
				if(castTransport)
				{
					return castTransport->GetValue();
				}
				return none;
			}
			
			template<typename T>
			const T* GetValuePointerAs() const
			{
				//Attempt to match the return type.
				const ReturnTypeTransport<T>* castTransport=dynamic_cast<const ReturnTypeTransport<T>*>(mValue.get());
				if(castTransport)
				{
					return &castTransport->GetValue();
				}
				return nullptr;
			}
		};

		struct AnyAssigner
		{
			template <typename T>
			void operator()(T& outValue)
			{
				mParameters.push_back(outValue);
			}
			std::vector< boost::any > mParameters;
		};
		
		class BoundFunction
		{
		public:
			BoundFunction(){}
			virtual ~BoundFunction(){}
			virtual std::string GetReturnType() const = 0;
			virtual std::string GetParameters() const = 0;
			virtual std::vector<std::string> GetParameterList() const = 0;
			virtual std::string GetName() const = 0;

			/**
			 * Set whether or not this function should support nested calls.
			 * When nested calls are disabled anything resembling a function call should be used as a parameter rather than being called against.
			 * For example, when nested calls is off:
			 * 
			 *  	Foo(Bar())
			 * 
			 * Foo will be invoked with "Bar()" rather than the result of Call("Bar()"). This feature is designed to give a little more control
			 * over parameter usage when calling using "any" parameters, since these won't have been parsed through the FunctionBinder until
			 * being called.
			 *
			 * Since this option changes behaviour at a low level, this option is not available at registration time. Instead you can access
			 * the bound function can call this method directly after registration.
			 */
			virtual void SetNestedCalls(bool nestedCalls) = 0;
			virtual bool GetNestedCalls() const = 0;

			/**
			 * Call the function with a list of strings,bool pairs.
			 * This is used internally and, but it can be
			 * @param binder The binder required for additional calls if detected.
			 * @param parameters The parameters as pairs of strings,bool where the bool indicates whether or not the parameter
			 * should be treated as a function call.
			 * @return A CallResult object with the status of the call and value on success.
			 */
			virtual CallResult operator()(FunctionBinder& binder, const std::vector< std::pair<std::string, bool> >& parameters) = 0;
			
			/**
			 * Call the function with a list of parameters.
			 * @param binder The binder required for additional calls if detected.
			 * @param parameters The parameters.
			 * @return A CallResult object with the status of the call and value on success.
			 */
			virtual CallResult operator()(FunctionBinder& binder, const std::vector< boost::any >& parameters) = 0;
			
			/**
			 * Overload to allow you to call this function more naturally.
			 * For example:
			 * 
			 *		binder.RegisterVoid("Foo",bind(&foo,placeholders::_1,placeholders::_2),false,boost::fusion::vector<int,bool>());
			 * 
			 *		shared_ptr<BoundFunction> foo = binder.GetFunction("Foo");
			 *		(*foo)(binder,500,false);
			 * 
			 * Before this we the FunctionBinder was just used for string calls, which incurs string conversion
			 * overhead
			 * 
			 * @param binder The binder required for additional calls if detected.
			 * @param parameters The parameters.
			 * @return A CallResult object with the status of the call and value on success.
			 */
			template< typename ...Parameters >
			CallResult operator()(FunctionBinder& binder, Parameters... parameters)
			{
				AnyAssigner assigner;
				boost::fusion::vector<Parameters...> parameterVector(parameters...);
				// We need to pass our assigner as a reference here because for_each copies the parameter (it
				// used to take a const reference.
				boost::fusion::for_each(parameterVector, std::ref(assigner));
				return (*this)(binder, assigner.mParameters);
			}
		};

		class Converter
		{
		public:
			Converter(FunctionBinder& binder, bool nestedCalls, const std::vector< std::pair<std::string, bool> >& paramters) :
				mBinder(binder),
				mNestedCalls(nestedCalls),
				mFailed(false),
				mCallFailed(false),
				mFailedCallStatus(CallStatuses::SUCCESS),
				mIndex(0),
				mParameters(paramters){}
			~Converter(){}

			void Reset()
			{
				mFailed = false;
				mCallFailed = false;
				mFailedCallStatus = CallStatuses::SUCCESS;
				mIndex = 0;
			}
			
			//! \brief operator() performs the conversion for each parameter.
			template <typename T>
			void operator()(T& outValue)
			{
				if(mIndex >= mParameters.size())
				{
					return;
				}

				T tempValue;
				if(mParameters[mIndex].second && mNestedCalls)
				{
					CallResult callResult = mBinder.Call(mParameters[mIndex].first,&tempValue);

					//If the function call was a success then we can set the output value and return.
					//This means that the function was called and the return value was set. An
					//incompatible return type to parameter type will cause the call to fail.
					if(callResult.mStatus==CallStatuses::SUCCESS)
					{
						outValue = tempValue;
						mIndex++;
						return;
					}

					//If the call result was anything but not found then there was an error in calling.
					//We are happy with not found because it is likely the parameter wasn't a function.
					if(callResult.mStatus!=CallStatuses::FUNCTION_NOT_FOUND)
					{
						mFailed = true;
						mCallFailed = true;
						mFailedCallStatus = CallStatuses::FUNCTION_NOT_FOUND;
						mIndex++;
						return;
					}
				}

				std::stringstream ss(mParameters[mIndex].first);

				ss >> std::setprecision(mBinder.mStreamPrecision);
				
				Utils::String::StreamAssignOrFail<T>::AssignOrFail(ss,tempValue);
				
				if(ss.fail())
				{
					ECHO_LOG_ERROR("Failed to convert: \"" << mParameters[mIndex].first << "\" to type " << DemangleName(typeid(T).name()));
					mFailed = true;
				}else
				{
					outValue = tempValue;
				}
				mIndex++;
			}

			void operator()(std::string& outValue)
			{
				if(mIndex >= mParameters.size())
				{
					return;
				}

				if(mParameters[mIndex].second && mNestedCalls)
				{
					std::string tempValue;
					CallResult callResult = mBinder.Call(mParameters[mIndex].first,&tempValue);

					//If the function call was a success then we can set the output value and return.
					//This means that the function was called and the return value was set. An
					//incompatible return type to parameter type will cause the call to fail.
					if(callResult.mStatus==CallStatuses::SUCCESS)
					{
						outValue = tempValue;
						mIndex++;
						return;
					}

					//If the call result was anything but not found then there was an error in calling.
					//We are happy with not found because it is likely the parameter wasn't a function.
					if(callResult.mStatus!=CallStatuses::FUNCTION_NOT_FOUND)
					{
						mFailed = true;
						mCallFailed = true;
						mFailedCallStatus = CallStatuses::FUNCTION_NOT_FOUND;
						mIndex++;
						return;
					}
				}

				outValue = mParameters[mIndex].first;
				mIndex++;
			}

			void operator()(bool& outValue)
			{
				if(mIndex >= mParameters.size())
				{
					return;
				}

				bool tempValue;
				if(mParameters[mIndex].second && mNestedCalls)
				{
					CallResult callResult = mBinder.Call(mParameters[mIndex].first,&tempValue);

					//If the function call was a success then we can set the output value and return.
					//This means that the function was called and the return value was set. An
					//incompatible return type to parameter type will cause the call to fail.
					if(callResult.mStatus==CallStatuses::SUCCESS)
					{
						outValue = tempValue;
						mIndex++;
						return;
					}

					//If the call result was anything but not found then there was an error in calling.
					//We are happy with not found because it is likely the parameter wasn't a function.
					if(callResult.mStatus!=CallStatuses::FUNCTION_NOT_FOUND)
					{
						mFailed = true;
						mCallFailed = true;
						mFailedCallStatus = CallStatuses::FUNCTION_NOT_FOUND;
						mIndex++;
						return;
					}
				}
				std::stringstream ss(mParameters[mIndex].first);

				// try with 1 and 0.
				ss >> std::noboolalpha >> tempValue;
				if(ss.fail())
				{
					ss.clear();

					// Re-assign the stream contents. Not doing this causes a failure on Android.
					ss.str(mParameters[mIndex].first);

					// try with true and false.
					ss >> std::boolalpha >> tempValue;
					if(ss.fail())
					{
						ECHO_LOG_ERROR("Failed to convert: \"" << mParameters[mIndex].first << "\" to type bool in specialised converter.");
						mFailed = true;
						mIndex++;
						return;
					}
				}
				mIndex++;

				outValue = tempValue;
			}

			bool Failed() const {return mFailed;}
			bool CallFailed() const {return mCallFailed;}
			CallResult GetFailedCallStatus() const {return mFailedCallStatus;}
		private:
			FunctionBinder& mBinder;
			bool mNestedCalls;
			bool mFailed;
			bool mCallFailed;
			CallStatus mFailedCallStatus;
			size_t mIndex;
			std::vector< std::pair<std::string, bool> > mParameters;
		};

		class AnyConverter
		{
		public:
			AnyConverter(FunctionBinder& binder, bool nestedCalls, const std::vector< boost::any >& paramters) :
				mBinder(binder),
				mNestedCalls(nestedCalls),
				mFailed(false),
				mCallFailed(false),
				mFailedCallStatus(CallStatuses::SUCCESS),
				mIndex(0),
				mParameters(paramters){}
			~AnyConverter(){}

			void Reset()
			{
				mFailed = false;
				mCallFailed = false;
				mFailedCallStatus = CallStatuses::SUCCESS;
				mIndex = 0;
			}

			//! \brief operator() performs the conversion for each parameter.
			template <typename T>
			void operator()(T& outValue)
			{
				if(mIndex >= mParameters.size())
				{
					return;
				}

				try
				{
					outValue = boost::any_cast<T>(mParameters[mIndex]);
					mIndex++;
				}
				catch(const boost::bad_any_cast&)
				{
					if(!mNestedCalls)
					{
						ECHO_LOG_ERROR("Failed to convert provided parameter at position " << mIndex << ", type: " << DemangleName(mParameters[mIndex].type().name()) << " to type " << DemangleName(typeid(T).name()));
						mFailed = true;
						mIndex++;
						return;
					}
					// Is it a string? It might be a string function call. Currently we don't support
					// using non-string functions as parameters.
					try
					{
						std::string stringFunction = boost::any_cast<std::string>(mParameters[mIndex]);
						
						T tempValue;
						CallResult callResult = mBinder.Call(stringFunction,&tempValue);

						//If the function call was a success then we can set the output value and return.
						//This means that the function was called and the return value was set. An
						//incompatible return type to parameter type will cause the call to fail.
						if(callResult.mStatus==CallStatuses::SUCCESS)
						{
							outValue = tempValue;
							mIndex++;
							return;
						}

						//If the call result was anything but success then this is an error.
						mFailed = true;
						mCallFailed = true;
						if(callResult.mStatus!=CallStatuses::FUNCTION_NOT_FOUND)
						{
							// Pass on the status
							mFailedCallStatus = callResult.mStatus;
						}else
						{
							// Otherwise it is a conversion error for this parameter
							mFailedCallStatus = CallStatuses::PARAMETER_CONVERSION_FAILED;
						}
						mIndex++;
						return;
					}
					catch(const boost::bad_any_cast&)
					{
						ECHO_LOG_ERROR("Failed to convert provided parameter at position " << mIndex << ", type: " << DemangleName(mParameters[mIndex].type().name()) << " to type " << DemangleName(typeid(T).name()));
						mFailed = true;
						mIndex++;
						return;
					}
				}
			}

			//! operator() for string
			//! @note Supports const char*, char*, char and std::string input.
			//! @note This method includes const casting because boost requires a const operator().
			void operator()(std::string& outValue)
			{
				if(mIndex >= mParameters.size())
				{
					return;
				}

				std::string tempValue;
				const boost::any& currentParameter = mParameters[mIndex];
				
				// We want to support char*. const char* and string
				const boost::typeindex::type_info& sourceType=currentParameter.type();
				if(sourceType==typeid(std::string))
				{
					tempValue = boost::any_cast<std::string>(currentParameter);
				}else
				if(sourceType==typeid(const std::string))
				{
					tempValue = boost::any_cast<const std::string>(currentParameter);
				}else
				if(sourceType==typeid(const char*))
				{
					tempValue = boost::any_cast<const char*>(currentParameter);
				}else
				if(sourceType==typeid(char*))
				{
					tempValue = boost::any_cast<char*>(currentParameter);
				}else
				if(sourceType==typeid(const char))
				{
					tempValue = boost::any_cast<const char>(currentParameter);
				}else
				if(sourceType==typeid(char))
				{
					tempValue = boost::any_cast<char>(currentParameter);
				}else
				{
					ECHO_LOG_ERROR("Failed to convert provided parameter at position " << mIndex << ", type: " << DemangleName(sourceType.name()) << " to type " << DemangleName(typeid(std::string).name()));
					mIndex++;
					mFailed = true;
					return;
				}
				mIndex++;
				
				if(mNestedCalls)
				{
					std::string returnedValue;
					CallResult callResult = mBinder.Call(tempValue,&returnedValue);

					if(callResult.mStatus==CallStatuses::SUCCESS)
					{
						outValue = returnedValue;
						return;
					}else
					if(callResult.mStatus==CallStatuses::RETURN_CONVERSION_FAILED)
					{
						// It was a function but the output wasn't a string
						mFailed = true;
						mCallFailed = true;
						mFailedCallStatus = CallStatuses::RETURN_CONVERSION_FAILED;
						return;
					}
				}
				
				outValue = tempValue;
			}
			
			bool Failed() const {return mFailed;}
			bool CallFailed() const {return mCallFailed;}
			CallStatus GetFailedCallStatus() const {return mFailedCallStatus;}
		private:
			FunctionBinder& mBinder;
			bool mNestedCalls;
			bool mFailed;
			bool mCallFailed;
			CallStatus mFailedCallStatus;
			size_t mIndex;
			std::vector< boost::any > mParameters;
		};
		
		template < typename T, bool hasRighShift >
		struct StreamDefaultOrOutputNoSerialiser;

		template < typename T >
		struct StreamDefaultOrOutputNoSerialiser< T, true >
		{
			void operator()(std::ostream& s, T& value) const
			{
				s << value;
			}
		};

		template < typename T >
		struct StreamDefaultOrOutputNoSerialiser< T, false >
		{
			void operator()(std::ostream& s, T&) const
			{
				s << "[has default but no string serialiser]";
			}
		};
		
		class ParametersStringBuilder
		{
		public:
			ParametersStringBuilder(bool hasDefaults) : mHasDefaults(hasDefaults), mFirstParameter(true){}
			~ParametersStringBuilder(){}

			template<typename T>
			void operator()(T& t)
			{
				if(!mFirstParameter)
				{
					mAllParameters << ",";
				}
				mAllParameters << DemangleName(typeid(t).name());
				if(mHasDefaults)
				{
					StreamDefaultOrOutputNoSerialiser<T, boost::has_left_shift<std::ostream, T>::value > outputDefault;
					mAllParameters << "=";
					outputDefault(mAllParameters,t);
				}
				mFirstParameter = false;
			}
			std::string GetFinalString() const {return mAllParameters.str();}
		private:
			std::stringstream mAllParameters;
			bool mHasDefaults;
			bool mFirstParameter;
		};
		
		class ParametersStringListBuilder
		{
		public:
			ParametersStringListBuilder(bool hasDefaults) : mHasDefaults(hasDefaults){}
			~ParametersStringListBuilder(){}

			template<typename T>
			void operator()(T& t)
			{
				std::stringstream ss;
				ss << DemangleName(typeid(t).name());
				if(mHasDefaults)
				{
					StreamDefaultOrOutputNoSerialiser<T, boost::has_left_shift<std::ostream, T>::value > outputDefault;
					ss << "=";
					outputDefault(ss,t);
				}
				mParameterStrings.push_back(ss.str());
			}
			std::vector<std::string> GetStringList() const {return mParameterStrings;}
		private:
			std::vector<std::string> mParameterStrings;
			bool mHasDefaults;
		};

		//! \brief Class used to invoke functions and encapsulate the return type into a transport object.
		template<typename Function, class ParameterSequence, typename ReturnType>
		class Invoker
		{
		public:
			shared_ptr< ReturnTypeTransport<ReturnType> > operator()(Function& funct, ParameterSequence& parameters)
			{
				return Echo::make_shared< ReturnTypeTransport<ReturnType> >(std::move(boost::fusion::invoke(funct,parameters)));
			}
		};

		//! \brief Specialisation for functions with a void return type.
		template<typename Function, class ParameterSequence>
		class Invoker<Function,ParameterSequence,void>
		{
		public:
			shared_ptr< ReturnTypeTransport<void> > operator()(Function& funct, ParameterSequence& parameters)
			{
				boost::fusion::invoke(funct,parameters);
				return Echo::make_shared< ReturnTypeTransport<void> >();
			}
		};

		template< typename Function, typename ParameterSequence>
		class StringFunction : public BoundFunction
		{
		public:
			typedef typename boost::fusion::result_of::invoke<Function,ParameterSequence>::type ReturnType;

			StringFunction(std::string name, Function funct, bool allowedDefaultParameters = false,
						 ParameterSequence defaultParameters = ParameterSequence()) :
					mName(name),
					mFunction(funct),
					mParameters(defaultParameters),
					mAllowedDefaultParameters(allowedDefaultParameters),
					mNestedCalls(true)
			{}
			~StringFunction(){}

			CallResult operator()(FunctionBinder& binder, const std::vector< std::pair<std::string, bool> >& inputParameters) override
			{
				using namespace boost;

				//Attempt to convert parameters into a sequence.
				if(!mAllowedDefaultParameters)
				{
					size_t numParams = fusion::size(mParameters);
					if(inputParameters.size()<numParams)
					{
						ECHO_LOG_ERROR("Insufficient parameters for function call " << mName << ". Requires " << numParams << " but only " << inputParameters.size() << " provided.");
						return CallStatuses::NOT_ENOUGH_PARAMETERS;
					}
				}

				//These are the parameters we are going to call with
				ParameterSequence parameters = mParameters;

				// Convert any input parameters to the appropriate types. The converter
				// will override each of the parameters in the container.
				Converter converter(binder, mNestedCalls, inputParameters);
				fusion::for_each(parameters,std::ref(converter));

				if(converter.Failed())
				{
					if(converter.CallFailed())
					{
						ECHO_LOG_ERROR("Call to function as parameter failed for: " << mName << ". Requires: " << GetParameters());
						return CallResult(converter.GetFailedCallStatus());
					}else
					{
						ECHO_LOG_ERROR("Failed to convert parameters for function: " << mName << ". Requires: " << GetParameters());
					}
					return CallResult(CallStatuses::PARAMETER_CONVERSION_FAILED);
				}

				Invoker<Function, ParameterSequence, ReturnType> invoker;
				return CallResult(CallStatuses::SUCCESS, invoker(mFunction,parameters));
			}

			CallResult operator()(FunctionBinder& binder, const std::vector< boost::any >& inputParameters) override
			{
				using namespace boost;

				//Attempt to convert parameters into a sequence.
				if(!mAllowedDefaultParameters)
				{
					size_t numParams = fusion::size(mParameters);
					if(inputParameters.size()<numParams)
					{
						ECHO_LOG_ERROR("Insufficient parameters for function call " << mName << ". Requires " << numParams << " but only " << inputParameters.size() << " provided.");
						return CallResult(CallStatuses::NOT_ENOUGH_PARAMETERS);
					}
				}

				//These are the parameters we are going to call with
				ParameterSequence parameters = mParameters;

				// Convert any input parameters to the appropriate types. The converter
				// will override each of the parameters in the container.
				AnyConverter converter(binder, mNestedCalls, inputParameters);
				fusion::for_each(parameters,std::ref(converter));

				if(converter.Failed())
				{
					if(converter.CallFailed())
					{
						ECHO_LOG_ERROR("Call to function as parameter failed for: " << mName << ". Requires: " << GetParameters());
						return CallResult(converter.GetFailedCallStatus());
					}else
					{
						ECHO_LOG_ERROR("Failed to convert parameters for function: " << mName << ". Requires: " << GetParameters());
					}
					return CallResult(CallStatuses::PARAMETER_CONVERSION_FAILED);
				}

				Invoker<Function, ParameterSequence, ReturnType> invoker;
				return CallResult(CallStatuses::SUCCESS, invoker(mFunction,parameters));
			}

			std::string GetReturnType() const override
			{
				return typeid(ReturnType).name();
			}

			std::string GetParameters() const override
			{
				using namespace boost;
				ParametersStringBuilder builder(mAllowedDefaultParameters);
				fusion::for_each(mParameters,std::ref(builder));
				return builder.GetFinalString();
			}

			std::vector<std::string> GetParameterList() const override
			{
				using namespace boost;
				ParametersStringListBuilder builder(mAllowedDefaultParameters);
				fusion::for_each(mParameters,std::ref(builder));
				return builder.GetStringList();
			}
			
			std::string GetName() const override
			{
				return mName;
			}

			void SetNestedCalls(bool nestedCalls) override
			{
				mNestedCalls = nestedCalls;
			}

			bool GetNestedCalls() const override
			{
				return mNestedCalls;
			}
		private:
			std::string mName;
			Function mFunction;
			ParameterSequence mParameters;
			bool mAllowedDefaultParameters;
			bool mNestedCalls;
		};

		template<typename Function, class ParameterSequence >
		bool Register(const std::string& name, Function funct, bool allowedDefaults = false, ParameterSequence defaultParameters = ParameterSequence())
		{
			if(mBindings.find(name)!=mBindings.end())
			{
				return false;
			}

			shared_ptr< StringFunction<Function, ParameterSequence> > f = Echo::make_shared< StringFunction<Function, ParameterSequence> >(name,funct,allowedDefaults,defaultParameters);
			mBindings.insert(std::make_pair(name,f));
			return true;
		}

		template<typename Function >
		bool RegisterVoid(const std::string& name, Function funct)
		{
			return Register(name, funct, true, boost::fusion::vector<>());
		}
		
		/**
		 * Register an existing function object
		 * @param name name of function
		 * @param funct the bound function
		 * @return true if the function was registered, false if a function with the same name already exists.
		 */
		bool Register(const std::string& name, shared_ptr<BoundFunction> funct)
		{
			if(mBindings.find(name)!=mBindings.end())
			{
				return false;
			}
			mBindings.insert(std::make_pair(name,funct));
			return true;
		}

		bool Deregister( const std::string& name)
		{
			BindingIterator it = mBindings.find(name);
			if(it==mBindings.end())
			{
				return false;
			}
			mBindings.erase(it);
			return true;
		}
		
		shared_ptr<BoundFunction> GetFunction( const std::string& name, bool useFallback = true) const
		{
			std::map< std::string, shared_ptr<BoundFunction> >::const_iterator it = mBindings.find(name);
			if(it!=mBindings.end())
			{
				return it->second;
			}
			if(mFallbackBinder && useFallback)
			{
				return mFallbackBinder->GetFunction(name,useFallback);
			}
			return 0;
		}

		CallResult Call(const std::string& function)
		{
			std::vector< std::pair<std::string, bool> > parameters;
			std::string functionName = SplitLine(function,parameters);
			BindingIterator it = mBindings.find(functionName);
			if(it==mBindings.end())
			{
				if(mFallbackBinder)
				{
					return mFallbackBinder->Call(function);
				}
				return CallResult(CallStatuses::FUNCTION_NOT_FOUND);
			}

			BoundFunction& funct = (*it->second);

			return funct(*this,parameters);
		}

		template<typename ReturnType>
		CallResult Call(const std::string& function, ReturnType* returnValue)
		{
			std::vector< std::pair<std::string, bool> > parameters;
			std::string functionName = SplitLine(function,parameters);
			BindingIterator it = mBindings.find(functionName);
			if(it==mBindings.end())
			{
				if(mFallbackBinder)
				{
					return mFallbackBinder->Call(function, returnValue);
				}
				return CallResult(CallStatuses::FUNCTION_NOT_FOUND);
			}

			BoundFunction& funct = (*it->second);

			CallResult callResult = funct(*this, parameters);
			if(callResult.mStatus==CallStatuses::SUCCESS)
			{
				if(returnValue)
				{
					//Attempt to match the return type.
					const ReturnType* returnedValue = callResult.GetValuePointerAs<ReturnType>();
					if(returnedValue)
					{
						*returnValue = *returnedValue;
					}else
					{
						ECHO_LOG_ERROR("Function call: \"" << functionName << "\" succeeded but could not cast return type to " << DemangleName(typeid(ReturnType).name())
								<< ". Return type is " << DemangleName(funct.GetReturnType()) << ".");
						return CallResult(CallStatuses::RETURN_CONVERSION_FAILED);
					}
				}
				return callResult;
			}
			ECHO_LOG_ERROR("Function call failed: " << functionName);
			return callResult;
		}
		
		template< typename ...Parameters >
		CallResult Call(const std::string& functionName, Parameters... parameters)
		{
			BindingIterator it = mBindings.find(functionName);
			if(it==mBindings.end())
			{
				if(mFallbackBinder)
				{
					return mFallbackBinder->Call<Parameters...>(functionName, parameters...);
				}
				return CallResult(CallStatuses::FUNCTION_NOT_FOUND);
			}

			BoundFunction& funct = (*it->second);
			return funct(*this,parameters...);
		}
		
		template<typename ReturnType, typename ...Parameters>
		CallResult Call(const std::string& functionName, ReturnType* returnValue, Parameters... parameters)
		{
			BindingIterator it = mBindings.find(functionName);
			if(it==mBindings.end())
			{
				if(mFallbackBinder)
				{
					return mFallbackBinder->Call<ReturnType, Parameters...>(functionName, returnValue, parameters...);
				}
				return CallResult(CallStatuses::FUNCTION_NOT_FOUND);
			}

			BoundFunction& funct = (*it->second);
			CallResult callResult = funct(*this, parameters...);
			if(callResult.mStatus==CallStatuses::SUCCESS)
			{
				if(callResult.mValue)
				{
					//Attempt to match the return type.
					const ReturnType* returnedvalue = callResult.GetValuePointerAs<ReturnType>();
					if(returnedvalue)
					{
						*returnValue = *returnedvalue;
					}else
					{
						ECHO_LOG_ERROR("Function call: \"" << functionName << "\" succeeded but could not cast return type to " << DemangleName(typeid(ReturnType).name())
								<< ". Return type is " << DemangleName(funct.GetReturnType()) << ".");
						return CallResult(CallStatuses::RETURN_CONVERSION_FAILED);
					}
				}
				return callResult;
			}
			ECHO_LOG_ERROR("Function call failed: " << functionName);
			return callResult;
		}

		//!\ brief Default constructor
		FunctionBinder() :	mFunctionRegEx(R"EOF(([^\(]*)\s*\(\s*([\S+\n\r\s]*)\))EOF"),	//Basic normal function usage, Function(parameters).
				mStreamPrecision(16),
				OPEN_PARENTHESIS('('),
				CLOSE_PARENTHESIS(')'),
				QUOTATION_MARK( '"'),
				ESCAPE('\\'),
				SPLIT_CHARACTER(',')
		{
			AddEscapedCharacter('n','\n');
			AddEscapedCharacter('\\','\\');
			AddEscapedCharacter('(','(');
			AddEscapedCharacter(')',')');
			AddEscapedCharacter('\"','\"');
		}
		~FunctionBinder(){}

		//!\brief Specify the regular expression to use to extract the function name and parameters from the calling line.
		//!\details String parsing is performed in two stages. The first stage is to extract the function and parameters
		//!			block from the string then the parameters block is parsed for special characters and nested function
		//!			calls.
		void SetFunctionRegEx(const std::string& regex)
		{
			mFunctionRegEx = regex;
		}

		const std::string& GetFunctionRegEx() const
		{
			return mFunctionRegEx;
		}
		
		/**
		 * Function parameters can use escaped characters to replace special characters.
		 * See the FunctionBinder() constructor for default characters.
		 *	
		 */
		void AddEscapedCharacter(char escapeCharacter, char replacedWith)
		{
			mEscapeCharacters.insert(std::make_pair(escapeCharacter,replacedWith));
		}

		/**
		 * Set the fallback binder for functions that aren't found in the current instance.
		 * You can set up a chain of FunctionBinders to propagate calls along if a function is not found.
		 * This is particularly useful for creating object specific function calls. It allows for a kind
		 * of basic scoping system.
		 */
		void SetFallback(shared_ptr<FunctionBinder> fallbackBinder)
		{
			mFallbackBinder = fallbackBinder;
		}
		
		/**
		 * Sets the precision of the streams for reading and writing values.
		 * This affects any future reads and writes.
		 * @param streamPrecision The stream precision, see std::setprecision()
		 */
		void SetStreamPrecision(Size streamPrecision)
		{
			mStreamPrecision = streamPrecision;
		}

		/**
		 * Get the current precision used for stream operations.
		 * @see SetStreamPrecision()
		 */
		Size GetStreamPrecision() const {return mStreamPrecision;}
		
		/**
		 * Get all of the function that can be called.
		 * @note Use GetFunctions() to get the function objects which can be queried for the name, return type and parameters.
		 * @return vector of function names.
		 */
		std::vector<std::string> GetFunctionNames() const
		{
			std::vector<std::string> names;
			names.reserve(mBindings.size());
			ConstBindingIterator it = mBindings.begin();
			ConstBindingIterator itEnd = mBindings.end();
			while(it!=itEnd)
			{
				names.push_back(it->first);
				it++;
			}
			return names;
		}

		/**
		 * Get all of the bound functions objects.
		 */
		std::vector< shared_ptr<BoundFunction> > GetFunctions() const
		{
			std::vector< shared_ptr<BoundFunction> > functions;
			functions.reserve(mBindings.size());
			ConstBindingIterator it = mBindings.begin();
			ConstBindingIterator itEnd = mBindings.end();
			while(it!=itEnd)
			{
				functions.push_back(it->second);
				it++;
			}
			return functions;
		}

		void EnableBuiltIn()
		{
			Register("Vector2",[](f32 x, f32 y){return Vector2(x,y);},false,boost::fusion::vector<f32,f32>());
			Register("Vector3",[](f32 x, f32 y, f32 z){return Vector3(x,y,z);},false,boost::fusion::vector<f32,f32,f32>());
			Register("Vector4",[](f32 x, f32 y, f32 z, f32 w){return Vector4(x,y,z,w);},false,boost::fusion::vector<f32,f32,f32,f32>());
			Register("Colour",[](f32 r, f32 g, f32 b, f32 a){return Colour(r,g,b,a);},false,boost::fusion::vector<f32,f32,f32,f32>());
		}
	private:
		std::string SplitLine(const std::string& line, std::vector< std::pair<std::string, bool> > & parameters) const
		{
			boost::regex functionExpression(mFunctionRegEx);

			boost::cmatch matches;
			std::string functionName;

			//Split up the line into function and parameters.
			if(boost::regex_match(line.c_str(), matches, functionExpression))
			{
				// what[0] contains the whole string 
				// what[1] contains the function name
				// what[2] contains the parameters
				functionName.assign(matches[1].first, matches[1].second);
				boost::algorithm::trim(functionName);
			
				// Split the parameters by the parameters token.
				// Track the level of quotation marks. A string is only a parameter on level 0.
				//	Ignore quotation marks preceeded by a backslash such as \".
				// Track the level of open and closed brackets. A string is only a parameters on level 0.
				//	Ignore brackets preceeded by a backslash such as \( or \).
				//	Ignore brackets during an open quotation block.
				// Replace special sequences such as \n.

				std::string params(matches[2].first, matches[2].second);
				std::string currentParameter;
				currentParameter.reserve(params.size());
				
				size_t level = 0;
				bool inQuotation = false;
				bool escapingCharacter = false;
				bool parameterCouldBeAFunction = false;
				
				for(size_t c=0; c<params.length(); ++c)
				{
					char currentCharacter = params[c];
					if(escapingCharacter)
					{
						escapingCharacter = false;
						std::map< char, char >::const_iterator it = mEscapeCharacters.find(currentCharacter);
						if(it==mEscapeCharacters.end())
						{
							ECHO_LOG_WARNING("Unrecognised escape sequenced encountered \"\\" << currentCharacter << "\". Ignoring.");
							continue;
						}
						
						//Add the replacement character.
						currentParameter+=it->second;
						continue;
					}
					
					// If we're not level 1 then it is just part of a parameter.
					// We only want to process escape characters and quotations  at level 0. Other levels are nested function
					// calls and the characters will processed with replacement and splitting appropriately when those
					// functions are called.
					if(level==0)
					{
						// First check for an escape. That basically determines if look at the next character further.
						if(params[c]==ESCAPE)
						{
							escapingCharacter = true;
							continue;
						}

						// Check for quotations and toggle as necessary. We don't have to worry about escaped quotation
						// because we deal with escape characters above.
						if(params[c]==QUOTATION_MARK)
						{
							inQuotation = !inQuotation;
							continue;
						}

						if(inQuotation)
						{
							currentParameter+=currentCharacter;
							//We intentionally continue here so we don't track level changes.
							continue;
						}
						
						
						//Now finally we'll look for the split character.
						if(currentCharacter==SPLIT_CHARACTER)
						{
							boost::algorithm::trim(currentParameter);
							parameters.push_back(std::make_pair(currentParameter,parameterCouldBeAFunction));
							currentParameter.resize(0);
							parameterCouldBeAFunction = false;
							continue;
						}
					}
				
					if(params[c]==OPEN_PARENTHESIS)
					{
						level++;
						//It could be a function, so mark this parameter to be called as a function.
						parameterCouldBeAFunction = true;
					}
					
					if(params[c]==CLOSE_PARENTHESIS)
					{
						if(level>0)
						{
							level--;
						}
					}
					currentParameter+=currentCharacter;
				}
				boost::algorithm::trim(currentParameter);
				parameters.push_back(std::make_pair(currentParameter,parameterCouldBeAFunction));
			}else
			{
				return line;
			}
			return functionName;
		}

		std::string mFunctionRegEx;				//!< Regular expression used to extract function name and the parameter block from the calling string.
		Size mStreamPrecision;					//!< The precision used for value conversions with streams.
		typedef std::map<std::string, shared_ptr<BoundFunction> >::iterator BindingIterator;
		typedef std::map<std::string, shared_ptr<BoundFunction> >::const_iterator ConstBindingIterator;
		std::map<std::string, shared_ptr<BoundFunction> > mBindings;
		shared_ptr<FunctionBinder> mFallbackBinder;	//!< If a function is not found then the call will be passed to this binder if the pointer is valid.

		const char OPEN_PARENTHESIS;
		const char CLOSE_PARENTHESIS;
		const char QUOTATION_MARK;
		const char ESCAPE;
		const char SPLIT_CHARACTER;
		std::map< char, char > mEscapeCharacters;
	};
}

#endif 
