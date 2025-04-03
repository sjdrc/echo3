#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/FontManager.h>
#include <echo/Application.h>
#include <echo/Graphics/MaterialAnimation.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Input/Input.h>
#include <echo/cpp/functional>
#include <echo/Util/FunctionBinder.h>
#include <echo/Util/StringSetter.h>

#include <echo/GUI/Screen.h>
#include <echo/GUI/Image.h>
#include <echo/GUI/Text.h>
#include <echo/GUI/TextBox.h>
#include <echo/GUI/Container.h>
#include <echo/GUI/Cursor.h>
#include <echo/GUI/Button.h>
#include <echo/GUI/GUI.h>

#include <doctest/doctest.h>

using namespace Echo;
using namespace Echo::GUI;

class GUITest : public Application
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
			mSetFunction(Maths::Abs(Maths::Sin(mAngle,false) * mMagnitudeMultiplier),
						Maths::Abs(Maths::Cos(mAngle,false) * mMagnitudeMultiplier));
		}
		SetFunction mSetFunction;
		f32 mAngle;
		f32 mSpeedMultiplier;
		f32 mMagnitudeMultiplier;
	};

	GUITest() : mTimeout(Seconds(30.0))
	{
		//mTimeout.AddTimeoutFunction(bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks");
		Initialise("GUITest","data/GUITest.config");
		
		shared_ptr<FunctionBinder> functionBinder(new FunctionBinder());

		mScene.reset(new Scene());

		shared_ptr<Camera> camera = mScene->CreateCamera();
		camera->SetPosition(0,0,5);
		camera->SetProjectionType(ProjectionTypes::ORTHOGRAPHIC);
		camera->SetOrthoWindowWidth(5);
		shared_ptr<Renderer> renderer= GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1.0,1),camera);
		
		AddTask(GetRenderer().get());
		AddTask(mTimeout);
		AddTask(*mScene);

		mScreen.reset(new Screen());
		mScene->AddRenderable(mScreen);
		
		mContainer.reset(new Container());
		mContainer->SetWidth(3.f);
		mContainer->SetHeight(2.f);
		mContainer->SetRenderAABB(true);
		
		// We would normally make the screen the root element but this will test the absolute positioning of
		// the root element outside of a screen. The screen will be used to render the cursor.
		mScene->AddRenderable(mContainer);

		//Create an image element.
		shared_ptr<Image> image(new Image(GetMaterialManager()->GetResource("Echo")));
		image->SetRelativePosition(0,0.5,0);
		image->SetSize(0.5,0.5);
		image->SetSizeMode(Element::SizeModes::HEIGHT_WITH_SET_ASPECT_RATIO);
		mContainer->AddElement(image);

		shared_ptr<Button> button(new Button(GetMaterialManager()->GetResource("ButtonIdle")));
		button->SetRelativePosition(1,0.5,0);
		button->SetHover(GetMaterialManager()->GetResource("ButtonHover"));
		button->SetPressed(GetMaterialManager()->GetResource("ButtonPressed"));
		button->SetReleased(GetMaterialManager()->GetResource("ButtonReleased"));
		button->SetSizeMode(Element::SizeModes::WIDTH_WITH_SET_ASPECT_RATIO);
		button->SetFunctionBinder(functionBinder);
		RenderPass* pass = GetMaterialManager()->GetResource("Echo")->GetPass(0);
		if(pass)
		{
			pass->SetVertexColouringEnabled(false);
			functionBinder->RegisterVoid("SetRed",bind(&RenderPass::SetDiffuse,pass,Colours::RED));
			functionBinder->RegisterVoid("SetWhite",bind(&RenderPass::SetDiffuse,pass,Colours::WHITE));
			button->AddPressedFunction("SetRed()");
			button->AddReleasedFunction("SetWhite()");
		}
		mContainer->AddElement(button);
		mScene->AddRenderable(button);

		shared_ptr<Image> image2(new Image(GetMaterialManager()->GetResource("Echo")));
		image2->SetRelativePosition(0.5,0.5,0);
		image2->SetSize(1.0,0.5);
		image2->SetAspectRatio(2.f);
		image2->SetSizeMode(Element::SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO);
		image2->SetColour(Colour(1,1,1,0.5));
		image2->SetRenderAABB(true);
		mContainer->AddElement(image2);

		shared_ptr<Image> image2b(new Image(GetMaterialManager()->GetResource("Echo")));
		image2b->SetRelativePosition(0.5,0.5,0);
		image2b->SetSize(0.5,1.0);
		image2b->SetAspectRatio(0.5f);
		image2b->SetSizeMode(Element::SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO);
		image2b->SetColour(Colour(1,1,1,0.5));
		image2b->SetRenderAABB(true);
		mContainer->AddElement(image2b);
		
		shared_ptr<Text> text(new Text("Echo", GetFontManager()));
		text->SetFont("TestFont");
		text->SetRelativePosition(0.5,0.5,1);
		text->SetSize(0.75,0.25);
		text->SetAspectRatio(3.f);
		text->SetRenderAABB(true);
		text->SetSizeMode(Element::SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO);
		mContainer->AddElement(text);
		
		functionBinder->RegisterVoid("ChangeEchoFontToTest",bind(&Text::SetFont,text,"TestFont",0.));
		functionBinder->RegisterVoid("ChangeEchoFontToDefault",bind(&Text::SetFont,text,"NotoSans",0.));

		text->SetFunctionBinder(functionBinder);
		text->AddConditionalFunction("root.aspect_ratio <= 4/3", "ChangeEchoFontToTest()");
		text->AddConditionalFunction("root.aspect_ratio > 4/3", "ChangeEchoFontToDefault()");
		
		shared_ptr<Image> image3(new Image(GetMaterialManager()->GetResource("Echo")));
		image3->SetRelativePosition(0,1,0);
		image3->SetSize(0.25,0.25);
		image3->SetColour(Colours::BLUE_BELL);
		image3->SetSizeMode(Element::SizeModes::STRETCH);
		mContainer->AddElement(image3);
		
		functionBinder->RegisterVoid("SetContainerRed",bind(&SceneEntity::SetColour,mContainer,Colours::RED));
		functionBinder->RegisterVoid("SetContainerBlue",bind(&SceneEntity::SetColour,mContainer,Colours::BLUE));
		mContainer->SetFunctionBinder(functionBinder);

		//This container will be resized, lets add some conditions to it.
		mContainer->AddConditionalFunction("(aspect_ratio*10)%2 == 0","SetContainerRed()");
		mContainer->AddConditionalFunction("(aspect_ratio*10)%2 != 0","SetContainerBlue()");

		shared_ptr<Image> imagePhysical(new Image(GetMaterialManager()->GetResource("Echo")));
		imagePhysical->SetRelativePosition(0.5,0.2,2);
		imagePhysical->SetPhysicalWidth(Millimetre(20));
		imagePhysical->SetPhysicalHeight(Millimetre(20));
		mScreen->AddElement(imagePhysical);
		
		shared_ptr<Image> cursor(new Image(GetMaterialManager()->GetResource("Cursor")));
		cursor->SetSize(0.5,0.5);
		cursor->SetSizeMode(Element::SizeModes::STRETCH);
		cursor->SetRelativePosition(0,0,4);
		mScreen->AddElement(cursor);

		mSpinner.reset(new Spinner(bind(&Element::SetHeight,mContainer,placeholders::_1),4.f,0.1f,0.f));
		AddTask(mSpinner.get());
		
		mCursor.reset(new Cursor(	GetInputManager()->GetInput<bool>("Mouse:Left"),
									GetInputManager()->GetInput<f32>("Mouse:X"),
									GetInputManager()->GetInput<f32>("Mouse:Y"),
									mContainer,renderer,cursor));
		AddTask(*mCursor);
		
		shared_ptr<TextBox> textBox(new TextBox("TextBox",GetFontManager()));
		textBox->SetFont("NotoSans",14);
		textBox->SetRelativePosition(0.5,0.25,2);
		textBox->SetSize(0.5,0.25);
		functionBinder->RegisterVoid("SetContainerBlue",bind(&SceneEntity::SetColour,mContainer,Colours::BLUE));
		mScreen->AddElement(textBox);
		
		shared_ptr<StringSetter> stringSetter(new StringSetter("Materials define how a mesh is going to be rendered. Materials are more than just a texture that is applied to the mesh. It also defines other things such as how the mesh will interact with the existing scene, whether it will be affected by lighting, the objects base colour, and a variety of other settings.", StringSetter::Increments::WORD, 10, bind(&TextBox::SetText,textBox,placeholders::_1)));
		AddTask(stringSetter);
		shared_ptr<Spinner> textBoxSpinner(new Spinner(bind(&Element::SetWidth,textBox,placeholders::_1),1.f,0.25f,0.f));
		AddTask(textBoxSpinner);
		
		//Load in an gui file
		mGUI.reset(new Echo::GUI::GUI(GetFileSystem(),GetMaterialManager(),GetFontManager()));
		mGUI->LoadScreenGUI("data/Interface.gui",renderer,*mScene,*GetInputManager());
	}
private:
	shared_ptr< Echo::GUI::GUI > mGUI;
	shared_ptr< Container > mContainer;
	shared_ptr< Screen > mScreen;
	shared_ptr< Scene > mScene;
	shared_ptr<Spinner> mSpinner;
	CountDownTimer mTimeout;
	shared_ptr<Cursor> mCursor;
};

TEST_CASE("GUI")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	GUITest guiTest;
	guiTest.Execute();
}
