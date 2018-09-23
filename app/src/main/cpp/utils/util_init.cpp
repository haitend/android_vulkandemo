
#include <cstdlib>
#include <assert.h>
#include <string.h>
#include "util_init.hpp"
using namespace std;

VkResult init_global_layer_properties(struct sample_info &info) {
    VkResult res;
    if (!InitVulkan()) {  //加载vulkan so
        LOGE("Failied initializing Vulkan APIs!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return res;
}

void init_instance_extension_names(struct sample_info &info) {
    info.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    info.instance_extension_names.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
}

VkResult init_instance(struct sample_info &info, char const *const app_short_name) {
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; //driver 可扩展
    app_info.pNext = NULL;
    app_info.pApplicationName = app_short_name; //　driver 对 应用优化，上报
    app_info.applicationVersion = 1;
    app_info.pEngineName = app_short_name;
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;//driver 可扩展
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledLayerCount = info.instance_layer_names.size();
    inst_info.ppEnabledLayerNames = info.instance_layer_names.size() ? info.instance_layer_names.data() : NULL;
    inst_info.enabledExtensionCount = info.instance_extension_names.size();
    inst_info.ppEnabledExtensionNames = info.instance_extension_names.data();

    VkResult res = vkCreateInstance(&inst_info, NULL, &info.inst);
    assert(res == VK_SUCCESS);

    return res;
}

void init_device_extension_names(struct sample_info &info) {
    info.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

VkResult init_device(struct sample_info &info) {
    VkResult res;
    VkDeviceQueueCreateInfo queue_info = {};

    float queue_priorities[1] = {0.0};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = NULL;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;
    queue_info.queueFamilyIndex = info.graphics_queue_family_index;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = info.device_extension_names.size();
    device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? info.device_extension_names.data() : NULL;
    device_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(info.gpus[0], &device_info, NULL, &info.device);
    assert(res == VK_SUCCESS);

    return res;
}

VkResult init_enumerate_device(struct sample_info &info, uint32_t gpu_count) {
    uint32_t const U_ASSERT_ONLY req_count = gpu_count;
    VkResult res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, NULL);
    assert(gpu_count);
    info.gpus.resize(gpu_count);

    res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, info.gpus.data());
    assert(!res && gpu_count >= req_count);

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, NULL);
    assert(info.queue_family_count >= 1);

    info.queue_props.resize(info.queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, info.queue_props.data());
    assert(info.queue_family_count >= 1);

    /* This is as good a place as any to do this */
    vkGetPhysicalDeviceMemoryProperties(info.gpus[0], &info.memory_properties);
    vkGetPhysicalDeviceProperties(info.gpus[0], &info.gpu_props);

    return res;
}


void init_connection(struct sample_info &info) {

}
void init_window(struct sample_info &info) {}

void destroy_window(struct sample_info &info) {}

void init_window_size(struct sample_info &info, int32_t default_width, int32_t default_height,ANativeWindow* nativeWindow) {
    AndroidGetWindowSize(&info.width, &info.height,nativeWindow);
}

