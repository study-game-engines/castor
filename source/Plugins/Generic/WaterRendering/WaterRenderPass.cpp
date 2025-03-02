#include "WaterRenderPass.hpp"

#include "WaterUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderQueue.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp>
#include <Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp>
#include <Castor3D/Render/Node/BillboardRenderNode.hpp>
#include <Castor3D/Render/Node/SubmeshRenderNode.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBackground.hpp>
#include <Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp>
#include <Castor3D/Shader/Shaders/GlslClusteredLights.hpp>
#include <Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslFog.hpp>
#include <Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/BillboardUbo.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>
#include <Castor3D/Shader/Ubos/MorphingUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

#include <ashespp/Image/StagingTexture.hpp>

#define Water_Debug 1

namespace water
{
	namespace
	{
		enum WaterIdx : uint32_t
		{
			eLightBuffer = uint32_t( castor3d::GlobalBuffersIdx::eCount ),
			eWaterUbo,
			eWaterNormals1,
			eWaterNormals2,
			eWaterNoise,
			eSceneNormals,
			eSceneDepth,
			eSceneResult,
			eBrdf,
		};

		void bindTexture( VkImageView view
			, VkSampler sampler
			, ashes::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			writes.push_back( castor3d::makeImageViewDescriptorWrite( view, sampler, index++ ) );
		}

		void loadImage( castor3d::Parameters const & params
			, castor::String const & prefix
			, castor::String const & name
			, castor3d::Engine & engine
			, crg::FrameGraph & graph
			, crg::RunnableGraph & runnable
			, castor::ImageLoader & loader
			, crg::ImageId & img
			, crg::ImageViewId & view
			, ashes::ImageView & result )
		{
			castor::Path folder;
			castor::Path relative;

			if ( params.get( prefix + "Folder", folder )
				&& params.get( prefix + "Relative", relative ) )
			{
				auto image = loader.load( relative, folder / relative, {} );
				auto format = image.getPixelFormat();

				if ( format == castor::PixelFormat::eR8G8B8_UNORM )
				{
					auto buffer = castor::PxBufferBase::create( image.getDimensions()
						, castor::PixelFormat::eR8G8B8A8_UNORM
						, image.getPxBuffer().getConstPtr()
						, image.getPxBuffer().getFormat()
						, image.getPxBuffer().getAlign() );
					image = castor::Image{ image.getName()
						, image.getPath()
						, *buffer };
				}

				auto dim = image.getDimensions();
				img = graph.createImage( crg::ImageData{ name + prefix
					, 0u
					, VK_IMAGE_TYPE_2D
					, VkFormat( format )
					, { dim.getWidth(), dim.getHeight(), 1u }
					, ( VK_IMAGE_USAGE_SAMPLED_BIT
						| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					, image.getLevels() } );

				view = graph.createView( crg::ImageViewData{ name + prefix + "Target"
					, img
					, 0u
					, VK_IMAGE_VIEW_TYPE_2D
					, img.data->info.format
					, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, image.getLevels(), 0u, 1u } } );

				engine.postEvent( castor3d::makeGpuFunctorEvent( castor3d::GpuEventType::ePreUpload
					, [format, dim, image, &img, &view, &result, &runnable]( castor3d::RenderDevice const & device
						, castor3d::QueueData const & queue )
					{
						auto buffer = image.getPixels();
						auto staging = device->createStagingTexture( VkFormat( format )
							, VkExtent2D{ dim.getWidth(), dim.getHeight() }
							, buffer->getLevels() );
						ashes::ImagePtr noiseImg = std::make_unique< ashes::Image >( *device
							, runnable.createImage( img )
							, img.data->info );
						result = ashes::ImageView{ ashes::ImageViewCreateInfo{ *noiseImg, view.data->info }
							, runnable.createImageView( view )
							, noiseImg.get() };
						auto data = device.graphicsData();
						staging->uploadTextureData( *queue.queue
							, *queue.commandPool
							, VkFormat( format )
							, buffer->getConstPtr()
							, result );
					} ) );

				graph.addInput( view
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			}
		}
	}

