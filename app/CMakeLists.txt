# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.
add_subdirectory(src/main/cpp/utils src/main/cpp/utils)

add_library(vulkan_demo SHARED
            src/main/cpp/vulkandemo.cpp
            src/main/cpp/vulkan_wrapper/vulkan_wrapper.cpp
            src/main/cpp/vulkandemo_jni.cpp
            )

target_include_directories(vulkan_demo PRIVATE
                           ${CMAKE_SOURCE_DIR}/../data
                           ${GLMINC_PREFIX}
                           ${SHADERC_SRC}/third_party/spirv-tools/include
                           ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
                           ${CMAKE_SOURCE_DIR}/../android/vulkan_wrapper)

get_filename_component(SHADERC_SRC
                       ${ANDROID_NDK}/sources/third_party/shaderc
                       ABSOLUTE)
add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
                      ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

# Build application's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D VK_USE_PLATFORM_ANDROID_KHR")

target_link_libraries( # Specifies the target library.
                       vulkan_demo

                       # Links the target library to the log library
                       # included in the NDK.
                       log
                       utils
                       shaderc_lib)
