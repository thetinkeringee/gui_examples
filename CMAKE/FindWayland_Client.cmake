cmake_minimum_required(VERSION 3.20)
# 
# Defines
#   WAYLAND_CLIENT_FOUND to TRUE  if Wayland client  libraries are found 
#   WAYLAND_CLIENT_LIBRARIES      The library
#   WAYLAND_CLIENT_LINK_LIBRARIES The full path to the library
#   WAYLAND_CLIENT_INCLUDE_DIRS
#  

project("Fix CMAKE"
    VERSION 0.0.1
    LANGUAGES 
    C 
)

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(WAYLAND_CLIENT QUIET wayland-client)
endif()

find_program(WAYLAND_SCANNER "wayland-scanner")

if (WAYLAND_SCANNER STREQUAL "WAYLAND_SCANNER-NOTFOUND") 
message(FATAL_ERROR "Unable to locate wayland-scanner")
else()
message(STATUS "wayland-scanner found")
endif()

