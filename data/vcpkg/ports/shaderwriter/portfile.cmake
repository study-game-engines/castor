vcpkg_from_github(OUT_SOURCE_PATH SOURCE_PATH
    REPO DragonJoker/ShaderWriter
    REF c6626373eb3714fad26f8008849818e97deefb20
    HEAD_REF development
    SHA512 c92ac248d24a20112d879cb7720619be2cb13bd5e77e85cd791aa1236e53f1327ba09246615610250b2fd11bba0a326f666c6563272defdb9df066bf7db0ba31
)

vcpkg_from_github(OUT_SOURCE_PATH CMAKE_SOURCE_PATH
    REPO DragonJoker/CMakeUtils
    REF 4e0292ed50d76dab5fc8c81840ae0e021dc60c2a
    HEAD_REF master
    SHA512 c79c6a5ef2e059b56d4de20cc73e74386bf8b6acea2f6b76fd9949a6a2760f82302c90419e4a753f50c30d01cc4f3a039e04b585f5c0d4461cce3464d9fb9c95
)

file(REMOVE_RECURSE "${SOURCE_PATH}/CMake")
file(COPY "${CMAKE_SOURCE_PATH}/" DESTINATION "${SOURCE_PATH}/CMake")

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" BUILD_STATIC)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        spirv SDW_BUILD_EXPORTER_SPIRV
        glsl  SDW_BUILD_EXPORTER_HLSL
        hlsl  SDW_BUILD_EXPORTER_GLSL
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF
        -DSDW_GENERATE_SOURCE=OFF
        -DSDW_BUILD_VULKAN_LAYER=OFF
        -DSDW_BUILD_TESTS=OFF
        -DSDW_BUILD_STATIC_SDW=${BUILD_STATIC}
        -DSDW_BUILD_STATIC_SDAST=${BUILD_STATIC}
        -DSDW_UNITY_BUILD=ON
        ${FEATURE_OPTIONS}
)

vcpkg_copy_pdbs()
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/shaderwriter)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
