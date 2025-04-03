#ifndef _ECHOINPUTDEVICE_H_
#define _ECHOINPUTDEVICE_H_

#include <echo/Kernel/Task.h>
#include <echo/Types.h>
#include <vector>
#include <map>
#include <boost/foreach.hpp>
#include <echo/cpp/functional>

namespace Echo
{
	/**
	 * Input state container.
	 * Used to abstractly deal with input state. For example, you may be interested
	 * in whether input has changed.
	 */
	class InputStateBase
	{
	public:
		InputStateBase(){}
		virtual ~InputStateBase(){}
		virtual bool operator==(const InputStateBase& rhs) const = 0;
		bool operator!=(const InputStateBase& rhs) const
		{
			return !(*this==rhs);
		}
		virtual InputStateBase& operator=(const InputStateBase& rhs) = 0;
		virtual bool IsSameType(const InputStateBase& other) const = 0;
		virtual void OutputTo(std::ostream& o) const = 0;
		inline friend std::ostream& operator <<(std::ostream& o, const InputStateBase& inputStateBase)
		{
			inputStateBase.OutputTo(o);
			return o;
		}
	};

	/**
	 * The object will contain the current value (state) and can be compared to other
	 * abstract objects. If the objects are of incompatible types then the comparison
	 * will be false.
	*/	
	template<typename T>
	class InputState : public InputStateBase
	{
	public:// neutral
		InputState(T value, T neutralValue) : mValue(value), mNeutralValue(neutralValue){}
		/**
		 * The default constructor will leave the value undefined.
		 * This should only be used when the object will used for type comparison.
		 */
		InputState(){}
		~InputState(){}
		
		bool IsSameType(const InputStateBase& other) const override
		{
			InputState const* t=dynamic_cast<InputState const*>(&other);
			return (t!=0);
		}

		bool operator==(const InputStateBase& rhs) const override
		{
			InputState<T> const* t=dynamic_cast<InputState<T> const*>(&rhs);
			return (t && t->mValue == mValue);
		}

		InputStateBase& operator=(const InputStateBase& rhs) override
		{
			InputState<T> const* t=dynamic_cast<InputState<T> const*>(&rhs);
			if(t)
			{
				mValue = t->mValue;
			}
			return *this;
		}
		
		InputState<T>& operator=(const InputState<T>& rhs)
		{
			mValue = rhs.mValue;
			return *this;
		}
		
		inline void OutputTo(std::ostream& o) const override
		{
			o << mValue;
		}
		inline operator T() const
		{
			return mValue;
		}
		inline const T& GetValue() const
		{
			return mValue;
		}

		inline void SetValue(const T& value)
		{
			mValue = value;
		}

		inline bool IsActive() const
		{
			return mValue!=mNeutralValue;
		}

		typedef function<void(const T&)> Callback;
	private:
		T mValue;
		T mNeutralValue;
	};

	class InputStateCheckerBase
	{
	public:
		InputStateCheckerBase(){}
		virtual ~InputStateCheckerBase(){}
		virtual void CheckAndFireCallbacks() = 0;
	};

	template<typename T>
	class InputStateChecker
	{
	public:
		InputStateChecker(const InputState<T>& currentState) : mLastState(currentState){}
		InputStateChecker(const InputStateChecker<T>& other)
		{
			mChangeCallbacks=other.mChangeCallbacks;
			mActiveCallbacks=other.mActiveCallbacks;
			mLastState = other.mLastState;
		}
		virtual ~InputStateChecker(){}
		void AddChangeCallback(const std::string& name, typename InputState<T>::Callback callback)
		{
			mChangeCallbacks[name].push_back(callback);
		}
		void AddActiveCallback(const std::string& name, typename InputState<T>::Callback callback)
		{
			mActiveCallbacks[name].push_back(callback);
		}
		virtual void CheckAndFireCallbacks(const InputState<T>& newState)
		{
			typedef std::pair< const std::string, std::vector< typename InputState<T>::Callback > > NamedCallbackSetPair;
			if(newState!=mLastState)
			{
				BOOST_FOREACH(NamedCallbackSetPair& callbackSet,mChangeCallbacks)
				{
					BOOST_FOREACH(typename InputState<T>::Callback& callback, callbackSet.second)
					{
						callback(newState.GetValue());
					}
				}
			}
			
			if(newState.IsActive() || mLastState.IsActive())
			{
				BOOST_FOREACH(NamedCallbackSetPair& callbackSet,mActiveCallbacks)
				{
					BOOST_FOREACH(typename InputState<T>::Callback& callback, callbackSet.second)
					{
						callback(newState.GetValue());
					}
				}
			}
			
			mLastState=newState;
		}
	private:
		std::map< std::string, std::vector< typename InputState<T>::Callback > > mChangeCallbacks;
		std::map< std::string, std::vector< typename InputState<T>::Callback > > mActiveCallbacks;
		InputState<T> mLastState;
	};	
	