void init_depth_buffer(struct sample_info &info) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    VkImageCreateInfo image_info = {};

    info.depth.format = VK_FORMAT_D24_UNORM_S8_UINT;

    const VkFormat depth_format = info.depth.format;
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], depth_format, &props);
    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
    } else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else {
        std::cout << "depth_format " << depth_format << " Unsupported.\n";
        exit(-1);
    }

    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depth_format;
    image_info.extent.width = info.width;
    image_info.extent.height = info.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = NUM_SAMPLES;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.flags = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = depth_format;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;

    if (depth_format == VK_FORMAT_D16_UNORM_S8_UINT || depth_format == VK_FORMAT_D24_UNORM_S8_UINT ||
        depth_format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkMemoryRequirements mem_reqs;

    /* Create image */
    res = vkCreateImage(info.device, &image_info, NULL, &info.depth.image);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(info.device, info.depth.image, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;
    /* Use the memory properties to determine the type of memory required */
    pass =
        memory_type_from_properties(info, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
    assert(pass);

    /* Allocate memory */
    res = vkAllocateMemory(info.device, &mem_alloc, NULL, &info.depth.mem);
    assert(res == VK_SUCCESS);

    /* Bind memory */
    res = vkBindImageMemory(info.device, info.depth.image, info.depth.mem, 0);
    assert(res == VK_SUCCESS);

    /* Create image view */
    view_info.image = info.depth.image;
    res = vkCreateImageView(info.device, &view_info, NULL, &info.depth.view);
    assert(res == VK_SUCCESS);
}

void init_swapchain_extension(struct sample_info &info,ANativeWindow* window) {
    VkResult U_ASSERT_ONLY res;

    GET_INSTANCE_PROC_ADDR(info.inst, CreateAndroidSurfaceKHR);

    VkAndroidSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    createInfo.window = window;

    res = info.fpCreateAndroidSurfaceKHR(info.inst, &createInfo, nullptr, &info.surface);

    assert(res == VK_SUCCESS);

    // Iterate over each queue to learn whether it supports presenting:
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(info.queue_family_count * sizeof(VkBool32));
    for (uint32_t i = 0; i < info.queue_family_count; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(info.gpus[0], i, info.surface, &pSupportsPresent[i]);
    }

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both
    info.graphics_queue_family_index = UINT32_MAX;
    info.present_queue_family_index = UINT32_MAX;
    for (uint32_t i = 0; i < info.queue_family_count; ++i) {
        if ((info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (info.graphics_queue_family_index == UINT32_MAX) info.graphics_queue_family_index = i;

            if (pSupportsPresent[i] == VK_TRUE) {
                info.graphics_queue_family_index = i;
                info.present_queue_family_index = i;
                break;
            }
        }
    }

    if (info.present_queue_family_index == UINT32_MAX) {
        // If didn't find a queue that supports both graphics and present, then
        // find a separate present queue.
        for (size_t i = 0; i < info.queue_family_count; ++i)
            if (pSupportsPresent[i] == VK_TRUE) {
                info.present_queue_family_index = i;
                break;
            }
    }
    free(pSupportsPresent);

    // Generate error if could not find queues that support graphics
    // and present
    if (info.graphics_queue_family_index == UINT32_MAX || info.present_queue_family_index == UINT32_MAX) {
        std::cout << "Could not find a queues for both graphics and present";
        exit(-1);
    }

    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface, &formatCount, NULL);
    assert(res == VK_SUCCESS);
    VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface, &formatCount, surfFormats);
    assert(res == VK_SUCCESS);
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        info.format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        assert(formatCount >= 1);
        info.format = surfFormats[0].format;
    }
    free(surfFormats);
}

void init_swap_chain(struct sample_info &info, VkImageUsageFlags usageFlags) {
    VkResult U_ASSERT_ONLY res;
    VkSurfaceCapabilitiesKHR surfCapabilities;

    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0], info.surface, &surfCapabilities);
    assert(res == VK_SUCCESS);

    uint32_t presentModeCount;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface, &presentModeCount, NULL);
    assert(res == VK_SUCCESS);
    VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
    assert(presentModes);
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface, &presentModeCount, presentModes);
    assert(res == VK_SUCCESS);

    VkExtent2D swapchainExtent;
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {

        swapchainExtent.width = info.width;
        swapchainExtent.height = info.height;
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        } else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
    } else {
        swapchainExtent = surfCapabilities.currentExtent;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
        if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = info.surface;
    swapchain_ci.minImageCount = desiredNumberOfSwapChainImages; //采用单buffer，双buffer，还是3buffer
    swapchain_ci.imageFormat = info.format; //显示支持的格式
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = compositeAlpha;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;

    swapchain_ci.clipped = false;

    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.imageUsage = usageFlags;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;
    uint32_t queueFamilyIndices[2] = {(uint32_t)info.graphics_queue_family_index, (uint32_t)info.present_queue_family_index};
    //You determined above the queue families for graphics and present queues. If they are different,
    // you need to do some additional work to allow the images to be shared between queue families.
    //如果图像队列和显示队列不在同一个队列，需要设置共享
    if (info.graphics_queue_family_index != info.present_queue_family_index) {
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
    }
    //创建swapchain ，A swapchain is an abstraction for an array of presentable images that are associated with a surface.
    res = vkCreateSwapchainKHR(info.device, &swapchain_ci, NULL, &info.swap_chain);
    assert(res == VK_SUCCESS);
    //获取支持的swapchain image 数量
    res = vkGetSwapchainImagesKHR(info.device, info.swap_chain, &info.swapchainImageCount, NULL);
    assert(res == VK_SUCCESS);
    //get a list of image handles and storing them in info.buffers[]. Now you have a list of target image handles.
    VkImage *swapchainImages = (VkImage *)malloc(info.swapchainImageCount * sizeof(VkImage));
    assert(swapchainImages);
    res = vkGetSwapchainImagesKHR(info.device, info.swap_chain, &info.swapchainImageCount, swapchainImages);
    assert(res == VK_SUCCESS);
    //绑定imageview 和 image handle
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        swap_chain_buffer sc_buffer;

        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.format = info.format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.flags = 0;

        sc_buffer.image = swapchainImages[i];

        color_image_view.image = sc_buffer.image;

        res = vkCreateImageView(info.device, &color_image_view, NULL, &sc_buffer.view);
        info.buffers.push_back(sc_buffer);
        assert(res == VK_SUCCESS);
    }
    free(swapchainImages);
    info.current_buffer = 0;

    if (NULL != presentModes) {
        free(presentModes);
    }
}

