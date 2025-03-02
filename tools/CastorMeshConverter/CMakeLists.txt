project( CastorMeshConverter )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D mesh file converter." )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	3 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/CastorMeshConverter.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/CastorMeshConverter.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
if ( WIN32 )
	find_rsc_file( ${PROJECT_NAME} bin_dos )
endif ()
add_target_min(
	${PROJECT_NAME}
	bin_dos
	""
	""
)
target_sources( ${PROJECT_NAME} 
	PRIVATE
		${CASTOR_EDITORCONFIG_FILE}
)
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
endif ()
target_include_directories( ${PROJECT_NAME} PRIVATE
	${Castor3DIncludeDirs}
	${CASTOR_SOURCE_DIR}/tools
	${CASTOR_BINARY_DIR}/tools
)
target_link_libraries( ${PROJECT_NAME} PRIVATE
	castor::SceneExporter
)
target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${CastorToolsDefinitions}
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		FOLDER "Tools"
)
install_target_ex( ${PROJECT_NAME}
	Castor3D
	Tools
	bin_dos
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
