#include <echo/Graphics/StereoscopicRenderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/RenderTarget.h>

namespace Echo
{
	StereoscopicRenderer::StereoscopicRenderer(shared_ptr<RenderTarget> renderTarget, std::string name, Scene& scene, u32 taskPriority) : MultiRenderer(name,taskPriority)
	{
		AddRenderTarget(renderTarget);
		mLeftEye = scene.CreateCamera("LeftEye");
		mRightEye = scene.CreateCamera("RightEye");
		mDistanceBetweenEyes = 0.f;
		mFocusSplit = 0.f;
		mLeftEye->SetNearPlane(0.05f);
		mLeftEye->SetFarPlane(100.f);
		mRightEye->SetNearPlane(0.05f);
		mRightEye->SetFarPlane(100.f);
		f32 renderTargetAspectRatio = static_cast<f32>(renderTarget->GetWidth()) / static_cast<f32>(renderTarget->GetHeight());
		mLeftEye->SetAspectRatio(renderTargetAspectRatio/2.f);
		mRightEye->SetAspectRatio(renderTargetAspectRatio/2.f);
		mLeftRenderer = CreateRenderer(make_shared<Viewport>(0.f,0.f,0.5f,1.f),mLeftEye,"LeftEye");
		mRightRenderer = CreateRenderer(make_shared<Viewport>(0.5f,0.f,1.f,1.f),mRightEye,"RightEye");
		mFocusNode = make_shared<Node>();
		mFocusNode->SetPosition(Vector3(0,0,-1.));

		mFocusNodeLeft = make_shared<Node>();
		mFocusNode->AddChild(*mFocusNodeLeft);

		mFocusNodeRight = make_shared<Node>();
		mFocusNode->AddChild(*mFocusNodeRight);

		mHeadNode.AddChild(*mLeftEye);
		mHeadNode.AddChild(*mRightEye);
		mHeadNode.AddChild(*mFocusNode);
		mLeftEye->SetAutoTracking(true,mFocusNodeLeft);
		mRightEye->SetAutoTracking(true,mFocusNodeRight);
		//Default eye distance for myself, measured with a ruler.
		SetEyeDistance(0.062f);
		mStereoOn = true;
	}

	StereoscopicRenderer::~StereoscopicRenderer()
	{

	}

	void StereoscopicRenderer::SetFocusDistance(f32 distanceToFocus)
	{
		Vector3 pos = mFocusNode->GetPosition();
		pos.z = -distanceToFocus;
		mFocusNode->SetPosition(pos);
	}

	f32 StereoscopicRenderer::GetFocusDistance()
	{
		return -mFocusNode->GetPosition().z;
	}

	void StereoscopicRenderer::SetEyeDistance(f32 distanceBetweenEyes)
	{
		mLeftEye->SetFrustumOffset(Vector2(-distanceBetweenEyes/2.f,0.f));
		mRightEye->SetFrustumOffset(Vector2(distanceBetweenEyes/2.f,0.f));
		mDistanceBetweenEyes = distanceBetweenEyes;
	}

	f32 StereoscopicRenderer::GetEyeDistance() const
	{
		return mDistanceBetweenEyes;
	}

	void StereoscopicRenderer::SetFocusSplit(f32 split)
	{
		mFocusSplit=split;
		mFocusNodeLeft->SetPosition(Vector3(-mFocusSplit/2.f,0,0));
		mFocusNodeRight->SetPosition(Vector3(mFocusSplit/2.f,0,0));
	}

	void StereoscopicRenderer::SetFOV(Radian fov)
	{
		mLeftEye->SetFOV(fov);
		mRightEye->SetFOV(fov);
	}

	Radian StereoscopicRenderer::GetFOV() const
	{
		return mLeftEye->GetFOV();
	}

	void StereoscopicRenderer::SetFOVy(Radian fov)
	{
		mLeftEye->SetFOVy(fov);
		mRightEye->SetFOVy(fov);
	}

	Radian StereoscopicRenderer::GetFOVy() const
	{
		return mLeftEye->GetFOVy();
	}

	void StereoscopicRenderer::SetStereo(bool on)
	{
		if(!mStereoOn && on)
		{
			mLeftRenderer->GetViewport()->SetFixed(0.f,0.f,0.5f,1.f);
			mRightRenderer->Resume();
			mLeftEye->SetAspectRatio(mLeftEye->GetAspectRatio()/2);
			SetEyeDistance(mDistanceBetweenEyes);
			mStereoOn = true;
		}else
		if(mStereoOn && !on)
		{
			mLeftRenderer->GetViewport()->SetFixed(0.f,0.f,1.f,1.f);
			mLeftEye->SetFrustumOffset(Vector2(0.f,0.f));
			mLeftEye->SetAspectRatio(mLeftEye->GetAspectRatio()*2);
			mRightRenderer->Pause();
			mStereoOn = false;
		}

	}
}
