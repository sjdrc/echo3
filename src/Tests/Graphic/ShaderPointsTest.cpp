#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Application.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/cpp/functional>

#include <echo/GUI/Cursor.h>
#include <echo/GUI/Screen.h>
#include <echo/Input/Input.h>

#include "SimpleControls.h"

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;


/**
 * Test that loads a colour image and single channel "depth" image and generates a point cloud.
 * Two TrackPad instances are used for the left and right mouse in combination with dragging for camera controls.
 */
class ShaderPoints : public Application
{
public:
	ShaderPoints()
	{
		Initialise("ShaderPoints","data/ShaderPointsTest.config");

		shared_ptr<Camera> camera = mScene.CreateCamera("Scene");
		camera->SetPosition(0,0,2);
		camera->LookAt(0,0,0);
		camera->SetNearPlane(0.1);
		camera->SetFarPlane(10);
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);

		camera->CalculateAndSetDistanceForYFOV(5,Vector3(0,0,0),Vector3(0,0,-1));
		AddTask(GetRenderer().get());

		AddTask(make_shared<Testing::SimpleControls>(camera,*this));
		//SetupControls(camera);
		
		shared_ptr<Mesh> mesh(new Mesh());
		shared_ptr<SubMesh> sm = mesh->CreateSubMesh();

		shared_ptr<Texture> colour = GetTextureManager()->GetResource("data/PointsRGB.png");
		shared_ptr<Texture> depth = GetTextureManager()->GetResource("data/PointsDepth.png");
		if(depth && colour)
		{
			shared_ptr<VertexBuffer> vertexBuffer = GeneratePointCloud(*depth,*colour,VertexBuffer::Types::STATIC);
			if(vertexBuffer)
			{
				sm->SetVertexBuffer(vertexBuffer);
				sm->SetElementBuffer(ElementBuffer::Types::DYNAMIC, ElementBuffer::IndexTypes::UNSIGNED_32BIT, ElementBuffer::ElementTypes::POINTS, vertexBuffer->GetNumberOfElements());

				shared_ptr<ElementBuffer> elementBuffer=sm->GetElementBuffer();
				auto points = elementBuffer->GetAccessor< ElementBuffer::Point<u32> >();
				for(Size i=0;i<vertexBuffer->GetNumberOfElements();++i)
				{
					points[i].mIndex = i;
				}
			}

			sm->SetMaterial(GetMaterialManager()->GetResource("ShaderPoints"));
			sm->Finalise();
			
			//Create a couple of Scene entities and set them up to use the cube meshes.
			mEntity = shared_ptr< SceneEntity >(new SceneEntity());
			mEntity->SetMesh(mesh);
			
			mScene.AddRenderable(mEntity);
		}

		AddTask(mScene);
	}


	shared_ptr<VertexBuffer> GeneratePointCloud(const Texture& depth, const Texture& colour, VertexBuffer::Type vertexBufferType)
	{
		const Texture::Accessor<u8> depthAccessor = const_cast<Texture&>(depth).GetAccessor<u8>();
		if(!depthAccessor)
		{
			ECHO_LOG_ERROR("Unable to get depth accessor.");
			return nullptr;
		}

		struct RGB
		{
			u8 mR;
			u8 mG;
			u8 mB;
		};

		const Texture::Accessor<RGB> rgbAccessor = const_cast<Texture&>(colour).GetAccessor<RGB>();
		if(!rgbAccessor)
		{
			ECHO_LOG_ERROR("Unable to get RGB accessor.");
			return nullptr;
		}

		if(depthAccessor.GetSize() != rgbAccessor.GetSize())
		{
			ECHO_LOG_ERROR("Depth and colour textures are not the same size depth:" << depthAccessor.GetSize() << " != colour:" << rgbAccessor.GetSize());
			return nullptr;
		}

		shared_ptr<VertexBuffer> buffer(new VertexBuffer(vertexBufferType));
		buffer->AddVertexAttribute("Position",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
		buffer->AddVertexAttribute("Colour",VertexAttribute(VertexAttribute::ComponentTypes::COLOUR_8,1));
		if(!buffer->Allocate(depthAccessor.GetSize()))
		{
			ECHO_LOG_ERROR("Unable to allocate VertexBuffer for point cloud generation.");
			return nullptr;
		}

		VertexBuffer::Accessor<Vector3> depthDest = buffer->GetAccessor<Vector3>("Position");
		if(!depthDest)
		{
			ECHO_LOG_ERROR("Unable to get depth destination accessor.");
			return nullptr;
		}
		VertexBuffer::Accessor<VertexColour> colourDest = buffer->GetAccessor<VertexColour>("Colour");
		if(!colourDest)
		{
			ECHO_LOG_ERROR("Unable to get colour destination accessor.");
			return nullptr;
		}

		Size numberOfValidPoints = 0;
		for (Size y = 0; y < depthAccessor.GetHeight(); ++y)
		{
			Size rowIndex = y * depthAccessor.GetWidth();
			for (Size x = 0; x < depthAccessor.GetWidth(); ++x)
			{
				Size index = rowIndex+x;
				u32 pixel = depthAccessor[index];
				// Skip any holes
				if (pixel == 0)
				{
					continue;
				}
				depthDest[numberOfValidPoints] = Vector3((static_cast<f32>(x)-1920.f*0.5f)/100.f,(static_cast<f32>(y)-1080.f*0.5f)/100.f,static_cast<f32>(pixel)/100.f);

				RGB colourPixelSrc = rgbAccessor[index];
				VertexColour colourPixel;
				colourPixel._RGBA.r = colourPixelSrc.mR;
				colourPixel._RGBA.g = colourPixelSrc.mG;
				colourPixel._RGBA.b = colourPixelSrc.mB;
				colourPixel._RGBA.a = 255;
				colourDest[numberOfValidPoints] = colourPixel;//Colours::Random();
				++numberOfValidPoints;
			}
		}
		ECHO_LOG_ERROR("numberOfValidPoints: " << numberOfValidPoints);
		buffer->SetNumberOfElements(numberOfValidPoints);
		buffer->IncrementVersion();
		return buffer;
	}
private:
	Scene mScene;
	shared_ptr< SceneEntity > mEntity;
};

TEST_CASE("ShaderPoints")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	ShaderPoints shaderPoints;
	shaderPoints.Execute();
}
