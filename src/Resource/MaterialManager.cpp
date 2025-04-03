#include <echo/FileSystem/FileSystem.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Graphics/CubeMapTexture.h>
#include <echo/Util/StringUtils.h>
#include <echo/Util/Capnp.h>
#include <echo/Resource/ShaderManager.h>
#include <echo/Graphics/ShaderProgram.h>
#include <echo/Maths/Vector2.h>
#include <echo/Util/Capnp.h>

namespace Echo
{
	MaterialManager::MaterialManager(FileSystem& fileSystem, TextureManager& textureManager,
									ShaderManager& geometryShaderManager,
									ShaderManager& vertexShaderManager,
									ShaderManager& fragmentShaderManager) :
		ResourceManager<Material>("material"),
		mFileSystem(fileSystem),
		mTextureManager(textureManager),
		mGeometryShaderManager(geometryShaderManager),
		mVertexShaderManager(vertexShaderManager),
		mFragmentShaderManager(fragmentShaderManager)
	{
	}

	MaterialManager::~MaterialManager()
	{
	}

	template< typename T>
	bool SetUniformVariable(ShaderProgram& shaderProgram, const std::string& name, const std::string& typeString, const std::string& valueString)
	{
		// The version of SetUniformVariable() that takes a string as the value
		// will attempt to convert it to the specified type. We perform this for
		// each type we want to convert. We pass false into the Verify function
		// to require explicit constructor strings.
		std::string params = valueString;
		if(	Utils::String::VerifyConstructorAndExtractParameters(params,typeString,false) &&
			shaderProgram.SetUniformVariable<T>(name,params,true))
		{
			auto v = shaderProgram.GetUniformVariable<T>(name,false);
			return true;
		}
		return false;
	}

