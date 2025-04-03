#include <echo/Graphics/Scene.h>
#include <echo/GUI/Screen.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/Camera.h>
#include <echo/Maths/Length.h>

namespace Echo
{
	namespace GUI
	{
		Screen::Screen() : mUpdateSizeToCamera(true)
		{
		}
		
		Screen::~Screen()
		{
			
		}

		Screen::Screen(const Screen& other)
		{
			*this = other;
		}
		
		Screen& Screen::operator=(const Screen& rhs)
		{
			Container::operator=(rhs);
			mUpdateSizeToCamera = rhs.mUpdateSizeToCamera;
			return *this;
		}

		shared_ptr<Element> Screen::_Clone() const
		{
			return shared_ptr<Element>(new Screen(*this));
		}
		
		void Screen::Accept(SceneRenderableVisitor& visitor)
		{
			if(mUpdateSizeToCamera)
			{
				const Camera* camera = visitor.GetCurrentCamera();
		
				if(camera)
				{
					Vector2 size;
					if(camera->GetProjectionType()==ProjectionTypes::PERSPECTIVE)
					{
						// Calculate the size width and height of the screen assuming the camera
						// is looking at it 90 degrees to the screen plane. The distance to the
						// screen can be used with the FOV to calculate the size of the plane in
						// view, i.e. the screen.
						Scalar distance = GetDerivedPosition().Distance(camera->GetPosition());
						Radian halfFOVx = camera->GetFOV() / 2.f;
						Radian halfFOVy = camera->GetFOVy() / 2.f;
						Scalar width = (Maths::Tan(halfFOVx.ValueRadians()) * distance) * 2.f;
						Scalar height = (Maths::Tan(halfFOVy.ValueRadians()) * distance) * 2.f;

						if(const RenderTarget* renderTarget = visitor.GetCurrentRenderTarget())
						{
							CalculateAndSetWorldUnits(renderTarget,Vector2(width,height));
						}

						size=Vector2(width,height);
					}else
					{
						size = Vector2(camera->GetOrthoWindowWidth(),camera->GetOrthoWindowHeight());
					}
					
					if(const RenderTarget* renderTarget = visitor.GetCurrentRenderTarget())
					{
						CalculateAndSetWorldUnits(renderTarget,size);
					}
					//The SetSize() function takes care of setting to the same size (i.e not causing an update)
					SetSize(size.x,size.y);
				}
			}
			SceneEntity::Accept(visitor);
		}
		
		void Screen::CalculateAndSetWorldUnits(const RenderTarget* renderTarget, Vector2 worldResolution)
		{
			//We need
			// RenderTarget X and Y resolution (pixels))
			// RenderTarget DPI (pixels per inch)
			// Viewport (percent of RenderTarget)
			// ViewportResolution (pixels))
			// Camera Near Frustum Plane Resolution (world units)
			Scalar xDPI = renderTarget->GetDisplayXDPI();
			Scalar yDPI = renderTarget->GetDisplayYDPI();
			if(xDPI==0.f || yDPI==0.f)
			{
				return;
			}
			Scalar xResolution = renderTarget->GetWidth();
			Scalar yResolution = renderTarget->GetHeight();
			Viewport* viewport = renderTarget->GetCurrentViewport();
			Viewport::Rectangle vr = viewport->GetRectangle(renderTarget->GetAspectRatio());

			//Viewport Width is in DPI. We need to find the ratio between the world space (X) and the display space.
			Metre metresPerPixelX(Inch(1 / xDPI));
			Scalar viewportWidth = vr.GetWidth() * xResolution;
			Scalar unitsPerPixelX = worldResolution.x / viewportWidth;
			Metre viewportPhysicalWidth(metresPerPixelX.Count() * viewportWidth);
			Scalar unitsPerMetreX = worldResolution.x / viewportPhysicalWidth.Count();

			//Viewport Height is in DPI. We need to find the ratio between the world space (y) and the display space.
			Metre metresPerPixelY(Inch(1 / yDPI));
			Scalar viewportHeight = vr.GetHeight() * yResolution;
			Scalar unitsPerPixelY = worldResolution.y / viewportHeight;
			Metre viewportPhysicalHeight(metresPerPixelY.Count() * viewportHeight);
			Scalar unitsPerMetreY = worldResolution.y / viewportPhysicalHeight.Count();
			
			SetWorldToMetreRatio(unitsPerMetreX,unitsPerMetreY);
			SetWorldToPixelRatio(unitsPerPixelX,unitsPerPixelY);
		}

		Scalar Screen::GetNativeAspectRatio()
		{
			// This is a bit of an odd request because the size is always adjusted via the Accept method.
			// We'll just return 1. because this shouldn't really be a thing.
			// If the size mode is set to use this method then size is discarded anyhow so it isn't a problem.
			// We don't bother sending the calculated screen aspect ratio because the method isn't public so
			// it shouldn't be outside of its intended use.
			return 1.f;
		}
	}
}
