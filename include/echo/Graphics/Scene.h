#ifndef _ECHOSCENE_H_
#define _ECHOSCENE_H_

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/Renderable.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/PickResult.h>
#include <echo/cpp/functional>
#include <boost/foreach.hpp>
#include <map>
#include <vector>

namespace Echo
{
	class Camera;
	class Light;
	class RenderTarget;
	class Matrix4;
	class Ray;
	class PickResult;
	
	class Scene : public TaskGroup, public SceneRenderableVisitor
	{
	public:
		typedef std::pair< f32, SceneRenderable* > DistanceRenderablePair;
		typedef function<void(SceneRenderable&, const Camera*, std::vector< DistanceRenderablePair >&)> DistanceFunction;
		
		Scene();
		Scene(const std::string& name);
		virtual ~Scene();

		/**
		 * Creates a camera for the Scene.
		 * @note This method creates a camera with a number as the name. It is a pretty naive implementation and there may be
		 * collisions. You are better to use the named version of this method.
		 * @return new Camera or nullptr if auto naming failed
		 */
		shared_ptr<Camera> CreateCamera();

		/**
		 * Creates a named camera for the Scene.
		 * @param name Camera name
		 * @return The new camera or nullptr if a camera with the name already exists
		 */
		shared_ptr<Camera> CreateCamera(const std::string& name);

		/**
		 * Get a camera by name
		 * @param name Camera name
		 * @return The camera or nullptr if the camera does not exist
		 */
		shared_ptr<Camera> GetCamera(const std::string& name);

		/**
		 * Remove a camera from the scene.
		 * @note This does not necessarily destroy the camera. If the camera is references somewhere else, such as a renderer then
		 * the camera may still render this Scene. This method just removes the internal reference to the camera.
		 * @param name Camera name
		 * @return true if the camera was removed, false if the camera didn't exist
		 */
		bool RemoveCamera(const std::string& name);

		/**
		 * Create a light for the scene
		 * @note This method creates a light with a number as the name. It is a pretty naive implementation and there may be
		 * collisions. You are better to use the named version of this method.
		 * @return new Light or nullptr if auto naming failed
		 */
		shared_ptr<Light> CreateLight();

		/**
		 * Create a named light for the scene
		 * @param name Light name
		 * @return new light or nullptr if the light already exists
		 */
		shared_ptr<Light> CreateLight(const std::string& name);

		/**
		 * Add an existing light to the scene.
		 * The light can be accessed later using GetLight()
		 * @return true if the light was added to the scene, false if a light with the same name already exists
		 */
		bool AddLight(shared_ptr<Light> light);

		/**
		 * Get a light in the scene
		 * @param name Light name
		 * @return the light, or nullptr if the light does not exist.
		 */
		shared_ptr<Light> GetLight(const std::string& name);

		/**
		 * Remove a light for the Scene
		 * @param name Light name
		 * @return true if the light was removed, false if the light didn't exist
		 */
		bool RemoveLight(const std::string& name);
		
		/**
		 * Add a renderable to the scene
		 * @note Anything that is to render needs to be added to a scene. If a SceneEntity is added, its children will also need
		 * to be added separately.
		 * @param sceneRenderable The renderable to add
		 * @param pickable determines if this renderable is pickable with Scene::Pick()
		 */
		void AddRenderable(shared_ptr< SceneRenderable > sceneRenderable, bool pickable = true);

		/**
		 * Remove a renderable from a scene.
		 */
		void RemoveRenderable(shared_ptr<SceneRenderable> sceneRenderable);
		
		/**
		 * Pick the closest object from the scene that are visible by the given camera and intersect with the ray.
		 * @note The scene axis aligned bounding boxes are used for the intersect test.
		 * @note This method may be expensive depending on the number of scene objects you have. You may need to consider
		 * other alternatives to Pick() if you want to pick items frequently (multiple times a frame).
		 * @param camera Camera that objects should be visible for.
		 * @param ray The ray to test for intersections on.
		 * @return A null pointer if no object intersects by the ray and is visible, otherwise the closest object to the camera.
		 */
		optional<PickResult> Pick(const Camera& camera, const Ray& ray);

		/**
		 * Build the render queue.
		 */
		void BuildRenderQueue(const Camera& camera);
		void SceneRenderableVisit(SceneRenderable& renderable) override;
		
		/**
		 * Override from SceneRenderableVisitor.
		 * @return If performing a Render operation, the current camera used for rendering the scene, otherwise null.
		 */
		virtual const Camera* GetCurrentCamera() override;
		/**
		 * Override from SceneRenderableVisitor
		 * @return If performing a Render operation the current render target used for rendering the scene, otherwise null.
		 */
		virtual const RenderTarget* GetCurrentRenderTarget() override;

		void Render(RenderTarget& renderTarget, const Camera& camera);
		
		void SetSkyBox(shared_ptr< SceneRenderable > skyBox);
		
		void SetUseOnlyZForDistanceCalculations(bool zOnly);
		
		/**
		 * Search for a renderable with the given name.
		 * @param name The name of the renderable you want to find.
		 * @param deepSearch if true and a matching renderable cannot be found at the Scene's top level then each
		 * renderable will be searched in an attempt to find a renderable.
		 */
		shared_ptr<SceneRenderable> FindRenderable(const std::string& name, bool deepSearch);

		/**
		 * Search for a renderable of a specific type.
		 * Often you only want to work with a specific type. This method saves you the trouble of casting.
		 * @see FindRenderable.
		 */
		template< class T >
		shared_ptr<T> FindRenderable(const std::string& name, bool deepSearch)
		{
			BOOST_FOREACH(shared_ptr< SceneRenderable >& renderable, mRenderables)
			{
				if(renderable->GetName()==name)
				{
					shared_ptr<T> ptr = dynamic_pointer_cast<T>(renderable);
					if(ptr)
					{
						return ptr;
					}
					//If the cast failed we'll keep searching just incase there is a different object with the same name.
				}
			}
			if(deepSearch)
			{
				BOOST_FOREACH(shared_ptr< SceneRenderable >& renderable, mRenderables)
				{
					shared_ptr<T> child = renderable->FindSharedChild<T>(name, deepSearch);
					if(child)
					{
						return child;
					}
				}
			}
			return shared_ptr<T>();
		}
	protected:
		std::vector< Light* > BuildLightList(const Node& node);

		void ApplyLights(RenderTarget& renderTarget, const Camera& camera);
		typedef std::pair< const std::string, shared_ptr< Light > > NamedLightPair;
		static bool DistanceCompare(const DistanceRenderablePair& a, const DistanceRenderablePair& b);
		void SceneAABBCentreDistanceCalculate(SceneRenderable& sceneRenderable, const Camera* camera, std::vector< DistanceRenderablePair >& renderQueue);
		void SceneAABBCentreZOnlyDistanceCalculate(SceneRenderable& renderable, const Camera* camera, std::vector< DistanceRenderablePair >& renderQueue);
		std::map< std::string, shared_ptr<Camera> > mCameras;
		std::map< std::string, shared_ptr<Light> > mLights;
		std::list< shared_ptr< SceneRenderable > > mRenderables;
		std::list< shared_ptr< SceneRenderable > > mPickableRenderables;
		std::vector< DistanceRenderablePair > mRenderQueue;
		shared_ptr< SceneRenderable > mSkyBox;
		DistanceFunction mDistanceFunction;
		const Camera* mCurrentCamera;				/// Camera set for operations over multiple methods.
		const RenderTarget* mCurrentRenderTarget;	/// RenderTarget set for operations over multiple methods.
	};
}
#endif
