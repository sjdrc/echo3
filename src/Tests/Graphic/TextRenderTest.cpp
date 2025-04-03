#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/TextEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Application.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/FontManager.h>
#include <echo/cpp/functional>

#include <echo/Chrono/CountDownTimer.h>
#include <doctest/doctest.h>

using namespace Echo;

class TextRenderTest : public Application
{
public:
	class Spinner : public Task
	{
	public:
		typedef function<void(f32,f32)> SetFunction;

		Spinner(SetFunction setFunction, f32 magnitudeMultiplier, f32 speedMultiplier, f32 startAngle) :
			mSetFunction(setFunction),
			mAngle(startAngle),
			mSpeedMultiplier(speedMultiplier),
			mMagnitudeMultiplier(magnitudeMultiplier)
		{
			
		}
		
		void Update(Seconds lastFrameTime)
		{
			mAngle += static_cast<f32>(lastFrameTime.count()) * mSpeedMultiplier;
			mSetFunction(Maths::Sin(mAngle,false) * mMagnitudeMultiplier,Maths::Cos(mAngle,false) * mMagnitudeMultiplier);
		}
		SetFunction mSetFunction;
		f32 mAngle;
		f32 mSpeedMultiplier;
		f32 mMagnitudeMultiplier;
	};

	TextRenderTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		GetConfiguration().Set<u32>("resolution.x",800);
		GetConfiguration().Set<u32>("resolution.y",600);
		Initialise("TextRenderTest");
		
		mScene.SetUseOnlyZForDistanceCalculations(true);
		
		//Load resource lists
		GetFontManager()->LoadList(GetFileSystem()->Open("data/fonts.list"));
		
		//The second camera will be moving around the Z axis at a distance and tacking another stationary node.
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetAutoTracking(true,make_shared<Node>(),Vector3(0,0,0));
		camera->SetPosition(0,0,10);

		//This spinner moves the second camera.
		mSpinner = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Camera::*)(f32,f32,f32)>(&Camera::SetPosition),camera,placeholders::_1,placeholders::_2,7),3.f,1.0f,0.f));
		AddTask(mSpinner.get());

		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);
		AddTask(GetRenderer().get());

		//The demo can be closed using the window or it will timeout automatically.
		AddTask(mTimeout);
		
		mTextEntity = shared_ptr< TextEntity >(new TextEntity("Hello World three four five areallylongwordthatshouldbescaled", GetFontManager()->GetResource("TestFont")));
		mTextEntity->SetUseMaxWidth(true);
		mTextEntity->SetMaxWidth(750);
		mTextEntity->UpdateMesh();
		Vector3 scale = mTextEntity->GetTextDimensions();
		scale.y = 5.f / scale.y;
		scale.x = scale.y;
		scale.z = 1.0f;
		mTextEntity->SetScale(scale);
		mScene.AddRenderable(mTextEntity);
	
		//Non-fixed width text, using a text scale rather than node scale.
		shared_ptr< TextEntity > line = make_shared<TextEntity>(UTF8String("\xE2\x88\x83y \xE2\x88\x80x \xC2\xAC(x \xE2\x89\xBA y)"), GetFontManager()->GetResource("DejaVu"));
		line->SetTextScale(0.01f);
		line->SetPosition(Vector3(0.f, 0.f, 0.01f));
		line->SetColour(Colours::ALIZARIN_CRIMSON);
		mScene.AddRenderable(line);

		line = make_shared<TextEntity>(UTF8String("Multi\nline"), GetFontManager()->GetResource("DejaVu"));
		line->SetTextScale(0.01f);
		line->SetPosition(Vector3(0.f, -2.f, 0.01f));
		line->SetColour(Colours::ALIZARIN_CRIMSON);
		mScene.AddRenderable(line);
		
		File textFile = GetFileSystem()->Open("data/Messages.txt");
		std::vector<std::string> lines;
		textFile.ReadLines(lines,false,"#");
		for(size_t i=0; i<lines.size();++i)
		{
			const std::string& line = lines[i];
			std::string left;
			UTF8String right;
			if(Utils::String::SplitInTwo(line,"=",left,right))
			{
				std::cout << "Creating text with font: " << left << " contents: " << right << std::endl;
				shared_ptr<Font> font=GetFontManager()->GetResource(left);
				shared_ptr<TextEntity> text=make_shared<TextEntity>(right,font);
				text->SetTextScale(0.01f);
				text->UpdateMesh();
				f32 y=i;
				y+=lines.size() * 0.6f;
				text->SetPosition(Vector3(0.f, y , 0.5f + 0.1f*i));
				text->SetColour(Colours::GOLD_METALLIC);
				mScene.AddRenderable(text);
			}else
			{
				std::cout << "Could not split line(" << i << "): " << line << std::endl;
			}
		}

		shared_ptr<Font> font = GetFontManager()->GetResource("TestFont");
		if(font)
		{
			mColourSpinner = shared_ptr<Spinner>(new Spinner(bind(&TextRenderTest::SetColour,this,placeholders::_1,placeholders::_2),1.f,1.0f,0.f));
			AddTask(mColourSpinner.get());
		}

		mTimeout2 = new CountDownTimer(Seconds(5.0),bind(&TextEntity::Set,mTextEntity,"Echo engine!"),"ChangeText");
		AddTask(mTimeout2);
	}
	
	void SetColour(f32 a,f32 b)
	{
		shared_ptr<Font> font = GetFontManager()->GetResource("TestFont");
		if(font)
		{
			font->GetMaterial()->GetPass(0)->SetDiffuse(Colour(a,b,1.f));
		}
	}
private:
	Scene mScene;
	shared_ptr< TextEntity > mTextEntity;
	shared_ptr<Spinner> mSpinner;
	shared_ptr<Spinner> mColourSpinner;
	CountDownTimer mTimeout;
	CountDownTimer* mTimeout2;
	CountDownTimer* mTimeoutChangeFormat;
};

TEST_CASE("TextRender")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	TextRenderTest textRenderTest;
	textRenderTest.Execute();
}

