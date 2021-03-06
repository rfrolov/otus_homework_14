cmake_minimum_required(VERSION 3.6)

project(yamr VERSION 1.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES *.cpp *.h)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "rfrolov")
set(CPACK_PACKAGE_CONTACT rfrolov@engineer.com)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Otus homework 14")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_BUILD $ENV{TRAVIS_BUILD_NUMBER})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

include(CPack)