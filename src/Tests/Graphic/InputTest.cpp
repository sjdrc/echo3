#include <echo/Kernel/Kernel.h>
#include <echo/Platform.h>
#ifndef ECHO_PLATFORM_WINDOWS
#include <echo/Platforms/GTK/GTKMouse.h>
#include <echo/Platforms/GTK/GTKKeyboard.h>
#endif
#include <echo/Input/MappedInputDevice.h>
#include <echo/Input/MappedInputDeviceLoader.h>
#include <echo/FileSystem/FileSystem.h>
#include <boost/foreach.hpp>
#include <echo/cpp/functional>

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;

/**
 * InputChecker is a test utility that monitors the state of a given list of inputs.
 * If the inputs that an InputChecker is given changes states between frames the InputChecker will
 * output that the state has changed.
 * 
 * When performing checks, an InputChecker does not know the type of inputs being tested. To allow
 * for this functionality InputBase and InputStateBase can be used to store and move Inputs around,
 * and check the state of the input without knowing the type to test. You can aquire an
 * InputStateBase object and compare it against another InputStateBase. The objects know what types
 * they are and perform type checking during abstract comparisons. Most of the time you'll use
 * Input objects directly, such as Input<bool> or Input<f32> (the most common types), but if you
 * come across a time where you might need to check for a general state change then this class may
 * prove useful.
 */
class InputChecker : public Task
{
public:
	struct CheckData
	{
		CheckData(shared_ptr< InputBase > input, shared_ptr<InputStateBase> last, shared_ptr<InputStateBase> current) : mInput(input), mLastValue(last), mCurrentValue(current){}
		~CheckData()
		{
		}
		shared_ptr< InputBase > mInput;
		shared_ptr<InputStateBase> mLastValue;
		shared_ptr<InputStateBase> mCurrentValue;
	};

	InputChecker(InputManager& inputManager) : Task("InputChecker"), mInputManager(inputManager)
	{

	}
	void Update(Seconds)
	{
		typedef std::pair<const std::string, CheckData* > NamedAnalogInput;
		BOOST_FOREACH(NamedAnalogInput& namedInput, mChecks)
		{
			namedInput.second->mInput->GetState(*namedInput.second->mCurrentValue);
			if(*namedInput.second->mLastValue!=*namedInput.second->mCurrentValue)
			{
				std::cout << namedInput.first << " changed: " << *namedInput.second->mCurrentValue << std::endl;
				*namedInput.second->mLastValue=*namedInput.second->mCurrentValue;
			}
		}
	}

	template<typename T>
	void AddCheck(const std::string& name)
	{
		shared_ptr< Input<T> > input = mInputManager.GetInput<T>(name);
		if(!input)
		{
			std::cout << "Input \"" << name << "\" not found" << std::endl;
			return;
		}
		CheckData* checkData = new CheckData(input, input->GetState(),input->GetState());
		mChecks[name] = checkData;
	}

private:
	InputManager& mInputManager;
	std::map<std::string, CheckData* > mChecks;
};

class DownHandler
{
public:
	void HandleInput(bool v)
	{
		ECHO_LOG_INFO("Down button changed state: " << v);
	}
};

class ClickHandler
{
public:
	void HandleInput(bool click)
	{
		ECHO_LOG_INFO("Mouse click changed state: " << click);
	}
};

class MoveHandler
{
public:
	MoveHandler(std::string name) : mName(name){}
	void HandleInput(f32 position)
	{
		ECHO_LOG_INFO(mName << ": " << position);
	}
private:
	std::string mName;
};