	/**
	 * Base for Input so input can be stored generically.
	 */
	class InputBase
	{
	public:
		InputBase(){}
		virtual ~InputBase(){}
		/**
		 * Get an object that represents the current state of the input.
		 * @see InputState.
		 * @return InputState
		 */
		inline shared_ptr<InputStateBase> GetState()
		{
			return shared_ptr<InputStateBase>(GetStateUnmanaged());
		}
		/**
		 * This method is used to get the state and have it placed in an existing state object.
		 * This avoids allocating on the heap.
		 * @note Do not call this method with the incorrect type passed in, no type checking is performed.
		 */
		virtual void GetState(InputStateBase& value)=0;
		
		/**
		 * When called the input state will be checked and if changed callbacks fired.
		 */
		virtual void CheckAndFireCallbacks()=0;

		/**
		 * Called by InputManager before CheckAndFireCallbacks() to give the specialised Input type
		 * an update opportunity.
		 */
		virtual void Update(){}
	private:
		/**
		 * Like GetState() but returns a raw pointer that you will have to manage the memory for.
		 * @warning The called is responsible for the memory of the returned object.
		 *		  If you don't want to be then you may prefer to use GetState().
		 */
		virtual InputStateBase* GetStateUnmanaged()=0;
	};

	
	/**
	 * Input.
	 * InputDevices make a number of input objects available. Typically these Input objects give
	 * direct access to an input data. For example if you had a control with a button "A" you
	 * could make the state of the button available through an Input<bool> object. These inputs
	 * are registered with the InputDevice (usually internally in a sub-class) which then makes
	 * the input available via the InputDevice::GetInput() method.
	 * 
	 * When an InputDevice is "Installed" on an InputManager then the input object can be
	 * accessed through the InputManager by name. This allows code to be detached from devices
	 * but still use input directly; this is especially useful for input mapping as you can
	 * use "shared_ptr< Input<bool> > mJump" and assign jump based on a configuration.
	 * @see InputManager for more information on acquiring input.
	 */
	template <typename T>
	class Input : public InputBase
	{
	public:
		Input(const Input<T>& ein) :
			mData(ein.mData),
			mNeutral(ein.mNeutral),
			mStateChecker(make_shared(ein.mStateChecker))
		{
		}
		Input(shared_ptr<T> d, T neutral) : mData(d), mNeutral(neutral)
		{
		}
		Input(shared_ptr<T> d) : mData(d), mNeutral()
		{
		}
		virtual ~Input()
		{
		}
		inline bool operator >(T rhs) const
		{
			return ((*mData)>rhs);
		}
		inline bool operator <(T rhs) const
		{
			return ((*mData)<rhs);
		}
		inline bool operator >=(T rhs) const
		{
			return ((*mData)>=rhs);
		}
		inline bool operator <=(T rhs) const
		{
			return ((*mData)<=rhs);
		}
		inline bool operator ==(T rhs) const
		{
			return ((*mData)==rhs);
		}
		inline bool operator !=(T rhs) const
		{
			return ((*mData)!=rhs);
		}
		inline operator T()
		{
			return (*mData);
		}
		T& operator* ()
		{
			return (*mData);
		}
		const T& operator* () const
		{
			return (*mData);
		}
		shared_ptr<T> mData;
		T mNeutral;
		
