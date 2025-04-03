#include <echo/Kernel/Kernel.h>
#include <echo/Platform.h>

#include <echo/FileSystem/FileSystem.h>

#include <echo/Chrono/CountDownTimer.h>

#include <echo/Audio/Audio.h>
#include <echo/Audio/AudioPlayer.h>
#include <echo/Audio/AudioBuffer.h>
#include <echo/Audio/AudioStream.h>
#include <echo/Resource/WavAudioSource.h>
#include <echo/Resource/OggAudioSource.h>
#include <echo/Resource/XMAudioSource.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>
#undef INFO
using namespace Echo;

TEST_CASE("Audio")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	//We will create a kernel which will manage the main program loop.
	Kernel kernel;
	
	//Create a default a FileSystem object. 
	shared_ptr<FileSystem> fileSystem =  Platform::CreateDefaultFileSystem("RenderTests");
	
	//The demo will timeout automatically.
	CountDownTimer timeout(Seconds(15.0),bind(&TaskManager::RemoveAllTasks,&kernel),"RemoveAllTasks");
	kernel.AddTask(timeout);
	
	shared_ptr<Audio> audio = Platform::CreateDefaultAudioSystem();
	if(audio)
	{
		shared_ptr<ExecutionModel> model = Platform::CreateExecutionModel();
		model->SetModelToUse(ExecutionModel::Models::NONE);
		kernel.SetExecutionModel(model);

		AudioPlayer audioPlayer(audio,fileSystem);
		audioPlayer.AddMusic("MyTrack","data/TestTrack.ogg");
		audioPlayer.AddSound("MySound","data/TestSample.wav");
		audioPlayer.PlayMusic("MyTrack");
		CountDownTimer timeout2(Seconds(5.0),bind(&AudioPlayer::PlaySound,&audioPlayer,"MySound"),"MySound");
		kernel.AddTask(timeout2);
		CountDownTimer timeout3(Seconds(10.0),bind(&AudioPlayer::PlaySound,&audioPlayer,"MySound"),"MySound");
		kernel.AddTask(timeout3);
		kernel.AddTask(*audio);
		kernel.Execute();
	}else
	{
		ECHO_LOG_INFO("Skipped Audio test because an AudioSystem isn't configured");
	}
}
