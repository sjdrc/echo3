#ifndef _ECHOMAPPEDINPUTDEVICE_H_
#define _ECHOMAPPEDINPUTDEVICE_H_

#include <echo/Kernel/Task.h>
#include <echo/Input/Input.h>
#include <echo/Input/InputDevice.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Quaternion.h>
#include <list>
#include <map>

namespace Echo
{
	class InputTransform;
	class InputMapping;
	
	/**
	 * A base for container classes that manage a variable to have values mapped to.
	 * InputDevice supports adding any variable as input but mapped input only knows
	 * about digital and analogue inputs. The components of a variable currently can't
	 * easily be added separately. This class enables automatic mapping to child
	 * components based on MappableInputBase specialisations and are created in
	 * MappedInputDevice automatically when CreateMappableInput() is called.
	 * You won't need to use this class directly.
	 * @see CreateMappableInput() for how this is used information.
	 */
	class MappableInputComponetCollection
	{
	public:
		virtual ~MappableInputComponetCollection(){}

		template< typename T >
		struct Component
		{
			Component(T* value) : mValue(value)
			{
				assert(mValue && "value must not be null");
			}
			shared_ptr< Input<T> > mSource;		//Source needs to be set before UpdateValue() is called.
			T* mValue;
			void UpdateValue()
			{
				*mValue=*mSource;
			}
		};
		
		typedef std::pair<const std::string, Component<f32> > NamedAnalogComponentPair;
		typedef std::pair<const std::string, Component<bool> > NamedDigitalComponentPair;
		typedef std::map<std::string, Component<f32> > AnalogComponentMap;
		typedef std::map<std::string, Component<bool> > DigitalComponentMap;
		
		Size GetNumberOfComponents()
		{
			return mDigitalComponents.size() + mAnalogComponents.size();
		}

		void UpdateComponentValues()
		{
			typedef std::pair<const std::string, Component<bool> > NamedDigitalComponent;
			BOOST_FOREACH(NamedDigitalComponent& ndc, mDigitalComponents)
			{
				ndc.second.UpdateValue();
			}
			typedef std::pair<const std::string, Component<f32> > NamedAnglogComponent;
			BOOST_FOREACH(NamedAnglogComponent& ndc, mAnalogComponents)
			{
				ndc.second.UpdateValue();
			}
		}
		
		AnalogComponentMap& GetAnalogComponents() {return mAnalogComponents;}
		DigitalComponentMap& GetDigitalComponents() {return mDigitalComponents;}
	protected:
		MappableInputComponetCollection(){}
		void AddComponent(std::string componentName, f32* component)
		{
			mAnalogComponents.insert(std::make_pair(componentName, Component<f32>(component)));
		}
		void AddComponent(std::string componentName, bool* component)
		{
			mDigitalComponents.insert(std::make_pair(componentName, Component<bool>(component)));
		}
	private:
		std::map<std::string, Component<f32> > mAnalogComponents;
		std::map<std::string, Component<bool> > mDigitalComponents;
	};
	
	template<typename T>
	class MappableInputBase : public MappableInputComponetCollection
	{
	public:
		MappableInputBase()
		{
			mValue=make_shared<T>();
		}
		virtual ~MappableInputBase(){}
		shared_ptr<T> GetInput(){return mValue;}
	protected:
		shared_ptr<T> mValue;
	};
	
	template<typename T>
	class MappableInput : public MappableInputBase<T>
	{
	public:
		MappableInput(){}
		virtual ~MappableInput(){}
	};

	
	template<>
	class MappableInput<Vector3> : public MappableInputBase<Vector3>
	{
	public:
		MappableInput()
		{
			AddComponent("x",&mValue->x);
			AddComponent("y",&mValue->y);
			AddComponent("z",&mValue->z);
		}
	};
	
	template<>
	class MappableInput<Vector2> : public MappableInputBase<Vector2>
	{
	public:
		MappableInput()
		{
			AddComponent("x",&mValue->x);
			AddComponent("y",&mValue->y);
		}
	};	