	castor::String const WaterRenderPass::Type = cuT( "c3d.water" );
	castor::String const WaterRenderPass::Name = cuT( "Water" );
	castor::String const WaterRenderPass::FullName = cuT( "Water rendering" );
	castor::String const WaterRenderPass::Param = cuT( "WaterConfig" );
	castor::String const WaterRenderPass::Normals1 = cuT( "WaterNormals1" );
	castor::String const WaterRenderPass::Normals2 = cuT( "WaterNormals2" );
	castor::String const WaterRenderPass::Noise = cuT( "WaterNoise" );

	WaterRenderPass::WaterRenderPass( castor3d::RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, castor3d::RenderNodesPassDesc const & renderPassDesc
		, castor3d::RenderTechniquePassDesc const & techniquePassDesc
		, castor3d::IsRenderPassEnabledUPtr isEnabled )
		: castor3d::RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, std::move( targetImage )
			, std::move( targetDepth )
			, renderPassDesc
			, techniquePassDesc }
		, m_isEnabled{ std::move( isEnabled ) }
		, m_linearWrapSampler{ device->createSampler( getName()
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR ) }
		, m_pointClampSampler{ device->createSampler( getName()
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST ) }
		, m_ubo{ device }
	{
		m_isEnabled->setPass( *this );
		auto params = getEngine()->getRenderPassTypeConfiguration( Type );

		if ( params.get( Param, m_configuration ) )
		{
			auto & data = m_ubo.getUbo().getData();
			data = m_configuration;
		}

		auto & loader = getEngine()->getImageLoader();
		loadImage( params, Normals1, getName(), *getEngine(), pass.graph, graph, loader, m_normals1Img, m_normals1View, m_normals1 );
		loadImage( params, Normals2, getName(), *getEngine(), pass.graph, graph, loader, m_normals2Img, m_normals2View, m_normals2 );
		loadImage( params, Noise, getName(), *getEngine(), pass.graph, graph, loader, m_noiseImg, m_noiseView, m_noise );
	}

	WaterRenderPass::~WaterRenderPass()
	{
	}

