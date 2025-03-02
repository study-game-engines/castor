/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferPool_HPP___
#define ___C3D_ObjectBufferPool_HPP___

#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <unordered_map>

namespace castor3d
{
	class VertexBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( GpuPackedBaseBufferUPtr vtx )
				: vertex{ std::move( vtx ) }
			{
			}

			GpuPackedBaseBufferUPtr vertex;
		};
		using BufferArray = std::vector< ModelBuffers >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API explicit VertexBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount	The wanted buffer element count.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount	Le nombre d'éléments voulu pour le tampon.
		 *\return		Le tampon GPU.
		 */
		template< typename VertexT >
		ObjectBufferOffset getBuffer( VkDeviceSize vertexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		BufferArray m_buffers;
	};

	class IndexBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( GpuPackedBaseBufferUPtr vtx )
				: vertex{ std::move( vtx ) }
			{
			}

			GpuPackedBaseBufferUPtr vertex;
		};
		using BufferArray = std::vector< ModelBuffers >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API explicit IndexBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount	The wanted buffer element count.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount	Le nombre d'éléments voulu pour le tampon.
		 *\return		Le tampon GPU.
		 */
		template< typename IndexT >
		ObjectBufferOffset getBuffer( VkDeviceSize vertexCount );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset );

	private:
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize size
			, BufferArray & array );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		BufferArray m_buffers;
	};

	class ObjectBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		struct ModelBuffers
		{
			explicit ModelBuffers( std::array< GpuPackedBaseBufferUPtr, size_t( SubmeshData::eCount ) > bufs = {} )
				: buffers{ std::move( bufs ) }
			{
			}

			std::array< GpuPackedBaseBufferUPtr, size_t( SubmeshData::eCount ) > buffers;
		};
		using BufferArray = std::vector< ModelBuffers >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	debugName		The debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	debugName		Le nom debug.
		 */
		C3D_API explicit ObjectBufferPool( RenderDevice const & device
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount		The wanted vertex count.
		 *\param[in]	indexCount		The wanted index count.
		 *\param[in]	submeshFlags	The components for which the result will have allocated buffers.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount		Le nombre de sommets voulus.
		 *\param[in]	indexCount		Le nombre d'indices voulus.
		 *\param[in]	submeshFlags	Les composants pour lesquels le résultat aura un buffer alloué.
		 *\return		Le tampon GPU.
		 */
		C3D_API ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, SubmeshFlags submeshFlags );
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	vertexCount		The wanted vertex count.
		 *\param[in]	indexBuffer		The index buffer to link to the result.
		 *\param[in]	submeshFlags	The components for which the result will have allocated buffers.
		 *\return		The GPU buffer.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	vertexCount		Le nombre de sommets voulus.
		 *\param[in]	indexBuffer		Le buffer d'indices à lier au résultat.
		 *\param[in]	submeshFlags	Les composants pour lesquels le résultat aura un buffer alloué.
		 *\return		Le tampon GPU.
		 */
		C3D_API ObjectBufferOffset getBuffer( VkDeviceSize vertexCount
			, ashes::BufferBase const * indexBuffer
			, SubmeshFlags submeshFlags );
		/**
		 *\~english
		 *\param[in]	buffer	The positions buffer.
		 *\return		The model buffers for the given positions buffer.
		 *\~french
		 *\param[in]	buffer	Le buffer de positions.
		 *\return		Les buffers de modèle liés au buffer de positions donné.
		 */
		C3D_API ModelBuffers const & getBuffers( ashes::BufferBase const & buffer );
		/**
		 *\~english
		 *\param[in]	buffer	The positions buffer.
		 *\return		The index buffer linked to the given positions buffer.
		 *\~french
		 *\param[in]	buffer	Le buffer de positions.
		 *\return		Le buffer d'indices lié buffer de positions donné.
		 */
		C3D_API ashes::BufferBase const & getIndexBuffer( ashes::BufferBase const & buffer );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( ObjectBufferOffset const & bufferOffset );

	private:
		C3D_API ObjectBufferOffset doGetBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, SubmeshFlags submeshFlags
			, bool isGpuComputed );
		C3D_API BufferArray::iterator doFindBuffer( VkDeviceSize vertexCount
			, VkDeviceSize indexCount
			, BufferArray & array );

	private:
		RenderDevice const & m_device;
		castor::String m_debugName;
		std::unordered_map< size_t, BufferArray > m_buffers;
		std::unordered_map< ashes::BufferBase const * , ashes::BufferBase const * > m_indexBuffers;
	};
}

#include "ObjectBufferPool.inl"

#endif
