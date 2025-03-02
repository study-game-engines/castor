/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Submesh_H___
#define ___C3D_Submesh_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Model/VertexGroup.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>

#include <unordered_map>

namespace castor3d
{
	template< typename T >
	struct SubmeshComponentAdder
	{
		static inline void add( castor::UniquePtr< T > component
			, Submesh & submesh );
	};

	class Submesh
		: public castor::OwnedBy< Mesh >
	{
	private:
		CU_DeclareList( castor::ByteArray, BytePtr );
		template< typename T >
		friend struct SubmeshComponentAdder;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	mesh	The parent mesh.
		 *\param[in]	id		The submesh ID.
		 *\param[in]	flags	The submesh flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	mesh	Le mesh parent.
		 *\param[in]	id		L'ID du sous-maillage.
		 *\param[in]	flags	Les indicateur du sous-maillage.
		 */
		C3D_API explicit Submesh( Mesh & mesh
			, uint32_t id = 1
			, SubmeshFlags const & flags = SubmeshFlags{} );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Submesh();
		/**
		 *\~english
		 *\brief		Initialises the submesh.
		 *\param[in]	device	The render device.
		 *\~french
		 *\brief		Initialise le sous-maillage.
		 *\param[in]	device	Le périphérique de rendu.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans the submesh.
		 *\param[in]	device	The render device.
		 *\~french
		 *\brief		Nettoie le sous-maillage.
		 *\param[in]	device	Le périphérique de rendu.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the buffers.
		 *\~french
		 *\brief		Met à jour les tampons.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Computes the containers (cube and sphere)
		 *\~french
		 *\brief		Calcule les conteneurs (cube et sphère)
		 */
		C3D_API void computeContainers();
		/**
		 *\~english
		 *\brief		Updates containers from given bounding box.
		 *\~french
		 *\brief		Met à jour les conteneurs depuis la bounding box donnée
		 */
		C3D_API void updateContainers( castor::BoundingBox const & boundingBox );
		/**
		 *\~english
		 *\return		The faces number
		 *\~french
		 *\return		Le nombre de faces de ce submesh
		 */
		C3D_API uint32_t getFaceCount()const;
		/**
		 *\~english
		 *\return		The points count
		 *\~french
		 *\return		Le nombre de vertices de ce sous-maillage
		 */
		C3D_API uint32_t getPointsCount()const;
		/**
		 *\~english
		 *\brief		Tests if the given Point3f is in mine
		 *\param[in]	position	The vertex to test
		 *\param[in]	precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le point donné fait partie de ceux de ce sous-maillage
		 *\param[in]	position	Le point à tester
		 *\param[in]	precision	La précision de comparaison
		 *\return		L'index du point s'il a été trouvé, -1 sinon
		 */
		C3D_API int isInMyPoints( castor::Point3f const & position, double precision );
		/**
		 *\~english
		 *\brief		Creates and Adds a vertex to my list
		 *\param[in]	x	The vertex X coordinate
		 *\param[in]	y	The vertex Y coordinate
		 *\param[in]	z	The vertex Z coordinate
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée un Vertex à partir des coordonnées données et l'ajoute à la liste
		 *\param[in]	x	Coordonnée X
		 *\param[in]	y	Coordonnée Y
		 *\param[in]	z	Coordonnée Y
		 *\return		Le vertex créé
		 */
		C3D_API InterleavedVertex addPoint( float x, float y, float z );
		/**
		 *\~english
		 *\brief		Adds a vertex to my list
		 *\param[in]	value	The vertex to add
		 *\return		The vertex
		 *\~french
		 *\brief		Crée un Vertex à partir du point donné et l'ajoute à la liste
		 *\param[in]	value	Le point
		 *\return		Le vertex créé
		 */
		C3D_API InterleavedVertex addPoint( castor::Point3f const & value );
		/**
		 *\~english
		 *\brief		Creates and Adds a vertex to my list
		 *\param[in]	value	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée un Vertex à partir des coordonnées données et l'ajoute à la liste
		 *\param[in]	value	Les coordonnées du point
		 *\return		Le vertex créé
		 */
		C3D_API InterleavedVertex addPoint( float * value );
		/**
		 *\~english
		 *\brief		Adds a vertex to the list.
		 *\param[in]	value	The vertex.
		 *\~french
		 *\brief		Ajoute un sommet à la liste.
		 *\param[in]	value	Le sommet.
		 */
		C3D_API void addPoint( InterleavedVertex const & value );
		/**
		 *\~english
		 *\brief		Adds a points list to my list.
		 *\param[in]	begin	The vertices data begin.
		 *\param[in]	end		The vertices data end.
		 *\~french
		 *\brief		Ajoute des points à la liste.
		 *\param[in]	begin	Le début des données de sommets.
		 *\param[in]	end		La fin des données de sommets.
		 */
		C3D_API void addPoints( InterleavedVertex const * const begin
			, InterleavedVertex const * const end );
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	reverted	\p true to inverse normals.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	reverted	\p true pour inverser les normales.
		 */
		C3D_API void computeNormals( bool reverted = false );
		/**
		 *\~english
		 *\return		The program flags.
		 *\param[in]	material	The material for which we want the flags.
		 *\~french
		 *\return		Les indicateurs de programme.
		 *\param[in]	material	Le matériau pour lequel on veut les indicateurs.
		 */
		C3D_API ProgramFlags getProgramFlags( Material const & material )const;
		/**
		 *\~english
		 *\return		The submesh flags.
		 *\param[in]	pass	The pass for which we want the flags.
		 *\~french
		 *\return		Les indicateurs de submesh.
		 *\param[in]	pass	La passe pour laquelle on veut les indicateurs.
		 */
		C3D_API SubmeshFlags getSubmeshFlags( Pass const * pass )const;
		/**
		 *\~english
		 *\return		The morphing flags.
		 *\~french
		 *\return		Les indicateurs de morphing.
		 */
		C3D_API MorphFlags getMorphFlags()const;
		/**
		 *\~english
		 *\brief		Instantiates the submesh with a geometry and its material.
		 *\param[in]	geometry	The geometry instantiating the submesh.
		 *\param[in]	oldMaterial	The old material.
		 *\param[in]	newMaterial	The new material.
		 *\param[in]	update		Tells if the instantiation component must be updated.
		 *\~french
		 *\brief		Instancie le submesh avec une géométrie et son matériau.
		 *\param[in]	geometry	La géométrie instantiant le submesh.
		 *\param[in]	oldMaterial	Le matériau précédent.
		 *\param[in]	newMaterial	Le nouveau matériau.
		 *\param[in]	update		Dit si le composant d'instantiation doit être mis à jour.
		 */
		C3D_API void instantiate( Geometry const * geometry
			, MaterialObs oldMaterial
			, MaterialObs newMaterial
			, bool update );
		/**
		*\~english
		*\return		The geometry buffers for given render node.
		 *\param[in]	node	The render node using this submesh.
		 *\param[in]	flags	The pipeline flags.
		*\~french
		*\return		Les tampons de géométrie associés au noeud de rendu donné.
		 *\param[in]	node	Le noeud de rendu utilisant ce sous-maillage.
		 *\param[in]	flags	Les indicateurs de pipeline.
		*/
		C3D_API GeometryBuffers const & getGeometryBuffers( SubmeshRenderNode const & node
			, PipelineFlags const & flags )const;
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief		Enables scene update on modifications.
		 *\param[in]	updateScene Tells if this call triggers a scene update.
		 *\~french
		 *\brief		Active la mise à jour de la scène en cas de modification.
		 *\param[in]	updateScene	Dit si cet appel déclenche une mise à jour de la scène.
		 */
		C3D_API void enableSceneUpdate( bool updateScene );
		C3D_API void setBaseData( SubmeshData submeshData, castor::Point3fArray data );
		C3D_API void setBaseData( SubmeshData submeshData, castor::Point4fArray data );
		inline void disableSceneUpdate();
		inline void needsUpdate();
		inline void addPoints( std::vector< InterleavedVertex > const & vertices );
		template< size_t Count >
		inline void addPoints( std::array< InterleavedVertex, Count > const & vertices );
		inline void setDefaultMaterial( MaterialObs material );
		inline void setIndexMapping( IndexMappingUPtr mapping );
		inline IndexMappingRPtr getIndexMapping()const;
		template< typename ComponentT, typename ... ParamsT >
		inline ComponentT * createComponent( ParamsT && ... params );
		inline void addComponent( SubmeshComponentUPtr component );
		template< typename ComponentT >
		inline void addComponent( castor::UniquePtr< ComponentT > component );
		inline void setTopology( VkPrimitiveTopology value );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API InterleavedVertex getInterleavedPoint( uint32_t index )const;
		C3D_API castor::Point3fArray const & getPositions()const;
		C3D_API castor::Point3fArray & getPositions();
		C3D_API castor::Point3fArray const & getNormals()const;
		C3D_API castor::Point3fArray & getNormals();
		C3D_API castor::Point4fArray const & getTangents()const;
		C3D_API castor::Point4fArray & getTangents();
		C3D_API castor::Point3fArray const & getBitangents()const;
		C3D_API castor::Point3fArray & getBitangents();
		C3D_API castor::Point3fArray const & getTexcoords0()const;
		C3D_API castor::Point3fArray & getTexcoords0();
		C3D_API castor::Point3fArray const & getTexcoords1()const;
		C3D_API castor::Point3fArray & getTexcoords1();
		C3D_API castor::Point3fArray const & getTexcoords2()const;
		C3D_API castor::Point3fArray & getTexcoords2();
		C3D_API castor::Point3fArray const & getTexcoords3()const;
		C3D_API castor::Point3fArray & getTexcoords3();
		C3D_API castor::Point3fArray const & getColours()const;
		C3D_API castor::Point3fArray & getColours();
		C3D_API castor::Point3fArray const & getBaseData( SubmeshData submeshData )const;
		C3D_API castor::Point3fArray & getBaseData( SubmeshData submeshData );
		C3D_API GpuBufferOffsetT< castor::Point4f > const & getMorphTargets()const;
		C3D_API uint32_t getMorphTargetsCount()const;
		C3D_API std::vector< Meshlet > const & getMeshlets()const;
		C3D_API std::vector< Meshlet > & getMeshlets();
		C3D_API uint32_t getMeshletsCount()const;
		C3D_API bool isDynamic()const;
		C3D_API bool isAnimated()const;
		C3D_API ObjectBufferOffset const & getFinalBufferOffsets( Geometry const & instance )const;
		C3D_API ObjectBufferOffset const & getSourceBufferOffsets()const;
		C3D_API GpuBufferOffsetT< Meshlet > const & getMeshletsBuffer()const;
		C3D_API GpuBufferOffsetT< MeshletCullData > const & getFinalMeshletsBounds( Geometry const & instance )const;
		C3D_API GpuBufferOffsetT< MeshletCullData > const & getSourceMeshletsBounds()const;
		C3D_API bool hasMorphComponent()const;
		C3D_API bool hasSkinComponent()const;
		C3D_API VkDeviceSize getVertexOffset( Geometry const & geometry )const;
		C3D_API VkDeviceSize getIndexOffset()const;
		C3D_API VkDeviceSize getMeshletOffset()const;
		inline SkeletonRPtr getSkeleton()const;
		inline MaterialObs getDefaultMaterial()const;
		inline castor::BoundingBox const & getBoundingBox()const;
		inline castor::BoundingBox & getBoundingBox();
		inline castor::BoundingSphere const & getBoundingSphere()const;
		inline castor::BoundingSphere & getBoundingSphere();
		inline bool isInitialised()const;
		inline Mesh const & getParent()const;
		inline Mesh & getParent();
		inline uint32_t getId()const;
		inline bool hasComponent( castor::String const & name )const;
		inline SubmeshComponentRPtr getComponent( castor::String const & name )const;
		template< typename ComponentT >
		inline ComponentT * getComponent()const;
		inline InstantiationComponent & getInstantiation();
		inline InstantiationComponent const & getInstantiation()const;
		inline SubmeshComponentIDMap const & getComponents()const;
		inline VkPrimitiveTopology getTopology()const;
		inline SubmeshFlags getFinalSubmeshFlags()const;
		/**@}*/

	private:
		uint32_t m_id;
		MaterialObs m_defaultMaterial;
		castor::BoundingBox m_box;
		castor::BoundingSphere m_sphere;
		SubmeshComponentIDMap m_components;
		InstantiationComponentRPtr m_instantiation{};
		IndexMappingRPtr m_indexMapping{};
		SubmeshFlags m_submeshFlags{ 0u };
		bool m_generated{ false };
		bool m_initialised{ false };
		bool m_dirty{ true };
		VkPrimitiveTopology m_topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		ObjectBufferOffset m_sourceBufferOffset;
		std::unordered_map< Geometry const *, ObjectBufferOffset > m_finalBufferOffsets;
		mutable std::unordered_map< size_t, GeometryBuffers > m_geometryBuffers;
		bool m_needsNormalsCompute{ false };
		bool m_disableSceneUpdate{ false };

		friend class BinaryWriter< Submesh >;
		friend class BinaryParser< Submesh >;
	};
}

#include "Submesh.inl"

#endif