	crg::FramePassArray WaterRenderPass::create( castor3d::RenderDevice const & device
		, castor3d::RenderTechnique & technique
		, castor3d::TechniquePasses & renderPasses
		, crg::FramePassArray previousPasses )
	{
		std::string name{ Name };
		auto isEnabled = new castor3d::IsRenderPassEnabled{};
		auto extent = technique.getTargetExtent();
		auto & graph = technique.getGraph().createPassGroup( name );

		auto & blitColourPass = graph.createPass( "CopyColour"
			, [extent, &device, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = std::make_unique< crg::ImageCopy >( framePass
					, context
					, runnableGraph
					, extent
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		blitColourPass.addDependencies( previousPasses );
		blitColourPass.addTransferInputView( technique.getSampledResult() );
		blitColourPass.addTransferOutputView( technique.getSampledIntermediate() );

		auto targetResult = technique.getTargetResult();
		auto targetDepth = technique.getTargetDepth();
		auto & result = graph.createPass( "NodesPass"
			, [extent, targetResult, targetDepth, &device, &technique, &renderPasses, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
		{
			auto res = std::make_unique< WaterRenderPass >( &technique
				, framePass
				, context
				, runnableGraph
				, device
				, targetResult
				, targetDepth
				, castor3d::RenderNodesPassDesc{ extent
					, technique.getCameraUbo()
					, technique.getSceneUbo()
					, technique.getRenderTarget().getCuller() }
					.safeBand( true )
					.allowClusteredLighting()
					.componentModeFlags( castor3d::ComponentModeFlag::eColour
						| castor3d::ComponentModeFlag::eNormals
						| castor3d::ComponentModeFlag::eOcclusion
						| castor3d::ComponentModeFlag::eDiffuseLighting
						| castor3d::ComponentModeFlag::eSpecularLighting
						| castor3d::ComponentModeFlag::eSpecifics )
				, castor3d::RenderTechniquePassDesc{ false, technique.getSsaoConfig() }
					.indirect( technique.getIndirectLighting() )
					.clustersConfig( technique.getClustersConfig() )
				, castor3d::IsRenderPassEnabledUPtr( isEnabled ) );
			renderPasses[size_t( Event )].push_back( res.get() );
			device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
				, res->getTimer() );
			return res;
		} );

		result.addDependency( blitColourPass );
		result.addDependency( technique.getGetLastDepthPass() );
		result.addDependency( technique.getGetLastOpaquePass() );
		result.addImplicitColourView( technique.getSampledIntermediate()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( technique.getDepthObj().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( technique.getNormal().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutDepthStencilView( technique.getTargetDepth() );
		result.addInOutColourView( technique.getTargetResult() );
		return { &result };
	}

	castor3d::ShaderFlags WaterRenderPass::getShaderFlags()const
	{
		return castor3d::ShaderFlag::eWorldSpace
			| castor3d::ShaderFlag::eViewSpace
			| castor3d::ShaderFlag::eNormal
			| castor3d::ShaderFlag::eTangentSpace
			| castor3d::ShaderFlag::eLighting
			| castor3d::ShaderFlag::eForceTexCoords
			| castor3d::ShaderFlag::eColour;
	}

	void WaterRenderPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
		{
			visitor.visit( cuT( "Dampening factor" ), m_configuration.dampeningFactor );
			visitor.visit( cuT( "Depth softening distance" ), m_configuration.depthSofteningDistance );
			visitor.visit( cuT( "Refraction ratio" ), m_configuration.refractionRatio );
			visitor.visit( cuT( "Refraction distortion factor" ), m_configuration.refractionDistortionFactor );
			visitor.visit( cuT( "Refraction height factor" ), m_configuration.refractionHeightFactor );
			visitor.visit( cuT( "Refraction distance factor" ), m_configuration.refractionDistanceFactor );
			visitor.visit( cuT( "Normal map scroll speed" ), m_configuration.normalMapScrollSpeed );
			visitor.visit( cuT( "Normal map scroll" ), m_configuration.normalMapScroll );
			visitor.visit( cuT( "SSR step size" ), m_configuration.ssrStepSize );
			visitor.visit( cuT( "SSR forward steps count" ), m_configuration.ssrForwardStepsCount );
			visitor.visit( cuT( "SSR backward steps count" ), m_configuration.ssrBackwardStepsCount );
			visitor.visit( cuT( "SSR depth mult." ), m_configuration.ssrDepthMult );
		}
	}

	void WaterRenderPass::doUpdateUbos( castor3d::CpuUpdater & updater )
	{
		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );
		m_configuration.time += float( tslf.count() ) / 100.0f;
		m_ubo.cpuUpdate( m_configuration );
	}

	bool WaterRenderPass::doIsValidPass( castor3d::Pass const & pass )const
	{
		return pass.getRenderPassInfo()
			&& pass.getRenderPassInfo()->name == Type
			&& areValidPassFlags( pass.getPassFlags() );
	}

	void WaterRenderPass::doFillAdditionalBindings( castor3d::PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( getCuller().getScene().getLightCache().createLayoutBinding( VK_SHADER_STAGE_FRAGMENT_BIT
			, WaterIdx::eLightBuffer ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterUbo
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_VERTEX_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_FRAGMENT_BIT ) ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterNormals1
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterNormals2
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterNoise
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eSceneNormals
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eSceneDepth
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eSceneResult
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eBrdf
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( WaterIdx::eBrdf ) + 1u;
		doAddShadowBindings( m_scene, flags, bindings, index );
		doAddEnvBindings( flags, bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
		doAddGIBindings( flags, bindings, index );
		doAddClusteredLightingBindings( m_parent->getRenderTarget(), flags, bindings, index );
	}

	ashes::PipelineDepthStencilStateCreateInfo WaterRenderPass::doCreateDepthStencilState( castor3d::PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE
			, VK_COMPARE_OP_GREATER };
	}

	ashes::PipelineColorBlendStateCreateInfo WaterRenderPass::doCreateBlendState( castor3d::PipelineFlags const & flags )const
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments
		{
			VkPipelineColorBlendAttachmentState
			{
				VK_TRUE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				castor3d::defaultColorWriteMask,
			},
		};
		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			std::move( attachments ),
		};
	}

	void WaterRenderPass::doFillAdditionalDescriptor( castor3d::PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::ShadowMapLightTypeArray const & shadowMaps
		, castor3d::ShadowBuffer const * shadowBuffer )
	{
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( WaterIdx::eLightBuffer ) );
		descriptorWrites.push_back( m_ubo.getDescriptorWrite( WaterIdx::eWaterUbo ) );
		auto index = uint32_t( WaterIdx::eWaterNormals1 );
		bindTexture( m_normals1, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_normals2, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_noise, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_parent->getNormal().sampledView, *m_pointClampSampler, descriptorWrites, index );
		bindTexture( getTechnique().getDepthObj().wholeView, *m_pointClampSampler, descriptorWrites, index );
		bindTexture( getTechnique().getIntermediate().wholeView, *m_pointClampSampler, descriptorWrites, index );
		bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledView
			, *getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampler
			, descriptorWrites
			, index );
		doAddShadowDescriptor( m_scene, flags, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptor( flags, descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( flags, descriptorWrites, index );
		doAddClusteredLightingDescriptor( m_parent->getRenderTarget(), flags, descriptorWrites, index );
	}

	castor3d::SubmeshFlags WaterRenderPass::doAdjustSubmeshFlags( castor3d::SubmeshFlags flags )const
	{
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords1 );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords2 );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords3 );
		return flags;
	}