void init_uniform_buffer(struct sample_info &info) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    float fov = glm::radians(45.0f);
    if (info.width > info.height) {
        fov *= static_cast<float>(info.height) / static_cast<float>(info.width);
    }
    info.Projection = glm::perspective(fov, static_cast<float>(info.width) / static_cast<float>(info.height), 0.1f, 100.0f);
    info.View = glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
                            glm::vec3(0, 0, 0),     // and looks at the origin
                            glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
                            );
    info.Model = glm::mat4(1.0f);
    info.Clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

    info.MVP = info.Clip* info.Projection * info.View * info.Model;
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = NULL;
    buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_info.size = sizeof(info.MVP);
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    res = vkCreateBuffer(info.device, &buf_info, NULL, &info.uniform_data.buf);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(info.device, info.uniform_data.buf, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;

    alloc_info.allocationSize = mem_reqs.size;
    pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       &alloc_info.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.uniform_data.mem));
    assert(res == VK_SUCCESS);
    uint8_t *pData;
    res = vkMapMemory(info.device, info.uniform_data.mem, 0, mem_reqs.size, 0, (void **)&pData);
    assert(res == VK_SUCCESS);

    memcpy(pData, &info.MVP, sizeof(info.MVP));
    vkUnmapMemory(info.device, info.uniform_data.mem);

    res = vkBindBufferMemory(info.device, info.uniform_data.buf, info.uniform_data.mem, 0);
    assert(res == VK_SUCCESS);

    info.uniform_data.buffer_info.buffer = info.uniform_data.buf;
    info.uniform_data.buffer_info.offset = 0;
    info.uniform_data.buffer_info.range = sizeof(info.MVP);
}

void init_descriptor_and_pipeline_layouts(struct sample_info &info, bool use_texture) {
    VkDescriptorSetLayoutBinding layout_bindings[2];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    if (use_texture) {
        layout_bindings[1].binding = 1;
        layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layout_bindings[1].descriptorCount = 1;
        layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layout_bindings[1].pImmutableSamplers = NULL;
    }

    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = NULL;
    descriptor_layout.bindingCount = use_texture ? 2 : 1;
    descriptor_layout.pBindings = layout_bindings;

    VkResult U_ASSERT_ONLY res;

    info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
    res = vkCreateDescriptorSetLayout(info.device, &descriptor_layout, NULL, info.desc_layout.data());
    assert(res == VK_SUCCESS);

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

    res = vkCreatePipelineLayout(info.device, &pPipelineLayoutCreateInfo, NULL, &info.pipeline_layout);
    assert(res == VK_SUCCESS);
}

void init_renderpass(struct sample_info &info, bool clear, VkImageLayout finalLayout) {

    VkResult U_ASSERT_ONLY res;
    /* Need attachments for render target and depth buffer */
    VkAttachmentDescription attachments[2];
    attachments[0].format = info.format;
    attachments[0].samples = NUM_SAMPLES;
    attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = finalLayout;
    attachments[0].flags = 0;

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment =  NULL;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;

    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = NULL;
    rp_info.attachmentCount =  1;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = NULL;

    res = vkCreateRenderPass(info.device, &rp_info, NULL, &info.render_pass);
    assert(res == VK_SUCCESS);
}

