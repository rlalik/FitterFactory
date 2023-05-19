if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/fitemall-${PROJECT_VERSION}"
      CACHE PATH ""
  )
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package FitemAll)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT FitemAll_Development
)

install(
    TARGETS fitemall
    EXPORT FitemAllTargets
    RUNTIME #
    COMPONENT FitemAll_Runtime
    LIBRARY #
    COMPONENT FitemAll_Runtime
    NAMELINK_COMPONENT FitemAll_Development
    ARCHIVE #
    COMPONENT FitemAll_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    FitemAll_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(FitemAll_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${FitemAll_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT FitemAll_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${FitemAll_INSTALL_CMAKEDIR}"
    COMPONENT FitemAll_Development
)

install(
    EXPORT FitemAllTargets
    NAMESPACE fitemall::
    DESTINATION "${FitemAll_INSTALL_CMAKEDIR}"
    COMPONENT FitemAll_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
