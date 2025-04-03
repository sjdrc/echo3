#include <echo/Graphics/Scene.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Light.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/cpp/functional>

#include <iostream>
#include <string>

#include <sstream>

namespace Echo
{
	Scene::Scene() : TaskGroup("Scene"), mCurrentCamera(0), mCurrentRenderTarget(0)
	{
		SetUseOnlyZForDistanceCalculations(false);
	}

	Scene::Scene(const std::string& name) : TaskGroup(name)
	{
	}

	Scene::~Scene()
	{
	}
	
	void Scene::SetSkyBox(shared_ptr< SceneRenderable > skyBox)
	{
		mSkyBox=skyBox;
	}

	shared_ptr<Camera> Scene::CreateCamera()
	{
		std::stringstream name;
		name << mCameras.size();
		return CreateCamera(name.str());
	}

	shared_ptr<Camera> Scene::CreateCamera(const std::string& name)
	{
		if(mCameras.find(name)!=mCameras.end())
		{
			ECHO_LOG_ERROR("Camera named '" << name << "' already exists");
			return shared_ptr<Camera>();
		}
		shared_ptr<Camera> camera=shared_ptr<Camera>(new Camera(name, *this));
		mCameras[name]=camera;
		return camera;
	}

	shared_ptr<Camera> Scene::GetCamera(const std::string& name)
	{
		std::map< std::string, shared_ptr<Camera> >::iterator cit=mCameras.find(name);
		if(cit==mCameras.end())
		{
			ECHO_LOG_ERROR("Camera named '" << name << "' not found");
			return shared_ptr<Camera>();
		}
		return cit->second;
	}

	bool Scene::RemoveCamera(const std::string& name)
	{
		std::map< std::string, shared_ptr<Camera> >::iterator cit=mCameras.find(name);
		if(cit==mCameras.end())
		{
			return false;
		}
		mCameras.erase(cit);
		return true;
	}
	
	shared_ptr<Light> Scene::CreateLight()
	{
		std::stringstream name;
		name << mLights.size();
		return CreateLight(name.str());
	}

	shared_ptr<Light> Scene::CreateLight(const std::string& name)
	{
		if(mLights.find(name)!=mLights.end())
		{
			ECHO_LOG_ERROR("Light named '" << name << "' already exists");
			return shared_ptr<Light>();
		}
		shared_ptr<Light> light=shared_ptr<Light>(new Light(name));
		mLights[name]=light;
		return light;
	}

	bool Scene::AddLight(shared_ptr<Light> light)
	{
		if(mLights.find(light->GetName())!=mLights.end())
		{
			ECHO_LOG_ERROR("Light named '" << light->GetName() << "' already exists");
			return false;
		}
		mLights[light->GetName()]=light;
		return true;
	}

	shared_ptr<Light> Scene::GetLight(const std::string& name)
	{
		std::map< std::string, shared_ptr<Light> >::iterator it=mLights.find(name);
		if(it==mLights.end())
		{
			ECHO_LOG_ERROR("Light named '" << name << "' not found");
			return shared_ptr<Light>();
		}
		return it->second;
	}

	bool Scene::RemoveLight(const std::string& name)
	{
		std::map< std::string, shared_ptr<Light> >::iterator it=mLights.find(name);
		if(it==mLights.end())
		{
			return false;
		}
		mLights.erase(it);
		return true;
	}
	
	void Scene::AddRenderable(shared_ptr< SceneRenderable > sceneRenderable, bool pickable)
	{
		if(!sceneRenderable)
		{
			ECHO_LOG_ERROR("Cannot add a null renderable.");
			return;
		}
		
		mRenderables.push_back(sceneRenderable);

		if(pickable)
		{
			mPickableRenderables.push_back(sceneRenderable);
		}
	}
	
	void Scene::RemoveRenderable(shared_ptr<SceneRenderable> sceneRenderable)
	{
		mRenderables.remove(sceneRenderable);
		mPickableRenderables.remove(sceneRenderable);
	}
	
	optional< PickResult > Scene::Pick(const Camera& camera, const Ray& ray)
	{
		typedef std::pair< AxisAlignedBox, shared_ptr< SceneRenderable > > BoxRenderablePair;
		std::vector< BoxRenderablePair > visibleCameraBoxes;
		visibleCameraBoxes.reserve(mPickableRenderables.size());
		
		// Transform each SceneRenderable scene AABB into camera space.
		BOOST_FOREACH(shared_ptr< SceneRenderable >& renderable, mPickableRenderables)
		{
			AxisAlignedBox sceneAABB=renderable->GetSceneAxisAlignedBox();
			if (camera.IsVisible(sceneAABB))
			{
				visibleCameraBoxes.push_back(std::make_pair(sceneAABB, renderable));
			}
		}

		// Check if the ray intersects the visible objects' AABB. If so then check the
		// intersect distance and return the closest.
		shared_ptr<SceneRenderable> closestRenderable;
		f32 closestDistance = 0.f;
		BOOST_FOREACH(const BoxRenderablePair& boxRenderablePair, visibleCameraBoxes)
		{
			std::pair<bool, f32> intersection = ray.Intersects(boxRenderablePair.first);
			if(intersection.first)
			{
				if(!closestRenderable)
				{
					closestDistance = intersection.second;
					closestRenderable = boxRenderablePair.second;
				}else
				{
					if(intersection.second < closestDistance)
					{
						closestDistance = intersection.second;
						closestRenderable = boxRenderablePair.second;
					}
				}
			}
		}
		if(!closestRenderable)
		{
			return none;
		}

		PickResult result;
		result.mSceneRenderable = closestRenderable;
		result.mDistance = closestDistance;
		result.mIntersectPosition = ray.GetPoint(closestDistance);
		result.mRay = ray;
		return result;
	}