		/**
		 * Get an object that represents the current state of the input.
		 * @see InputState.
		 * @return InputState
		 */
		inline shared_ptr<InputState<T> > GetState()
		{
			return shared_ptr<InputState<T> >(GetStateUnmanaged());
		}
		
		/**
		 * GetState method that avoids allocation.
		 * @note No type checking is performed. This method assumes that the passed in object is of the correct type.
		 */
		void GetState(InputStateBase& other) override
		{
			static_cast< InputState<T>* >(&other)->SetValue(*mData);
		}
		
		void CheckAndFireCallbacks() override
		{
			if(mStateChecker)
			{
				mStateChecker->CheckAndFireCallbacks(InputState<T>(*mData,mNeutral));
			}
		}
		
		InputStateChecker<T>& GetStateChecker()
		{
			if(!mStateChecker)
			{
				mStateChecker = shared_ptr< InputStateChecker<T> >(new InputStateChecker<T>(InputState<T>(*mData,mNeutral)));
			}
			return *mStateChecker;
		}
	private:
		shared_ptr< InputStateChecker<T> > mStateChecker;
		/**
		 * Like GetState() but returns a raw pointer that you will have to manage the memory for.
		 * @warning The called is responsible for the memory of the returned object.
		 * @see If you don't want to manage memory manually, prefer to use GetState().
		 */
		InputState<T>* GetStateUnmanaged() override
		{
			return new InputState<T>(*mData,mNeutral);
		}
	};
	
	/**
	 * A specialised type of input that updates the input value from a raw pointer.
	 * Updates will occur while the owner pointer can be locked successfully. This is
	 * a mechanism to avoid trying to access invalid pointers.
	 */
	template<typename T, typename U>
	class OwnedRawInput : public Input<T>
	{
	public:
		OwnedRawInput(weak_ptr<U> owner, T* dataSource, T neutral) :
			Input<T>(make_shared<T>(),neutral),
			mDataSource(dataSource),
			mOwner(owner)
		{}
		virtual void Update() override
		{
			//If we can lock the owner, it is safe to update the variable.
			shared_ptr<U> owner=mOwner.lock();
			if(owner)
			{
				*Input<T>::mData = *mDataSource;
			}
		}
	private:
		T* mDataSource;
		weak_ptr<U> mOwner;
	};
	
	/**
	 * A specialised type of input that updates the input value from a function call.
	 * Updates will occur while the owner pointer can be locked successfully.
	 */
	template<typename T, typename U>
	class OwnedFunctorInput : public Input<T>
	{
	public:
		typedef function<T()> Functor;
		OwnedFunctorInput(weak_ptr<U> owner, Functor dataSource, T neutral) :
			Input<T>(make_shared<T>(),neutral),
			mDataSource(dataSource),
			mOwner(owner)
		{}
		virtual void Update() override
		{
			//If we can lock the owner, it is safe to update the variable.
			shared_ptr<U> owner=mOwner.lock();
			if(owner)
			{
				*Input<T>::mData = mDataSource();
			}
		}
	private:
		Functor mDataSource;
		weak_ptr<U> mOwner;
	};
	
	class InputDevice : public Task
	{
	public:
		InputDevice(const std::string& deviceName);
		virtual ~InputDevice();
		void ClearAllInputs();
		void SetName(const std::string& name){mName=name;}
		const std::string& GetName() const {return mName;}

		bool SupportsVibration() const {return mSupportsVibration;}

		//If a device implementation supports vibrations it should implement these methods
		virtual void Vibrate(f32 time, f32 strength);	//time in seconds, strength percent
		virtual bool IsVibrating();						//returns true if the device is vibrating
		virtual void StopVibration();					//Stop the vibration if it is active

		template<typename T>
		bool AddInput(const std::string& inputName, shared_ptr<T> input, T neutral)
		{
			std::map< std::string, shared_ptr<InputBase> >::iterator it=mInputs.find(inputName);
			if(it!=mInputs.end())
			{
				return false;
			}
			mInputs[inputName]=shared_ptr< InputBase >(new Input<T>(input, neutral));
			return true;
		}
		
