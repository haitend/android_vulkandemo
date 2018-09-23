
#include "utils/util_init.hpp"
struct Vertex {
    float posX, posY, posZ, posW;  // Position data
    float r, g, b, a;              // Color
};
static const Vertex Data[] = {
        -1.f,-1.f,-1.f,1.f,1.f,1.f,0.f,1.f,
        1.f,-1.f,-1.f,1.f,1.f,1.f,0.f,1.f,
        -1.f,1.f,-1.f,1.f,1.f,1.f,0.f,1.f
};
static const char *vertShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, binding = 0) uniform bufferVals {\n"
    "    mat4 mvp;\n"
    "} myBufferVals;\n"
    "layout (location = 0) in vec4 pos;\n"
    "layout (location = 1) in vec4 inColor;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = inColor;\n"
    "   gl_Position = myBufferVals.mvp * pos;\n"
            "}\n";

static const char *fragShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (location = 0) in vec4 color;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = color;\n"
    "}\n";
struct sample_info info = {};
VkFence drawFence;
VkSemaphore imageAcquiredSemaphore;
int vulkan_demo(ANativeWindow* window) {

    char sample_title[] = "vulkan demo";

    //load  vulkan so
    init_global_layer_properties(info);

    init_instance_extension_names(info);

    init_device_extension_names(info);
    //vulkan instance
    init_instance(info, sample_title);
    //get device
    init_enumerate_device(info);
    //screen size
    init_window_size(info, 10, 10,window);

    init_swapchain_extension(info,window);
    //logic device
    init_device(info);

    init_command_pool(info);

    init_command_buffer(info);

    execute_begin_command_buffer(info);

    init_device_queue(info);

    init_swap_chain(info);

    init_uniform_buffer(info);

    init_descriptor_and_pipeline_layouts(info, false);

    init_descriptor_pool(info, false);

    init_descriptor_set(info, false);

    init_renderpass(info);

    init_shaders(info, vertShaderText, fragShaderText);

    init_framebuffers(info);

    init_vertex_buffer(info, Data, sizeof(Data), sizeof(Data[0]), false);

    init_pipeline_cache(info);
    //fixed-function
    init_pipeline(info);

    /* VULKAN_KEY_START */
    acquireNextImage(info,imageAcquiredSemaphore);

    buildCommand(info);

    submitCommand(info,drawFence,imageAcquiredSemaphore);

    present(info,drawFence);

    wait_seconds(1);

    vkDestroySemaphore(info.device, imageAcquiredSemaphore, NULL);
    vkDestroyFence(info.device, drawFence, NULL);
    destroy_pipeline(info);
    destroy_pipeline_cache(info);
    destroy_descriptor_pool(info);
    destroy_vertex_buffer(info);
    destroy_framebuffers(info);
    destroy_shaders(info);
    destroy_renderpass(info);
    destroy_descriptor_and_pipeline_layouts(info);
    destroy_uniform_buffer(info);
    destroy_swap_chain(info);
    destroy_command_buffer(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_window(info);
    destroy_instance(info);
    return 0;
}