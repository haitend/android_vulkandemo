file(GLOB UTILS_SOURCE *.cpp)

set(SAMPLES_DATA_DIR ${SAMPLES_DATA_DIR} "${PROJECT_SOURCE_DIR}/API-Samples/data")
if(SDK_INCLUDE_PATH)
    include_directories( ${SAMPLES_DATA_DIR} ${GLSLANG_SPIRV_INCLUDE_DIR} ${GLMINC_PREFIX} ${SDK_INCLUDE_PATH} )
else()
    include_directories( ${SAMPLES_DATA_DIR} ${GLSLANG_SPIRV_INCLUDE_DIR} ${GLMINC_PREFIX} )
endif()


    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -DVK_USE_PLATFORM_ANDROID_KHR")

add_library(utils STATIC ${UTILS_SOURCE})

if(ANDROID)
   target_include_directories(utils PRIVATE
                              ${ANDROID_NDK}/sources/android/native_app_glue
                              ${CMAKE_CURRENT_SOURCE_DIR}/../android/vulkan_wrapper
                              ${CMAKE_CURRENT_SOURCE_DIR}/../data
                              ${ANDROID_NDK}/sources/third_party/shaderc/include)
   target_link_libraries(utils
                              android
                              log
                              )
endif()

