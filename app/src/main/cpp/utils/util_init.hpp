
#ifndef UTIL_INIT
#define UTIL_INIT

#include "util.hpp"

VkResult init_global_layer_properties(sample_info &info);

void init_instance_extension_names(struct sample_info &info);
VkResult init_instance(struct sample_info &info,
                       char const *const app_short_name);
void init_device_extension_names(struct sample_info &info);
VkResult init_device(struct sample_info &info);
VkResult init_enumerate_device(struct sample_info &info,
                               uint32_t gpu_count = 1);

void init_swapchain_extension(struct sample_info &info,ANativeWindow* window);
void init_command_pool(struct sample_info &info);
void init_command_buffer(struct sample_info &info);
void execute_begin_command_buffer(struct sample_info &info);
void init_device_queue(struct sample_info &info);
void init_swap_chain(
    struct sample_info &info,
    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                   VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
void init_uniform_buffer(struct sample_info &info);
void init_descriptor_and_pipeline_layouts(struct sample_info &info,
                                          bool use_texture);
void init_renderpass(
    struct sample_info &info,  bool clear = true,
    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
void init_vertex_buffer(struct sample_info &info, const void *vertexData,
                        uint32_t dataSize, uint32_t dataStride,
                        bool use_texture);
void init_framebuffers(struct sample_info &info);
void init_descriptor_pool(struct sample_info &info, bool use_texture);
void init_descriptor_set(struct sample_info &info, bool use_texture);
void init_shaders(struct sample_info &info, const char *vertShaderText,
                  const char *fragShaderText);
void init_pipeline_cache(struct sample_info &info);
void init_pipeline(struct sample_info &info, VkBool32 include_vi = true);

void init_window_size(struct sample_info &info, int32_t default_width,
                      int32_t default_height,ANativeWindow* nativeWindow);
void acquireNextImage(struct sample_info &info,VkSemaphore& imageAcquiredSemaphore);
void buildCommand(struct sample_info &info);
void submitCommand(struct sample_info &info,VkFence& drawFence,VkSemaphore imageAcquiredSemaphore);
void present(struct sample_info &info,VkFence& drawFence);
void destroy_pipeline(struct sample_info &info);
void destroy_pipeline_cache(struct sample_info &info);
void destroy_descriptor_pool(struct sample_info &info);
void destroy_vertex_buffer(struct sample_info &info);
void destroy_framebuffers(struct sample_info &info);
void destroy_shaders(struct sample_info &info);
void destroy_renderpass(struct sample_info &info);
void destroy_descriptor_and_pipeline_layouts(struct sample_info &info);
void destroy_uniform_buffer(struct sample_info &info);
void destroy_swap_chain(struct sample_info &info);
void destroy_command_buffer(struct sample_info &info);
void destroy_command_pool(struct sample_info &info);
void destroy_device(struct sample_info &info);
void destroy_instance(struct sample_info &info);
void destroy_window(struct sample_info &info);

#endif // UTIL_INIT
