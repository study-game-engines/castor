option( CASTOR_BUILDGRP_TONEMAPS "Build Castor3D Tone mapping plug-ins" TRUE )
option( CASTOR_BUILD_TONEMAP_HAARM "Build Haarm Pieter Duiker Tone mapping plug-in" TRUE )
option( CASTOR_BUILD_TONEMAP_HEJL "Build Hejl Burgess Dawson Tone mapping plug-in" TRUE )
option( CASTOR_BUILD_TONEMAP_REINHARD "Build Reinhard Tone mapping plug-in" TRUE )
option( CASTOR_BUILD_TONEMAP_UNCHARTED2 "Build Uncharted 2 Tone mapping plug-in" TRUE )
option( CASTOR_BUILD_TONEMAP_ACES "Build ACES Tone mapping plug-in" TRUE )

function( ToneMappingsInit )
	set( Linear "no (Not wanted)" PARENT_SCOPE )
	set( Haarm "no (Not wanted)" PARENT_SCOPE )
	set( Hejl "no (Not wanted)" PARENT_SCOPE )
	set( Reinhard "no (Not wanted)" PARENT_SCOPE )
	set( Uncharted2 "no (Not wanted)" PARENT_SCOPE )
	set( Aces "no (Not wanted)" PARENT_SCOPE )
endfunction( ToneMappingsInit )

function( ToneMappingsBuild )
	set( ToneMappingIncludes
		${CASTOR_SOURCE_DIR}/include/Core
		${CASTOR_BINARY_DIR}/include/Core
		${CASTOR_SOURCE_DIR}/source/Plugins/ToneMappings
		${CASTOR_BINARY_DIR}/source/Plugins/ToneMappings
	)

	set( Build ${Linear} )
	add_subdirectory( Plugins/ToneMappings/LinearToneMapping )
	set( Linear ${Build} PARENT_SCOPE )

	if ( ${CASTOR_BUILDGRP_TONEMAPS} )
		if( ${CASTOR_BUILD_TONEMAP_HAARM} )
			set( Build ${Haarm} )
			add_subdirectory( Plugins/ToneMappings/HaarmPieterDuikerToneMapping )
			set( Haarm ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TONEMAP_HEJL} )
			set( Build ${Hejl} )
			add_subdirectory( Plugins/ToneMappings/HejlBurgessDawsonToneMapping )
			set( Hejl ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TONEMAP_REINHARD} )
			set( Build ${Reinhard} )
			add_subdirectory( Plugins/ToneMappings/ReinhardToneMapping )
			set( Reinhard ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TONEMAP_UNCHARTED2} )
			set( Build ${Uncharted2} )
			add_subdirectory( Plugins/ToneMappings/Uncharted2ToneMapping )
			set( Uncharted2 ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TONEMAP_ACES} )
			set( Build ${Aces} )
			add_subdirectory( Plugins/ToneMappings/ACESToneMapping )
			set( Aces ${Build} PARENT_SCOPE )
		endif()
	endif ()

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
endfunction( ToneMappingsBuild )

function( ToneMappingsSummary msg )
	set( msg_tmp "\n\n  ToneMapping plugins :" )
	set( msg_tmp "${msg_tmp}\n    Linear               ${Linear}" )
	if ( ${CASTOR_BUILDGRP_TONEMAPS} )
		if( ${CASTOR_BUILD_TONEMAP_HAARM} )
			set( msg_tmp "${msg_tmp}\n    Haarm Pieter Duiker  ${Haarm}" )
		endif ()
		if( ${CASTOR_BUILD_TONEMAP_HEJL} )
			set( msg_tmp "${msg_tmp}\n    Hejl Burgess Dawson  ${Hejl}" )
		endif ()
		if( ${CASTOR_BUILD_TONEMAP_REINHARD} )
			set( msg_tmp "${msg_tmp}\n    Reinhard             ${Reinhard}" )
		endif ()
		if( ${CASTOR_BUILD_TONEMAP_UNCHARTED2} )
			set( msg_tmp "${msg_tmp}\n    Uncharted2           ${Uncharted2}" )
		endif ()
		if( ${CASTOR_BUILD_TONEMAP_ACES} )
			set( msg_tmp "${msg_tmp}\n    ACES                 ${Aces}" )
		endif ()
	endif ()

	set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( ToneMappingsSummary )

function( ToneMappingsSetup )
	cpack_add_component( LinearToneMapping
		DISPLAY_NAME "Linear ToneMapping"
		DESCRIPTION "Allows Linear Tone mapping."
		GROUP Core
		DEPENDS Castor3D
		INSTALL_TYPES Minimal Full
	)
	if ( ${CASTOR_BUILDGRP_TONEMAPS} )
		if( ${CASTOR_BUILD_TONEMAP_HAARM} )
			cpack_add_component( HaarmPieterDuikerToneMapping
				DISPLAY_NAME "Haarm Pieter Duiker ToneMapping"
				DESCRIPTION "Allows Haarm Pieter Duiker Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif()
		if( ${CASTOR_BUILD_TONEMAP_HEJL} )
			cpack_add_component( HejlBurgessDawsonToneMapping
				DISPLAY_NAME "Hejl Burgess Dawson ToneMapping"
				DESCRIPTION "Allows Haarm Peter Duiker Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif()
		if( ${CASTOR_BUILD_TONEMAP_REINHARD} )
			cpack_add_component( ReinhardToneMapping
				DISPLAY_NAME "Reinhard ToneMapping"
				DESCRIPTION "Allows Reinhard Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif()
		if( ${CASTOR_BUILD_TONEMAP_UNCHARTED2} )
			cpack_add_component( Uncharted2ToneMapping
				DISPLAY_NAME "Uncharted 2 ToneMapping"
				DESCRIPTION "Allows Uncharted 2 Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif()
		if( ${CASTOR_BUILD_TONEMAP_ACES} )
			cpack_add_component( ACESToneMapping
				DISPLAY_NAME "ACES ToneMapping"
				DESCRIPTION "Allows ACES Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif()
	endif ()
endfunction( ToneMappingsSetup )
