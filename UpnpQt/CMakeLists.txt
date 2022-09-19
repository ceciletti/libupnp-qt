set(upnpqt_SRC
    discover.cpp
    service_p.h
    service.cpp
    device.cpp
    internetgatewaydevice.cpp
    wanconnectiondevice.cpp
    wanconnectionservice.cpp
    reply.cpp
    soapenvelope.cpp
    soapenvelope.h
)
set(upnpqt_HEADERS
    global.h
    discover.h
    service.h
    device.h
    internetgatewaydevice.h
    wanconnectiondevice.h
    wanconnectionservice.h
    reply.h
)
add_library(UpnpQt
    ${upnpqt_SRC}
    ${upnpqt_HEADERS}
)

configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(UpnpQt::Core ALIAS UpnpQt)

#Set target properties
target_include_directories(UpnpQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
        $<INSTALL_INTERFACE:include/upnp-qt5>
    PRIVATE
        ${CMAKE_BINARY_DIR}
)

target_compile_features(UpnpQt PRIVATE cxx_auto_type)
target_compile_options(UpnpQt PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

target_link_libraries(UpnpQt
    PUBLIC
        Qt5::Core
        Qt5::Network
    PRIVATE
        Qt5::Xml
)

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/UpnpQt)

#This is required so that the exported target has the name UpnpQt and not upnpqt
set_target_properties(UpnpQt PROPERTIES
    EXPORT_NAME Core
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)
set_property(TARGET UpnpQt PROPERTY PUBLIC_HEADER ${upnpqt_HEADERS})

install(TARGETS UpnpQt
    EXPORT upnpqt-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION include/upnp-qt5/UpnpQt COMPONENT devel
)

#install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#Export the targets to a script
install(EXPORT upnpqt-targets
  FILE
    UpnpQtTargets.cmake
  NAMESPACE
    UpnpQt::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/UpnpQtConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/UpnpQtConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/UpnpQtConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/UpnpQtConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/UpnpQtConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT upnpqt-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/UpnpQtTargets.cmake NAMESPACE UpnpQt::)

#Register package in user's package registry
export(PACKAGE UpnpQt)