	shared_ptr<Material> MaterialManager::LoadResource(const std::string& resourceFileName, const std::string& resourceName)
	{
		std::string wholeFile;
		File file = mFileSystem.Open(resourceFileName);
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open \"" << resourceFileName << "\"");
			return shared_ptr<Material>();
		}
		file.ReadFileIntoString(wholeFile);

		try
		{
			shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Material>(wholeFile);
			if(!message)
			{
				ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFileName << "\"");
				return nullptr;
			}
			Resources::Material::Reader materialData = message->GetReader<Resources::Material>();

			shared_ptr<Material> material(new Material());
			ParseMaterial(materialData,resourceName,resourceFileName,*material);
			return material;
		}catch(...)
		{
		}
		ECHO_LOG_ERROR("Failed to load material resource " << resourceFileName << " as " << resourceName);
		return nullptr;
	}

	bool MaterialManager::LoadIntoResource(const std::string& resourceName, Material& materialToLoadInto)
	{
		//We need to find the resource file name.
		std::string resourceFileName = GetResourceFileName(resourceName);
		if(resourceFileName.empty())
		{
			return false;
		}

		std::string wholeFile;
		File file = mFileSystem.Open(resourceFileName);
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open \"" << resourceFileName << "\"");
			return false;
		}
		file.ReadFileIntoString(wholeFile);
		try
		{
			shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Material>(wholeFile);
			if(!message)
			{
				ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFileName << "\"");
				return false;
			}
			Resources::Material::Reader materialData = message->GetReader<Resources::Material>();
			ParseMaterial(materialData,resourceName,resourceFileName,materialToLoadInto);
			return true;
		}catch(...)
		{
		}
		ECHO_LOG_ERROR("Failed to load font resource " << resourceFileName << " as " << resourceName);
		return false;
	}

	bool MaterialManager::ProcessListEntry(const std::string& key, const std::string& resourceFileName, const std::vector<std::string>& params)
	{
		if(key=="material.bundle")
		{
			std::string wholeFile;
			File file = mFileSystem.Open(resourceFileName);
			if(!file.IsOpen())
			{
				ECHO_LOG_ERROR("Could not open \"" << resourceFileName << "\"");
				return false;
			}
			file.ReadFileIntoString(wholeFile);
			try
			{
				shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Materials>(wholeFile);
				if(!message)
				{
					ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFileName << "\"");
					return false;
				}
				Resources::Materials::Reader materialBundleData = message->GetReader<Resources::Materials>();
				if(!materialBundleData.hasMaterial())
				{
					ECHO_LOG_WARNING("Material bundle file \"" << resourceFileName << "\" does not list any materials.");
					return true;
				}
				auto materials = materialBundleData.getMaterial();
				for(int i = 0; i < materials.size(); ++i)
				{
					// Since there isn't a resource name we'll use: resourceFile:i this becomes the lookup name
					// if the material doesn't have a name.
					std::stringstream ss; ss << resourceFileName << ":" << i;
					shared_ptr<Material> material(new Material());
					ParseMaterial(materials[i],ss.str(),resourceFileName, *material);
					AddResource(material->GetName(),material);
				}
				return true;
			}
			catch(...)
			{
				ECHO_LOG_ERROR("Failed to load fonts: " << resourceFileName);
				return false;
			}
		}
		//Any other keys are fine.
		return true;
	}
	
	void MaterialManager::ParseMaterial(Resources::Material::Reader md, const std::string& materialName, const std::string& materialFile, Material& material)
	{
		if(md.hasName())
		{
			material.SetName(md.getName());
		} else
		{
			material.SetName(materialName);
		}
		ECHO_LOG("Material","Material Name: " << material.GetName());

		if(md.hasPass())
		{
			auto passes = md.getPass();
			ECHO_LOG("Material","\t" << passes.size() << " passes");
			for(Size p = 0; p < passes.size(); ++p)
			{
				Resources::Material::Pass::Reader pd = passes[p];
				RenderPass pass;

				BlendMode defaultBlendMode = pass.GetBlendMode();
				pass.SetBlendMode(ConvertBlendMode(pd.getBlendMode(),defaultBlendMode));

				if(pd.getBlendMode()==Resources::Material::BlendMode::SUBTRACT ||
					pd.getBlendMode()==Resources::Material::BlendMode::MODULATE ||
					pd.getBlendMode()==Resources::Material::BlendMode::MODULATE_AND_BRIGHTEN ||
					pd.getBlendMode()==Resources::Material::BlendMode::MODULATE_AND_BRIGHTEN_MORE)
				{
					ECHO_LOG_WARNING("Unsupported blend mode specified. Defaulting to using " << defaultBlendMode);
					pass.SetBlendMode(defaultBlendMode);
				}

				pass.SetAlphaTestFunction(ConvertAlphatestFunction(pd.getAlphaTestFunction(),pass.GetAlphaTestFunction()));

				// The default is 0 in RenderPass and capnp objects default to 0
				pass.SetAlphaTestValue(pd.getAlphaTestValue());
				pass.SetDepthFunction(ConvertDepthFunction(pd.getDepthFunction(),pass.GetDepthFunction()));
				pass.SetCullMode(ConvertCullMode(pd.getCullMode(),pass.GetCullMode()));
				pass.SetLightingEnabled(pd.getLighting());
				pass.SetDepthWriteEnabled(pd.getDepthWrite());
				pass.SetDepthCheckEnabled(pd.getDepthCheck());
				pass.SetVertexColouringEnabled(pd.getVertexColour());
				if(pd.hasAmbient())
				{
					pass.SetAmbient(ConvertColour(pd.getAmbient()));
				}

				if(pd.hasDiffuse())
				{
					pass.SetDiffuse(ConvertColour(pd.getDiffuse()));
				}

				if(pd.hasSpecular())
				{
					pass.SetSpecular(ConvertColour(pd.getSpecular()));
				}

				if(pd.hasEmissive())
				{
					pass.SetEmissive(ConvertColour(pd.getEmissive()));
				}

				f32 s = Maths::Clamp(pd.getShininess(),0.f,1.f);
				pass.SetShininess(s);
				pass.SetActive(pd.getActive());

				if(pd.hasTextureUnit())
				{
					auto textureUnits = pd.getTextureUnit();
					ECHO_LOG("Material","\t\t" << textureUnits.size() << " texture units");
					for(Size tu = 0; tu < textureUnits.size(); ++tu)
					{
						ECHO_LOG("Material","Texture Unit: " << tu);
						Resources::Material::TextureUnit::Reader tud = textureUnits[tu];
						TextureUnit textureUnit;
						textureUnit.SetBlendMode(ConvertBlendMode(tud.getBlendMode(),BlendModes::MODULATE));
						textureUnit.SetWrapModeU(ConvertTextureWrapMode(tud.getWrapU(),textureUnit.GetWrapModeU()));
						textureUnit.SetWrapModeV(ConvertTextureWrapMode(tud.getWrapV(),textureUnit.GetWrapModeV()));
						textureUnit.SetMinFilter(ConvertTextureFilter(tud.getMinFilter(),textureUnit.GetMinFilter()));
						textureUnit.SetMagFilter(ConvertTextureFilter(tud.getMagFilter(),textureUnit.GetMagFilter()));

						Colour manualColour;
						if(tud.hasManualColour())
						{
							manualColour = ConvertColour(tud.getManualColour());
							ECHO_LOG("Material","\t\t\tManual Colour: " << manualColour);
						}
						
						if(tud.hasAlphaBlend())
						{
							auto alphaBlend = tud.getAlphaBlend();
							LayerBlendModeEx blendModeEx = textureUnit.GetAlphaBlendMode();
							blendModeEx.blendType = LayerBlendTypes::ALPHA;
							blendModeEx.operation = ConvertLayerBlendOperationEx(alphaBlend.getOperation(),blendModeEx.operation);
							blendModeEx.source1 = ConvertLayerBlendSource(alphaBlend.getSource1(),blendModeEx.source1);
							blendModeEx.source2 = ConvertLayerBlendSource(alphaBlend.getSource2(),blendModeEx.source2);
							blendModeEx.colour = manualColour;
							textureUnit.SetAlphaBlendMode(blendModeEx);
						}

						if(tud.hasColourBlend())
						{
							auto colourBlend = tud.getColourBlend();
							LayerBlendModeEx blendModeEx = textureUnit.GetBlendMode();
							blendModeEx.blendType = LayerBlendTypes::COLOUR;
							blendModeEx.operation = ConvertLayerBlendOperationEx(colourBlend.getOperation(), blendModeEx.operation);
							blendModeEx.source1 = ConvertLayerBlendSource(colourBlend.getSource1(), blendModeEx.source1);
							blendModeEx.source2 = ConvertLayerBlendSource(colourBlend.getSource2(), blendModeEx.source2);
							blendModeEx.colour = manualColour;
							textureUnit.SetBlendMode(blendModeEx);
						}

						if(tud.hasImage())
						{
							ECHO_LOG("Material","\tImage: " << tud.getImage().cStr());
							shared_ptr<Texture> texture;
							texture = mTextureManager.GetResource( tud.getImage().cStr(), materialFile );
							
							if(texture)
							{
								textureUnit.SetTexture(texture);

								if(tud.hasTexGen())
								{
									auto texGen = tud.getTexGen();
									TextureUnit::TextureGenerationModeSet texGenMode = textureUnit.GetTexGenMode();
									//MaterialOptions::TextureGenerationMode::PlaneData* planeData;
									texGenMode.mMode = ConvertTextureGenerationMode(texGen.getMode(), texGenMode.mMode);
									
									if(texGen.hasQPlane())
									{
										texGenMode.mQPlaneData = ConvertPlaneData(texGen.getQPlane());
									}
									if(texGen.hasRPlane())
									{
										texGenMode.mRPlaneData = ConvertPlaneData(texGen.getRPlane());
									}
									if(texGen.hasSPlane())
									{
										texGenMode.mSPlaneData = ConvertPlaneData(texGen.getSPlane());
									}
									if(texGen.hasTPlane())
									{
										texGenMode.mTPlaneData = ConvertPlaneData(texGen.getTPlane());
									}
									textureUnit.SetTexGenMode(texGenMode);
								}
								
								textureUnit.SetUScrollSpeed(tud.getUScroll());
								textureUnit.SetVScrollSpeed(tud.getVScroll());
								textureUnit.SetUScaleSpeed(tud.getUScale());
								textureUnit.SetVScaleSpeed(tud.getVScale());
								textureUnit.SetRotationSpeed(Vector3(tud.getXRot(),tud.getYRot(),tud.getZRot()));
								textureUnit.SetTextureUnitSetToUse(tud.getTextureCoordinateSet());
							}
						}

						if(tud.hasCubemap())
						{
							auto cm = tud.getCubemap();
							shared_ptr<CubeMapTexture> cubeMap = make_shared<CubeMapTexture>(cm.getName().cStr(),mTextureManager);
							if(cm.hasFront())
							{
								cubeMap->SetFront(cm.getFront().cStr());
							}
							if(cm.hasBack())
							{
								cubeMap->SetBack(cm.getBack().cStr());
							}
							if(cm.hasLeft())
							{
								cubeMap->SetLeft(cm.getLeft().cStr());
							}
							if(cm.hasRight())
							{
								cubeMap->SetRight(cm.getRight().cStr());
							}
							if(cm.hasTop())
							{
								cubeMap->SetTop(cm.getTop().cStr());
							}
							if(cm.hasBottom())
							{
								cubeMap->SetBottom(cm.getBottom().cStr());
							}
							textureUnit.SetCubeMap(cubeMap);
						}				
						pass.AddTextureUnit(textureUnit);
					}
				}else
				{
					ECHO_LOG("Material","\t\tNo texture units");
				}
				pass.SetPointAndLineSize(pd.getPointAndLineSize());
				if(pd.hasProgram())
				{
					ECHO_LOG("Material","\t\tHas program");
					auto program = pd.getProgram();
					if(program.hasFragmentShader() || program.hasVertexShader() > 0)
					{
						ShaderProgram* shaderProgram = new ShaderProgram();

						if(program.hasGeometryShader())
						{
							auto geometryShaders = program.getGeometryShader();
							for(Size g = 0; g < geometryShaders.size(); ++g)
							{
								shared_ptr<Shader> shader = mGeometryShaderManager.GetResource(geometryShaders[g].cStr(), materialFile);
								if(shader)
								{
									shaderProgram->AddShader(shader);
								}
							}
						}
						if(program.hasVertexShader())
						{
							auto vertexShaders = program.getVertexShader();
							for(Size v = 0; v < vertexShaders.size(); ++v)
							{
								shared_ptr<Shader> shader = mVertexShaderManager.GetResource(vertexShaders[v], materialFile);
								if(shader)
								{
									shaderProgram->AddShader(shader);
								}
							}
						}

						if(program.hasFragmentShader())
						{
							auto fragmentShaders = program.getFragmentShader();
							for(Size f = 0; f < fragmentShaders.size(); ++f)
							{
								shared_ptr<Shader> shader = mFragmentShaderManager.GetResource(fragmentShaders[f], materialFile);
								if(shader)
								{
									shaderProgram->AddShader(shader);
								}
							}
						}

						shaderProgram->IncrementVersion();

						if(program.hasMatrixVariable())
						{
							pass.mProgramViewProjectionWorld = shaderProgram->GetUniformVariable<Matrix4>(program.getMatrixVariable().cStr(),true);
						}

						if(program.hasCameraPositionVariable())
						{
							pass.mProgramCameraPosition = shaderProgram->GetUniformVariable<Vector3>(program.getCameraPositionVariable().cStr(),true);
						}

						if(program.hasWorldMatrix())
						{
							pass.mProgramWorldMatrix = shaderProgram->GetUniformVariable<Matrix4>(program.getWorldMatrix().cStr(),true);
						}

						if(program.hasViewMatrix())
						{
							pass.mProgramViewMatrix = shaderProgram->GetUniformVariable<Matrix4>(program.getViewMatrix().cStr(),true);
						}

						if(program.hasProjectionMatrix())
						{
							pass.mProgramProjectionMatrix = shaderProgram->GetUniformVariable<Matrix4>(program.getProjectionMatrix().cStr(),true);
						}
						
						if(program.hasVariable())
						{
							auto variables = program.getVariable();
							for(Size v = 0; v < variables.size(); ++v)
							{
								std::string name;
								std::string valueString;
								if(Utils::String::SplitInTwo(variables[v].cStr()," ",name,valueString))
								{
									// "Sampler" isn't a type but it is used to distinguish the value as a reference to
									// a sampler by index rather than a float.
									if(SetUniformVariable<int>(*shaderProgram, name,"Sampler",valueString))
									{
										continue;
									}

									if(SetUniformVariable<Vector4>(*shaderProgram, name,"Vector4",valueString))
									{
										continue;
									}

									if(SetUniformVariable<Vector3>(*shaderProgram, name,"Vector3",valueString))
									{
										continue;
									}

									if(SetUniformVariable<Vector2>(*shaderProgram, name,"Vector2",valueString))
									{
										continue;
									}

									if(SetUniformVariable<Matrix4>(*shaderProgram, name,"Matrix4",valueString))
									{
										continue;
									}

									if(SetUniformVariable<Matrix3>(*shaderProgram, name,"Matrix3",valueString))
									{
										continue;
									}

									if(SetUniformVariable<int>(*shaderProgram, name,"int",valueString))
									{
										continue;
									}

									if(SetUniformVariable<bool>(*shaderProgram, name,"bool",valueString))
									{
										continue;
									}

									if(SetUniformVariable<float>(*shaderProgram, name,"float",valueString))
									{
										continue;
									}

									// Fallback to float
									if(	shaderProgram->SetUniformVariable<float>(name,valueString,true))
									{
										auto v = shaderProgram->GetUniformVariable<float>(name,false);
										continue;
									}
									
									ECHO_LOG_ERROR("Could not convert variable \"" << name << "\" with value: " << valueString);
								}else
								{
									ECHO_LOG_ERROR("Could not parse variable \"" << variables[v].cStr() << "\" Format needs to be \"name value\"");
								}
							}
						}

						pass.SetProgram(shared_ptr<ShaderProgram>(shaderProgram));
					}
				}
				material.AddPass(pass);
			}
		}
	}

	shared_ptr<Material> MaterialManager::CreateMaterial(const std::string& resourceName)
	{
		shared_ptr<Material> material(new Material());
		material->SetName(resourceName);
		ResourceManager<Material>::AddResource(resourceName, material);
		return ResourceManager<Material>::GetResource(resourceName);
	}

	shared_ptr<Material> MaterialManager::CreateMaterialUsingImage(const std::string& imageFileName)
	{
		shared_ptr<Texture> texture = mTextureManager.GetResource(imageFileName);
		if(!texture)
		{
			return shared_ptr<Material>();
		}
		shared_ptr<Material> material(new Material());
		material->SetToDefaultMaterial();
		TextureUnit textureUnit;
		textureUnit.SetTexture(texture);
		if(texture->HasAlpha())
		{
			//This might need changing when we address T93
			material->GetPass(0)->SetBlendMode(BlendModes::TRANSPARENT);
		}
		material->GetPass(0)->AddTextureUnit(textureUnit);
		ResourceManager<Material>::AddResource(imageFileName, material);
		return ResourceManager<Material>::GetResource(imageFileName);
	}

	FileSystem* MaterialManager::GetFileSystem() const
	{
		return &mFileSystem;
	}
	
	TextureUnit::TextureFilter MaterialManager::ConvertTextureFilter(Resources::Material::TextureFilter textureFilterData, TextureUnit::TextureFilter defaultTextureFilter)
	{
		switch(textureFilterData)
		{
			case Resources::Material::TextureFilter::NOT_SET:
				return defaultTextureFilter;
			case Resources::Material::TextureFilter::NEAREST:
				return TextureUnit::TextureFilters::NEAREST;
			default:
			case Resources::Material::TextureFilter::LINEAR:
				return TextureUnit::TextureFilters::LINEAR;
		}
		return defaultTextureFilter;
	}
	
	TextureUnit::TextureWrapMode MaterialManager::ConvertTextureWrapMode(Resources::Material::TextureWrapMode mode, TextureUnit::TextureWrapMode defaultTextureWrapMode)
	{
		switch(mode)
		{
			case Resources::Material::TextureWrapMode::NOT_SET:
				return defaultTextureWrapMode;
			case Resources::Material::TextureWrapMode::CLAMP:
				return TextureUnit::TextureWrapModes::CLAMP;
			case Resources::Material::TextureWrapMode::CLAMP_TO_EDGE:
				return TextureUnit::TextureWrapModes::CLAMP_TO_EDGE;
			default:
			case Resources::Material::TextureWrapMode::REPEAT:
				return TextureUnit::TextureWrapModes::REPEAT;
		}
		return defaultTextureWrapMode;
	}
	
	TextureUnit::TextureGenerationMode MaterialManager::ConvertTextureGenerationMode(Resources::Material::TextureGeneration::Mode mode, TextureUnit::TextureGenerationMode defaultTextureGenerationMode)
	{
		switch(mode)
		{
			case Resources::Material::TextureGeneration::Mode::NOT_SET:
				return defaultTextureGenerationMode;
			case Resources::Material::TextureGeneration::Mode::NONE:
				return TextureUnit::TextureGenerationModes::NONE;
			case Resources::Material::TextureGeneration::Mode::LINEAR:
				return TextureUnit::TextureGenerationModes::LINEAR;
			case Resources::Material::TextureGeneration::Mode::EYE_LINEAR:
				return TextureUnit::TextureGenerationModes::EYE_LINEAR;
			case Resources::Material::TextureGeneration::Mode::SPHERE:
				return TextureUnit::TextureGenerationModes::SPHERE;
			case Resources::Material::TextureGeneration::Mode::CUBE:
				return TextureUnit::TextureGenerationModes::CUBE;
		}
		return defaultTextureGenerationMode;
	}
	
	TextureUnit::TextureGenerationModeSet::PlaneData MaterialManager::ConvertPlaneData(Resources::Material::TextureGeneration::Plane::Reader p)
	{
		TextureUnit::TextureGenerationModeSet::PlaneData planeData;
		planeData.a = p.getA();
		planeData.b = p.getB();
		planeData.c = p.getC();
		planeData.d = p.getD();
		return planeData;
	}
	
	LayerBlendSource MaterialManager::ConvertLayerBlendSource(Resources::Material::Blend::BlendSource source, LayerBlendSource defaultLayerBlendSource)
	{
		switch(source)
		{
			case Resources::Material::Blend::BlendSource::NOT_SET:
				return defaultLayerBlendSource;
			case Resources::Material::Blend::BlendSource::CURRENT:
				return LayerBlendSources::CURRENT;
			case Resources::Material::Blend::BlendSource::TEXTURE:
				return LayerBlendSources::TEXTURE;
			case Resources::Material::Blend::BlendSource::DIFFUSE:
				return LayerBlendSources::DIFFUSE;
			case Resources::Material::Blend::BlendSource::SPECULAR:
				return LayerBlendSources::SPECULAR;
			case Resources::Material::Blend::BlendSource::MANUAL:
				return LayerBlendSources::MANUAL;
		}
		return defaultLayerBlendSource;
	}
	
	LayerBlendOperationEx MaterialManager::ConvertLayerBlendOperationEx(Resources::Material::Blend::BlendOperation operation, LayerBlendOperationEx defaultLayerBlendOperationEx)
	{
		switch(operation)
		{
			case Resources::Material::Blend::BlendOperation::NOT_SET:
				return defaultLayerBlendOperationEx;
			case Resources::Material::Blend::BlendOperation::ADD:
				return LayerBlendOperationExs::ADD;
			case Resources::Material::Blend::BlendOperation::MODULATE:
				return LayerBlendOperationExs::MODULATE;
			case Resources::Material::Blend::BlendOperation::MODULATE_X2:
				return LayerBlendOperationExs::MODULATE_X2;
			case Resources::Material::Blend::BlendOperation::MODULATE_X4:
				return LayerBlendOperationExs::MODULATE_X4;
			case Resources::Material::Blend::BlendOperation::BLEND_DIFFUSE_COLOUR:
				return LayerBlendOperationExs::BLEND_DIFFUSE_COLOUR;
			case Resources::Material::Blend::BlendOperation::ADD_SIGNED:
				return LayerBlendOperationExs::ADD_SIGNED;
			case Resources::Material::Blend::BlendOperation::ADD_SMOOTH:
				return LayerBlendOperationExs::ADD_SMOOTH;
			case Resources::Material::Blend::BlendOperation::SUBTRACT:
				return LayerBlendOperationExs::SUBTRACT;
			case Resources::Material::Blend::BlendOperation::SOURCE1:
				return LayerBlendOperationExs::SOURCE1;
			case Resources::Material::Blend::BlendOperation::SOURCE2:
				return LayerBlendOperationExs::SOURCE2;
			case Resources::Material::Blend::BlendOperation::BLEND_DIFFUSE_ALPHA:
				return LayerBlendOperationExs::BLEND_DIFFUSE_ALPHA;
			case Resources::Material::Blend::BlendOperation::BLEND_TEXTURE_ALPHA:
				return LayerBlendOperationExs::BLEND_TEXTURE_ALPHA;
			case Resources::Material::Blend::BlendOperation::BLEND_CURRENT_ALPHA:
				return LayerBlendOperationExs::BLEND_CURRENT_ALPHA;
			case Resources::Material::Blend::BlendOperation::MANUAL:
				return LayerBlendOperationExs::BLEND_MANUAL;
			case Resources::Material::Blend::BlendOperation::BLEND_DOT_PRODUCT:
				return LayerBlendOperationExs::DOTPRODUCT;
		}
		return defaultLayerBlendOperationEx;
	}
	
	Colour MaterialManager::ConvertColour(Resources::Material::Colour::Reader colour)
	{
		if(colour.hasS())
		{
			return Colour(colour.getS());
		}
		return Colour(colour.getR(),colour.getG(),colour.getB(),colour.getA());
	}

	BlendMode MaterialManager::ConvertBlendMode(Resources::Material::BlendMode mode, BlendMode defaultBlendMode)
	{
		switch(mode)
		{
			case Resources::Material::BlendMode::NOT_SET:
				return defaultBlendMode;
			case Resources::Material::BlendMode::NONE:
				return BlendModes::NONE;
			case Resources::Material::BlendMode::TRANSPARENT:
				return BlendModes::TRANSPARENT;
			case Resources::Material::BlendMode::ADDITIVE:
				return BlendModes::ADDITIVE;
			case Resources::Material::BlendMode::ADDITIVE_COLOUR:
				return BlendModes::ADDITIVE_COLOUR;
			case Resources::Material::BlendMode::MODULATE:
				return BlendModes::MODULATE;
			case Resources::Material::BlendMode::MODULATE_AND_BRIGHTEN:
				return BlendModes::MODULATE_AND_BRIGHTEN;
			case Resources::Material::BlendMode::MODULATE_AND_BRIGHTEN_MORE:
				return BlendModes::MODULATE_AND_BRIGHTEN_MORE;
			case Resources::Material::BlendMode::SUBTRACT:
				return BlendModes::SUBTRACT;
		}
		return defaultBlendMode;
	}
	
	RenderPass::DepthFunction MaterialManager::ConvertDepthFunction(Resources::Material::DepthFunction depthFunction, RenderPass::DepthFunction defaultDepthFunction)
	{
		switch(depthFunction)
		{
			case Resources::Material::DepthFunction::NOT_SET:
				return defaultDepthFunction;
			case Resources::Material::DepthFunction::NEVER:
				return RenderPass::DepthFunctions::NEVER;
			case Resources::Material::DepthFunction::LESS:
				return RenderPass::DepthFunctions::LESS;
			case Resources::Material::DepthFunction::EQUAL:
				return RenderPass::DepthFunctions::EQUAL;
			case Resources::Material::DepthFunction::LESS_OR_EQUAL:
				return RenderPass::DepthFunctions::LESS_OR_EQUAL;
			case Resources::Material::DepthFunction::GREATER:
				return RenderPass::DepthFunctions::GREATER;
			case Resources::Material::DepthFunction::NOT_EQUAL:
				return RenderPass::DepthFunctions::NOT_EQUAL;
			case Resources::Material::DepthFunction::GREATER_OR_EQUAL:
				return RenderPass::DepthFunctions::GREATER_OR_EQUAL;
			case Resources::Material::DepthFunction::ALWAYS:
				return RenderPass::DepthFunctions::ALWAYS;
		}
		return defaultDepthFunction;
	}
	
	RenderPass::CullMode MaterialManager::ConvertCullMode(Resources::Material::CullMode mode, RenderPass::CullMode defaultCullMode)
	{
		switch(mode)
		{
			case Resources::Material::CullMode::NOT_SET:
				return defaultCullMode;
			case Resources::Material::CullMode::NONE:
				return RenderPass::CullModes::NONE;
			case Resources::Material::CullMode::BACK:
				return RenderPass::CullModes::BACK;
			case Resources::Material::CullMode::FRONT:
				return RenderPass::CullModes::FRONT;
		}
		return defaultCullMode;
	}

	RenderPass::AlphaTestFunction MaterialManager::ConvertAlphatestFunction(Resources::Material::AlphaTestFunction alphaTestFunction, RenderPass::AlphaTestFunction defaultAlphaTestFunction)
	{
		switch(alphaTestFunction)
		{
			case Resources::Material::AlphaTestFunction::NOT_SET:
				return defaultAlphaTestFunction;
			case Resources::Material::AlphaTestFunction::NEVER:
				return RenderPass::AlphaTestFunctions::NEVER;
			case Resources::Material::AlphaTestFunction::LESS:
				return RenderPass::AlphaTestFunctions::LESS;
			case Resources::Material::AlphaTestFunction::EQUAL:
				return RenderPass::AlphaTestFunctions::EQUAL;
			case Resources::Material::AlphaTestFunction::LESS_OR_EQUAL:
				return RenderPass::AlphaTestFunctions::LESS_OR_EQUAL;
			case Resources::Material::AlphaTestFunction::GREATER:
				return RenderPass::AlphaTestFunctions::GREATER;
			case Resources::Material::AlphaTestFunction::NOT_EQUAL:
				return RenderPass::AlphaTestFunctions::NOT_EQUAL;
			case Resources::Material::AlphaTestFunction::GREATER_OR_EQUAL:
				return RenderPass::AlphaTestFunctions::GREATER_OR_EQUAL;
			case Resources::Material::AlphaTestFunction::ALWAYS:
				return RenderPass::AlphaTestFunctions::ALWAYS;
		}
		return defaultAlphaTestFunction;
	}
	
}