	template<>
	class MappableInput<Quaternion> : public MappableInputBase<Quaternion>
	{
	public:
		MappableInput()
		{
			AddComponent("w",&mValue->w);
			AddComponent("x",&mValue->x);
			AddComponent("y",&mValue->y);
			AddComponent("z",&mValue->z);
		}
	};
	
	/**
	 * A MappedInputDevice allows you to create a device with custom inputs that acquire their
	 * values from another input source.
	 * MappedInputDevice supports mappings from digital to analogue values.
	 */
	class MappedInputDevice : public InputDevice
	{
	public:
		MappedInputDevice(const std::string& name, InputManager& inputManager);
		~MappedInputDevice();

		void Vibrate(f32 time, f32 strength) override;
		bool IsVibrating() override;
		void StopVibration() override;

		//Creates an input and automatically resolves the source type
		//threshold and analogValue are only used if a Pseudo input is created
		//otherwise each is ignored.
		//These are the highest level Input Creation functions. It is recommended
		//to use these.
		bool CreateDigitalInput(const std::string& inputName, const std::string& inputSourceName, f32 threshold=0.5f);
		bool CreateAnalogInput(const std::string& inputName, const std::string& inputSourceName, f32 analogValue=1.0f);
		bool CreateAnalogInput(const std::string& inputName, const std::vector<std::string>& inputSourceNames, const std::vector<f32>& analogValues);

		//Removes input
		bool RemoveInput(const std::string& inputName);

		//Change an input's source
		bool ChangeDigitalInput(const std::string& inputName, const std::string& newIputSourceName, f32 threshold=0.5f);
		bool ChangeAnalogInput(const std::string& inputName, const std::string& inputSourceName, f32 analogValue=1.0f);
		bool ChangeAnalogInput(const std::string& inputName, const std::vector<std::string>& inputSourceNames, const std::vector<f32>& analogValues);

		shared_ptr<InputMapping> GetInputMapping(const std::string& inputName);

		void Update(Seconds lastFrameTime) override;
		
		template < typename T >
		bool ResolveInputName(const Input<T>& input, std::string& outName)
		{
			if(mInputManager)
			{
				return mInputManager->ResolveInputName<T>(input,outName);
			}
			return false;
		}

