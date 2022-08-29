/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "RenderModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	struct RenderTechniquePassDesc
	{
		RenderTechniquePassDesc( bool environment
			, SsaoConfig const & ssaoConfig )
			: m_ssaoConfig{ ssaoConfig }
		{
			if ( environment )
			{
				addFlag( m_shaderFlags, ShaderFlag::eEnvironmentMapping );
			}
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV configuration.
		 *\~french
		 *\param[in]	value	La configuration des LPV.
		 */
		RenderTechniquePassDesc & lpvConfigUbo( LpvGridConfigUbo const & value )
		{
			m_lpvConfigUbo = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The Layered LPV configuration.
		 *\~french
		 *\param[in]	value	La configuration des Layered LPV.
		 */
		RenderTechniquePassDesc & llpvConfigUbo( LayeredLpvGridConfigUbo const & value )
		{
			m_llpvConfigUbo = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The VCT configuration.
		 *\~french
		 *\param[in]	value	La configuration du VCT.
		 */
		RenderTechniquePassDesc & vctConfigUbo( VoxelizerUbo const & value )
		{
			m_vctConfigUbo = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & ssao( Texture const & value )
		{
			m_ssao = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & lpvResult( LightVolumePassResult const & value )
		{
			m_lpvResult = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & llpvResult( LightVolumePassResultArray const & value )
		{
			m_llpvResult = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The VCT first bounce result.
		 *\~french
		 *\param[in]	value	Le résultat du premier rebond de VCT.
		 */
		RenderTechniquePassDesc & vctFirstBounce( Texture const & value )
		{
			m_vctFirstBounce = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The VCT second bounce result.
		 *\~french
		 *\param[in]	value	Le résultat du second rebond de VCT.
		 */
		RenderTechniquePassDesc & vctSecondaryBounce( Texture const & value )
		{
			m_vctSecondaryBounce = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & hasVelocity( bool value )
		{
			if ( value )
			{
				addFlag( m_shaderFlags, ShaderFlag::eVelocity );
			}
			else
			{
				remFlag( m_shaderFlags, ShaderFlag::eVelocity );
			}

			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & addShaderFlag( ShaderFlags value )
		{
			m_shaderFlags |= value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & shaderFlags( ShaderFlags value )
		{
			m_shaderFlags = value;
			return *this;
		}

		SsaoConfig const & m_ssaoConfig;
		Texture const * m_ssao{};
		LpvGridConfigUbo const * m_lpvConfigUbo{};
		LayeredLpvGridConfigUbo const * m_llpvConfigUbo{};
		VoxelizerUbo const * m_vctConfigUbo{};
		LightVolumePassResult const * m_lpvResult{};
		LightVolumePassResultArray const * m_llpvResult{};
		Texture const * m_vctFirstBounce{};
		Texture const * m_vctSecondaryBounce{};
		ShaderFlags m_shaderFlags{ ShaderFlag::eWorldSpace
			| ShaderFlag::eViewSpace
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eLighting
			| ShaderFlag::eOpacity
			| ShaderFlag::eColour };
	};

	class RenderTechniquePass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent	The parent technique.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent	La technique parente.
		 *\param[in]	scene	La scène.
		 */
		C3D_API RenderTechniquePass( RenderTechnique * parent
			, Scene const & scene );

	public:
		C3D_API virtual ~RenderTechniquePass() = default;
		/**
		 *\copydoc	RenderNodesPass::accept
		 */
		C3D_API virtual void accept( RenderTechniqueVisitor & visitor )
		{
		}
		/**
		 *\copydoc	RenderNodesPass::accept
		 */
		C3D_API virtual void update( CpuUpdater & updater )
		{
		}
		/**
		 *\copydoc	RenderNodesPass::createPipelineFlags
		 */
		C3D_API virtual PipelineFlags createPipelineFlags( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, PassFlags passFlags
			, RenderPassTypeID renderPassTypeID
			, PassTypeID passTypeID
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureFlagsArray const & textures
			, SubmeshFlags const & submeshFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const = 0;
		/**
		 *\copydoc	RenderNodesPass::areValidPassFlags
		 */
		C3D_API virtual bool areValidPassFlags( PassFlags const & passFlags )const = 0;
		/**
		 *\copydoc	RenderNodesPass::getShaderFlags
		 */
		C3D_API virtual ShaderFlags getShaderFlags()const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API Engine * getEngine()const;

		Scene const & getScene()
		{
			return m_scene;
		}

		Scene const & getScene()const
		{
			return m_scene;
		}

		RenderTechnique const & getTechnique()const
		{
			return *m_parent;
		}
		/**@}*/

	protected:
		RenderTechnique * m_parent{};
		Scene const & m_scene;
	};

	class RenderTechniqueNodesPass
		: public RenderNodesPass
		, public RenderTechniquePass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent				The parent technique.
		 *\param[in]	pass				The parent frame pass.
		 *\param[in]	context				The rendering context.
		 *\param[in]	graph				The runnable graph.
		 *\param[in]	device				The GPU device.
		 *\param[in]	typeName			The pass type name.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	typeName			Le nom du type de la passe.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API RenderTechniqueNodesPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, crg::ImageData const * targetImage
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );

	public:
		/**
		 *\copydoc	RenderNodesPass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc	RenderNodesPass::accept
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc	RenderNodesPass::createPipelineFlags
		 */
		C3D_API PipelineFlags createPipelineFlags( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, PassFlags passFlags
			, RenderPassTypeID renderPassTypeID
			, PassTypeID passTypeID
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureFlagsArray const & textures
			, SubmeshFlags const & submeshFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const override;
		/**
		 *\copydoc	RenderNodesPass::getShaderFlags
		 */
		C3D_API ShaderFlags getShaderFlags()const override
		{
			return m_shaderFlags;
		}
		/**
		 *\copydoc	RenderNodesPass::areValidPassFlags
		 */
		C3D_API bool areValidPassFlags( PassFlags const & passFlags )const override
		{
			return RenderNodesPass::areValidPassFlags( passFlags );
		}
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Engine * getEngine()const
		{
			return RenderTechniquePass::getEngine();
		}

		Scene const & getScene()const
		{
			return RenderTechniquePass::getScene();
		}

		RenderTechnique const & getTechnique()const
		{
			return RenderTechniquePass::getTechnique();
		}
		/**@}*/

	protected:
		C3D_API ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		C3D_API SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		C3D_API void doAddShadowBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddBackgroundBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddEnvBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddGIBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddShadowDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;
		C3D_API void doAddBackgroundDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageData const & targetImage
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;
		C3D_API void doAddEnvDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;
		C3D_API void doAddGIDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t & index )const;

	private:
		using RenderNodesPass::update;

		void doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	protected:
		Camera * m_camera{ nullptr };
		ShaderFlags m_shaderFlags{};
		SsaoConfig m_ssaoConfig;
		Texture const * m_ssao;
		LpvGridConfigUbo const * m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const * m_llpvConfigUbo;
		VoxelizerUbo const * m_vctConfigUbo;
		LightVolumePassResult const * m_lpvResult;
		LightVolumePassResultArray const * m_llpvResult;
		Texture const * m_vctFirstBounce;
		Texture const * m_vctSecondaryBounce;
	};
}

#endif