		template<typename T, typename U>
		bool AddRawInput(const std::string& inputName, weak_ptr<U> owner, T* source, T neutral)
		{
			std::map< std::string, shared_ptr<InputBase> >::iterator it=mInputs.find(inputName);
			if(it!=mInputs.end())
			{
				return false;
			}
			mInputs[inputName]=shared_ptr< InputBase >(new OwnedRawInput<T,U>(owner,source,neutral));
			return true;
		}
		
		template<typename T, typename U>
		bool AddFunctorInput(const std::string& inputName, weak_ptr<U> owner,typename OwnedFunctorInput<T,U>::Functor source, T neutral)
		{
			std::map< std::string, shared_ptr<InputBase> >::iterator it=mInputs.find(inputName);
			if(it!=mInputs.end())
			{
				return false;
			}
			mInputs[inputName]=shared_ptr< InputBase >(new OwnedFunctorInput<T,U>(owner,source,neutral));
			return true;
		}		
		
		bool RemoveInput(const std::string& inputName);
		
		bool HasInput(const std::string& inputName) const;

		template <typename T>
		bool ResolveInputName(const Input<T>& input, std::string& outName)
		{
			std::map< std::string, shared_ptr< InputBase > >::iterator it=mInputs.begin();
			std::map< std::string, shared_ptr< InputBase > >::iterator itEnd=mInputs.end();
			while(it!=itEnd)
			{
				Input<T>* inputCast = dynamic_cast< Input<T>* >(it->second.get());
				if(inputCast)
				{
					if(inputCast->mData == input.mData)
					{
						outName=it->first;
						return true;
					}
				}
				++it;
			}
			return false;
		}

		template<typename T>
		shared_ptr< Input<T> > GetInput(const std::string& inputName)
		{
			std::map< std::string, shared_ptr< InputBase > >::iterator it=mInputs.find(inputName);
			if(it!=mInputs.end())
			{
				shared_ptr< Input<T> > inputCast = dynamic_pointer_cast< Input<T> >(it->second);
				if(inputCast)
				{
					return inputCast;
				}
			}
			return shared_ptr< Input<T> >();
		}

		/**
		 * Get all inputs.
		 * Add the inputs to the provided vector.
		 * @param inputsOut The vector is not cleared.
		 */
		void GetInputs(std::vector<std::string>& inputsOut) const;

		/**
		 * Get all inputs
		 * @return a vector of inputs by name.
		 */
		std::vector<std::string> GetInputs() const;

		/**
		 * Add a state change callback for a specific input.
		 * When the state of the specified input changes.
		 * @param inputName
		 * @param inputState
		 * @return 
		 */
		template <typename T>
		bool AddChangeCallback(const std::string& inputName, typename InputState<T>::Callback callback, std::string callbackName="")
		{
			shared_ptr< Input<T> > input = GetInput<T>(inputName);
			if(input)
			{
				InputStateChecker<T>& stateChecker = input->GetStateChecker();
				stateChecker.AddChangeCallback(callbackName,callback);
				return true;
			}
			return false;
		}
		
		template <typename T>
		bool AddActiveCallback(const std::string& inputName, typename InputState<T>::Callback callback, std::string callbackName="")
		{
			shared_ptr< Input<T> > input = GetInput<T>(inputName);
			if(input)
			{
				InputStateChecker<T>& stateChecker = input->GetStateChecker();
				stateChecker.AddActiveCallback(callbackName,callback);
				return true;
			}
			return false;
		}		
		virtual void Update(Seconds) override;

		const std::map< std::string, shared_ptr<InputBase> >& GetAllInputs() const
		{
			return mInputs;
		}
	protected:
		std::string mName;
		std::map< std::string, shared_ptr<InputBase> > mInputs;
		bool mSupportsVibration;
		//Devices need to EnableVibrationSupport() if they support vibration
		void EnableVibrationSupport();
	};
}
#endif
