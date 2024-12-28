cmake_minimum_required(VERSION 3.27)

project("Wayland GUI Examples"
    VERSION 0.0.1
    LANGUAGES 
    C 
)

set(executable_name "wayland_01" CACHE STRING "Name of the exectuable to be produced" FORCE) 

# name the exectuable that will be created
add_executable( ${executable_name} )

# Set what version of C will be used 
set_target_properties( ${executable_name}  
    PROPERTIES
        C_STANDARD              23
        C_STANDARD_REQUIRED     TRUE
        C_EXTENSIONS            FALSE
)
cmake_path(SET module_path NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../../CMAKE")
message(STATUS ${module_path})
list(APPEND CMAKE_MODULE_PATH ${module_path})
message(STATUS ${CMAKE_MODULE_PATH})

find_package(Wayland_Client) 

if(WAYLAND_CLIENT_FOUND)

# Directory in which generated source will be stored
set(GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GEN_DIR} )

# Generate the needed xdg-shell code
execute_process(
COMMAND ${WAYLAND_SCANNER} private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml  xdg-shell.c
WORKING_DIRECTORY ${GEN_DIR}
)

# Generate the client header for xdg-shell
execute_process(
COMMAND ${WAYLAND_SCANNER} client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell-client.h 
WORKING_DIRECTORY ${GEN_DIR}
)

target_include_directories( ${executable_name}
    PRIVATE
    ${GEN_DIR}
    
)

# Specify the libraries to link
target_link_libraries( ${executable_name}
    PRIVATE
    ${WAYLAND_CLIENT_LINK_LIBRARIES}
)

# Add the actual source files to be compiled
target_sources( ${executable_name}
    PRIVATE
    main.c 
    ${GEN_DIR}/xdg-shell.c
)






else()
    message(FATAL_ERROR "Unable to find wayland-client")

endif()

