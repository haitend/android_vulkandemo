#include <jni.h>
#include <android/native_window_jni.h>
#include "utils/util.hpp"
extern "C" {

JNIEXPORT void JNICALL
Java_com_vulkan_demo_VulkanDemo_setSurface(
        JNIEnv *env, jclass type,jobject surface) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr) {
        return;
    }
    vulkan_demo(window);
}
}