		/**
		 * Creates an input of the given type with components mapped to the sourceNames.
		 * The provided type needs to have a MappableInput specialisation so this method can create
		 * the corresponding inputs. You can create your own MappableInput or use some of the
		 * predefined types.
		 * Each component is given the name inputName.componentName
		 * MappableInput<Vector3> is a predefined specialisation that provides x,y and z components
		 * for mapping.
		 * A use case example might be:
		 * 
		 *	std::vector<std::string> inputSources;
		 *	inputSources.push_back("Controller:X");
		 *	inputSources.push_back("Controller:Y");
		 *	playerInput->CreateMappableInput<Vector2>("Move", inputSources, Vector2::ZERO);
		 *
		 * This creates the Input "Move" where by the inputs "Controller:X" and "Controller:Y" are
		 * mapped to Move.x and Move.y respectively.
		 *
		 * @param inputName The name of the input. Use GetInput<T>() to get the input.
		 * @param sourceNames The source names that will be mapped to each of the components for the type.
		 * The sources should define mappings to exhaust all analog components first then the digital
		 * components, you should know in advance how many of each are required.
		 * @return true if all inputs were mapped, otherwise false to indicate. On failure the state of
		 * the device will be restored (i.e. if some mapped inputs were created they will be removed).
		 */		
		template<class T >
		bool CreateMappableInput(const std::string& inputName, const std::vector<std::string>& sourceNames, T neutral)
		{
			if(HasInput(inputName))
			{
				ECHO_LOG_ERROR("MappedInputDevice::CreateMappableInput(): Input with name " << inputName << " already exists.");
				return false;
			}
			
			shared_ptr< MappableInput<T> > mappableInput = make_shared< MappableInput<T> >();
			if(sourceNames.size()!=mappableInput->GetNumberOfComponents())
			{
				ECHO_LOG_ERROR("MappedInputDevice::CreateMappableInput(): the number of sources passed in does not match the target type components for " << inputName << ".");
				return false;
			}
			Size sourceIndex = 0;
			std::vector<std::string> createdInputs;
			typedef MappableInputComponetCollection::NamedAnalogComponentPair NamedAnalogComponentPair;
			BOOST_FOREACH(NamedAnalogComponentPair& namedComponent, mappableInput->GetAnalogComponents())
			{
				std::string componentInputName = inputName + "." + namedComponent.first;
				if(CreateAnalogInput(componentInputName, sourceNames[sourceIndex]))
				{
					namedComponent.second.mSource = GetInput<f32>(componentInputName);
					createdInputs.push_back(componentInputName);
				}else
				{
					BOOST_FOREACH(const std::string& createdInput, createdInputs)
					{
						RemoveInput(createdInput);
					}
					return false;
				}
				sourceIndex++;
			}
			typedef MappableInputComponetCollection::NamedDigitalComponentPair NamedDigitalComponentPair;
			BOOST_FOREACH(NamedDigitalComponentPair& namedComponent, mappableInput->GetDigitalComponents())
			{
				std::string componentInputName = inputName + "." + namedComponent.first;
				if(CreateDigitalInput(componentInputName, sourceNames[sourceIndex]))
				{
					namedComponent.second.mSource = GetInput<bool>(componentInputName);
					createdInputs.push_back(componentInputName);
				}else
				{
					BOOST_FOREACH(const std::string& createdInput, createdInputs)
					{
						RemoveInput(createdInput);
					}
					return false;
				}
				sourceIndex++;
			}

			//Finally add the base input - This won't fail because we already checked for existing inputs.
			AddInput(inputName, mappableInput->GetInput(), neutral);
			
			mMappableInputs[inputName] = mappableInput;
			return true;
		}
	private:
		/**
		 * When the task manager is removed it is most likely going to be the InputManager.
		 * The task manager will be removed manually by someone or automatically when it is destroyed.
		 * We will assume the worst and stop using it.
		 * @param manager
		 */
		void OnTaskManagerRemoved(TaskManager& manager) override;

		InputManager* mInputManager;
		std::map< std::string, shared_ptr<InputMapping> > mInputMappings;
		std::map< std::string, shared_ptr<MappableInputComponetCollection> > mMappableInputs;
		std::map< std::string, std::list<shared_ptr<InputDevice> > > mVibratingInputs;
		bool ApplyDigitialMapping(const std::string& name, shared_ptr<InputTransform> transform);
		bool ApplyAnalogMapping(const std::string& name, shared_ptr<InputTransform> transform);
		shared_ptr<InputTransform> _CreateAnalogInputTransform(const std::vector<std::string>& inputSourceNames, const std::vector<f32>& analogValues, std::list<shared_ptr<InputDevice> >& outVibratingDevices);
		shared_ptr<InputTransform> _CreateDigitalInputTransform(const std::string& inputSourceName, f32& threshold, std::list<shared_ptr<InputDevice> >& outVibratingDevices);
	};
	
	class InputTransform
	{
	public:
		struct InputSourceType
		{
			enum InputSourceTypeEnum{
				UNKNOWN=0,
				ANALOG=1,
				DIGITAL=2};
		};

		InputTransform(){}
		virtual ~InputTransform(){}
		virtual InputSourceType::InputSourceTypeEnum GetInputSourceType()=0;
		virtual InputSourceType::InputSourceTypeEnum GetInputTargetType()=0;	
		virtual void GetInputSourceName(std::vector< std::string >& sourceNamesOut)=0;
		virtual f32 GetAnalogValue(){return 0;}
		virtual bool GetDigitalValue(){return 0;}
	};