	castor3d::ProgramFlags WaterRenderPass::doAdjustProgramFlags( castor3d::ProgramFlags flags )const
	{
		remFlag( flags, castor3d::ProgramFlag::eInstantiation );
		return flags;
	}

	castor3d::ShaderPtr WaterRenderPass::doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const
	{
		using namespace castor3d;
		sdw::FragmentWriter writer;

		bool enableTextures = flags.enableTextures();
		bool hasDiffuseGI = flags.hasDiffuseGI();

		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::CookTorranceBRDF cookTorrance{ writer, brdf };
		shader::Fog fog{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		C3D_Water( writer
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveNormals1 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_waveNormals1"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveNormals2 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_waveNormals2"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveNoise = writer.declCombinedImg< FImg2DR32 >( "c3d_waveNoise"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_normals = writer.declCombinedImg< FImg2DRgba32 >( "c3d_normals"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_depthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObj"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_colour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_colour"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, nullptr
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, index /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, true /* enableVolumetric */ };
		shader::ReflectionModel reflections{ writer
			, utils
			, index
			, uint32_t( RenderPipeline::eBuffers )
			, lights.hasIblSupport() };
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, castor3d::makeExtent2D( m_size )
			, true
			, index
			, RenderPipeline::eBuffers );
		shader::GlobalIllumination indirect{ writer
			, utils
			, index
			, RenderPipeline::eBuffers
			, flags.getGlobalIlluminationFlags() };
		shader::ClusteredLights clusteredLights{ writer
			, index
			, RenderPipeline::eBuffers
			, getClustersConfig() };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		// Fragment Outputs
		auto outColour( writer.declOutput< sdw::Vec4 >( "outColour", 0 ) );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, sdw::VoidT >( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, sdw::FragmentOut{ writer }
			, [&]( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT > in
				, sdw::FragmentOut out )
			{
				shader::DebugOutput output{ getDebugConfig()
					, cuT( "Water" )
					, c3d_cameraData.debugIndex()
					, outColour
					, Water_Debug != 0 };
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );

				auto normalMapCoords1 = writer.declLocale( "normalMapCoords1"
					, in.texture0.xy() + c3d_waterData.time() * c3d_waterData.normalMapScroll().xy() * c3d_waterData.normalMapScrollSpeed().x() );
				auto normalMapCoords2 = writer.declLocale( "normalMapCoords2"
					, in.texture0.xy() + c3d_waterData.time() * c3d_waterData.normalMapScroll().zw() * c3d_waterData.normalMapScrollSpeed().y() );
				auto hdrCoords = writer.declLocale( "hdrCoords"
					, in.fragCoord.xy() / vec2( c3d_colour.getSize( 0_i ) ) );

				auto normalMap1 = writer.declLocale( "normalMap1"
					, ( c3d_waveNormals1.sample( normalMapCoords1 ).rgb() * 2.0_f ) - 1.0_f );
				auto normalMap2 = writer.declLocale( "normalMap2"
					, ( c3d_waveNormals2.sample( normalMapCoords2 ).rgb() * 2.0_f ) - 1.0_f );
				auto texSpace = writer.declLocale( "texSpace"
					, mat3( tangent.xyz(), bitangent, normal));
				auto finalNormal = writer.declLocale( "finalNormal"
					, normalize( texSpace * normalMap1.xyz() ) );
				finalNormal += normalize( texSpace * normalMap2.xyz() );
				finalNormal = normalize( finalNormal );
				output.registerOutput( "Final Normal", finalNormal );

				if ( flags.isFrontCulled() )
				{
					finalNormal = -finalNormal;
				}

				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto surface = writer.declLocale( "surface"
					, shader::Surface{ in.fragCoord.xyz()
						, in.viewPosition.xyz()
						, in.worldPosition.xyz()
						, finalNormal } );
				auto components = writer.declLocale( "components"
					, shader::BlendComponents{ materials
						, material
						, surface } );
				output.registerOutput( "Material F0", components.f0 );

				if ( auto lightingModel = lights.getLightingModel() )
				{
					// Direct Lighting
					shader::OutputComponents lighting{ writer, false };
					lightingModel->finish( passShaders
						, surface
						, utils
						, c3d_cameraData.position()
						, components );
					auto lightSurface = shader::LightSurface::create( writer
						, "lightSurface"
						, c3d_cameraData.position()
						, surface.worldPosition.xyz()
						, surface.viewPosition.xyz()
						, surface.clipPosition
						, surface.normal );
					lights.computeCombinedDifSpec( clusteredLights
						, components
						, *backgroundModel
						, lightSurface
						, modelData.isShadowReceiver()
						, lightSurface.clipPosition().xy()
						, lightSurface.viewPosition().z()
						, output
						, lighting );
					lightingModel->adjustDirectSpecular( components
						, lighting.specular );
					// Standard lighting models don't necessarily translate all that well to water.
					// It can end up looking very glossy and plastic-like, having much more form than it really should.
					// So here, I'm sampling some noise with three different sets of texture coordinates to try and achieve
					// that sparkling look that defines water specularity.
					auto specularNoise = writer.declLocale( "specularNoise"
						, c3d_waveNoise.sample( normalMapCoords1 * 0.5_f ) );
					specularNoise *= c3d_waveNoise.sample( normalMapCoords2 * 0.5_f );
					specularNoise *= c3d_waveNoise.sample( in.texture0.xy() * 0.5_f );
					lighting.specular *= specularNoise;
					output.registerOutput( "Specular Noise", specularNoise );
					output.registerOutput( "Noised Specular", lighting.specular );


					// Indirect Lighting
					lightSurface.updateL( utils
						, components.normal
						, components.f0
						, components );
					auto indirectOcclusion = indirect.computeOcclusion( flags.getGlobalIlluminationFlags()
						, lightSurface
						, output );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
						, lightSurface
						, indirectOcclusion
						, output );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
						, lightSurface
						, components.roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf
						, output );
					auto indirectAmbient = indirect.computeAmbient( flags.getGlobalIlluminationFlags()
						, lightIndirectDiffuse.xyz()
						, output );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, length( lightIndirectDiffuse.xyz() )
								, lightSurface.difF() )
							: vec3( 0.0_f ) ) );
					output.registerOutput( "Indirect", "Diffuse", indirectDiffuse );

