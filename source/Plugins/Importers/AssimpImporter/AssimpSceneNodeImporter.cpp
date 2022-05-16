#include "AssimpImporter/AssimpSceneNodeImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace c3d_assimp
{
	AssimpSceneNodeImporter::AssimpSceneNodeImporter( castor3d::Engine & engine )
		: castor3d::SceneNodeImporter{ engine }
	{
	}

	bool AssimpSceneNodeImporter::doImportSceneNode( castor3d::SceneNode & node )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = node.getName();
		auto it = file.getNodes().find( name );

		if ( it == file.getNodes().end() )
		{
			return false;
		}

		castor3d::log::info << cuT( "  SceneNode found: [" ) << name << cuT( "]" ) << std::endl;
		auto parent = node.getScene()->tryFindSceneNode( name );

		if ( parent.lock() )
		{
			node.attachTo( *parent.lock() );
		}

		castor::Point3f scale, position;
		castor::Quaternion rotate;
		castor::matrix::decompose( it->second.transform, position, scale, rotate );
		node.setPosition( position );
		node.setScale( scale );
		node.setOrientation( rotate );

		return true;
	}
}