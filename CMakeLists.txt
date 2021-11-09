cmake_minimum_required(VERSION 3.10)

project(dbus_server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-3.0)

if(GTK_FOUND)
  # Setup CMake to use GTK+, tell the compiler where to look for headers
  # and to the linker where to look for libraries
  message("GTK_VERSION ${GTK_VERSION} ${GTK_INCLUDE_DIRS} ${GTK_LIBRARIES}")
  include_directories(${GTK_INCLUDE_DIRS})
  add_executable(dbus_server main.cpp dbus_client.cpp dbus_server.cpp)
  target_link_libraries(dbus_server ${GTK_LIBRARIES})
endif()

