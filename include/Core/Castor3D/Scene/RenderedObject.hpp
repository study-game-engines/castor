/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderedObject_H___
#define ___C3D_RenderedObject_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class RenderedObject
	{
	public:
		C3D_API void fillEntry( uint32_t nodeId
			, Pass const & pass
			, SceneNode const & sceneNode
			, uint32_t meshletCount
			, ModelBufferConfiguration & modelData );
		C3D_API void fillEntryOffsets( uint32_t nodeId
			, VkDeviceSize vertexOffset
			, VkDeviceSize indexOffset
			, VkDeviceSize meshletOffset );

		void setVisible( bool value )
		{
			m_visible = value;
		}

		void setShadowCaster( bool value )
		{
			m_castsShadows = value;
		}

		void setShadowReceiver( bool value )
		{
			m_receivesShadows = value;
		}

		void setCullable( bool value )
		{
			m_cullable = value;
		}

		bool isVisible()const
		{
			return m_visible;
		}

		bool isShadowCaster()const
		{
			return m_castsShadows;
		}

		bool isShadowReceiver()const
		{
			return m_receivesShadows;
		}

		bool isCullable()const
		{
			return m_cullable;
		}

	private:
		struct Offsets
		{
			VkDeviceSize vertexOffset{};
			VkDeviceSize indexOffset{};
			VkDeviceSize meshletOffset{};
		};

	private:
		bool m_visible{ true };
		bool m_castsShadows{ true };
		bool m_receivesShadows{ true };
		bool m_cullable{ true };
		bool m_firstUpdate{ true };
		std::unordered_map< uint32_t, std::pair< ModelBufferConfiguration *, Offsets > > m_modelsDataOffsets{};
	};
}

#endif
