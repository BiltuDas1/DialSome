cmake_minimum_required(VERSION 3.16)
project(DialSome VERSION 0.1 LANGUAGES CXX)

# Standard setup (handles AUTOMOC, AUTOUIC, and GNUInstallDirs)
find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.8)

# Install OpenSSL
if (ANDROID)
    include(${CMAKE_CURRENT_SOURCE_DIR}/libraries/android_openssl/android_openssl.cmake)
endif()

# Define the executable
qt_add_executable(DialSome
    main.cpp
)

# Attach OpenSSL
if (ANDROID)
    add_android_openssl_libraries(DialSome)
endif()

# Add the QML Module
# This handles C++ (backend), QML (Main), and internal resource registration
qt_add_qml_module(DialSome
    URI DialSome
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        backend.h backend.cpp
)

# Handle extra resources (Config files, etc.)
qt_add_resources(DialSome "configs"
    PREFIX "/"
    FILES qtquickcontrols2.conf
)

# Platform Specific Properties
set_target_properties(DialSome PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(DialSome PRIVATE Qt6::Quick)

# Android Specifics
if(ANDROID)
    set_property(TARGET DialSome PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
endif()

# Deployment (Crucial for Windows/macOS/Linux)
if(NOT ANDROID AND NOT IOS)
    include(GNUInstallDirs)
    install(TARGETS DialSome
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    # This function is not supported on mobile platforms
    qt_generate_deploy_app_script(
        TARGET DialSome
        OUTPUT_SCRIPT deploy_script
    )
    install(SCRIPT ${deploy_script})
endif()
