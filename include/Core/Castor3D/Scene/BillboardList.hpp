/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardList_H___
#define ___C3D_BillboardList_H___

#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/RenderedObject.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	class BillboardBase
		: public RenderedObject
	{
	public:
		using IdRenderNode = std::pair< uint32_t, BillboardRenderNode const * >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene			The parent scene.
		 *\param[in]	node			The parent scene node.
		 *\param[in]	vertexLayout	The layout for the vertex buffer.
		 *\param[in]	vertexStride	The vertex stride.
		 *\param[in]	vertexBuffer	The vertex buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene			La scène parente.
		 *\param[in]	node			Le noeud de scène parent.
		 *\param[in]	vertexLayout	Le layout du tampon de sommets.
		 *\param[in]	vertexStride	Le stride du tampon de sommets.
		 *\param[in]	vertexBuffer	Le tampon de sommets.
		 */
		C3D_API BillboardBase( Scene & scene
			, SceneNode * node
			, ashes::PipelineVertexInputStateCreateInfoPtr vertexLayout
			, uint32_t vertexStride
			, GpuBufferOffsetT< uint8_t > vertexBuffer = {} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~BillboardBase();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements.
		 *\param		device	The current device.
		 *\param[in]	count	The elements count.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise les éléments GPU.
		 *\param		device	Le device actuel.
		 *\param[in]	count	Le nombre d'éléments.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\param		device	The current device.
		 *\~french
		 *\brief		Nettoie les elements GPU
		 *\param		device	Le device actuel.
		 */
		C3D_API void cleanup( RenderDevice const & device );
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
		 *\brief		Retrieves the object ID in models buffer.
		 *\param[in]	pass	The material pass.
		 *\~french
		 *\brief		Récupère l'ID de l'objet dans le buffer de modèles.
		 *\param[in]	pass	La passe de matériau.
		 */
		C3D_API uint32_t getId( Pass const & pass )const;
		/**
		 *\~english
		 *\brief		Retrieves the object render node.
		 *\param[in]	pass	The material pass.
		 *\~french
		 *\brief		Récupère le noeud de rendu de l'objet.
		 *\param[in]	pass	La passe de matériau.
		 */
		C3D_API BillboardRenderNode const * getRenderNode( Pass const & pass )const;
		/**
		 *\~english
		 *\brief		Fills the given data with this billboard's properties.
		 *\param[out]	data	The data to fill.
		 *\~french
		 *\brief		Remplit les données données avec les propriétés de ce billboard.
		 *\param[out]	data	Les données à remplir.
		 */
		C3D_API void fillData( BillboardUboConfiguration & data )const;
		/**
		 *\~english
		 *\brief		Sets the object render node and ID in models buffer.
		 *\param[in]	pass		The material pass.
		 *\param[in]	renderNode	The render node.
		 *\param[in]	id			The ID.
		 *\~french
		 *\brief		Définit le noeud de rendu de l'objet et son ID dans le buffer de modèles.
		 *\param[in]	pass		La passe de matériau.
		 *\param[in]	renderNode	Le noeud de rendu.
		 *\param[in]	id			L'ID.
		 */
		C3D_API void setId( Pass const & pass
			, BillboardRenderNode const * renderNode
			, uint32_t id );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API SubmeshFlags getSubmeshFlags()const;
		C3D_API ProgramFlags getProgramFlags()const;

		MaterialObs getMaterial()const
		{
			return m_material;
		}

		castor::Point2f const & getDimensions()const
		{
			return m_dimensions;
		}

		uint32_t getCount()const
		{
			return m_count;
		}

		bool isInitialised()const
		{
			return m_initialised;
		}

		GpuBufferOffsetT< uint8_t > const & getVertexBuffer()const
		{
			return m_vertexBuffer;
		}

		GpuBufferOffsetT< uint8_t > & getVertexBuffer()
		{
			return m_vertexBuffer;
		}

		uint32_t getVertexStride()
		{
			return m_vertexStride;
		}

		GeometryBuffers const & getGeometryBuffers()const
		{
			return m_geometryBuffers;
		}

		ObjectBufferOffset const & getBufferOffsets()const
		{
			return getGeometryBuffers().bufferOffset;
		}

		Scene const & getParentScene()const
		{
			return m_scene;
		}

		Scene & getParentScene()
		{
			return m_scene;
		}

		SceneNode * getNode()const
		{
			return m_node;
		}

		BillboardType getBillboardType()const
		{
			return m_billboardType;
		}

		BillboardSize getBillboardSize()const
		{
			return m_billboardSize;
		}

		std::unordered_map< Pass const *, IdRenderNode > const & getIds()const
		{
			return m_ids;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Setters.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setDimensions( castor::Point2f const & value )
		{
			m_dimensions = value;
		}

		void setCenterOffset( uint32_t value )
		{
			m_centerOffset = value;
		}

		void setNode( SceneNode & value )
		{
			m_node = &value;
		}

		void setBillboardType( BillboardType value )
		{
			m_billboardType = value;
		}

		void setBillboardSize( BillboardSize value )
		{
			m_billboardSize = value;
		}

		void setGpuFilled( bool value )
		{
			m_gpuFilled = value;
		}

		C3D_API void setMaterial( MaterialObs value );
		C3D_API void setCount( uint32_t value );
		/**@}*/

	private:
		void doGatherBuffers( ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts );

	public:
		using Quad = std::array< BillboardVertex, 4u >;

	protected:
		Scene & m_scene;
		SceneNode * m_node{};
		MaterialObs m_material{};
		castor::Point2f m_dimensions;
		castor::Point3f m_cameraPosition;
		GpuBufferOffsetT< uint8_t > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
		uint32_t m_vertexStride;
		ashes::PipelineVertexInputStateCreateInfoPtr m_quadLayout;
		GeometryBuffers m_geometryBuffers;
		bool m_needUpdate{ true };
		bool m_initialised{ false };
		bool m_gpuFilled{ false };
		uint32_t m_count{ 0u };
		uint32_t m_centerOffset{ 0u };
		BillboardType m_billboardType{ BillboardType::eCylindrical };
		BillboardSize m_billboardSize{ BillboardSize::eDynamic };
		std::unordered_map< Pass const *, IdRenderNode > m_ids{};
	};

	class BillboardList
		: public MovableObject
		, public BillboardBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The name.
		 *\param[in]	scene	The parent scene.
		 *\param[in]	parent	The parent scene node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom.
		 *\param[in]	scene	La scene parente.
		 *\param[in]	parent	Le noeud de scène parent.
		 */
		C3D_API BillboardList( castor::String const & name
			, Scene & scene
			, SceneNode & parent );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The name.
		 *\param[in]	scene	The parent scene.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom.
		 *\param[in]	scene	La scene parente.
		 */
		C3D_API BillboardList( castor::String const & name
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Initialises GPU side elements
		 *\param		device	The current device.
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise les elements GPU
		 *\param		device	Le device actuel.
		 *\return		\p true si tout s'est bien passe
		 */
		C3D_API bool initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Removes a point from the list
		 *\param[in]	index	The point index
		 *\~french
		 *\brief		Retire un point de la liste
		 *\param[in]	index	L'index du point
		 */
		C3D_API void removePoint( uint32_t index );
		/**
		 *\~english
		 *\brief		adds a point to the list
		 *\param[in]	position	The point
		 *\~french
		 *\brief		Ajoute un point a la liste
		 *\param[in]	position	Le point
		 */
		C3D_API void addPoint( castor::Point3f const & position );
		/**
		 *\~english
		 *\brief		adds a points list to the list
		 *\param[in]	positions	The points list
		 *\~french
		 *\brief		Ajoute une liste de points a la liste
		 *\param[in]	positions	La liste de points
		 */
		C3D_API void addPoints( castor::Point3fArray const & positions );
		/**
		 *\~english
		 *\brief		Attaches the movable object to a node
		 *\param[in]	node	The parent scene node.
		 *\~french
		 *\brief		Attache l'object à un noeud
		 *\param[in]	node	Le noeud de scène parent.
		 */
		C3D_API void attachTo( SceneNode & node )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::Point3f const & getAt( uint32_t index )const
		{
			return m_arrayPositions[index];
		}

		inline castor::Point3fArrayIt begin()
		{
			return m_arrayPositions.begin();
		}

		inline castor::Point3fArrayConstIt begin()const
		{
			return m_arrayPositions.begin();
		}

		inline castor::Point3fArrayIt end()
		{
			return m_arrayPositions.end();
		}

		inline castor::Point3fArrayConstIt end()const
		{
			return m_arrayPositions.end();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Setters.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setAt( uint32_t index, castor::Point3f const & position )
		{
			m_needUpdate = true;
			m_arrayPositions[index] = position;
		}
		/**@}*/

	protected:
		castor::Point3fArray m_arrayPositions;
	};
}

#endif