void init_framebuffers(struct sample_info &info) {

    VkResult U_ASSERT_ONLY res;
    VkImageView attachments[2];
    attachments[1] = info.depth.view;

    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = NULL;
    fb_info.renderPass = info.render_pass;
    fb_info.attachmentCount = 1;
    fb_info.pAttachments = attachments;
    fb_info.width = info.width;
    fb_info.height = info.height;
    fb_info.layers = 1;

    uint32_t i;

    info.framebuffers = (VkFramebuffer *)malloc(info.swapchainImageCount * sizeof(VkFramebuffer));

    for (i = 0; i < info.swapchainImageCount; i++) {
        attachments[0] = info.buffers[i].view;
        res = vkCreateFramebuffer(info.device, &fb_info, NULL, &info.framebuffers[i]);
        assert(res == VK_SUCCESS);
    }
}
void init_command_pool(struct sample_info &info) {
    /* DEPENDS on init_swapchain_extension() */
    VkResult U_ASSERT_ONLY res;

    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = info.graphics_queue_family_index;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    res = vkCreateCommandPool(info.device, &cmd_pool_info, NULL, &info.cmd_pool);
    assert(res == VK_SUCCESS);
}

void init_command_buffer(struct sample_info &info) {
    /* DEPENDS on init_swapchain_extension() and init_command_pool() */
    VkResult U_ASSERT_ONLY res;

    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = NULL;
    cmd.commandPool = info.cmd_pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd);
    assert(res == VK_SUCCESS);
}
void execute_begin_command_buffer(struct sample_info &info) {
    /* DEPENDS on init_command_buffer() */
    VkResult U_ASSERT_ONLY res;

    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = NULL;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = NULL;

    res = vkBeginCommandBuffer(info.cmd, &cmd_buf_info);
    assert(res == VK_SUCCESS);
}
void init_device_queue(struct sample_info &info) {
    vkGetDeviceQueue(info.device, info.graphics_queue_family_index, 0, &info.graphics_queue);
    if (info.graphics_queue_family_index == info.present_queue_family_index) {
        info.present_queue = info.graphics_queue;
    } else {
        vkGetDeviceQueue(info.device, info.present_queue_family_index, 0, &info.present_queue);
    }
}

void init_vertex_buffer(struct sample_info &info, const void *vertexData, uint32_t dataSize, uint32_t dataStride,
                        bool use_texture) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;

    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = NULL;
    buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buf_info.size = dataSize;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    res = vkCreateBuffer(info.device, &buf_info, NULL, &info.vertex_buffer.buf);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(info.device, info.vertex_buffer.buf, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;

    alloc_info.allocationSize = mem_reqs.size;
    pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       &alloc_info.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.vertex_buffer.mem));
    assert(res == VK_SUCCESS);
    info.vertex_buffer.buffer_info.range = mem_reqs.size;
    info.vertex_buffer.buffer_info.offset = 0;

    uint8_t *pData;
    res = vkMapMemory(info.device, info.vertex_buffer.mem, 0, mem_reqs.size, 0, (void **)&pData);
    assert(res == VK_SUCCESS);

    memcpy(pData, vertexData, dataSize);

    vkUnmapMemory(info.device, info.vertex_buffer.mem);

    res = vkBindBufferMemory(info.device, info.vertex_buffer.buf, info.vertex_buffer.mem, 0);
    assert(res == VK_SUCCESS);

    info.vi_binding.binding = 0;
    info.vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    info.vi_binding.stride = dataStride;

    info.vi_attribs[0].binding = 0;
    info.vi_attribs[0].location = 0;
    info.vi_attribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    info.vi_attribs[0].offset = 0;
    info.vi_attribs[1].binding = 0;
    info.vi_attribs[1].location = 1;
    info.vi_attribs[1].format = use_texture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    info.vi_attribs[1].offset = 16;
}

