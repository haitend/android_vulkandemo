#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <android/native_window_jni.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <unistd.h>
#include <android/log.h>
#include "../vulkan_wrapper/vulkan_wrapper.h" // Include Vulkan_wrapper and dynamically load symbols.
#include <vulkan/vulkan.h>

#define NUM_DESCRIPTOR_SETS 1

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

#define FENCE_TIMEOUT 100000000

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
    {                                                                          \
        info.fp##entrypoint =                                                  \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
        if (info.fp##entrypoint == NULL) {                                     \
            std::cout << "vkGetDeviceProcAddr failed to find vk" #entrypoint;  \
            exit(-1);                                                          \
        }                                                                      \
    }

#define U_ASSERT_ONLY
typedef struct _swap_chain_buffers {
    VkImage image;
    VkImageView view;
} swap_chain_buffer;

typedef struct {
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
} layer_properties;

struct sample_info {
    PFN_vkCreateAndroidSurfaceKHR fpCreateAndroidSurfaceKHR;

    VkSurfaceKHR surface;

    std::vector<const char *> instance_layer_names;
    std::vector<const char *> instance_extension_names;
    std::vector<layer_properties> instance_layer_properties;
    VkInstance inst;

    std::vector<const char *> device_extension_names;
    std::vector<VkPhysicalDevice> gpus;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    VkPhysicalDeviceProperties gpu_props;
    std::vector<VkQueueFamilyProperties> queue_props;
    VkPhysicalDeviceMemoryProperties memory_properties;

    VkFramebuffer *framebuffers;
    int width, height;
    VkFormat format;

    uint32_t swapchainImageCount;
    VkSwapchainKHR swap_chain;
    std::vector<swap_chain_buffer> buffers;

    VkCommandPool cmd_pool;

    struct {
        VkFormat format;

        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;


    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform_data;

    struct {
        VkDescriptorImageInfo image_info;
    } texture_data;

    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } vertex_buffer;
    VkVertexInputBindingDescription vi_binding;
    VkVertexInputAttributeDescription vi_attribs[2];

    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 Clip;
    glm::mat4 MVP;

    VkCommandBuffer cmd; // Buffer for initialization commands
    VkPipelineLayout pipeline_layout;
    std::vector<VkDescriptorSetLayout> desc_layout;
    VkPipelineCache pipelineCache;
    VkRenderPass render_pass;
    VkPipeline pipeline;

    VkPipelineShaderStageCreateInfo shaderStages[2];

    VkDescriptorPool desc_pool;
    std::vector<VkDescriptorSet> desc_set;

    uint32_t current_buffer;
    uint32_t queue_family_count;

};

bool memory_type_from_properties(struct sample_info &info, uint32_t typeBits,
                                 VkFlags requirements_mask,
                                 uint32_t *typeIndex);

bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader,
               std::vector<unsigned int> &spirv);
void init_glslang();
void finalize_glslang();
void wait_seconds(int seconds);

int vulkan_demo(ANativeWindow* window);

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VK-SAMPLE", __VA_ARGS__))
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "VK-SAMPLE", __VA_ARGS__);

void AndroidGetWindowSize(int32_t *width, int32_t *height,ANativeWindow* window);
