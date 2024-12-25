cmake_minimum_required(VERSION 3.27)

project("Windows GUI Examples"
    VERSION 0.0.1
    LANGUAGES 
    C     
)

set(executable_name "windows01" CACHE STRING "Name of the exectuable to be produced" FORCE) 

# name the exectuable that will be created
# WIN32 will cause the entry point to be winMain
add_executable( ${executable_name} WIN32)

# Set what version of C will be used 
set_target_properties( ${executable_name}  
    PROPERTIES
        C_STANDARD              23
        C_STANDARD_REQUIRED     TRUE
        C_EXTENSIONS            FALSE
        
)

target_sources( ${executable_name}
    PRIVATE
    main.c 
)


if((CMAKE_C_COMPILER_ID  STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    target_compile_options( ${executable_name}
    PUBLIC
    -municode -mwindows
    )
    target_link_options( ${executable_name} 
        PUBLIC
        -municode -mwindows
    )
endif()