					// Reflection
					auto bgDiffuseReflection = writer.declLocale( "bgDiffuseReflection"
						, vec3( 0.0_f ) );
					auto bgSpecularReflection = writer.declLocale( "bgSpecularReflection"
						, vec3( 0.0_f ) );
					lightSurface.updateN( utils
						, components.normal
						, components.f0
						, components );
					reflections.computeReflections( components
						, lightSurface
						, *backgroundModel
						, modelData.getEnvMapIndex()
						, components.hasReflection
						, bgDiffuseReflection
						, bgSpecularReflection
						, output );
					auto backgroundReflection = writer.declLocale( "backgroundReflection"
						, bgDiffuseReflection + bgSpecularReflection );
					output.registerOutput( "Background Reflection", backgroundReflection );
					auto ssrResult = writer.declLocale( "ssrResult"
						, reflections.computeScreenSpace( c3d_cameraData
							, lightSurface.viewPosition()
							, components.normal
							, hdrCoords
							, c3d_waterData.ssrSettings()
							, c3d_depthObj
							, c3d_normals
							, c3d_colour
							, output ) );
					auto reflectionResult = writer.declLocale( "reflectionResult"
						, mix( backgroundReflection, ssrResult.xyz(), ssrResult.www() ) );
					output.registerOutput( "Reflection Result", reflectionResult );


