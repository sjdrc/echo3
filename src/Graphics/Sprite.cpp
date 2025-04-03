#include <echo/Graphics/Sprite.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/Camera.h>

namespace Echo
{
	Sprite::Sprite(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
		: SceneEntity(position, orientation, scale),
			mAlwaysFaceCamera(true),
			mFrameHasTransform(false),
			mFlippedHorizontally(false),
			mFlippedVertically(false),
			mUseAnimationSize(true),
			mCurrentFrameTime(Seconds(0.)),
			mCurrentFrameIndex(0),
			mPixelToWorldUnitRatio(100.f),
			mCachedSizeInPixels(0,0),
			mAnimationMode(SpriteAnimation::Modes::NOT_SET)
	{
		CreateUnitQuad();
		SetMaterial(make_shared<Material>());
		AddChild(mNode);
	}
	
	Sprite::Sprite(shared_ptr<Material> material, const Vector3& position, const Quaternion& orientation, const Vector3& scale)
		: SceneEntity(position, orientation, scale),
			mAlwaysFaceCamera(true),
			mFrameHasTransform(false),
			mFlippedHorizontally(false),
			mFlippedVertically(false),
			mUseAnimationSize(true),
			mCurrentFrameTime(Seconds(0.)),
			mCurrentFrameIndex(0),
			mPixelToWorldUnitRatio(100.f),
			mCachedSizeInPixels(0,0),
			mAnimationMode(SpriteAnimation::Modes::NOT_SET)
	{
		CreateUnitQuad();
		SetMaterial(material);
		AddChild(mNode);
	}
	
	Sprite::Sprite(const Sprite& other)
		: SceneEntity(other),
			mAlwaysFaceCamera(other.mAlwaysFaceCamera),
			mFrameHasTransform(other.mFrameHasTransform),
			mFlippedHorizontally(other.mFlippedHorizontally),
			mFlippedVertically(other.mFlippedVertically),
			mUseAnimationSize(other.mUseAnimationSize),
			mCurrentFrameTime(other.mCurrentFrameTime),
			mCurrentFrameIndex(other.mCurrentFrameIndex),
			mPixelToWorldUnitRatio(other.mPixelToWorldUnitRatio),
			mCachedSizeInPixels(other.mCachedSizeInPixels),
			mAnimationMode(other.mAnimationMode)
	{
		AddChild(mNode);
	}
	
	Sprite::~Sprite(void)
	{
	}
	
	void Sprite::SetAlwaysFaceCamera(bool alwaysFaceCamera)
	{
		mAlwaysFaceCamera=alwaysFaceCamera;
		if(mAlwaysFaceCamera)
		{
			SetInitialOrientation(GetOrientation());
			SetInheritOrientation(false);
		}else
		{
			SetOrientation(GetInitialOrientation());
			SetInheritOrientation(true);
		}
	}
	
	void Sprite::CreateUnitQuad(void)
	{
		SetMesh(make_shared<Mesh>());
		GetMesh()->CreateQuadSubMesh();
		shared_ptr<Material> material(new Material());
		material->SetToDefaultMaterial();
		GetMesh()->SetMaterial(material);
	}
	
	void Sprite::Accept(SceneRenderableVisitor& visitor)
	{
		if(mAlwaysFaceCamera)
		{
			const Camera* camera = visitor.GetCurrentCamera();
			if(camera)
			{
				Quaternion cameraOrientation = camera->GetDerivedOrientation();
				//Set our orientation as the inverse of the camera's.
				SetOrientation(-cameraOrientation);
			}
		}
		SceneEntity::Accept(visitor);
	}
	
	void Sprite::SetAnimation(shared_ptr<SpriteAnimation> animation)
	{
		mAnimation=animation;
		mCurrentFrameTime=Seconds(0.);
		if(mAnimation)
		{
			size_t numberOfFrames = mAnimation->GetNumberOfFrames();
			if(numberOfFrames==0)
			{
				return;
			}
			ProcessFrame();
		}
	}

	void Sprite::Update(Seconds lastFrameTime)
	{
		if(!mAnimation)
		{
			return;
		}
		size_t numberOfFrames = mAnimation->GetNumberOfFrames();
		if(numberOfFrames==0)
		{
			return;
		}
		mCurrentFrameTime += lastFrameTime;
		if(mCurrentFrameTime >= mAnimation->GetFrame(mCurrentFrameIndex).frameTime())
		{
			mCurrentFrameTime = Seconds(0.);
			mCurrentFrameIndex++;
			if(mCurrentFrameIndex >= numberOfFrames)
			{
				ProcessAnimationEnd();
			}
			ProcessFrame();
		}
	}
	
	void Sprite::ProcessFrame()
	{
		//NOTE: This is an internal method that expects mAnimation to be valid.
		//		Do not call it if this is not the case.
		SpriteAnimation::Frame& frame = mAnimation->GetFrame(mCurrentFrameIndex);
		
		if(!frame.texture())
		{
			frame.texture(mAnimation->AcquireTexture(frame.resource()));
		}
		if(GetMesh())
		{
			if(frame.texture())
			{
				GetMesh()->GetSubMesh(0)->GetMaterial()->SetTexture(frame.texture());
			}
			if(mUseAnimationSize)
			{
				f32 width = (frame.width() != 0) ? frame.width() : mAnimation->mWidthInPixels;
				f32 height = (frame.height() != 0) ? frame.height() : mAnimation->mHeightInPixels;
				_SetSizeInPixels(width, height);
			}
			SetTextureCoordinates(frame.uv1(), frame.uv2());
		}
		mNode.SetPosition(frame.offset());
		mNode.SetScale(frame.scale());
		mNode.SetOrientation(frame.orientation());
		mFrameHasTransform = frame.HasTransform();
		mAnimation->ProcessFrameCallbacks(*this,mCurrentFrameIndex);
	}	
	
	bool Sprite::OnStart()
	{
		OnResume();	//To perform frame processing if an animation is present.
		return true;
	}
	
	void Sprite::OnResume(bool)
	{
		if(mAnimation)
		{
			if(mAnimation->GetNumberOfFrames()>0 || mCurrentFrameIndex >= mAnimation->GetNumberOfFrames())
			{
				return;
			}
			ProcessFrame();
		}
	}
	
	void Sprite::ProcessAnimationEnd()
	{
		//NOTE: This is an internal method that expects mCurrentFrameIndex and mAnimation to be valid.
		//		Do not call it if this is not the case.

		SpriteAnimation::Mode mode;
		if(mAnimationMode==SpriteAnimation::Modes::NOT_SET)
		{
			mode = mAnimation->GetMode();
		}else
		{
			mode = mAnimationMode;
		}
		mAnimation->ProcessEndFrameCallbacks(*this);
		switch(mode)
		{
			case SpriteAnimation::Modes::LOOP:
				mCurrentFrameIndex = 0;
				return;
			case SpriteAnimation::Modes::PLAY_ONCE_HIDE_AND_RESTART:
				Pause();
				//Fall through to restart
			case SpriteAnimation::Modes::PLAY_ONCE_AND_RESTART:
				mCurrentFrameIndex = 0;
				break;
			case SpriteAnimation::Modes::PLAY_ONCE_AND_HIDE:
				Pause();
				//Fall through to apply the PLAY_ONCE logic
			case SpriteAnimation::Modes::PLAY_ONCE:
				mCurrentFrameIndex=mAnimation->GetNumberOfFrames()-1;
			default:
				break;
		}
		//All modes except LOOP need to have their update paused.
		Pause();
	}

	void Sprite::SetSize(f32 width, f32 height)
	{
		_SetSizeInPixels(width*mPixelToWorldUnitRatio,height*mPixelToWorldUnitRatio);
	}

	void Sprite::SetSizeInPixels(f32 width, f32 height)
	{
		_SetSizeInPixels(width,height);
		mUseAnimationSize = false;
	}
	
	void Sprite::_SetSizeInPixels(f32 width, f32 height)
	{
		assert(GetMesh());
		assert(GetMesh()->GetSubMesh(0));
		
		mCachedSizeInPixels.x=width;
		mCachedSizeInPixels.y=height;
		shared_ptr<SubMesh> subMesh = GetMesh()->GetSubMesh(0);
		VertexBuffer::Accessor<Vector3> verts = subMesh->GetComponents<Vector3>("Position");
		f32 halfWidth = (width / 2) / mPixelToWorldUnitRatio;
		f32 halfHeight = (height / 2) / mPixelToWorldUnitRatio;
		verts[0].x = -halfWidth;
		verts[0].y = halfHeight;

		verts[1].x = halfWidth;
		verts[1].y = halfHeight;

		verts[2].x = -halfWidth;
		verts[2].y = -halfHeight;

		verts[3].x = halfWidth;
		verts[3].y = -halfHeight;
		subMesh->Finalise();
	}
	
	void Sprite::SetPixelToWorldUnitRatio(f32 ratio)
	{
		mPixelToWorldUnitRatio = ratio;
		if(mAnimation)
		{
			ProcessFrame();
		}
		_SetSizeInPixels(mCachedSizeInPixels.x,mCachedSizeInPixels.y);
	}

	void Sprite::SetTextureCoordinates(const TextureUV& uv1, const TextureUV& uv2)
	{
		assert(GetMesh());
		assert(GetMesh()->GetSubMesh(0));
		VertexBuffer::Accessor<TextureUV> uvs = GetMesh()->GetSubMesh(0)->GetComponents<TextureUV>("UV0");
		
		// U normal		U flipped
		// AB			BA
		// CD			DC

		// V normal		V flipped
		// AB			CD
		// CD			AB
		
		if(mFlippedHorizontally)
		{
			//Swap the horizontal coordinates
			uvs[1].u = uv1.u;
			uvs[0].u = uv2.u;
			uvs[3].u = uv1.u;
			uvs[2].u = uv2.u;
		}else
		{
			uvs[0].u = uv1.u;
			uvs[1].u = uv2.u;
			uvs[2].u = uv1.u;
			uvs[3].u = uv2.u;
		}
		
		if(mFlippedVertically)
		{
			//Swap the vertical coordinates
			uvs[2].v = uv1.v;
			uvs[3].v = uv1.v;
			uvs[0].v = uv2.v;
			uvs[1].v = uv2.v;
		}else
		{
			uvs[0].v = uv1.v;
			uvs[1].v = uv1.v;
			uvs[2].v = uv2.v;
			uvs[3].v = uv2.v;
		}
	}

	void Sprite::Render(RenderContext& renderContext, Colour compoundDiffuse)
	{
		if(mFrameHasTransform && GetMesh())
		{
			GetMesh()->Render(renderContext, mNode.GetTransform(), renderContext.mViewMatrix*mNode.GetTransform(), compoundDiffuse);
		}else
		{
			SceneEntity::Render(renderContext, compoundDiffuse);
		}
	}
	
	void Sprite::SetFlippedHorizontally(bool flippedHorizontally)
	{
		if(flippedHorizontally!=mFlippedHorizontally)
		{
			mFlippedHorizontally=flippedHorizontally;

			//Update the texture coordinates which will perform flipping as needed.
			if(mAnimation)
			{
				if(GetMesh() && mCurrentFrameIndex < mAnimation->GetNumberOfFrames())
				{
					SpriteAnimation::Frame& frame = mAnimation->GetFrame(mCurrentFrameIndex);
					SetTextureCoordinates(frame.uv1(), frame.uv2());
				}
			}
		}
	}

	void Sprite::SetFlippedVertically(bool flippedVertically)
	{
		if(flippedVertically!=mFlippedVertically)
		{
			mFlippedVertically=flippedVertically;

			//Update the texture coordinates which will perform flipping as needed.
			if(mAnimation)
			{
				if(GetMesh() && mCurrentFrameIndex < mAnimation->GetNumberOfFrames())
				{
					SpriteAnimation::Frame& frame = mAnimation->GetFrame(mCurrentFrameIndex);
					SetTextureCoordinates(frame.uv1(), frame.uv2());
				}
			}
		}
	}
}
