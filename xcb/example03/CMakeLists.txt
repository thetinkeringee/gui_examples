cmake_minimum_required(VERSION 3.27)

project("XCB GUI Examples"
    VERSION 0.0.1
    LANGUAGES 
    C 
)

set(executable_name "example03" CACHE STRING "Name of the exectuable to be produced" FORCE) 

# name the exectuable that will be created
add_executable( ${executable_name} )

# Set what version of C will be used 
set_target_properties( ${executable_name}  
    PROPERTIES
        C_STANDARD              23
        C_STANDARD_REQUIRED     TRUE
        C_EXTENSIONS            FALSE
)

# This will find xcb, xkb, and xlib among other files.
# see https://cmake.org/cmake/help/latest/module/FindX11.html
find_package(X11) 

if (NOT X11_xcb_FOUND OR NOT X11_xcb_util_FOUND)

    message(FATAL_ERROR "Unable to find xcb or xcb-util")

else()

# Specify the libraries to link
target_link_libraries( ${executable_name}
    PRIVATE
    X11::xcb
    X11::xcb_util
)

# Add the actual source files to be compiled
target_sources( ${executable_name}
    PRIVATE
    main.c 
    util.c
)

endif()