	class AnalogToAnalogInputTransform : public InputTransform
	{
	private:
		shared_ptr< Input<f32> > mInputSource;
		MappedInputDevice& mMappedInputDevice;
	public:
		AnalogToAnalogInputTransform(MappedInputDevice& mappedInputDevice) : mMappedInputDevice(mappedInputDevice){}
		~AnalogToAnalogInputTransform(){}
		InputSourceType::InputSourceTypeEnum GetInputSourceType(){return InputSourceType::ANALOG;}
		InputSourceType::InputSourceTypeEnum GetInputTargetType(){return InputSourceType::ANALOG;}
		void GetInputSourceName(std::vector< std::string >& sourceNamesOut)	
		{
			std::string nameOut;
			if(mMappedInputDevice.ResolveInputName(*mInputSource,nameOut))
			{
				sourceNamesOut.push_back(nameOut);
			}
		}
		void SetSource(shared_ptr< Input<f32> > inputSource)
		{
			mInputSource=inputSource;
		}
		f32 GetAnalogValue()
		{
			return *(mInputSource->mData);
		}
	};

	class DigitalToDigitalInputTransform : public InputTransform
	{
	private:
		shared_ptr< Input<bool> > mInputSource;
		MappedInputDevice& mMappedInputDevice;
	public:
		DigitalToDigitalInputTransform(MappedInputDevice& mappedInputDevice) : mMappedInputDevice(mappedInputDevice){}
		~DigitalToDigitalInputTransform(){}
		InputSourceType::InputSourceTypeEnum GetInputSourceType(){return InputSourceType::DIGITAL;}
		InputSourceType::InputSourceTypeEnum GetInputTargetType(){return InputSourceType::DIGITAL;}
		void GetInputSourceName(std::vector< std::string >& sourceNamesOut)	
		{
			std::string nameOut;
			if(mMappedInputDevice.ResolveInputName(*mInputSource,nameOut))
			{
				sourceNamesOut.push_back(nameOut);
			}
		}
		void SetSource(shared_ptr< Input<bool> > inputSource){mInputSource=inputSource;}
		bool GetDigitalValue(){return *(mInputSource->mData);}
	};

	class AnalogToDigitalInputTransform : public InputTransform
	{
	private:
		MappedInputDevice& mMappedInputDevice;
		f32 mThreshold;
		shared_ptr< Input<f32> > mInputSource;
	public:
		AnalogToDigitalInputTransform(MappedInputDevice& mappedInputDevice, f32 threshold) : mMappedInputDevice(mappedInputDevice), mThreshold(threshold){}
		~AnalogToDigitalInputTransform(){}
		InputSourceType::InputSourceTypeEnum GetInputSourceType(){return InputSourceType::ANALOG;}
		InputSourceType::InputSourceTypeEnum GetInputTargetType(){return InputSourceType::DIGITAL;}
		void GetInputSourceName(std::vector< std::string >& sourceNamesOut)	
		{
			std::string nameOut;
			if(mMappedInputDevice.ResolveInputName(*mInputSource,nameOut))
			{
				sourceNamesOut.push_back(nameOut);
			}
		}
		void SetSource(shared_ptr< Input<f32> > inputSource){mInputSource=inputSource;}
		bool GetDigitalValue()
		{
			if(mThreshold>0)
			{
				return ((*(mInputSource->mData))>=mThreshold);
			}else
			{
				return ((*(mInputSource->mData))<=mThreshold);
			}
			return false;
		}
	};

