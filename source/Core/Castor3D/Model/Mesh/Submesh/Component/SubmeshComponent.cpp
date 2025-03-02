#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

CU_ImplementSmartPtr( castor3d, SubmeshComponent )

namespace castor3d
{
	SubmeshComponent::SubmeshComponent( Submesh & submesh
		, castor::String const & type
		, uint32_t id )
		: castor::OwnedBy< Submesh >{ submesh }
		, m_type{ type }
		, m_id{ id }
	{
	}

	bool SubmeshComponent::initialise( RenderDevice const & device )
	{
		if ( !m_initialised || m_dirty )
		{
			m_initialised = doInitialise( device );
		}

		return m_initialised;
	}

	void SubmeshComponent::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			doCleanup( device );
		}
	}

	void SubmeshComponent::upload( UploadData & uploader )
	{
		if ( m_initialised && m_dirty )
		{
			doUpload( uploader );
			m_dirty = false;
		}
	}
}