void init_descriptor_pool(struct sample_info &info, bool use_texture) {

    VkResult U_ASSERT_ONLY res;
    VkDescriptorPoolSize type_count[2];
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount = 1;
    if (use_texture) {
        type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        type_count[1].descriptorCount = 1;
    }

    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = NULL;
    descriptor_pool.maxSets = 1;
    descriptor_pool.poolSizeCount = use_texture ? 2 : 1;
    descriptor_pool.pPoolSizes = type_count;

    res = vkCreateDescriptorPool(info.device, &descriptor_pool, NULL, &info.desc_pool);
    assert(res == VK_SUCCESS);
}

void init_descriptor_set(struct sample_info &info, bool use_texture) {
    /* DEPENDS on init_descriptor_pool() */

    VkResult U_ASSERT_ONLY res;

    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = NULL;
    alloc_info[0].descriptorPool = info.desc_pool;
    alloc_info[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    alloc_info[0].pSetLayouts = info.desc_layout.data();

    info.desc_set.resize(NUM_DESCRIPTOR_SETS);
    res = vkAllocateDescriptorSets(info.device, alloc_info, info.desc_set.data());
    assert(res == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = info.desc_set[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &info.uniform_data.buffer_info;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    if (use_texture) {
        writes[1] = {};
        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstSet = info.desc_set[0];
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = &info.texture_data.image_info;
        writes[1].dstArrayElement = 0;
    }

    vkUpdateDescriptorSets(info.device, use_texture ? 2 : 1, writes, 0, NULL);
}

void init_shaders(struct sample_info &info, const char *vertShaderText, const char *fragShaderText) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY retVal;
    if (!(vertShaderText || fragShaderText)) return;

    init_glslang();
    VkShaderModuleCreateInfo moduleCreateInfo;

    if (vertShaderText) {
        std::vector<unsigned int> vtx_spv;
        info.shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.shaderStages[0].pNext = NULL;
        info.shaderStages[0].pSpecializationInfo = NULL;
        info.shaderStages[0].flags = 0;
        info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        info.shaderStages[0].pName = "main";

        retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
        assert(retVal);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = NULL;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = vtx_spv.data();
        res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[0].module);
        assert(res == VK_SUCCESS);
    }

    if (fragShaderText) {
        std::vector<unsigned int> frag_spv;
        info.shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.shaderStages[1].pNext = NULL;
        info.shaderStages[1].pSpecializationInfo = NULL;
        info.shaderStages[1].flags = 0;
        info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        info.shaderStages[1].pName = "main";

        retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, frag_spv);
        assert(retVal);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = NULL;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = frag_spv.data();
        res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[1].module);
        assert(res == VK_SUCCESS);
    }

    finalize_glslang();
}

void init_pipeline_cache(struct sample_info &info) {
    VkResult U_ASSERT_ONLY res;

    VkPipelineCacheCreateInfo pipelineCache;
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCache.pNext = NULL;
    pipelineCache.initialDataSize = 0;
    pipelineCache.pInitialData = NULL;
    pipelineCache.flags = 0;
    res = vkCreatePipelineCache(info.device, &pipelineCache, NULL, &info.pipelineCache);
    assert(res == VK_SUCCESS);
}