	class DigitalToAnalogInputTransform : public InputTransform
	{
	private:
		struct DigitalState
		{
			shared_ptr< Input<bool> > mSource;
			f32 mValue;
			DigitalState(shared_ptr< Input<bool> > source, f32 v=1.0f) : mSource(source), mValue(v){}
			~DigitalState(){}
		};
		MappedInputDevice& mMappedInputDevice;
		std::vector< DigitalState > mInputSourcesAndValues;
	public:
		DigitalToAnalogInputTransform(MappedInputDevice& mappedInputDevice, f32 inputValue, shared_ptr< Input<bool> > inputSource) :
			mMappedInputDevice(mappedInputDevice)
		{
			AddSource(inputSource,inputValue);
		}
		~DigitalToAnalogInputTransform(){}
		InputSourceType::InputSourceTypeEnum GetInputSourceType(){return InputSourceType::DIGITAL;}
		InputSourceType::InputSourceTypeEnum GetInputTargetType(){return InputSourceType::ANALOG;}
		void GetInputSourceName(std::vector< std::string >& sourceNamesOut)	
		{
			for(Size s=0;s<mInputSourcesAndValues.size();++s)
			{
				std::string nameOut;
				if(mMappedInputDevice.ResolveInputName(*(mInputSourcesAndValues[s].mSource),nameOut))
				{
					sourceNamesOut.push_back(nameOut);
				}
			}
		}
		void AddSource(shared_ptr< Input<bool> > inputSource, f32 inputValue)
		{
			if(inputSource->mData)
			{
				mInputSourcesAndValues.push_back(DigitalState(inputSource,inputValue));
			}
		}
		f32 GetAnalogValue()
		{
			f32 v=0;
			Size numSources=mInputSourcesAndValues.size();
			for(Size s=0;s<numSources;++s)
			{
				DigitalState& currentInput=mInputSourcesAndValues[s];
				if(*(currentInput.mSource->mData))
				{
					v=currentInput.mValue;
				}
			}
			return v;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//Input mappings
	class InputMapping
	{
	protected:
		shared_ptr<InputTransform> mTransform;
		bool mInvert;
	public:
		InputMapping(shared_ptr<InputTransform> transform) : mTransform(transform), mInvert(false){}
		virtual ~InputMapping(){}
		virtual InputTransform::InputSourceType::InputSourceTypeEnum GetInputTargetType()=0;
		void EnableInvertedResult(){mInvert=true;}
		void DisableInvertedResult(){mInvert=false;}
		void SetTransform(shared_ptr<InputTransform> transform)
		{
			mTransform=transform;
		}
		shared_ptr<InputTransform> GetTransform() const {return mTransform;}
		virtual void Update()=0;
	};

	class DigitalInputMapping : public InputMapping
	{
	private:
		shared_ptr<bool> mDigitalValue;
	public:
		DigitalInputMapping(shared_ptr<InputTransform> transform) : InputMapping(transform)
		{
			mDigitalValue = make_shared<bool>(false);
		}
		~DigitalInputMapping(){}	
		InputTransform::InputSourceType::InputSourceTypeEnum GetInputTargetType(){return InputTransform::InputSourceType::DIGITAL;}
		shared_ptr<bool> GetDigitalValuePtr() {return mDigitalValue;}
		void Update()
		{
			if(mInvert)
			{
				*mDigitalValue=!mTransform->GetDigitalValue();
			}else
			{
				*mDigitalValue=mTransform->GetDigitalValue();
			}
		}
	};

	class AnalogInputMapping : public InputMapping
	{
	private:
		shared_ptr<f32> mAnalogValue;
	public:
		AnalogInputMapping(shared_ptr<InputTransform> transform) : InputMapping(transform)
		{
			mAnalogValue = make_shared<f32>(0.f);
		}
		~AnalogInputMapping(){}
		InputTransform::InputSourceType::InputSourceTypeEnum GetInputTargetType(){return InputTransform::InputSourceType::ANALOG;}
		shared_ptr<f32> GetAnalogValuePtr() {return mAnalogValue;}
		void Update()
		{
			if(mInvert)
			{
				*mAnalogValue=-(mTransform->GetAnalogValue());
			}else
			{
				*mAnalogValue=mTransform->GetAnalogValue();
			}
		}
	};
}
#endif 