void KeyboardAndMouseTest()
{
	shared_ptr<FileSystem> fileSystem = Platform::CreateDefaultFileSystem("Input Tests");
	
	//We will create a kernel which will manage the main program loop.
	Kernel kernel;
	
	//Create a window render target. Using the platform interface will create the default one for the target platform.
	shared_ptr<RenderTarget> target = Platform::CreateRenderTarget("Window","Keyboard And Mouse Test",kernel,800,300,32,false);

	//Create an input manager.
	shared_ptr<InputManager> inputManager;
	
	#ifndef ECHO_PLATFORM_WINDOWS
	target->SetPrimary(true);
	inputManager = make_shared<InputManager>();

	// This is a GTK specific test and will not work on all platforms. Normally you don't need to create input device objects
	// yourself unless you're adding a new device or platform implementation. If you're using this Test as reference then you
	// are probably using Application for your application setup  (if you're not then I'm interested to know why not) and as
	// such you should already have a default InputManager initialised with some default. Use GetInputManager() to get this
	// object and then use GetInput<T> to acquire input objects. Assuming you're within the Application class scope:
	//
	// shared_ptr< Input<bool> > aButton = GetInputManager("InputDeviceName:InputName");
	//
	// Should get you a shared pointer to the input object described by the string. The input devices available depend on the
	// platform. Although Echo defines inputs that should be available on all platforms. A configuration file to configure
	// inputs for each platform is recommended. Here is an example of getting a button from the Keyboard.
	//
	// shared_ptr< Input<bool> > spacebar = GetInputManager("Keyboard:Space");
	//
	// Analogue input objects use the f32 type. Input can be created for any object type though, so the following is also legal.
	//
	// shared_ptr< Input<Vector3> > position = GetInputManager("SomeDevice:Position");
	// 

	// As mentioned above, this setup is done only for this test, use Application::GetInputManager() or
	// Platform::CreateDefaultInputManager() to get an InputManager to get input objects from.
	shared_ptr<GTKMouse> mouse = make_shared<GTKMouse>(dynamic_pointer_cast<GTKWindow,RenderTarget>(target));
	inputManager->InstallDevice("Mouse",mouse);

	shared_ptr<GTKKeyboard> keyboard = make_shared<GTKKeyboard>(dynamic_pointer_cast<GTKWindow,RenderTarget>(target));
	inputManager->InstallDevice("Keyboard",keyboard);
	#else
	inputManager = Platform::CreateDefaultInputManager(target);

	#endif
	
	//The input checker is used to output any change of state for inputs in this test. It is not a standard object.
	InputChecker inputChecker(*inputManager);
	inputChecker.AddCheck<f32>("Mouse:X");
	inputChecker.AddCheck<f32>("Mouse:Y");
	
	kernel.AddTask(inputManager);

	// Mapped input devices allow us to map input from a device to a virtual device.
	// We'll create a mapped device with some inputs which will be used as though it represented all of a player's input.
	// The benefit of this is that the mappings can be changed any time without affecting the input objects that be in use.
	shared_ptr<MappedInputDevice> mappedDevice = shared_ptr<MappedInputDevice>(new MappedInputDevice("MappedDevice",*inputManager));
	mappedDevice->CreateDigitalInput("Left","Keyboard:a");
	mappedDevice->CreateDigitalInput("Right","Keyboard:d");
	mappedDevice->CreateDigitalInput("Up","Keyboard:w");
	mappedDevice->CreateDigitalInput("Down","Keyboard:s");
	
	mappedDevice->CreateAnalogInput("X","Mouse:X");
	mappedDevice->CreateAnalogInput("Y","Mouse:Y");
	mappedDevice->CreateDigitalInput("Click","Mouse:Left");
	
	//These handlers will be in scope the whole time the kernel is executed.
	DownHandler downHandler;
	ClickHandler clickHandler;
	MoveHandler xMoveHandler("X");
	MoveHandler yMoveHandler("Y");
	
	// Add some state change callbacks to the device. It is better to add this to mapped input devices and use multiple
	// mapped input devices for different program states (even using a different InputManager for different states is
	// useful). Adding callbacks to a mapped device means that you can keep the mapped device set up for a specific state.
	// This allows you to have an "A" button mapped to different devices with each device set up with different callbacks,
	// simplifying your callback logic.
	mappedDevice->AddChangeCallback<bool>("Down",bind(&DownHandler::HandleInput,&downHandler, placeholders::_1));
	mappedDevice->AddChangeCallback<bool>("Click",bind(&ClickHandler::HandleInput,&clickHandler, placeholders::_1));
	mappedDevice->AddChangeCallback<f32>("X",bind(&MoveHandler::HandleInput,&xMoveHandler, placeholders::_1));
	mappedDevice->AddChangeCallback<f32>("Y",bind(&MoveHandler::HandleInput,&yMoveHandler, placeholders::_1));
	
	//Install the device and name it for reference.
	inputManager->InstallDevice("Player",mappedDevice);
	inputChecker.AddCheck<bool>("Player:Down");
	inputChecker.AddCheck<bool>("Player:Up");
	inputChecker.AddCheck<bool>("Player:Left");
	inputChecker.AddCheck<bool>("Player:Right");

	//Test the input mapping loader
	shared_ptr<MappedInputDevice> loadedMappedDevice = MappedInputDeviceLoader::LoadDevice(fileSystem->Open("data/Controller.input"),*inputManager);
	REQUIRE_MESSAGE(bool(loadedMappedDevice),"Could not create input device from data/Controller.input");

	inputManager->InstallDevice("Controller",loadedMappedDevice);
	inputChecker.AddCheck<bool>("Controller:CA");
	inputChecker.AddCheck<f32>("Controller:StickA");
	inputChecker.AddCheck<f32>("Controller:StickB");
	
	kernel.AddTask(&inputChecker);
	
	kernel.SetExecutionModel(Platform::CreateExecutionModel());
	kernel.Execute();
}

class MultiTypeInputDevice : public InputDevice
{
public:
	MultiTypeInputDevice() : InputDevice("MultiTypeInputDevice")
	{
		mVector3 = make_shared<Vector3>();
		*mVector3 = Vector3(10,20,30);
		mQuaternion = make_shared< Quaternion>();
		mQuaternion->FromAngleAxis(Radian(Maths::HALF_PI),Vector3(1,0,0));
		AddInput("Position", mVector3, Vector3::ZERO);
		AddInput("Orientation", mQuaternion, Quaternion::ZERO);
	}
	~MultiTypeInputDevice()
	{}
private:
	shared_ptr<Vector3> mVector3;
	shared_ptr< Quaternion> mQuaternion;
};

void GeneralTypeTest()
{
	//Create an input manager.
	InputManager inputManager;
	
	shared_ptr<MultiTypeInputDevice> keyboard = make_shared<MultiTypeInputDevice>();
	inputManager.InstallDevice("MultiTypeInput",keyboard);
	
	//Make sure that the device doesn't return a valid input if the input doesn't exist.
	shared_ptr< Input<Vector3> > position = inputManager.GetInput<Vector3>("MultiTypeInput:Something");
	CHECK(bool(!position));
	position = inputManager.GetInput<Vector3>("MultiTypeInput:Position");
	REQUIRE(position);
	CHECK( bool(*position == Vector3(10,20,30)));
	
	shared_ptr< Input<Quaternion> > orientation = inputManager.GetInput<Quaternion>("MultiTypeInput:Orientation");
	REQUIRE(orientation);
	CHECK( bool(*orientation == Quaternion(Radian(Maths::HALF_PI),Vector3(1,0,0))));
}


TEST_CASE("Input")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	KeyboardAndMouseTest();
	GeneralTypeTest();
}

