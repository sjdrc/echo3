#include <doctest/doctest.h>
#undef INFO
#include <echo/Application.h>
#include <echo/GUI/Text.h>
#include <echo/GUI/Menu.h>
#include <echo/GUI/GUI.h>
#include <echo/Application.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Input/MIDIInputDevice.h>
#include <echo/Input/InputMonitor.h>
#include <echo/Input/InputMonitorListener.h>

using namespace Echo;
using namespace Echo::GUI;

class MIDITest : public Application
{
public:
	MIDITest() : mTimeout(Seconds(30.0), bind(&TaskManager::RemoveAllTasks, this), "RemoveAllTasks")
	{
	}

	bool Initialise()
	{
		if (!Application::Initialise("MIDITest", "data/MIDITest.config"))
		{
			return false;
		}
		
		AddTask(GetRenderer());
		AddTask(mTimeout);
		
		shared_ptr<FunctionBinder> functionBinder = make_shared<FunctionBinder>();
		functionBinder->RegisterVoid("Exit", bind(&Kernel::Stop, this));
		functionBinder->RegisterVoid("ResetTimeout", bind(&CountDownTimer::SetTimeout, ref(mTimeout), Seconds(30.)));
		
		shared_ptr<Menu> menu(new Menu(GetFileSystem(), GetMaterialManager(), GetFontManager(), GetInputManager(), GetRenderer(), functionBinder));
		menu->AddGUI("data/MIDIView.gui","MIDI");
		menu->Push("MIDI");
		AddTask(menu);

		if(!menu->GetCurrentGUI())
		{
			return false;
		}

		shared_ptr<Text> channel = menu->GetCurrentGUI()->FindElement<Text>("Channel");
		shared_ptr<Text> pitch = menu->GetCurrentGUI()->FindElement<Text>("Pitch");
		shared_ptr<Text> value = menu->GetCurrentGUI()->FindElement<Text>("Value");

		// This is Linux specific
		File midiDevice = GetFileSystem()->Open(GetConfiguration().Get("device", "/dev/dmmidi1"),File::OpenModes::READ);
		if(!midiDevice.IsOpen())
		{
			return false;
		}

		GetInputManager()->InstallDevice("MIDI",shared_ptr<InputDevice>(new MIDIInputDevice("MIDI1",midiDevice)));
		
		shared_ptr<InputMonitor> monitor(new InputMonitor(GetInputManager()));

		class InputThing : public InputMonitorListener
		{
		public:
			InputThing(shared_ptr<Text> channel, shared_ptr<Text> pitch, shared_ptr<Text> value) :
				mChannel(channel),
				mPitch(pitch),
				mValue(value)
			{}

			void OnDigitalInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input< bool > > input) override
			{
				ECHO_LOG_INFO("Device: " << deviceName << " Input: " << inputName << " Value: " << **input);
			}

			void OnAnalogInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input< f32 > > input) override
			{
				ECHO_LOG_INFO("Device: " << deviceName << " Input: " << inputName << " Value: " << **input);
				std::string channel;
				std::string pitchString;
				if(Utils::String::SplitInTwo(inputName,":",channel,pitchString))
				{
					if(mChannel)
					{
						mChannel->SetText(channel);
					}
					if(mPitch)
					{
						mPitch->SetText(pitchString);
					}
					if(mValue)
					{
						mValue->SetText(**input);

					}
				}
			}
		private:
			shared_ptr<Text> mChannel;
			shared_ptr<Text> mPitch;
			shared_ptr<Text> mValue;
		};

		monitor->AddListener(make_shared<InputThing>(channel,pitch,value));
		AddTask(monitor);
		return true;
	}
private:
	CountDownTimer mTimeout;
};

TEST_CASE("MIDI")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO | Echo::Logger::LogLevels::ERROR | Echo::Logger::LogLevels::WARNING);

	MIDITest midiTest;
	REQUIRE(midiTest.Initialise());
	midiTest.Execute();
}