	void Scene::BuildRenderQueue(const Camera& camera)
	{
		mRenderQueue.resize(0);
		mCurrentCamera = &camera;

		BOOST_FOREACH(shared_ptr< SceneRenderable >& renderable, mRenderables)
		{
			renderable->Accept(*this);
		}

		std::sort(mRenderQueue.begin(),mRenderQueue.end(),DistanceCompare);
		mCurrentCamera = 0;
		mCurrentRenderTarget = 0;
	}
	
	void Scene::SceneRenderableVisit(SceneRenderable& renderable)
	{
		mDistanceFunction(renderable,mCurrentCamera,mRenderQueue);
	}

	const Camera* Scene::GetCurrentCamera()
	{
		return mCurrentCamera;
	}

	const RenderTarget* Scene::GetCurrentRenderTarget()
	{
		return mCurrentRenderTarget;
	}

	void Scene::Render(RenderTarget& renderTarget, const Camera& camera)
	{
		// There is room for optimisation here by caching the results so if you want
		// render the same scene twice you don't need to build it every time.
		mCurrentRenderTarget = &renderTarget;

		BuildRenderQueue(camera);
		renderTarget.SetModelViewMatrix(camera.GetViewMatrix());

		ApplyLights(renderTarget,camera);
		
		const Matrix4& viewMatrix = camera.GetViewMatrix();
		{
			std::vector< Light* > lights = BuildLightList(camera);
			RenderContext renderContext(renderTarget,
				viewMatrix,
				renderTarget.GetProjectionMatrix(),
				viewMatrix * renderTarget.GetProjectionMatrix(),
				camera,
				lights);

			if(mSkyBox)
			{
				mSkyBox->SetPosition(camera.GetPosition());
				mSkyBox->Render(renderContext, Colours::WHITE);
			}
		}

		//Need to sort the renderables.
		BOOST_REVERSE_FOREACH(DistanceRenderablePair& renderable, mRenderQueue)
		{
			std::vector< Light* > lights = BuildLightList(*renderable.second);
			RenderContext renderContext(renderTarget,
				viewMatrix,
				renderTarget.GetProjectionMatrix(),
				viewMatrix * renderTarget.GetProjectionMatrix(),
				camera,
				lights);

			renderable.second->Render(renderContext, Colours::WHITE);
		}
		
		BOOST_FOREACH(DistanceRenderablePair& renderable, mRenderQueue)
		{
			renderable.second->Leave(*this);
		}
		mCurrentRenderTarget = 0;
	}
	
	bool Scene::DistanceCompare(const DistanceRenderablePair& a, const DistanceRenderablePair& b)
	{
		return (a.first<b.first);
	}
	
	void Scene::SetUseOnlyZForDistanceCalculations(bool zOnly)
	{
		if(zOnly)
		{
			mDistanceFunction = bind(&Scene::SceneAABBCentreZOnlyDistanceCalculate, this, placeholders::_1, placeholders::_2, placeholders::_3);
		}else
		{
			mDistanceFunction = bind(&Scene::SceneAABBCentreDistanceCalculate, this, placeholders::_1, placeholders::_2, placeholders::_3);
		}
	}
	
	void Scene::SceneAABBCentreDistanceCalculate(SceneRenderable& renderable, const Camera* camera, std::vector< DistanceRenderablePair >& renderQueue)
	{
		// Transform each SceneRenderable scene AABB into camera space.
		AxisAlignedBox sceneAABB=renderable.GetSceneAxisAlignedBox();
		if (camera->IsVisible(sceneAABB))
		{
			f32 SquaredDistance = sceneAABB.GetCentre().DistanceSquared(camera->GetPosition());
			renderQueue.push_back(std::make_pair(SquaredDistance, &renderable));
		}
	}


	void Scene::SceneAABBCentreZOnlyDistanceCalculate(SceneRenderable& renderable, const Camera* camera, std::vector< DistanceRenderablePair >& renderQueue)
	{
		// Transform each SceneRenderable scene AABB into camera space.
		AxisAlignedBox sceneAABB=renderable.GetSceneAxisAlignedBox();
		if (camera->IsVisible(sceneAABB))
		{
			f32 distance = Maths::Abs(sceneAABB.GetMaximum().z - camera->GetPosition().z);
			renderQueue.push_back(std::make_pair(distance, &renderable));
		}
	}

	std::vector< Light* > Scene::BuildLightList(const Node& node)
	{
		std::vector< Light* > lights;
		lights.reserve(mLights.size());
		for(auto& lightPair : mLights)
		{
			lights.push_back(lightPair.second.get());
		}
		Vector3 position = node.GetDerivedPosition();
		std::sort(lights.begin(),lights.end(),[position](const Light* l, const Light* r)
		{
			auto distanceL = (l->GetDerivedPosition() - position).LengthSquared();
			auto distanceR = (r->GetDerivedPosition() - position).LengthSquared();
			return distanceL < distanceR;
		});
		return lights;
	}
	
	void Scene::ApplyLights(RenderTarget& renderTarget, const Camera& camera)
	{
		u32 lightNumber = 0;
		BOOST_FOREACH(NamedLightPair& lightPair, mLights)
		{
			shared_ptr< Light >& light=lightPair.second;
			renderTarget.SetLight(lightNumber,light.get());
			++lightNumber;
		}
	}

	shared_ptr<SceneRenderable> Scene::FindRenderable(const std::string& name, bool deepSearch)
	{
		return FindRenderable<SceneRenderable>(name, deepSearch);
	}
}