					// Refraction
					// Wobbly refractions
					auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
						, ( hdrCoords + ( ( components.normal.xz() + components.normal.xy() ) * 0.5_f ) * c3d_waterData.refractionDistortionFactor() ) );
					auto distortedDepth = writer.declLocale( "distortedDepth"
						, c3d_depthObj.sample( distortedTexCoord ).r() );
					auto distortedPosition = writer.declLocale( "distortedPosition"
						, c3d_cameraData.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
					auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedTexCoord, hdrCoords ) );
					auto refractionResult = writer.declLocale( "refractionResult"
						, c3d_colour.sample( refractionTexCoord ).rgb() * components.colour );
					output.registerOutput( "Refraction", refractionResult );
					//  Retrieve non distorted scene colour.
					auto sceneDepth = writer.declLocale( "sceneDepth"
						, c3d_depthObj.sample( hdrCoords ).r() );
					auto scenePosition = writer.declLocale( "scenePosition"
						, c3d_cameraData.curProjToWorld( utils, hdrCoords, sceneDepth ) );
					// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
					auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
						, clamp( distance( scenePosition, in.worldPosition.xyz() ) / c3d_waterData.depthSofteningDistance(), 0.0_f, 1.0_f ) );
					output.registerOutput( "Depth Softened Alpha", depthSoftenedAlpha );
					auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedPosition, scenePosition ) );
					auto waterTransmission = writer.declLocale( "waterTransmission"
						, components.colour * ( indirectAmbient + indirectDiffuse ) * lighting.diffuse );
					auto heightFactor = writer.declLocale( "heightFactor"
						, c3d_waterData.refractionHeightFactor() * ( c3d_cameraData.farPlane() - c3d_cameraData.nearPlane() ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, vec3( clamp( ( in.worldPosition.y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
					output.registerOutput( "Height Mixed Refraction", refractionResult );
					auto distanceFactor = writer.declLocale( "distanceFactor"
						, c3d_waterData.refractionDistanceFactor() * ( c3d_cameraData.farPlane() - c3d_cameraData.nearPlane() ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, utils.saturate( vec3( utils.saturate( length( in.viewPosition ) / distanceFactor ) ) ) );
					output.registerOutput( "Distance Mixed Refraction", refractionResult );


					//Combine all that
					auto fresnelFactor = writer.declLocale( "fresnelFactor"
						, vec3( utils.fresnelMix( reflections.computeIncident( lightSurface.worldPosition(), c3d_cameraData.position() )
							, components.normal
							, c3d_waterData.refractionRatio() ) ) );
					output.registerOutput( "Fresnel Factor", fresnelFactor );
					reflectionResult *= fresnelFactor;
					output.registerOutput( "Final Reflection", reflectionResult );
					refractionResult *= vec3( 1.0_f ) - fresnelFactor;
					output.registerOutput( "Final Refraction", refractionResult );

					outColour = vec4( lighting.specular + lightIndirectSpecular
							+ components.emissiveColour * components.emissiveFactor
							+ ( refractionResult )
							+ ( reflectionResult * indirectAmbient )
							+ lighting.scattering
						, depthSoftenedAlpha );

				}
				else
				{
					outColour = vec4( components.colour, 1.0_f );
				}

				if ( flags.hasFog() )
				{
					outColour = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
						, outColour
						, in.worldPosition.xyz()
						, c3d_cameraData.position()
						, c3d_sceneData );
				}

				backgroundModel->applyVolume( in.fragCoord.xy()
					, utils.lineariseDepth( in.fragCoord.z(), c3d_cameraData.nearPlane(), c3d_cameraData.farPlane() )
					, vec2( c3d_cameraData.renderSize() )
					, c3d_cameraData.depthPlanes()
					, outColour );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
