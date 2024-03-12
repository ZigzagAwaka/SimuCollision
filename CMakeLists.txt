cmake_minimum_required(VERSION 3.20)

project(SimuCollision)

# Set the folder where the executables are created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# include c3ga
message(STATUS "c3ga garamon cmake part ..." )
add_subdirectory(lib/garamon_c3ga INTERFACE)
include_directories(${CMAKE_SOURCE_DIR}/lib/garamon_c3ga/src/)

# set the module path for "findLib.cmake"
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

# include eigen
find_package(Eigen3)
if(${EIGEN3_FOUND})
    message(STATUS "lib EIGEN3 found")
    message(STATUS "  version " ${EIGEN3_VERSION_STRING})
    message(STATUS "  include " ${EIGEN3_INCLUDE_DIR})
else()
    message(STATUS "lib EIGEN3 manually included")
    include_directories("E:\\Universite\\M2\\geo_proj\\SimuCollision\\lib\\garamon_c3ga\\include\\eigen3") # manually specify the include location
endif()

# Include glimac
add_subdirectory(glimac)

# Create a target for each TP
function(setup_tp TP_NUMBER)
    set(TARGET_NAME SimuCollision)
    add_executable(${TARGET_NAME})
    target_compile_features(${TARGET_NAME} PRIVATE cxx_std_17)

    # Enable many good warnings
    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /WX /W3)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -W -Wall -Wextra -Wpedantic -pedantic-errors)
        #target_compile_options(${TARGET_NAME} PRIVATE -Werror -W -Wall -Wextra -Wpedantic -pedantic-errors)
    endif()
    
    # Add all the source and header files
    target_include_directories(${TARGET_NAME} PRIVATE ${TP_NUMBER})
    file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS ${TP_NUMBER}/*)
    target_sources(${TARGET_NAME} PRIVATE ${MY_SOURCES})

    # Add glimac as a dependency
    target_link_libraries(${TARGET_NAME} glimac)

    # Copy the assets and the shaders to the output folder (where the executable is created)
    include("CMakeUtils/files_and_folders.cmake")
    Cool__target_copy_folder(${TARGET_NAME} assets)
    Cool__target_copy_folder(${TARGET_NAME} ${TP_NUMBER}/shaders)
endfunction(setup_tp)

setup_tp(src)