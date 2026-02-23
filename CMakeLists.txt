cmake_minimum_required(VERSION 3.16)
project(DialSome VERSION 0.1 LANGUAGES CXX)

# Extra Metadatas
add_definitions(-DGITHUB_REPO="https://github.com/BiltuDas1/DialSome")
add_definitions(-DGITHUB_CONFIGURATIONS="https://raw.githubusercontent.com/BiltuDas1/DialSome/refs/heads/main/settings.ini")

find_package(Qt6 REQUIRED COMPONENTS
    Quick
    QuickControls2
    Network
    WebSockets
)

qt_standard_project_setup(REQUIRES 6.8)

# OpenSSL
if (ANDROID)
    include(${CMAKE_CURRENT_SOURCE_DIR}/libraries/android_openssl/android_openssl.cmake)
else()
    find_package(OpenSSL REQUIRED)
endif()

qt_add_executable(DialSome
    main.cpp
)

if (ANDROID)
    add_android_openssl_libraries(DialSome)
endif()

qt_add_qml_module(DialSome
    URI DialSome
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        backend.h backend.cpp
)

qt_add_resources(DialSome "configs"
    PREFIX "/"
    FILES qtquickcontrols2.conf
)

target_link_libraries(DialSome PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::WebSockets
)

if(ANDROID)
    set_property(TARGET DialSome PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/android)
endif()
