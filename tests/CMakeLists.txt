# Testing library
set (${self} test_${PROJECT_NAME})
# ph_fetch (catch2)
# find_package(Catch2 REQUIRED)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE HEADER_LIST "include/*.hpp")
file(GLOB SOURCE_LIST *.cpp)
add_executable (test_${PROJECT_NAME} ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries (test_${PROJECT_NAME} PUBLIC ${PROJECT_NAME} Catch2::Catch2 ${Boost_LIBRARIES})
target_include_directories (test_${PROJECT_NAME} PRIVATE include PUBLIC ${PROJECT_NAME} ${Boost_INCLUDE_DIRS})
ph_precompile (TARGET test_${PROJECT_NAME} PRIVATE PCH ${std_pch})
# add_test (NAME testtest_${PROJECT_NAME} COMMAND ${self}) 


if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_target_properties(test_${PROJECT_NAME}          PROPERTIES FOLDER "${PROJECT_NAME}")
else()
    set_target_properties(test_${PROJECT_NAME}           PROPERTIES FOLDER "dependencies/${PROJECT_NAME}")
endif()
# add_custom_target(
#   GenexDemo
#   COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:test_${PROJECT_NAME}>"
#   VERBATIM
# )


add_custom_command(
  TARGET test_${PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:test_${PROJECT_NAME}>"
#   VERBATIM
)
