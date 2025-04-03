#ifndef _ECHOGUISCREEN_H_
#define _ECHOGUISCREEN_H_

#include <echo/GUI/Container.h>

namespace Echo
{
	class Scene;
	
	namespace GUI
	{
		/**
		 * Screen is a container object that will resize to fit the screen it is being rendered to.
		 */
		class Screen : public Container
		{
		public:
			Screen();
			virtual ~Screen();

			Screen(const Screen& other);
			Screen& operator=(const Screen& rhs);
			
			/**
			 * Set the flag that will make this element update its size to fit the camera's view.
			 * @param updateSizeToCamera true to cause the update.
			 */
			void SetUpdateSizeToCamera(bool updateSizeToCamera) {mUpdateSizeToCamera=updateSizeToCamera;}
			
		private:
			virtual shared_ptr<Element> _Clone() const override;
			
			virtual void Accept(SceneRenderableVisitor& visitor) override;
			virtual Scalar GetNativeAspectRatio() override;
			void CalculateAndSetWorldUnits(const RenderTarget* renderTarget, Vector2 worldResolution);
			bool mUpdateSizeToCamera;
		};
	}
}
#endif 
