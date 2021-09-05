cmake_minimum_required(VERSION 3.18)

find_program (MEMORYCHECK_COMMAND valgrind)
set (MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")


project (neural_networks CXX)



if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	# include (CTest)
	set (CMAKE_CXX_STANDARD 20)
	set (CMAKE_CXX_STANDARD_REQUIRED ON)
	set (CMAKE_CXX_EXTENSIONS ON)
	set_property (GLOBAL PROPERTY USE_FOLDERS ON)
	# find_package(OpenSSL REQUIRED)
endif()

if (NOT TARGET ph_cmake)
	include (FetchContent)
	FetchContent_Declare(
  		ph_cmake
  		GIT_REPOSITORY https://github.com/phiwen96/ph_cmake.git
  		GIT_TAG        0.0.0)

	FetchContent_GetProperties(ph_cmake)
	
	if (NOT ph_cmake_POPULATED)
  		FetchContent_Populate (ph_cmake)
  		add_subdirectory (${ph_cmake_SOURCE_DIR} ${ph_cmake_BINARY_DIR})
	endif ()
endif ()


# set(GLFW_BUILD_EXAMPLES OFF)
# set(GLFW_BUILD_TESTS OFF)
# set(GLFW_BUILD_DOCS OFF)

# Fetch (TARGETS "glfw" REPO "glfw" TAG "3.3.4" )



Fetch (TARGETS ph_data_structures ph_concepts ph_string)

# find_package (OpenSSL REQUIRED)

# find_package(Vulkan REQUIRED)



# if (NOT TARGET ctre)
# 	include (FetchContent)
# 	FetchContent_Declare(
#   		ctre
#   		GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
#   		GIT_TAG        main)

# 	FetchContent_GetProperties(ctre)
	
# 	if (NOT ctre_POPULATED)
#   		FetchContent_Populate (ctre)
#   		add_subdirectory (${ctre_SOURCE_DIR} ${ctre_BINARY_DIR})
# 	endif ()
# endif ()

 
# find_package (Boost 1.76.0 REQUIRED COMPONENTS system)

# ph_fetch_git_repos (USER "phiwen96" REPOS "ph_concepts" "ph_system")

# ph_create_modules (

# 	MODULES 
# 		"network"
# 		"common"

# 	INCLUDE
# 		"${CMAKE_CURRENT_LIST_DIR}/include/neural_networks"

# 	SRC
# 		"${CMAKE_CURRENT_LIST_DIR}/src"
# )

add_subdirectory (src)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	add_subdirectory (tests)
endif ()


# cmake -Wno-dev -DCMAKE_CONFIGURATION_TYPES="Debug;Release;Kuk" -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/usr/bin/g++ -S . -B xbuild -GXcode && cmake --open xbuild
# cmake -Wno-dev -DCMAKE_CONFIGURATION_TYPES="Debug;Release;Kuk" -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/usr/bin/g++ -S . -B xbuild -GXcode