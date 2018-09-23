
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "util.hpp"
#include <unordered_map>
#include "shaderc/shaderc.hpp"

using namespace std;
bool memory_type_from_properties(struct sample_info &info, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    for (uint32_t i = 0; i < info.memory_properties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((info.memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

void init_glslang() {

}

void finalize_glslang() {

}
struct shader_type_mapping {
    VkShaderStageFlagBits vkshader_type;
    shaderc_shader_kind shaderc_type;
};
static const shader_type_mapping shader_map_table[] = {
    {VK_SHADER_STAGE_VERTEX_BIT, shaderc_glsl_vertex_shader},
    {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, shaderc_glsl_tess_control_shader},
    {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, shaderc_glsl_tess_evaluation_shader},
    {VK_SHADER_STAGE_GEOMETRY_BIT, shaderc_glsl_geometry_shader},
    {VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader},
    {VK_SHADER_STAGE_COMPUTE_BIT, shaderc_glsl_compute_shader},
};
shaderc_shader_kind MapShadercType(VkShaderStageFlagBits vkShader) {
    for (auto shader : shader_map_table) {
        if (shader.vkshader_type == vkShader) {
            return shader.shaderc_type;
        }
    }
    assert(false);
    return shaderc_glsl_infer_from_source;
}

bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv) {
    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult module =
        compiler.CompileGlslToSpv(pshader, strlen(pshader), MapShadercType(shader_type), "shader");
    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        LOGE("Error: Id=%d, Msg=%s", module.GetCompilationStatus(), module.GetErrorMessage().c_str());
        return false;
    }
    spirv.assign(module.cbegin(), module.cend());
    return true;
}

void wait_seconds(int seconds) {
    sleep(seconds);
}

void AndroidGetWindowSize(int32_t *width, int32_t *height,ANativeWindow* nativeWindow) {
    *width = ANativeWindow_getWidth(nativeWindow);
    *height = ANativeWindow_getHeight(nativeWindow);
}
