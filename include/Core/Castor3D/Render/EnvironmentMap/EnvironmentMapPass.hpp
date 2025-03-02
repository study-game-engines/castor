/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMapPass_H___
#define ___C3D_EnvironmentMapPass_H___

#include "EnvironmentMapModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	class EnvironmentMapPass
		: public castor::OwnedBy< EnvironmentMap >
		, public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The parent graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	environmentMap	The parent reflection map.
		 *\param[in]	faceNode		The node from which the camera is created.
		 *\param[in]	index			The cube index this pass renders to.
		 *\param[in]	face			The cube face this pass renders to.
		 *\param[in]	background		The scene background.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			The parent graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	environmentMap	Le reflection map parente.
		 *\param[in]	faceNode		Le noeud depuis lequel on crée la caméra.
		 *\param[in]	index			L'index du cube que cette passe dessine.
		 *\param[in]	face			La face du cube que cette passe dessine.
		 *\param[in]	background		Le fond de la scène.
		 */
		C3D_API EnvironmentMapPass( RenderDevice const & device
			, EnvironmentMap & environmentMap
			, SceneNodeUPtr faceNode
			, uint32_t index
			, CubeMapFace face
			, SceneBackground & background );
		C3D_API ~EnvironmentMapPass();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders the environment map.
		 *\param[in]	toWait	The semaphores to wait.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine la texture d'environnement.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API void record();
		/**
		 *\~english
		 *\brief		Renders the environment map.
		 *\param[in]	toWait	The semaphores to wait.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine la texture d'environnement.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		/**
		 *\~english
		 *\brief		Attaches this pass to given node.
		 *\remarks		Set the ignored node for the internal render passes.
		 *\param[in]	node	The node.
		 *\~french
		 *\brief		Attache cette passe au noeud donné.
		 *\remarks		Définit le noeud ignoré pour les render passes internes.
		 *\param[in]	node	Le noeud.
		 */
		C3D_API void attachTo( SceneNode & node );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		crg::FramePass const & getLastPass()const
		{
			return *m_transparentPassDesc;
		}
		/**@}*/

	private:
		crg::FramePass & doCreateOpaquePass( crg::FramePass const * previousPass );
		crg::FramePass & doCreateTransparentPass( crg::FramePass const * previousPass );
		void doCreateGenMipmapsPass( crg::FramePass const * previousPass );

	private:
		RenderDevice const & m_device;
		crg::FrameGraph m_graph;
		SceneBackground & m_background;
		SceneNodeUPtr m_node;
		uint32_t m_index;
		CubeMapFace m_face;
		CameraUPtr m_camera;
		SceneNode const * m_currentNode{};
		SceneCullerUPtr m_culler;
		CameraUbo m_cameraUbo;
		HdrConfigUbo m_hdrConfigUbo;
		SceneUbo m_sceneUbo;
		crg::ImageViewId m_colourRenderView;
		crg::ImageViewId m_colourResultView;
		crg::ImageViewId m_depthView;
		BackgroundRendererUPtr m_backgroundRenderer;
		crg::FramePass * m_opaquePassDesc{};
		RenderTechniqueNodesPass * m_opaquePass{};
		crg::FramePass * m_transparentPassDesc{};
		RenderTechniqueNodesPass * m_transparentPass{};
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