void init_pipeline(struct sample_info &info, VkBool32 include_vi) {
    VkResult U_ASSERT_ONLY res;
    //
    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

    VkPipelineVertexInputStateCreateInfo vi;
    memset(&vi, 0, sizeof(vi));
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (include_vi) {
        vi.pNext = NULL;
        vi.flags = 0;
        vi.vertexBindingDescriptionCount = 1;
        vi.pVertexBindingDescriptions = &info.vi_binding;
        vi.vertexAttributeDescriptionCount = 2;
        vi.pVertexAttributeDescriptions = info.vi_attribs;
    }
    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = NULL;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; //顶点如果形成基元，相当于trangle——fan等

    VkPipelineRasterizationStateCreateInfo rs;
    //它也会执行深度测试depth testing、面裁切face culling和裁剪测试，
    // 它可以对输出的片元进行配置，决定是否输出整个图元拓扑或者是边框(线框渲染)。
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = NULL;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_CLOCKWISE; //glFrontFace
    rs.depthClampEnable = VK_FALSE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable = VK_FALSE;
    rs.depthBiasConstantFactor = 0;
    rs.depthBiasClamp = 0;
    rs.depthBiasSlopeFactor = 0;
    rs.lineWidth = 1.0f;
    //blend 模式
    VkPipelineColorBlendStateCreateInfo cb;
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.flags = 0;
    cb.pNext = NULL;
    VkPipelineColorBlendAttachmentState att_state[1];
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable = VK_FALSE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cb.attachmentCount = 1;
    cb.pAttachments = att_state;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;
    //设置viewpoint
    VkPipelineViewportStateCreateInfo vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.pNext = NULL;
    vp.flags = 0;
    // Temporary disabling dynamic viewport on Android because some of drivers doesn't
    // support the feature.
    VkViewport viewports;
    viewports.minDepth = 0.0f;
    viewports.maxDepth = 1.0f;
    viewports.x = 0;
    viewports.y = 0;
    viewports.width = info.width;
    viewports.height = info.height;
    VkRect2D scissor;
    scissor.extent.width = info.width;
    scissor.extent.height = info.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vp.viewportCount = NUM_VIEWPORTS;
    vp.scissorCount = NUM_SCISSORS;
    vp.pScissors = &scissor;
    vp.pViewports = &viewports;
    //使用depth 或者 stencil缓冲区
    VkPipelineDepthStencilStateCreateInfo ds;
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.pNext = NULL;
    ds.flags = 0;
    ds.depthTestEnable = false;
    ds.depthWriteEnable = false;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.stencilTestEnable = VK_FALSE;
    ds.back.failOp = VK_STENCIL_OP_KEEP;
    ds.back.passOp = VK_STENCIL_OP_KEEP;
    ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
    ds.back.compareMask = 0;
    ds.back.reference = 0;
    ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
    ds.back.writeMask = 0;
    ds.minDepthBounds = 0;
    ds.maxDepthBounds = 0;
    ds.stencilTestEnable = VK_FALSE;
    ds.front = ds.back;

    VkPipelineMultisampleStateCreateInfo ms;
    //用于配置多重采样。所谓多重采样是抗锯齿anti-aliasing的一种实现。它通过组合多个多边形的片段着色器结果，光栅化到同一个像素。
    // 这主要发生在边缘，这也是最引人注目的锯齿出现的地方。如果只有一个多边形映射到像素是不需要多次运行片段着色器进行采样的，
    // 相比高分辨率来说，它会花费较低的开销。开启该功能需要GPU支持。
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pNext = NULL;
    ms.flags = 0;
    ms.pSampleMask = NULL;
    ms.rasterizationSamples = NUM_SAMPLES;
    ms.sampleShadingEnable = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable = VK_FALSE;
    ms.minSampleShading = 0.0;

    VkGraphicsPipelineCreateInfo pipeline;
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.pNext = NULL;
    pipeline.layout = info.pipeline_layout;
    pipeline.basePipelineHandle = VK_NULL_HANDLE;
    pipeline.basePipelineIndex = 0;
    pipeline.flags = 0;
    pipeline.pVertexInputState = &vi;
    pipeline.pInputAssemblyState = &ia;
    pipeline.pRasterizationState = &rs;
    pipeline.pColorBlendState = &cb;
    pipeline.pTessellationState = NULL;
    pipeline.pMultisampleState = &ms;
    pipeline.pDynamicState = &dynamicState;
    pipeline.pViewportState = &vp;
    pipeline.pDepthStencilState = &ds;
    pipeline.pStages = info.shaderStages;
    pipeline.stageCount = 2;
    pipeline.renderPass = info.render_pass;
    pipeline.subpass = 0;

    res = vkCreateGraphicsPipelines(info.device, info.pipelineCache, 1, &pipeline, NULL, &info.pipeline);
    assert(res == VK_SUCCESS);
}

