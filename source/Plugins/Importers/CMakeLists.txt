option( CASTOR_BUILDGRP_IMPORTER "Build Castor3D Importer plugins" TRUE )
option( CASTOR_BUILD_IMPORTER_ASSIMP "Build Assimp Importer plugin (needs ASSIMP library)" TRUE )
option( CASTOR_BUILD_IMPORTER_GLTF "Build GLTF Importer plugin (needs fastgltf library)" TRUE )

function( ImportersInit )
	set( Assimp "no (Not wanted)" PARENT_SCOPE )
	set( Gltf "no (Not wanted)" PARENT_SCOPE )
endfunction( ImportersInit )

function( ImportersBuild )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( ImporterIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Importers
			${CASTOR_BINARY_DIR}/source/Plugins/Importers
			${Castor3D_MATERIAL_DIRS}
		)
		set( ImporterDependencies
			${Castor3D_MATERIAL_DEPS}
		)

		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( Build ${Gltf} )
			add_subdirectory( Plugins/Importers/GltfImporter )
			set( Gltf ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( Build ${Assimp} )
			add_subdirectory( Plugins/Importers/AssimpImporter )
			set( Assimp ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_LIST
			${CASTOR_INSTEXP_LIST}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_PLUGINS
			${CASTOR_INSTEXP_PLUGINS}
			PARENT_SCOPE
		)
	endif ()
endfunction( ImportersBuild )

function( ImportersSummary msg )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( msg_tmp "\n\n  Importer plugins:" )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( msg_tmp "${msg_tmp}\n    Assimp               ${Assimp}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( msg_tmp "${msg_tmp}\n    GLTF                 ${Gltf}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ImportersSummary )

function( ImportersSetup )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			cpack_add_component( AssimpImporter
				DISPLAY_NAME "AssimpImporter"
				DESCRIPTION "Allows the mesh import using Assimp library in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			cpack_add_component( GltfImporter
				DISPLAY_NAME "GltfImporter"
				DESCRIPTION "Allows the mesh import using fastgltf library in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
	endif ()
endfunction( ImportersSetup )
