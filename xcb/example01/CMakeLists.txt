cmake_minimum_required(VERSION 3.27)

project("window"
    VERSION 0.0.1
    LANGUAGES 
    C 
    CXX 
    
)

add_executable(window)
set_target_properties(window
    PROPERTIES
        C_STANDARD              23
        C_STANDARD_REQUIRED     TRUE
        CXX_STANDARD            20
        CXX_STANDARD_REQUIRED   TRUE 
)

# This will find xcb, xkb, and xlib
find_package(X11) 

if (NOT X11_xcb_FOUND OR NOT X11_xcb_util_FOUND)
    message(FATAL_ERROR "Unable to find xcb or xcb-util")
else()
target_link_libraries(
        window
        PRIVATE
        X11::xcb
        X11::xcb_util
)

target_sources( window
    PRIVATE
    main.c 
)

endif()

