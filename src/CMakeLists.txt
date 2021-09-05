# find_package(Boost REQUIRED)

file(GLOB_RECURSE HEADER_LIST "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB SOURCE_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library (ph_neural_networks
	${SOURCE_LIST}  ${HEADER_LIST} 
)

# target_sources (ph_neural_networks PRIVATE in/platform.hpp)
set (self ${PROJECT_NAME})

set_target_properties (${self} PROPERTIES CXX_STANDARD 20)
target_compile_features (${self} PUBLIC cxx_std_20)




# add_custom_target(
#     custom_header_target
#     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/custom_header.h
# )
# ph_precompile (TARGET ph_concepts PRIVATE PCH ${std_pch} ${boost_pch})

# add_library (ph::network ALIAS ph_neural_networks)
# ${Vulkan_INCLUDE_DIR}
# ${Vulkan_LIBRARIES}
target_link_libraries (ph_neural_networks PUBLIC ph_string ph_concepts ph_data_structures)
target_include_directories (ph_neural_networks PUBLIC ph_string ph_concepts ph_data_structures ${PROJECT_SOURCE_DIR}/include)
# target_include_directories (ph_neural_networks PUBLIC ${Boost_INCLUDE_DIRS})
#  OpenSSL::SSL
target_compile_features (ph_neural_networks PUBLIC cxx_std_20)

option(JS_ONLY "Compiles to native JS (No WASM)" OFF)


if(JS_ONLY)
	message(STATUS "Setting compilation target to native JavaScript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(ph_neural_networks PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORTED_FUNCTIONS='[_main]'")
else(JS_ONLY)
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")
	set_target_properties(ph_neural_networks PROPERTIES LINK_FLAGS "-s WASM=1 -s EXPORTED_FUNCTIONS='[_main]'")
endif(JS_ONLY)

# target_compile_options(ph_neural_networks PUBLIC "-ast-dump  -fsyntax-only")
# echo 'export LDFLAGS="-L/opt/homebrew/opt/openssl@1.1/lib"' >> ~/.zshrc
# echo 'export CPPFLAGS="-I/opt/homebrew/opt/openssl@1.1/include"' >> ~/.zshrc
target_include_directories(ph_neural_networks PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/ph_neural_networks>
    $<INSTALL_INTERFACE:include/ph_neural_networks>
)

target_include_directories(ph_neural_networks PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

else()
    set_target_properties(${PROJECT_NAME}          PROPERTIES FOLDER "dependencies")
endif()
