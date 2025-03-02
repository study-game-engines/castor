/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_H___
#define ___C3D_SHADOW_MAP_PASS_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	class ShadowMapPass
		: public RenderNodesPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	typeName	The shadow map pass type name.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\param[in]	needsVsm	Tells if the pass needs to generate variance shadow map.
		 *\param[in]	needsRsm	Tells if the pass needs to generate reflective shadow maps.
		 *\param[in]	isStatic	Tells if the pass is for static nodes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	typeName	Le nom du type de passe de shadow map.
		 *\param[in]	cameraUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 *\param[in]	needsVsm	Dit si la passe doit générer la variance shadow map.
		 *\param[in]	needsRsm	Dit si la passe doit générer les reflective shadow maps.
		 *\param[in]	isStatic	Dit si la passe est pour les noeuds statiques.
		 */
		C3D_API ShadowMapPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, CameraUbo const & cameraUbo
			, SceneCuller & culler
			, ShadowMap const & shadowMap
			, bool needsVsm
			, bool needsRsm
			, bool isStatic );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool isPassEnabled()const override;
		C3D_API bool isUpToDate()const;
		/**
		 *\copydoc		castor3d::RenderNodesPass::getShaderFlags
		 */
		ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eLighting
				| ShaderFlag::eTangentSpace
				| ShaderFlag::eOpacity
				| ( m_needsVsm ? ShaderFlag::eVsmShadowMap : ShaderFlag::eNone )
				| ( m_needsRsm ? ShaderFlag::eRsmShadowMap : ShaderFlag::eNone )
				| ( m_needsRsm ? ShaderFlag::eColour : ShaderFlag::eNone )
				| doGetShaderFlags();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setUpToDate();
		/**@}*/

	private:
		bool doIsValidRenderable( RenderedObject const & object )const override;
		SubmeshFlags doAdjustSubmeshFlags( SubmeshFlags flags )const override;
		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer )override;

		C3D_API virtual ShaderFlags doGetShaderFlags()const = 0;

	protected:
		ShadowMap const & m_shadowMap;
		mutable bool m_initialised{ false };
		bool m_outOfDate{ true };
		ShadowMapUbo m_shadowMapUbo;
		bool m_needsVsm;
		bool m_needsRsm;
	};
}

#endif