void acquireNextImage(struct sample_info &info,VkSemaphore& imageAcquiredSemaphore)
{
    VkResult U_ASSERT_ONLY res;
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = NULL;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    res = vkCreateSemaphore(info.device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
    assert(res == VK_SUCCESS);
    //从交换链中获取一个图像
    //    在帧缓冲区中，使用作为附件的图像来执行命令缓冲区中的命令
    //    为了最终呈现，将图像返还到交换链
    res = vkAcquireNextImageKHR(info.device, info.swap_chain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,
                                &info.current_buffer);

    assert(res == VK_SUCCESS);

}
void buildCommand(struct sample_info &info)
{
    VkResult U_ASSERT_ONLY res;
    VkClearValue clear_values[2];
    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;

    VkRenderPassBeginInfo rp_begin;
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = NULL;
    rp_begin.renderPass = info.render_pass;
    rp_begin.framebuffer = info.framebuffers[info.current_buffer];
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = info.width;
    rp_begin.renderArea.extent.height = info.height;
    rp_begin.clearValueCount = 2;
    rp_begin.pClearValues = clear_values;

    vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
    vkCmdBindDescriptorSets(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
                            info.desc_set.data(), 0, NULL);

    const VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(info.cmd, 0, 1, &info.vertex_buffer.buf, offsets);

    vkCmdDraw(info.cmd, 12 * 3, 1, 0, 0);
    vkCmdEndRenderPass(info.cmd);
    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);
}

void submitCommand(struct sample_info &info,VkFence& drawFence,VkSemaphore imageAcquiredSemaphore)
{
    VkResult U_ASSERT_ONLY res;

    const VkCommandBuffer cmd_bufs[] = {info.cmd};
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);

    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 1;
    submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
    submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;

    res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
    assert(res == VK_SUCCESS);
}

void present(struct sample_info &info,VkFence& drawFence)
{
    VkResult U_ASSERT_ONLY res;

    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swap_chain;
    present.pImageIndices = &info.current_buffer;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;

    do {
        res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);

    assert(res == VK_SUCCESS);
    res = vkQueuePresentKHR(info.present_queue, &present);
    assert(res == VK_SUCCESS);
}

void destroy_pipeline(struct sample_info &info) { vkDestroyPipeline(info.device, info.pipeline, NULL); }

void destroy_pipeline_cache(struct sample_info &info) { vkDestroyPipelineCache(info.device, info.pipelineCache, NULL); }

void destroy_uniform_buffer(struct sample_info &info) {
    vkDestroyBuffer(info.device, info.uniform_data.buf, NULL);
    vkFreeMemory(info.device, info.uniform_data.mem, NULL);
}

void destroy_descriptor_and_pipeline_layouts(struct sample_info &info) {
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++) vkDestroyDescriptorSetLayout(info.device, info.desc_layout[i], NULL);
    vkDestroyPipelineLayout(info.device, info.pipeline_layout, NULL);
}

void destroy_descriptor_pool(struct sample_info &info) { vkDestroyDescriptorPool(info.device, info.desc_pool, NULL); }

void destroy_shaders(struct sample_info &info) {
    vkDestroyShaderModule(info.device, info.shaderStages[0].module, NULL);
    vkDestroyShaderModule(info.device, info.shaderStages[1].module, NULL);
}

void destroy_command_buffer(struct sample_info &info) {
    VkCommandBuffer cmd_bufs[1] = {info.cmd};
    vkFreeCommandBuffers(info.device, info.cmd_pool, 1, cmd_bufs);
}

void destroy_command_pool(struct sample_info &info) { vkDestroyCommandPool(info.device, info.cmd_pool, NULL); }

void destroy_depth_buffer(struct sample_info &info) {
    vkDestroyImageView(info.device, info.depth.view, NULL);
    vkDestroyImage(info.device, info.depth.image, NULL);
    vkFreeMemory(info.device, info.depth.mem, NULL);
}

void destroy_vertex_buffer(struct sample_info &info) {
    vkDestroyBuffer(info.device, info.vertex_buffer.buf, NULL);
    vkFreeMemory(info.device, info.vertex_buffer.mem, NULL);
}

void destroy_swap_chain(struct sample_info &info) {
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyImageView(info.device, info.buffers[i].view, NULL);
    }
    vkDestroySwapchainKHR(info.device, info.swap_chain, NULL);
}

void destroy_framebuffers(struct sample_info &info) {
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyFramebuffer(info.device, info.framebuffers[i], NULL);
    }
    free(info.framebuffers);
}

void destroy_renderpass(struct sample_info &info) { vkDestroyRenderPass(info.device, info.render_pass, NULL); }

void destroy_device(struct sample_info &info) {
    vkDeviceWaitIdle(info.device);
    vkDestroyDevice(info.device, NULL);
}

void destroy_instance(struct sample_info &info) { vkDestroyInstance(info.inst, NULL); }

