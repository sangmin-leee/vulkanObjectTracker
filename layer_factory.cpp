
// This file is ***GENERATED***.  Do Not Edit.
// See layer_factory_generator.py for modifications.

/* Copyright (c) 2015-2020 The Khronos Group Inc.
 * Copyright (c) 2015-2020 Valve Corporation
 * Copyright (c) 2015-2020 LunarG, Inc.
 * Copyright (c) 2015-2020 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Tobin Ehlis <tobine@google.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#include <string.h>
#include <mutex>

#define VALIDATION_ERROR_MAP_IMPL

#include "vk_loader_platform.h"
#include "vk_dispatch_table_helper.h"
#include "vk_layer_data.h"
#include "vk_layer_extension_utils.h"
#include "vk_layer_logging.h"
#include "vk_extension_helper.h"
#include "vk_layer_utils.h"

class layer_factory;
std::vector<layer_factory *> global_interceptor_list;
debug_report_data *vlf_report_data = VK_NULL_HANDLE;

#include "layer_factory.h"

struct instance_layer_data {
    VkLayerInstanceDispatchTable dispatch_table;
    VkInstance instance = VK_NULL_HANDLE;
    debug_report_data *report_data = nullptr;
    std::vector<VkDebugReportCallbackEXT> logging_callback;
    std::vector<VkDebugUtilsMessengerEXT> logging_messenger;
    InstanceExtensions extensions;
};

struct device_layer_data {
    debug_report_data *report_data = nullptr;
    VkLayerDispatchTable dispatch_table;
    DeviceExtensions extensions = {};
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    instance_layer_data *instance_data = nullptr;
};

static std::unordered_map<void *, device_layer_data *> device_layer_data_map;
static std::unordered_map<void *, instance_layer_data *> instance_layer_data_map;

#include "interceptor_objects.h"

using mutex_t = std::mutex;
using lock_guard_t = std::lock_guard<mutex_t>;
using unique_lock_t = std::unique_lock<mutex_t>;

namespace vulkan_layer_factory {

using std::unordered_map;

static mutex_t global_lock;

static const VkLayerProperties global_layer = {
    "VK_LAYER_LUNARG_layer_factory", VK_LAYER_API_VERSION, 1, "LunarG Layer Factory Layer",
};

static const VkExtensionProperties instance_extensions[] = {{VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_SPEC_VERSION}};

extern const std::unordered_map<std::string, void*> name_to_funcptr_map;


// Manually written functions

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char *funcName) {
    assert(device);
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    const auto &item = name_to_funcptr_map.find(funcName);
    if (item != name_to_funcptr_map.end()) {
        return reinterpret_cast<PFN_vkVoidFunction>(item->second);
    }
    auto &table = device_data->dispatch_table;
    if (!table.GetDeviceProcAddr) return nullptr;
    return table.GetDeviceProcAddr(device, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char *funcName) {
    instance_layer_data *instance_data;
    const auto &item = name_to_funcptr_map.find(funcName);
    if (item != name_to_funcptr_map.end()) {
        return reinterpret_cast<PFN_vkVoidFunction>(item->second);
    }
    instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    auto &table = instance_data->dispatch_table;
    if (!table.GetInstanceProcAddr) return nullptr;
    return table.GetInstanceProcAddr(instance, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetPhysicalDeviceProcAddr(VkInstance instance, const char *funcName) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    auto &table = instance_data->dispatch_table;
    if (!table.GetPhysicalDeviceProcAddr) return nullptr;
    return table.GetPhysicalDeviceProcAddr(instance, funcName);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties) {
    return util_GetLayerProperties(1, &global_layer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount,
                                                              VkLayerProperties *pProperties) {
    return util_GetLayerProperties(1, &global_layer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                    VkExtensionProperties *pProperties) {
    if (pLayerName && !strcmp(pLayerName, global_layer.layerName))
        return util_GetExtensionProperties(1, instance_extensions, pCount, pProperties);

    return VK_ERROR_LAYER_NOT_PRESENT;
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName,
                                                                  uint32_t *pCount, VkExtensionProperties *pProperties) {
    if (pLayerName && !strcmp(pLayerName, global_layer.layerName)) return util_GetExtensionProperties(0, NULL, pCount, pProperties);

    assert(physicalDevice);

    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    return instance_data->dispatch_table.EnumerateDeviceExtensionProperties(physicalDevice, NULL, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                              VkInstance *pInstance) {
    VkLayerInstanceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) return VK_ERROR_INITIALIZATION_FAILED;
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    // Init dispatch array and call registration functions
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateInstance(pCreateInfo, pAllocator, pInstance);
    }

    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);

    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(*pInstance), instance_layer_data_map);
    instance_data->instance = *pInstance;
    layer_init_instance_dispatch_table(*pInstance, &instance_data->dispatch_table, fpGetInstanceProcAddr);
    instance_data->report_data = new debug_report_data{};
    instance_data->extensions.InitFromInstanceCreateInfo((pCreateInfo->pApplicationInfo ? pCreateInfo->pApplicationInfo->apiVersion : VK_API_VERSION_1_0), pCreateInfo);
    layer_debug_report_actions(instance_data->report_data, pAllocator, "lunarg_layer_factory");
    layer_debug_messenger_actions(instance_data->report_data, pAllocator, "lunarg_layer_factory");
    vlf_report_data = instance_data->report_data;

    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateInstance(pCreateInfo, pAllocator, pInstance, result);
    }

    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
    dispatch_key key = get_dispatch_key(instance);
    instance_layer_data *instance_data = GetLayerDataPtr(key, instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyInstance(instance, pAllocator);
    }

    instance_data->dispatch_table.DestroyInstance(instance, pAllocator);

    lock_guard_t lock(global_lock);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyInstance(instance, pAllocator);
    }
    // Clean up logging callback, if any
    while (instance_data->logging_messenger.size() > 0) {
        VkDebugUtilsMessengerEXT messenger = instance_data->logging_messenger.back();
        layer_destroy_callback(instance_data->report_data, messenger, pAllocator);
        instance_data->logging_messenger.pop_back();
    }
    while (instance_data->logging_callback.size() > 0) {
        VkDebugReportCallbackEXT callback = instance_data->logging_callback.back();
        layer_destroy_callback(instance_data->report_data, callback, pAllocator);
        instance_data->logging_callback.pop_back();
    }
    layer_debug_utils_destroy_instance(instance_data->report_data);
    FreeLayerDataPtr(key, instance_layer_data_map);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(VkPhysicalDevice gpu, const VkDeviceCreateInfo *pCreateInfo,
                                            const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(gpu), instance_layer_data_map);

    unique_lock_t lock(global_lock);
    VkLayerDeviceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)fpGetInstanceProcAddr(instance_data->instance, "vkCreateDevice");
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
    }
    lock.unlock();

    VkResult result = fpCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);

    lock.lock();
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDevice(gpu, pCreateInfo, pAllocator, pDevice, result);
    }
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(*pDevice), device_layer_data_map);
    device_data->instance_data = instance_data;
    layer_init_device_dispatch_table(*pDevice, &device_data->dispatch_table, fpGetDeviceProcAddr);
    device_data->device = *pDevice;
    device_data->physical_device = gpu;
    device_data->report_data = instance_data->report_data;
    VkPhysicalDeviceProperties physical_device_properties{};
    instance_data->dispatch_table.GetPhysicalDeviceProperties(gpu, &physical_device_properties);
    device_data->extensions.InitFromDeviceCreateInfo(&instance_data->extensions, physical_device_properties.apiVersion, pCreateInfo);
    lock.unlock();

    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator) {
    dispatch_key key = get_dispatch_key(device);
    device_layer_data *device_data = GetLayerDataPtr(key, device_layer_data_map);

    unique_lock_t lock(global_lock);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDevice(device, pAllocator);
    }
    lock.unlock();

    device_data->dispatch_table.DestroyDevice(device, pAllocator);

    lock.lock();
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDevice(device, pAllocator);
    }

    FreeLayerDataPtr(key, device_layer_data_map);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugReportCallbackEXT(VkInstance instance,
                                                            const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                                            const VkAllocationCallbacks *pAllocator,
                                                            VkDebugReportCallbackEXT *pCallback) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
    }
    VkResult result = instance_data->dispatch_table.CreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
    result = layer_create_report_callback(instance_data->report_data, false, pCreateInfo, pAllocator, pCallback);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                                         const VkAllocationCallbacks *pAllocator) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
    }
    instance_data->dispatch_table.DestroyDebugReportCallbackEXT(instance, callback, pAllocator);
    layer_destroy_callback(instance_data->report_data, callback, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
    }
}



VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    }
    VkResult result = instance_data->dispatch_table.EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures*                   pFeatures) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    }
    instance_data->dispatch_table.GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties*                         pFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkImageFormatProperties*                    pImageFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceProperties(physicalDevice, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    }
    device_data->dispatch_table.GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL QueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueSubmit(queue, submitCount, pSubmits, fence);
    }
    VkResult result = device_data->dispatch_table.QueueSubmit(queue, submitCount, pSubmits, fence);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueSubmit(queue, submitCount, pSubmits, fence, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL QueueWaitIdle(
    VkQueue                                     queue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueWaitIdle(queue);
    }
    VkResult result = device_data->dispatch_table.QueueWaitIdle(queue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueWaitIdle(queue, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL DeviceWaitIdle(
    VkDevice                                    device) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDeviceWaitIdle(device);
    }
    VkResult result = device_data->dispatch_table.DeviceWaitIdle(device);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDeviceWaitIdle(device, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
    }
    VkResult result = device_data->dispatch_table.AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAllocateMemory(device, pAllocateInfo, pAllocator, pMemory, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL FreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallFreeMemory(device, memory, pAllocator);
    }
    device_data->dispatch_table.FreeMemory(device, memory, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallFreeMemory(device, memory, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL MapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallMapMemory(device, memory, offset, size, flags, ppData);
    }
    VkResult result = device_data->dispatch_table.MapMemory(device, memory, offset, size, flags, ppData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallMapMemory(device, memory, offset, size, flags, ppData, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL UnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallUnmapMemory(device, memory);
    }
    device_data->dispatch_table.UnmapMemory(device, memory);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallUnmapMemory(device, memory);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL FlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    }
    VkResult result = device_data->dispatch_table.FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL InvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallInvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    }
    VkResult result = device_data->dispatch_table.InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallInvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
    }
    device_data->dispatch_table.GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindBufferMemory(device, buffer, memory, memoryOffset);
    }
    VkResult result = device_data->dispatch_table.BindBufferMemory(device, buffer, memory, memoryOffset);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindBufferMemory(device, buffer, memory, memoryOffset, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindImageMemory(device, image, memory, memoryOffset);
    }
    VkResult result = device_data->dispatch_table.BindImageMemory(device, image, memory, memoryOffset);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindImageMemory(device, image, memory, memoryOffset, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
    }
    device_data->dispatch_table.GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageMemoryRequirements(device, image, pMemoryRequirements);
    }
    device_data->dispatch_table.GetImageMemoryRequirements(device, image, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageMemoryRequirements(device, image, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
    device_data->dispatch_table.GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkSampleCountFlagBits                       samples,
    VkImageUsageFlags                           usage,
    VkImageTiling                               tiling,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties*              pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL QueueBindSparse(
    VkQueue                                     queue,
    uint32_t                                    bindInfoCount,
    const VkBindSparseInfo*                     pBindInfo,
    VkFence                                     fence) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
    }
    VkResult result = device_data->dispatch_table.QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueBindSparse(queue, bindInfoCount, pBindInfo, fence, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateFence(device, pCreateInfo, pAllocator, pFence);
    }
    VkResult result = device_data->dispatch_table.CreateFence(device, pCreateInfo, pAllocator, pFence);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateFence(device, pCreateInfo, pAllocator, pFence, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyFence(device, fence, pAllocator);
    }
    device_data->dispatch_table.DestroyFence(device, fence, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyFence(device, fence, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL ResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetFences(device, fenceCount, pFences);
    }
    VkResult result = device_data->dispatch_table.ResetFences(device, fenceCount, pFences);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetFences(device, fenceCount, pFences, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetFenceStatus(device, fence);
    }
    VkResult result = device_data->dispatch_table.GetFenceStatus(device, fence);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetFenceStatus(device, fence, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL WaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallWaitForFences(device, fenceCount, pFences, waitAll, timeout);
    }
    VkResult result = device_data->dispatch_table.WaitForFences(device, fenceCount, pFences, waitAll, timeout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallWaitForFences(device, fenceCount, pFences, waitAll, timeout, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
    }
    VkResult result = device_data->dispatch_table.CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroySemaphore(device, semaphore, pAllocator);
    }
    device_data->dispatch_table.DestroySemaphore(device, semaphore, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroySemaphore(device, semaphore, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateEvent(device, pCreateInfo, pAllocator, pEvent);
    }
    VkResult result = device_data->dispatch_table.CreateEvent(device, pCreateInfo, pAllocator, pEvent);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateEvent(device, pCreateInfo, pAllocator, pEvent, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyEvent(device, event, pAllocator);
    }
    device_data->dispatch_table.DestroyEvent(device, event, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyEvent(device, event, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetEventStatus(
    VkDevice                                    device,
    VkEvent                                     event) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetEventStatus(device, event);
    }
    VkResult result = device_data->dispatch_table.GetEventStatus(device, event);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetEventStatus(device, event, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL SetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSetEvent(device, event);
    }
    VkResult result = device_data->dispatch_table.SetEvent(device, event);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSetEvent(device, event, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL ResetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetEvent(device, event);
    }
    VkResult result = device_data->dispatch_table.ResetEvent(device, event);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetEvent(device, event, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
    }
    VkResult result = device_data->dispatch_table.CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyQueryPool(device, queryPool, pAllocator);
    }
    device_data->dispatch_table.DestroyQueryPool(device, queryPool, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyQueryPool(device, queryPool, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetQueryPoolResults(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
    }
    VkResult result = device_data->dispatch_table.GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
    }
    VkResult result = device_data->dispatch_table.CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateBuffer(device, pCreateInfo, pAllocator, pBuffer, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyBuffer(device, buffer, pAllocator);
    }
    device_data->dispatch_table.DestroyBuffer(device, buffer, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyBuffer(device, buffer, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateBufferView(device, pCreateInfo, pAllocator, pView);
    }
    VkResult result = device_data->dispatch_table.CreateBufferView(device, pCreateInfo, pAllocator, pView);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateBufferView(device, pCreateInfo, pAllocator, pView, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyBufferView(device, bufferView, pAllocator);
    }
    device_data->dispatch_table.DestroyBufferView(device, bufferView, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyBufferView(device, bufferView, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateImage(device, pCreateInfo, pAllocator, pImage);
    }
    VkResult result = device_data->dispatch_table.CreateImage(device, pCreateInfo, pAllocator, pImage);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateImage(device, pCreateInfo, pAllocator, pImage, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyImage(device, image, pAllocator);
    }
    device_data->dispatch_table.DestroyImage(device, image, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyImage(device, image, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageSubresourceLayout(device, image, pSubresource, pLayout);
    }
    device_data->dispatch_table.GetImageSubresourceLayout(device, image, pSubresource, pLayout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageSubresourceLayout(device, image, pSubresource, pLayout);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateImageView(device, pCreateInfo, pAllocator, pView);
    }
    VkResult result = device_data->dispatch_table.CreateImageView(device, pCreateInfo, pAllocator, pView);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateImageView(device, pCreateInfo, pAllocator, pView, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyImageView(device, imageView, pAllocator);
    }
    device_data->dispatch_table.DestroyImageView(device, imageView, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyImageView(device, imageView, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
    }
    VkResult result = device_data->dispatch_table.CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyShaderModule(device, shaderModule, pAllocator);
    }
    device_data->dispatch_table.DestroyShaderModule(device, shaderModule, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyShaderModule(device, shaderModule, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
    }
    VkResult result = device_data->dispatch_table.CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyPipelineCache(device, pipelineCache, pAllocator);
    }
    device_data->dispatch_table.DestroyPipelineCache(device, pipelineCache, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyPipelineCache(device, pipelineCache, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPipelineCacheData(device, pipelineCache, pDataSize, pData);
    }
    VkResult result = device_data->dispatch_table.GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPipelineCacheData(device, pipelineCache, pDataSize, pData, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL MergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallMergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
    }
    VkResult result = device_data->dispatch_table.MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallMergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    }
    VkResult result = device_data->dispatch_table.CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    }
    VkResult result = device_data->dispatch_table.CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyPipeline(device, pipeline, pAllocator);
    }
    device_data->dispatch_table.DestroyPipeline(device, pipeline, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyPipeline(device, pipeline, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
    }
    VkResult result = device_data->dispatch_table.CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyPipelineLayout(device, pipelineLayout, pAllocator);
    }
    device_data->dispatch_table.DestroyPipelineLayout(device, pipelineLayout, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyPipelineLayout(device, pipelineLayout, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateSampler(device, pCreateInfo, pAllocator, pSampler);
    }
    VkResult result = device_data->dispatch_table.CreateSampler(device, pCreateInfo, pAllocator, pSampler);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateSampler(device, pCreateInfo, pAllocator, pSampler, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySampler(
    VkDevice                                    device,
    VkSampler                                   sampler,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroySampler(device, sampler, pAllocator);
    }
    device_data->dispatch_table.DestroySampler(device, sampler, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroySampler(device, sampler, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
    }
    VkResult result = device_data->dispatch_table.CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    }
    device_data->dispatch_table.DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
    }
    VkResult result = device_data->dispatch_table.CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDescriptorPool(device, descriptorPool, pAllocator);
    }
    device_data->dispatch_table.DestroyDescriptorPool(device, descriptorPool, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDescriptorPool(device, descriptorPool, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL ResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetDescriptorPool(device, descriptorPool, flags);
    }
    VkResult result = device_data->dispatch_table.ResetDescriptorPool(device, descriptorPool, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetDescriptorPool(device, descriptorPool, flags, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
    }
    VkResult result = device_data->dispatch_table.AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL FreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
    }
    VkResult result = device_data->dispatch_table.FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    }
    device_data->dispatch_table.UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
    }
    VkResult result = device_data->dispatch_table.CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyFramebuffer(device, framebuffer, pAllocator);
    }
    device_data->dispatch_table.DestroyFramebuffer(device, framebuffer, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyFramebuffer(device, framebuffer, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
    }
    VkResult result = device_data->dispatch_table.CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyRenderPass(device, renderPass, pAllocator);
    }
    device_data->dispatch_table.DestroyRenderPass(device, renderPass, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyRenderPass(device, renderPass, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL GetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetRenderAreaGranularity(device, renderPass, pGranularity);
    }
    device_data->dispatch_table.GetRenderAreaGranularity(device, renderPass, pGranularity);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetRenderAreaGranularity(device, renderPass, pGranularity);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
    }
    VkResult result = device_data->dispatch_table.CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyCommandPool(device, commandPool, pAllocator);
    }
    device_data->dispatch_table.DestroyCommandPool(device, commandPool, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyCommandPool(device, commandPool, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL ResetCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolResetFlags                     flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetCommandPool(device, commandPool, flags);
    }
    VkResult result = device_data->dispatch_table.ResetCommandPool(device, commandPool, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetCommandPool(device, commandPool, flags, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    }
    VkResult result = device_data->dispatch_table.AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL FreeCommandBuffers(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
    }
    device_data->dispatch_table.FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBeginCommandBuffer(commandBuffer, pBeginInfo);
    }
    VkResult result = device_data->dispatch_table.BeginCommandBuffer(commandBuffer, pBeginInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBeginCommandBuffer(commandBuffer, pBeginInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL EndCommandBuffer(
    VkCommandBuffer                             commandBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallEndCommandBuffer(commandBuffer);
    }
    VkResult result = device_data->dispatch_table.EndCommandBuffer(commandBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallEndCommandBuffer(commandBuffer, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL ResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetCommandBuffer(commandBuffer, flags);
    }
    VkResult result = device_data->dispatch_table.ResetCommandBuffer(commandBuffer, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetCommandBuffer(commandBuffer, flags, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    }
    device_data->dispatch_table.CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
    }
    device_data->dispatch_table.CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
    }
    device_data->dispatch_table.CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetLineWidth(commandBuffer, lineWidth);
    }
    device_data->dispatch_table.CmdSetLineWidth(commandBuffer, lineWidth);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetLineWidth(commandBuffer, lineWidth);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    }
    device_data->dispatch_table.CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetBlendConstants(commandBuffer, blendConstants);
    }
    device_data->dispatch_table.CmdSetBlendConstants(commandBuffer, blendConstants);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetBlendConstants(commandBuffer, blendConstants);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
    }
    device_data->dispatch_table.CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
    }
    device_data->dispatch_table.CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
    }
    device_data->dispatch_table.CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetStencilReference(commandBuffer, faceMask, reference);
    }
    device_data->dispatch_table.CmdSetStencilReference(commandBuffer, faceMask, reference);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetStencilReference(commandBuffer, faceMask, reference);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    }
    device_data->dispatch_table.CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
    }
    device_data->dispatch_table.CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    }
    device_data->dispatch_table.CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
    device_data->dispatch_table.CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
    device_data->dispatch_table.CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatch(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
    }
    device_data->dispatch_table.CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatchIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDispatchIndirect(commandBuffer, buffer, offset);
    }
    device_data->dispatch_table.CmdDispatchIndirect(commandBuffer, buffer, offset);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDispatchIndirect(commandBuffer, buffer, offset);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    }
    device_data->dispatch_table.CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    }
    device_data->dispatch_table.CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    }
    device_data->dispatch_table.CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    }
    device_data->dispatch_table.CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    }
    device_data->dispatch_table.CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    }
    device_data->dispatch_table.CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
    }
    device_data->dispatch_table.CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    }
    device_data->dispatch_table.CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    }
    device_data->dispatch_table.CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    }
    device_data->dispatch_table.CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResolveImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageResolve*                       pRegions) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    }
    device_data->dispatch_table.CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetEvent(commandBuffer, event, stageMask);
    }
    device_data->dispatch_table.CmdSetEvent(commandBuffer, event, stageMask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetEvent(commandBuffer, event, stageMask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdResetEvent(commandBuffer, event, stageMask);
    }
    device_data->dispatch_table.CmdResetEvent(commandBuffer, event, stageMask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdResetEvent(commandBuffer, event, stageMask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
    device_data->dispatch_table.CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
    device_data->dispatch_table.CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginQuery(commandBuffer, queryPool, query, flags);
    }
    device_data->dispatch_table.CmdBeginQuery(commandBuffer, queryPool, query, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginQuery(commandBuffer, queryPool, query, flags);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndQuery(commandBuffer, queryPool, query);
    }
    device_data->dispatch_table.CmdEndQuery(commandBuffer, queryPool, query);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndQuery(commandBuffer, queryPool, query);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
    }
    device_data->dispatch_table.CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
    }
    device_data->dispatch_table.CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    }
    device_data->dispatch_table.CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
    }
    device_data->dispatch_table.CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    }
    device_data->dispatch_table.CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdNextSubpass(
    VkCommandBuffer                             commandBuffer,
    VkSubpassContents                           contents) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdNextSubpass(commandBuffer, contents);
    }
    device_data->dispatch_table.CmdNextSubpass(commandBuffer, contents);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdNextSubpass(commandBuffer, contents);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndRenderPass(
    VkCommandBuffer                             commandBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndRenderPass(commandBuffer);
    }
    device_data->dispatch_table.CmdEndRenderPass(commandBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndRenderPass(commandBuffer);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdExecuteCommands(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
    }
    device_data->dispatch_table.CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
    }
}


VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindBufferMemory2(device, bindInfoCount, pBindInfos);
    }
    VkResult result = device_data->dispatch_table.BindBufferMemory2(device, bindInfoCount, pBindInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindBufferMemory2(device, bindInfoCount, pBindInfos, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindImageMemory2(device, bindInfoCount, pBindInfos);
    }
    VkResult result = device_data->dispatch_table.BindImageMemory2(device, bindInfoCount, pBindInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindImageMemory2(device, bindInfoCount, pBindInfos, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    }
    device_data->dispatch_table.GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDeviceMask(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDeviceMask(commandBuffer, deviceMask);
    }
    device_data->dispatch_table.CmdSetDeviceMask(commandBuffer, deviceMask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDeviceMask(commandBuffer, deviceMask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatchBase(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    }
    device_data->dispatch_table.CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceGroups(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallEnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    }
    VkResult result = instance_data->dispatch_table.EnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallEnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements2(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
    device_data->dispatch_table.GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    }
    instance_data->dispatch_table.GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceProperties2(physicalDevice, pProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceProperties2(physicalDevice, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceProperties2(physicalDevice, pProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL TrimCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallTrimCommandPool(device, commandPool, flags);
    }
    device_data->dispatch_table.TrimCommandPool(device, commandPool, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallTrimCommandPool(device, commandPool, flags);
    }
}

VKAPI_ATTR void VKAPI_CALL GetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceQueue2(device, pQueueInfo, pQueue);
    }
    device_data->dispatch_table.GetDeviceQueue2(device, pQueueInfo, pQueue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceQueue2(device, pQueueInfo, pQueue);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
    }
    VkResult result = device_data->dispatch_table.CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySamplerYcbcrConversion(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
    }
    device_data->dispatch_table.DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorUpdateTemplate(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
    }
    VkResult result = device_data->dispatch_table.CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorUpdateTemplate(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
    }
    device_data->dispatch_table.DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSetWithTemplate(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallUpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
    }
    device_data->dispatch_table.UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallUpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
    }
    device_data->dispatch_table.GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
    }
}


VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectCount(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirectCount(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRenderPass2(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
    }
    VkResult result = device_data->dispatch_table.CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBeginRenderPass2(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    const VkSubpassBeginInfo*                   pSubpassBeginInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    }
    device_data->dispatch_table.CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdNextSubpass2(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassBeginInfo*                   pSubpassBeginInfo,
    const VkSubpassEndInfo*                     pSubpassEndInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    }
    device_data->dispatch_table.CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndRenderPass2(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassEndInfo*                     pSubpassEndInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
    }
    device_data->dispatch_table.CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL ResetQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetQueryPool(device, queryPool, firstQuery, queryCount);
    }
    device_data->dispatch_table.ResetQueryPool(device, queryPool, firstQuery, queryCount);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetQueryPool(device, queryPool, firstQuery, queryCount);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreCounterValue(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    uint64_t*                                   pValue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSemaphoreCounterValue(device, semaphore, pValue);
    }
    VkResult result = device_data->dispatch_table.GetSemaphoreCounterValue(device, semaphore, pValue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSemaphoreCounterValue(device, semaphore, pValue, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL WaitSemaphores(
    VkDevice                                    device,
    const VkSemaphoreWaitInfo*                  pWaitInfo,
    uint64_t                                    timeout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallWaitSemaphores(device, pWaitInfo, timeout);
    }
    VkResult result = device_data->dispatch_table.WaitSemaphores(device, pWaitInfo, timeout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallWaitSemaphores(device, pWaitInfo, timeout, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL SignalSemaphore(
    VkDevice                                    device,
    const VkSemaphoreSignalInfo*                pSignalInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSignalSemaphore(device, pSignalInfo);
    }
    VkResult result = device_data->dispatch_table.SignalSemaphore(device, pSignalInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSignalSemaphore(device, pSignalInfo, result);
    }
    return result;
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL GetBufferDeviceAddress(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferDeviceAddress(device, pInfo);
    }
    VkDeviceAddress result = device_data->dispatch_table.GetBufferDeviceAddress(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferDeviceAddress(device, pInfo);
    }
    return result;
}

VKAPI_ATTR uint64_t VKAPI_CALL GetBufferOpaqueCaptureAddress(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferOpaqueCaptureAddress(device, pInfo);
    }
    uint64_t result = device_data->dispatch_table.GetBufferOpaqueCaptureAddress(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferOpaqueCaptureAddress(device, pInfo);
    }
    return result;
}

VKAPI_ATTR uint64_t VKAPI_CALL GetDeviceMemoryOpaqueCaptureAddress(
    VkDevice                                    device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
    }
    uint64_t result = device_data->dispatch_table.GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroySurfaceKHR(instance, surface, pAllocator);
    }
    instance_data->dispatch_table.DestroySurfaceKHR(instance, surface, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroySurfaceKHR(instance, surface, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    }
    VkResult result = device_data->dispatch_table.CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroySwapchainKHR(device, swapchain, pAllocator);
    }
    device_data->dispatch_table.DestroySwapchainKHR(device, swapchain, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroySwapchainKHR(device, swapchain, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pSwapchainImageCount,
    VkImage*                                    pSwapchainImages) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
    }
    VkResult result = device_data->dispatch_table.GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
    }
    VkResult result = device_data->dispatch_table.AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL QueuePresentKHR(
    VkQueue                                     queue,
    const VkPresentInfoKHR*                     pPresentInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueuePresentKHR(queue, pPresentInfo);
    }
    VkResult result = device_data->dispatch_table.QueuePresentKHR(queue, pPresentInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueuePresentKHR(queue, pPresentInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupPresentCapabilitiesKHR(
    VkDevice                                    device,
    VkDeviceGroupPresentCapabilitiesKHR*        pDeviceGroupPresentCapabilities) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
    }
    VkResult result = device_data->dispatch_table.GetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupSurfacePresentModesKHR(
    VkDevice                                    device,
    VkSurfaceKHR                                surface,
    VkDeviceGroupPresentModeFlagsKHR*           pModes) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
    }
    VkResult result = device_data->dispatch_table.GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceGroupSurfacePresentModesKHR(device, surface, pModes, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pRectCount,
    VkRect2D*                                   pRects) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImage2KHR(
    VkDevice                                    device,
    const VkAcquireNextImageInfoKHR*            pAcquireInfo,
    uint32_t*                                   pImageIndex) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
    }
    VkResult result = device_data->dispatch_table.AcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPropertiesKHR*                     pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlanePropertiesKHR*                pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);
    }
    VkResult result = instance_data->dispatch_table.GetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);
    }
    VkResult result = instance_data->dispatch_table.CreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);
    }
    VkResult result = instance_data->dispatch_table.GetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL CreateSharedSwapchainsKHR(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainCreateInfoKHR*             pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchains) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
    }
    VkResult result = device_data->dispatch_table.CreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains, result);
    }
    return result;
}

#ifdef VK_USE_PLATFORM_XLIB_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateXlibSurfaceKHR(
    VkInstance                                  instance,
    const VkXlibSurfaceCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    Display*                                    dpy,
    VisualID                                    visualID) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
    }
    VkBool32 result = instance_data->dispatch_table.GetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_XLIB_KHR */

#ifdef VK_USE_PLATFORM_XCB_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateXcbSurfaceKHR(
    VkInstance                                  instance,
    const VkXcbSurfaceCreateInfoKHR*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceXcbPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    xcb_connection_t*                           connection,
    xcb_visualid_t                              visual_id) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
    }
    VkBool32 result = instance_data->dispatch_table.GetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_XCB_KHR */

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateWaylandSurfaceKHR(
    VkInstance                                  instance,
    const VkWaylandSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    struct wl_display*                          display) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
    }
    VkBool32 result = instance_data->dispatch_table.GetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WAYLAND_KHR */

#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateAndroidSurfaceKHR(
    VkInstance                                  instance,
    const VkAndroidSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateAndroidSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateAndroidSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateAndroidSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_ANDROID_KHR */

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
    }
    VkBool32 result = instance_data->dispatch_table.GetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WIN32_KHR */




VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
    }
    instance_data->dispatch_table.GetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    }
}


VKAPI_ATTR void VKAPI_CALL GetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    }
    device_data->dispatch_table.GetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDeviceMaskKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDeviceMaskKHR(commandBuffer, deviceMask);
    }
    device_data->dispatch_table.CmdSetDeviceMaskKHR(commandBuffer, deviceMask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDeviceMaskKHR(commandBuffer, deviceMask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatchBaseKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    }
    device_data->dispatch_table.CmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    }
}



VKAPI_ATTR void VKAPI_CALL TrimCommandPoolKHR(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallTrimCommandPoolKHR(device, commandPool, flags);
    }
    device_data->dispatch_table.TrimCommandPoolKHR(device, commandPool, flags);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallTrimCommandPoolKHR(device, commandPool, flags);
    }
}


VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceGroupsKHR(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallEnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    }
    VkResult result = instance_data->dispatch_table.EnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallEnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties, result);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalBufferPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    }
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleKHR(
    VkDevice                                    device,
    const VkMemoryGetWin32HandleInfoKHR*        pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
    }
    VkResult result = device_data->dispatch_table.GetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandlePropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    HANDLE                                      handle,
    VkMemoryWin32HandlePropertiesKHR*           pMemoryWin32HandleProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
    }
    VkResult result = device_data->dispatch_table.GetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdKHR(
    VkDevice                                    device,
    const VkMemoryGetFdInfoKHR*                 pGetFdInfo,
    int*                                        pFd) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryFdKHR(device, pGetFdInfo, pFd);
    }
    VkResult result = device_data->dispatch_table.GetMemoryFdKHR(device, pGetFdInfo, pFd);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryFdKHR(device, pGetFdInfo, pFd, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdPropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    int                                         fd,
    VkMemoryFdPropertiesKHR*                    pMemoryFdProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
    }
    VkResult result = device_data->dispatch_table.GetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties, result);
    }
    return result;
}

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalSemaphorePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    }
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkImportSemaphoreWin32HandleInfoKHR*  pImportSemaphoreWin32HandleInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
    }
    VkResult result = device_data->dispatch_table.ImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkSemaphoreGetWin32HandleInfoKHR*     pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
    }
    VkResult result = device_data->dispatch_table.GetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreFdKHR(
    VkDevice                                    device,
    const VkImportSemaphoreFdInfoKHR*           pImportSemaphoreFdInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
    }
    VkResult result = device_data->dispatch_table.ImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreFdKHR(
    VkDevice                                    device,
    const VkSemaphoreGetFdInfoKHR*              pGetFdInfo,
    int*                                        pFd) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSemaphoreFdKHR(device, pGetFdInfo, pFd);
    }
    VkResult result = device_data->dispatch_table.GetSemaphoreFdKHR(device, pGetFdInfo, pFd);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSemaphoreFdKHR(device, pGetFdInfo, pFd, result);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetKHR(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
    }
    device_data->dispatch_table.CmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer                             commandBuffer,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    const void*                                 pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
    }
    device_data->dispatch_table.CmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
    }
}





VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
    }
    VkResult result = device_data->dispatch_table.CreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
    }
    device_data->dispatch_table.DestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSetWithTemplateKHR(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallUpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
    }
    device_data->dispatch_table.UpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallUpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
    }
}



VKAPI_ATTR VkResult VKAPI_CALL CreateRenderPass2KHR(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
    }
    VkResult result = device_data->dispatch_table.CreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBeginRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    const VkSubpassBeginInfo*                   pSubpassBeginInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    }
    device_data->dispatch_table.CmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdNextSubpass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassBeginInfo*                   pSubpassBeginInfo,
    const VkSubpassEndInfo*                     pSubpassEndInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    }
    device_data->dispatch_table.CmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassEndInfo*                     pSubpassEndInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
    }
    device_data->dispatch_table.CmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
    }
}


VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSwapchainStatusKHR(device, swapchain);
    }
    VkResult result = device_data->dispatch_table.GetSwapchainStatusKHR(device, swapchain);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSwapchainStatusKHR(device, swapchain, result);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalFencePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    }
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkImportFenceWin32HandleInfoKHR*      pImportFenceWin32HandleInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
    }
    VkResult result = device_data->dispatch_table.ImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkFenceGetWin32HandleInfoKHR*         pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
    }
    VkResult result = device_data->dispatch_table.GetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL ImportFenceFdKHR(
    VkDevice                                    device,
    const VkImportFenceFdInfoKHR*               pImportFenceFdInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallImportFenceFdKHR(device, pImportFenceFdInfo);
    }
    VkResult result = device_data->dispatch_table.ImportFenceFdKHR(device, pImportFenceFdInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallImportFenceFdKHR(device, pImportFenceFdInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceFdKHR(
    VkDevice                                    device,
    const VkFenceGetFdInfoKHR*                  pGetFdInfo,
    int*                                        pFd) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetFenceFdKHR(device, pGetFdInfo, pFd);
    }
    VkResult result = device_data->dispatch_table.GetFenceFdKHR(device, pGetFdInfo, pFd);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetFenceFdKHR(device, pGetFdInfo, pFd, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    uint32_t*                                   pCounterCount,
    VkPerformanceCounterKHR*                    pCounters,
    VkPerformanceCounterDescriptionKHR*         pCounterDescriptions) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
    }
    VkResult result = instance_data->dispatch_table.EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkQueryPoolPerformanceCreateInfoKHR*  pPerformanceQueryCreateInfo,
    uint32_t*                                   pNumPasses) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(physicalDevice, pPerformanceQueryCreateInfo, pNumPasses);
    }
    instance_data->dispatch_table.GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(physicalDevice, pPerformanceQueryCreateInfo, pNumPasses);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(physicalDevice, pPerformanceQueryCreateInfo, pNumPasses);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireProfilingLockKHR(
    VkDevice                                    device,
    const VkAcquireProfilingLockInfoKHR*        pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAcquireProfilingLockKHR(device, pInfo);
    }
    VkResult result = device_data->dispatch_table.AcquireProfilingLockKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAcquireProfilingLockKHR(device, pInfo, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL ReleaseProfilingLockKHR(
    VkDevice                                    device) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallReleaseProfilingLockKHR(device);
    }
    device_data->dispatch_table.ReleaseProfilingLockKHR(device);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallReleaseProfilingLockKHR(device);
    }
}



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkSurfaceCapabilities2KHR*                  pSurfaceCapabilities) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormat2KHR*                        pSurfaceFormats) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats, result);
    }
    return result;
}



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayProperties2KHR*                    pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlaneProperties2KHR*               pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayModeProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModeProperties2KHR*                pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkDisplayPlaneInfo2KHR*               pDisplayPlaneInfo,
    VkDisplayPlaneCapabilities2KHR*             pCapabilities) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);
    }
    VkResult result = instance_data->dispatch_table.GetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities, result);
    }
    return result;
}





VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
    device_data->dispatch_table.GetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
}



VKAPI_ATTR VkResult VKAPI_CALL CreateSamplerYcbcrConversionKHR(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);
    }
    VkResult result = device_data->dispatch_table.CreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySamplerYcbcrConversionKHR(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
    }
    device_data->dispatch_table.DestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
    }
}


VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindBufferMemory2KHR(device, bindInfoCount, pBindInfos);
    }
    VkResult result = device_data->dispatch_table.BindBufferMemory2KHR(device, bindInfoCount, pBindInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindBufferMemory2KHR(device, bindInfoCount, pBindInfos, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindImageMemory2KHR(device, bindInfoCount, pBindInfos);
    }
    VkResult result = device_data->dispatch_table.BindImageMemory2KHR(device, bindInfoCount, pBindInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindImageMemory2KHR(device, bindInfoCount, pBindInfos, result);
    }
    return result;
}

#ifdef VK_ENABLE_BETA_EXTENSIONS
#endif /* VK_ENABLE_BETA_EXTENSIONS */


VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutSupportKHR(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
    }
    device_data->dispatch_table.GetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
    }
}


VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}










VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreCounterValueKHR(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    uint64_t*                                   pValue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSemaphoreCounterValueKHR(device, semaphore, pValue);
    }
    VkResult result = device_data->dispatch_table.GetSemaphoreCounterValueKHR(device, semaphore, pValue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSemaphoreCounterValueKHR(device, semaphore, pValue, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL WaitSemaphoresKHR(
    VkDevice                                    device,
    const VkSemaphoreWaitInfo*                  pWaitInfo,
    uint64_t                                    timeout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallWaitSemaphoresKHR(device, pWaitInfo, timeout);
    }
    VkResult result = device_data->dispatch_table.WaitSemaphoresKHR(device, pWaitInfo, timeout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallWaitSemaphoresKHR(device, pWaitInfo, timeout, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL SignalSemaphoreKHR(
    VkDevice                                    device,
    const VkSemaphoreSignalInfo*                pSignalInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSignalSemaphoreKHR(device, pSignalInfo);
    }
    VkResult result = device_data->dispatch_table.SignalSemaphoreKHR(device, pSignalInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSignalSemaphoreKHR(device, pSignalInfo, result);
    }
    return result;
}







VKAPI_ATTR VkDeviceAddress VKAPI_CALL GetBufferDeviceAddressKHR(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferDeviceAddressKHR(device, pInfo);
    }
    VkDeviceAddress result = device_data->dispatch_table.GetBufferDeviceAddressKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferDeviceAddressKHR(device, pInfo);
    }
    return result;
}

VKAPI_ATTR uint64_t VKAPI_CALL GetBufferOpaqueCaptureAddressKHR(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferOpaqueCaptureAddressKHR(device, pInfo);
    }
    uint64_t result = device_data->dispatch_table.GetBufferOpaqueCaptureAddressKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferOpaqueCaptureAddressKHR(device, pInfo);
    }
    return result;
}

VKAPI_ATTR uint64_t VKAPI_CALL GetDeviceMemoryOpaqueCaptureAddressKHR(
    VkDevice                                    device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceMemoryOpaqueCaptureAddressKHR(device, pInfo);
    }
    uint64_t result = device_data->dispatch_table.GetDeviceMemoryOpaqueCaptureAddressKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceMemoryOpaqueCaptureAddressKHR(device, pInfo);
    }
    return result;
}

#ifdef VK_ENABLE_BETA_EXTENSIONS

VKAPI_ATTR VkResult VKAPI_CALL CreateDeferredOperationKHR(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator,
    VkDeferredOperationKHR*                     pDeferredOperation) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDeferredOperationKHR(device, pAllocator, pDeferredOperation);
    }
    VkResult result = device_data->dispatch_table.CreateDeferredOperationKHR(device, pAllocator, pDeferredOperation);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDeferredOperationKHR(device, pAllocator, pDeferredOperation, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDeferredOperationKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDeferredOperationKHR(device, operation, pAllocator);
    }
    device_data->dispatch_table.DestroyDeferredOperationKHR(device, operation, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDeferredOperationKHR(device, operation, pAllocator);
    }
}

VKAPI_ATTR uint32_t VKAPI_CALL GetDeferredOperationMaxConcurrencyKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeferredOperationMaxConcurrencyKHR(device, operation);
    }
    uint32_t result = device_data->dispatch_table.GetDeferredOperationMaxConcurrencyKHR(device, operation);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeferredOperationMaxConcurrencyKHR(device, operation);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeferredOperationResultKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeferredOperationResultKHR(device, operation);
    }
    VkResult result = device_data->dispatch_table.GetDeferredOperationResultKHR(device, operation);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeferredOperationResultKHR(device, operation, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL DeferredOperationJoinKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDeferredOperationJoinKHR(device, operation);
    }
    VkResult result = device_data->dispatch_table.DeferredOperationJoinKHR(device, operation);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDeferredOperationJoinKHR(device, operation, result);
    }
    return result;
}
#endif /* VK_ENABLE_BETA_EXTENSIONS */


VKAPI_ATTR VkResult VKAPI_CALL GetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties);
    }
    VkResult result = device_data->dispatch_table.GetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineExecutableStatisticsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pStatisticCount,
    VkPipelineExecutableStatisticKHR*           pStatistics) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics);
    }
    VkResult result = device_data->dispatch_table.GetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineExecutableInternalRepresentationsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPipelineExecutableInternalRepresentationsKHR(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);
    }
    VkResult result = device_data->dispatch_table.GetPipelineExecutableInternalRepresentationsKHR(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPipelineExecutableInternalRepresentationsKHR(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations, result);
    }
    return result;
}

#ifdef VK_ENABLE_BETA_EXTENSIONS
#endif /* VK_ENABLE_BETA_EXTENSIONS */



VKAPI_ATTR void VKAPI_CALL CmdCopyBuffer2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyBufferInfo2KHR*                 pCopyBufferInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyBuffer2KHR(commandBuffer, pCopyBufferInfo);
    }
    device_data->dispatch_table.CmdCopyBuffer2KHR(commandBuffer, pCopyBufferInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyBuffer2KHR(commandBuffer, pCopyBufferInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyImage2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyImageInfo2KHR*                  pCopyImageInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyImage2KHR(commandBuffer, pCopyImageInfo);
    }
    device_data->dispatch_table.CmdCopyImage2KHR(commandBuffer, pCopyImageInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyImage2KHR(commandBuffer, pCopyImageInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyBufferToImage2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyBufferToImageInfo2KHR*          pCopyBufferToImageInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyBufferToImage2KHR(commandBuffer, pCopyBufferToImageInfo);
    }
    device_data->dispatch_table.CmdCopyBufferToImage2KHR(commandBuffer, pCopyBufferToImageInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyBufferToImage2KHR(commandBuffer, pCopyBufferToImageInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyImageToBuffer2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyImageToBufferInfo2KHR*          pCopyImageToBufferInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyImageToBuffer2KHR(commandBuffer, pCopyImageToBufferInfo);
    }
    device_data->dispatch_table.CmdCopyImageToBuffer2KHR(commandBuffer, pCopyImageToBufferInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyImageToBuffer2KHR(commandBuffer, pCopyImageToBufferInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBlitImage2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkBlitImageInfo2KHR*                  pBlitImageInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBlitImage2KHR(commandBuffer, pBlitImageInfo);
    }
    device_data->dispatch_table.CmdBlitImage2KHR(commandBuffer, pBlitImageInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBlitImage2KHR(commandBuffer, pBlitImageInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResolveImage2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkResolveImageInfo2KHR*               pResolveImageInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdResolveImage2KHR(commandBuffer, pResolveImageInfo);
    }
    device_data->dispatch_table.CmdResolveImage2KHR(commandBuffer, pResolveImageInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdResolveImage2KHR(commandBuffer, pResolveImageInfo);
    }
}


VKAPI_ATTR void VKAPI_CALL DebugReportMessageEXT(
    VkInstance                                  instance,
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
    }
    instance_data->dispatch_table.DebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
    }
}








VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectTagEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectTagInfoEXT*        pTagInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDebugMarkerSetObjectTagEXT(device, pTagInfo);
    }
    VkResult result = device_data->dispatch_table.DebugMarkerSetObjectTagEXT(device, pTagInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDebugMarkerSetObjectTagEXT(device, pTagInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectNameEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectNameInfoEXT*       pNameInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDebugMarkerSetObjectNameEXT(device, pNameInfo);
    }
    VkResult result = device_data->dispatch_table.DebugMarkerSetObjectNameEXT(device, pNameInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDebugMarkerSetObjectNameEXT(device, pNameInfo, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerBeginEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
    }
    device_data->dispatch_table.CmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerEndEXT(
    VkCommandBuffer                             commandBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDebugMarkerEndEXT(commandBuffer);
    }
    device_data->dispatch_table.CmdDebugMarkerEndEXT(commandBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDebugMarkerEndEXT(commandBuffer);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerInsertEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
    }
    device_data->dispatch_table.CmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
    }
}




VKAPI_ATTR void VKAPI_CALL CmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
    }
    device_data->dispatch_table.CmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    }
    device_data->dispatch_table.CmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    }
    device_data->dispatch_table.CmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags,
    uint32_t                                    index) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
    }
    device_data->dispatch_table.CmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    uint32_t                                    index) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
    }
    device_data->dispatch_table.CmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectByteCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkBuffer                                    counterBuffer,
    VkDeviceSize                                counterBufferOffset,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
    }
    device_data->dispatch_table.CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
    }
}


VKAPI_ATTR uint32_t VKAPI_CALL GetImageViewHandleNVX(
    VkDevice                                    device,
    const VkImageViewHandleInfoNVX*             pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageViewHandleNVX(device, pInfo);
    }
    uint32_t result = device_data->dispatch_table.GetImageViewHandleNVX(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageViewHandleNVX(device, pInfo);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetImageViewAddressNVX(
    VkDevice                                    device,
    VkImageView                                 imageView,
    VkImageViewAddressPropertiesNVX*            pProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageViewAddressNVX(device, imageView, pProperties);
    }
    VkResult result = device_data->dispatch_table.GetImageViewAddressNVX(device, imageView, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageViewAddressNVX(device, imageView, pProperties, result);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}






VKAPI_ATTR VkResult VKAPI_CALL GetShaderInfoAMD(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    VkShaderStageFlagBits                       shaderStage,
    VkShaderInfoTypeAMD                         infoType,
    size_t*                                     pInfoSize,
    void*                                       pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
    }
    VkResult result = device_data->dispatch_table.GetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo, result);
    }
    return result;
}


#ifdef VK_USE_PLATFORM_GGP

VKAPI_ATTR VkResult VKAPI_CALL CreateStreamDescriptorSurfaceGGP(
    VkInstance                                  instance,
    const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateStreamDescriptorSurfaceGGP(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateStreamDescriptorSurfaceGGP(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateStreamDescriptorSurfaceGGP(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_GGP */




VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkExternalMemoryHandleTypeFlagsNV           externalHandleType,
    VkExternalImageFormatPropertiesNV*          pExternalImageFormatProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties, result);
    }
    return result;
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleNV(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkExternalMemoryHandleTypeFlagsNV           handleType,
    HANDLE*                                     pHandle) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryWin32HandleNV(device, memory, handleType, pHandle);
    }
    VkResult result = device_data->dispatch_table.GetMemoryWin32HandleNV(device, memory, handleType, pHandle);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryWin32HandleNV(device, memory, handleType, pHandle, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WIN32_KHR */

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif /* VK_USE_PLATFORM_WIN32_KHR */


#ifdef VK_USE_PLATFORM_VI_NN

VKAPI_ATTR VkResult VKAPI_CALL CreateViSurfaceNN(
    VkInstance                                  instance,
    const VkViSurfaceCreateInfoNN*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_VI_NN */






VKAPI_ATTR void VKAPI_CALL CmdBeginConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfoEXT*   pConditionalRenderingBegin) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
    }
    device_data->dispatch_table.CmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndConditionalRenderingEXT(commandBuffer);
    }
    device_data->dispatch_table.CmdEndConditionalRenderingEXT(commandBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndConditionalRenderingEXT(commandBuffer);
    }
}


VKAPI_ATTR void VKAPI_CALL CmdSetViewportWScalingNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewportWScalingNV*                 pViewportWScalings) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
    }
    device_data->dispatch_table.CmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
    }
}


VKAPI_ATTR VkResult VKAPI_CALL ReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallReleaseDisplayEXT(physicalDevice, display);
    }
    VkResult result = instance_data->dispatch_table.ReleaseDisplayEXT(physicalDevice, display);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallReleaseDisplayEXT(physicalDevice, display, result);
    }
    return result;
}

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT

VKAPI_ATTR VkResult VKAPI_CALL AcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAcquireXlibDisplayEXT(physicalDevice, dpy, display);
    }
    VkResult result = instance_data->dispatch_table.AcquireXlibDisplayEXT(physicalDevice, dpy, display);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAcquireXlibDisplayEXT(physicalDevice, dpy, display, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetRandROutputDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    RROutput                                    rrOutput,
    VkDisplayKHR*                               pDisplay) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay);
    }
    VkResult result = instance_data->dispatch_table.GetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_XLIB_XRANDR_EXT */


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL DisplayPowerControlEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayPowerInfoEXT*                pDisplayPowerInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDisplayPowerControlEXT(device, display, pDisplayPowerInfo);
    }
    VkResult result = device_data->dispatch_table.DisplayPowerControlEXT(device, display, pDisplayPowerInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDisplayPowerControlEXT(device, display, pDisplayPowerInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterDeviceEventEXT(
    VkDevice                                    device,
    const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallRegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
    }
    VkResult result = device_data->dispatch_table.RegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallRegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterDisplayEventEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayEventInfoEXT*                pDisplayEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallRegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
    }
    VkResult result = device_data->dispatch_table.RegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallRegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainCounterEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkSurfaceCounterFlagBitsEXT                 counter,
    uint64_t*                                   pCounterValue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
    }
    VkResult result = device_data->dispatch_table.GetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetSwapchainCounterEXT(device, swapchain, counter, pCounterValue, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetRefreshCycleDurationGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkRefreshCycleDurationGOOGLE*               pDisplayTimingProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
    }
    VkResult result = device_data->dispatch_table.GetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPastPresentationTimingGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE*             pPresentationTimings) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
    }
    VkResult result = device_data->dispatch_table.GetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings, result);
    }
    return result;
}







VKAPI_ATTR void VKAPI_CALL CmdSetDiscardRectangleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstDiscardRectangle,
    uint32_t                                    discardRectangleCount,
    const VkRect2D*                             pDiscardRectangles) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
    }
    device_data->dispatch_table.CmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
    }
}





VKAPI_ATTR void VKAPI_CALL SetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
    }
    device_data->dispatch_table.SetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
    }
}

#ifdef VK_USE_PLATFORM_IOS_MVK

VKAPI_ATTR VkResult VKAPI_CALL CreateIOSSurfaceMVK(
    VkInstance                                  instance,
    const VkIOSSurfaceCreateInfoMVK*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_IOS_MVK */

#ifdef VK_USE_PLATFORM_MACOS_MVK

VKAPI_ATTR VkResult VKAPI_CALL CreateMacOSSurfaceMVK(
    VkInstance                                  instance,
    const VkMacOSSurfaceCreateInfoMVK*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_MACOS_MVK */




VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSetDebugUtilsObjectNameEXT(device, pNameInfo);
    }
    VkResult result = device_data->dispatch_table.SetDebugUtilsObjectNameEXT(device, pNameInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSetDebugUtilsObjectNameEXT(device, pNameInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSetDebugUtilsObjectTagEXT(device, pTagInfo);
    }
    VkResult result = device_data->dispatch_table.SetDebugUtilsObjectTagEXT(device, pTagInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSetDebugUtilsObjectTagEXT(device, pTagInfo, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL QueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
    }
    device_data->dispatch_table.QueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL QueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueEndDebugUtilsLabelEXT(queue);
    }
    device_data->dispatch_table.QueueEndDebugUtilsLabelEXT(queue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueEndDebugUtilsLabelEXT(queue);
    }
}

VKAPI_ATTR void VKAPI_CALL QueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
    }
    device_data->dispatch_table.QueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
    device_data->dispatch_table.CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdEndDebugUtilsLabelEXT(commandBuffer);
    }
    device_data->dispatch_table.CmdEndDebugUtilsLabelEXT(commandBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdEndDebugUtilsLabelEXT(commandBuffer);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
    device_data->dispatch_table.CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
    }
    VkResult result = instance_data->dispatch_table.CreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    }
    instance_data->dispatch_table.DestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL SubmitDebugUtilsMessageEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
    }
    instance_data->dispatch_table.SubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
    }
}

#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetAndroidHardwareBufferPropertiesANDROID(
    VkDevice                                    device,
    const struct AHardwareBuffer*               buffer,
    VkAndroidHardwareBufferPropertiesANDROID*   pProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetAndroidHardwareBufferPropertiesANDROID(device, buffer, pProperties);
    }
    VkResult result = device_data->dispatch_table.GetAndroidHardwareBufferPropertiesANDROID(device, buffer, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetAndroidHardwareBufferPropertiesANDROID(device, buffer, pProperties, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryAndroidHardwareBufferANDROID(
    VkDevice                                    device,
    const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo,
    struct AHardwareBuffer**                    pBuffer) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryAndroidHardwareBufferANDROID(device, pInfo, pBuffer);
    }
    VkResult result = device_data->dispatch_table.GetMemoryAndroidHardwareBufferANDROID(device, pInfo, pBuffer);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryAndroidHardwareBufferANDROID(device, pInfo, pBuffer, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_ANDROID_KHR */








VKAPI_ATTR void VKAPI_CALL CmdSetSampleLocationsEXT(
    VkCommandBuffer                             commandBuffer,
    const VkSampleLocationsInfoEXT*             pSampleLocationsInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
    }
    device_data->dispatch_table.CmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    VkSampleCountFlagBits                       samples,
    VkMultisamplePropertiesEXT*                 pMultisampleProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
    }
    instance_data->dispatch_table.GetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
    }
}








VKAPI_ATTR VkResult VKAPI_CALL GetImageDrmFormatModifierPropertiesEXT(
    VkDevice                                    device,
    VkImage                                     image,
    VkImageDrmFormatModifierPropertiesEXT*      pProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
    }
    VkResult result = device_data->dispatch_table.GetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetImageDrmFormatModifierPropertiesEXT(device, image, pProperties, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL CreateValidationCacheEXT(
    VkDevice                                    device,
    const VkValidationCacheCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkValidationCacheEXT*                       pValidationCache) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
    }
    VkResult result = device_data->dispatch_table.CreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyValidationCacheEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyValidationCacheEXT(device, validationCache, pAllocator);
    }
    device_data->dispatch_table.DestroyValidationCacheEXT(device, validationCache, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyValidationCacheEXT(device, validationCache, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL MergeValidationCachesEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        dstCache,
    uint32_t                                    srcCacheCount,
    const VkValidationCacheEXT*                 pSrcCaches) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallMergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
    }
    VkResult result = device_data->dispatch_table.MergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallMergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetValidationCacheDataEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
    }
    VkResult result = device_data->dispatch_table.GetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetValidationCacheDataEXT(device, validationCache, pDataSize, pData, result);
    }
    return result;
}




VKAPI_ATTR void VKAPI_CALL CmdBindShadingRateImageNV(
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 imageView,
    VkImageLayout                               imageLayout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
    }
    device_data->dispatch_table.CmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkShadingRatePaletteNV*               pShadingRatePalettes) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
    }
    device_data->dispatch_table.CmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetCoarseSampleOrderNV(
    VkCommandBuffer                             commandBuffer,
    VkCoarseSampleOrderTypeNV                   sampleOrderType,
    uint32_t                                    customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV*          pCustomSampleOrders) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
    }
    device_data->dispatch_table.CmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
    }
}


VKAPI_ATTR VkResult VKAPI_CALL CreateAccelerationStructureNV(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoNV*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureNV*                  pAccelerationStructure) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
    }
    VkResult result = device_data->dispatch_table.CreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyAccelerationStructureKHR(
    VkDevice                                    device,
    VkAccelerationStructureKHR                  accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
    }
    device_data->dispatch_table.DestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL DestroyAccelerationStructureNV(
    VkDevice                                    device,
    VkAccelerationStructureKHR                  accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
    }
    device_data->dispatch_table.DestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
    }
}

VKAPI_ATTR void VKAPI_CALL GetAccelerationStructureMemoryRequirementsNV(
    VkDevice                                    device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR*                   pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BindAccelerationStructureMemoryKHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoKHR* pBindInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindAccelerationStructureMemoryKHR(device, bindInfoCount, pBindInfos);
    }
    VkResult result = device_data->dispatch_table.BindAccelerationStructureMemoryKHR(device, bindInfoCount, pBindInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindAccelerationStructureMemoryKHR(device, bindInfoCount, pBindInfos, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindAccelerationStructureMemoryNV(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoKHR* pBindInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
    }
    VkResult result = device_data->dispatch_table.BindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    const VkAccelerationStructureInfoNV*        pInfo,
    VkBuffer                                    instanceData,
    VkDeviceSize                                instanceOffset,
    VkBool32                                    update,
    VkAccelerationStructureKHR                  dst,
    VkAccelerationStructureKHR                  src,
    VkBuffer                                    scratch,
    VkDeviceSize                                scratchOffset) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
    }
    device_data->dispatch_table.CmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    VkAccelerationStructureKHR                  dst,
    VkAccelerationStructureKHR                  src,
    VkCopyAccelerationStructureModeKHR          mode) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
    }
    device_data->dispatch_table.CmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdTraceRaysNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    raygenShaderBindingTableBuffer,
    VkDeviceSize                                raygenShaderBindingOffset,
    VkBuffer                                    missShaderBindingTableBuffer,
    VkDeviceSize                                missShaderBindingOffset,
    VkDeviceSize                                missShaderBindingStride,
    VkBuffer                                    hitShaderBindingTableBuffer,
    VkDeviceSize                                hitShaderBindingOffset,
    VkDeviceSize                                hitShaderBindingStride,
    VkBuffer                                    callableShaderBindingTableBuffer,
    VkDeviceSize                                callableShaderBindingOffset,
    VkDeviceSize                                callableShaderBindingStride,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
    }
    device_data->dispatch_table.CmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRayTracingPipelinesNV(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoNV*     pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    }
    VkResult result = device_data->dispatch_table.CreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetRayTracingShaderGroupHandlesKHR(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
    }
    VkResult result = device_data->dispatch_table.GetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetRayTracingShaderGroupHandlesNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
    }
    VkResult result = device_data->dispatch_table.GetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetAccelerationStructureHandleNV(
    VkDevice                                    device,
    VkAccelerationStructureKHR                  accelerationStructure,
    size_t                                      dataSize,
    void*                                       pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
    }
    VkResult result = device_data->dispatch_table.GetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdWriteAccelerationStructuresPropertiesKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureKHR*           pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    }
    device_data->dispatch_table.CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureKHR*           pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    }
    device_data->dispatch_table.CmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CompileDeferredNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    shader) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCompileDeferredNV(device, pipeline, shader);
    }
    VkResult result = device_data->dispatch_table.CompileDeferredNV(device, pipeline, shader);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCompileDeferredNV(device, pipeline, shader, result);
    }
    return result;
}






VKAPI_ATTR VkResult VKAPI_CALL GetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
    }
    VkResult result = device_data->dispatch_table.GetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties, result);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL CmdWriteBufferMarkerAMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
    }
    device_data->dispatch_table.CmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
    }
}



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pTimeDomainCount,
    VkTimeDomainEXT*                            pTimeDomains) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetCalibratedTimestampsEXT(
    VkDevice                                    device,
    uint32_t                                    timestampCount,
    const VkCalibratedTimestampInfoEXT*         pTimestampInfos,
    uint64_t*                                   pTimestamps,
    uint64_t*                                   pMaxDeviation) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
    }
    VkResult result = device_data->dispatch_table.GetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation, result);
    }
    return result;
}




#ifdef VK_USE_PLATFORM_GGP
#endif /* VK_USE_PLATFORM_GGP */





VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    taskCount,
    uint32_t                                    firstTask) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
    }
    device_data->dispatch_table.CmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksIndirectNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
    }
    device_data->dispatch_table.CmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
    device_data->dispatch_table.CmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    }
}




VKAPI_ATTR void VKAPI_CALL CmdSetExclusiveScissorNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstExclusiveScissor,
    uint32_t                                    exclusiveScissorCount,
    const VkRect2D*                             pExclusiveScissors) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
    }
    device_data->dispatch_table.CmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
    }
}


VKAPI_ATTR void VKAPI_CALL CmdSetCheckpointNV(
    VkCommandBuffer                             commandBuffer,
    const void*                                 pCheckpointMarker) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
    }
    device_data->dispatch_table.CmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
    }
}

VKAPI_ATTR void VKAPI_CALL GetQueueCheckpointDataNV(
    VkQueue                                     queue,
    uint32_t*                                   pCheckpointDataCount,
    VkCheckpointDataNV*                         pCheckpointData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
    }
    device_data->dispatch_table.GetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
    }
}



VKAPI_ATTR VkResult VKAPI_CALL InitializePerformanceApiINTEL(
    VkDevice                                    device,
    const VkInitializePerformanceApiInfoINTEL*  pInitializeInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallInitializePerformanceApiINTEL(device, pInitializeInfo);
    }
    VkResult result = device_data->dispatch_table.InitializePerformanceApiINTEL(device, pInitializeInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallInitializePerformanceApiINTEL(device, pInitializeInfo, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL UninitializePerformanceApiINTEL(
    VkDevice                                    device) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallUninitializePerformanceApiINTEL(device);
    }
    device_data->dispatch_table.UninitializePerformanceApiINTEL(device);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallUninitializePerformanceApiINTEL(device);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CmdSetPerformanceMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceMarkerInfoINTEL*         pMarkerInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetPerformanceMarkerINTEL(commandBuffer, pMarkerInfo);
    }
    VkResult result = device_data->dispatch_table.CmdSetPerformanceMarkerINTEL(commandBuffer, pMarkerInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetPerformanceMarkerINTEL(commandBuffer, pMarkerInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CmdSetPerformanceStreamMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceStreamMarkerInfoINTEL*   pMarkerInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetPerformanceStreamMarkerINTEL(commandBuffer, pMarkerInfo);
    }
    VkResult result = device_data->dispatch_table.CmdSetPerformanceStreamMarkerINTEL(commandBuffer, pMarkerInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetPerformanceStreamMarkerINTEL(commandBuffer, pMarkerInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CmdSetPerformanceOverrideINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceOverrideInfoINTEL*       pOverrideInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetPerformanceOverrideINTEL(commandBuffer, pOverrideInfo);
    }
    VkResult result = device_data->dispatch_table.CmdSetPerformanceOverrideINTEL(commandBuffer, pOverrideInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetPerformanceOverrideINTEL(commandBuffer, pOverrideInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquirePerformanceConfigurationINTEL(
    VkDevice                                    device,
    const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
    VkPerformanceConfigurationINTEL*            pConfiguration) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAcquirePerformanceConfigurationINTEL(device, pAcquireInfo, pConfiguration);
    }
    VkResult result = device_data->dispatch_table.AcquirePerformanceConfigurationINTEL(device, pAcquireInfo, pConfiguration);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAcquirePerformanceConfigurationINTEL(device, pAcquireInfo, pConfiguration, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL ReleasePerformanceConfigurationINTEL(
    VkDevice                                    device,
    VkPerformanceConfigurationINTEL             configuration) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallReleasePerformanceConfigurationINTEL(device, configuration);
    }
    VkResult result = device_data->dispatch_table.ReleasePerformanceConfigurationINTEL(device, configuration);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallReleasePerformanceConfigurationINTEL(device, configuration, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL QueueSetPerformanceConfigurationINTEL(
    VkQueue                                     queue,
    VkPerformanceConfigurationINTEL             configuration) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(queue), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallQueueSetPerformanceConfigurationINTEL(queue, configuration);
    }
    VkResult result = device_data->dispatch_table.QueueSetPerformanceConfigurationINTEL(queue, configuration);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallQueueSetPerformanceConfigurationINTEL(queue, configuration, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPerformanceParameterINTEL(
    VkDevice                                    device,
    VkPerformanceParameterTypeINTEL             parameter,
    VkPerformanceValueINTEL*                    pValue) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPerformanceParameterINTEL(device, parameter, pValue);
    }
    VkResult result = device_data->dispatch_table.GetPerformanceParameterINTEL(device, parameter, pValue);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPerformanceParameterINTEL(device, parameter, pValue, result);
    }
    return result;
}



VKAPI_ATTR void VKAPI_CALL SetLocalDimmingAMD(
    VkDevice                                    device,
    VkSwapchainKHR                              swapChain,
    VkBool32                                    localDimmingEnable) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSetLocalDimmingAMD(device, swapChain, localDimmingEnable);
    }
    device_data->dispatch_table.SetLocalDimmingAMD(device, swapChain, localDimmingEnable);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSetLocalDimmingAMD(device, swapChain, localDimmingEnable);
    }
}

#ifdef VK_USE_PLATFORM_FUCHSIA

VKAPI_ATTR VkResult VKAPI_CALL CreateImagePipeSurfaceFUCHSIA(
    VkInstance                                  instance,
    const VkImagePipeSurfaceCreateInfoFUCHSIA*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_FUCHSIA */

#ifdef VK_USE_PLATFORM_METAL_EXT

VKAPI_ATTR VkResult VKAPI_CALL CreateMetalSurfaceEXT(
    VkInstance                                  instance,
    const VkMetalSurfaceCreateInfoEXT*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateMetalSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateMetalSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateMetalSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_METAL_EXT */












VKAPI_ATTR VkDeviceAddress VKAPI_CALL GetBufferDeviceAddressEXT(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetBufferDeviceAddressEXT(device, pInfo);
    }
    VkDeviceAddress result = device_data->dispatch_table.GetBufferDeviceAddressEXT(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetBufferDeviceAddressEXT(device, pInfo);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceToolPropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pToolCount,
    VkPhysicalDeviceToolPropertiesEXT*          pToolProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties, result);
    }
    return result;
}




VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceCooperativeMatrixPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkCooperativeMatrixPropertiesNV*            pProperties) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceCooperativeMatrixPropertiesNV(physicalDevice, pPropertyCount, pProperties);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceCooperativeMatrixPropertiesNV(physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceCooperativeMatrixPropertiesNV(physicalDevice, pPropertyCount, pProperties, result);
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pCombinationCount,
    VkFramebufferMixedSamplesCombinationNV*     pCombinations) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(physicalDevice, pCombinationCount, pCombinations);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(physicalDevice, pCombinationCount, pCombinations);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(physicalDevice, pCombinationCount, pCombinations, result);
    }
    return result;
}



#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModes2EXT(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceSurfacePresentModes2EXT(physicalDevice, pSurfaceInfo, pPresentModeCount, pPresentModes);
    }
    VkResult result = instance_data->dispatch_table.GetPhysicalDeviceSurfacePresentModes2EXT(physicalDevice, pSurfaceInfo, pPresentModeCount, pPresentModes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceSurfacePresentModes2EXT(physicalDevice, pSurfaceInfo, pPresentModeCount, pPresentModes, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallAcquireFullScreenExclusiveModeEXT(device, swapchain);
    }
    VkResult result = device_data->dispatch_table.AcquireFullScreenExclusiveModeEXT(device, swapchain);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallAcquireFullScreenExclusiveModeEXT(device, swapchain, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL ReleaseFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallReleaseFullScreenExclusiveModeEXT(device, swapchain);
    }
    VkResult result = device_data->dispatch_table.ReleaseFullScreenExclusiveModeEXT(device, swapchain);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallReleaseFullScreenExclusiveModeEXT(device, swapchain, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupSurfacePresentModes2EXT(
    VkDevice                                    device,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkDeviceGroupPresentModeFlagsKHR*           pModes) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceGroupSurfacePresentModes2EXT(device, pSurfaceInfo, pModes);
    }
    VkResult result = device_data->dispatch_table.GetDeviceGroupSurfacePresentModes2EXT(device, pSurfaceInfo, pModes);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceGroupSurfacePresentModes2EXT(device, pSurfaceInfo, pModes, result);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL CreateHeadlessSurfaceEXT(
    VkInstance                                  instance,
    const VkHeadlessSurfaceCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateHeadlessSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateHeadlessSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateHeadlessSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL CmdSetLineStippleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    lineStippleFactor,
    uint16_t                                    lineStipplePattern) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetLineStippleEXT(commandBuffer, lineStippleFactor, lineStipplePattern);
    }
    device_data->dispatch_table.CmdSetLineStippleEXT(commandBuffer, lineStippleFactor, lineStipplePattern);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetLineStippleEXT(commandBuffer, lineStippleFactor, lineStipplePattern);
    }
}



VKAPI_ATTR void VKAPI_CALL ResetQueryPoolEXT(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallResetQueryPoolEXT(device, queryPool, firstQuery, queryCount);
    }
    device_data->dispatch_table.ResetQueryPoolEXT(device, queryPool, firstQuery, queryCount);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallResetQueryPoolEXT(device, queryPool, firstQuery, queryCount);
    }
}



VKAPI_ATTR void VKAPI_CALL CmdSetCullModeEXT(
    VkCommandBuffer                             commandBuffer,
    VkCullModeFlags                             cullMode) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetCullModeEXT(commandBuffer, cullMode);
    }
    device_data->dispatch_table.CmdSetCullModeEXT(commandBuffer, cullMode);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetCullModeEXT(commandBuffer, cullMode);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetFrontFaceEXT(
    VkCommandBuffer                             commandBuffer,
    VkFrontFace                                 frontFace) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetFrontFaceEXT(commandBuffer, frontFace);
    }
    device_data->dispatch_table.CmdSetFrontFaceEXT(commandBuffer, frontFace);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetFrontFaceEXT(commandBuffer, frontFace);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetPrimitiveTopologyEXT(
    VkCommandBuffer                             commandBuffer,
    VkPrimitiveTopology                         primitiveTopology) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology);
    }
    device_data->dispatch_table.CmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetViewportWithCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetViewportWithCountEXT(commandBuffer, viewportCount, pViewports);
    }
    device_data->dispatch_table.CmdSetViewportWithCountEXT(commandBuffer, viewportCount, pViewports);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetViewportWithCountEXT(commandBuffer, viewportCount, pViewports);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetScissorWithCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetScissorWithCountEXT(commandBuffer, scissorCount, pScissors);
    }
    device_data->dispatch_table.CmdSetScissorWithCountEXT(commandBuffer, scissorCount, pScissors);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetScissorWithCountEXT(commandBuffer, scissorCount, pScissors);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindVertexBuffers2EXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes,
    const VkDeviceSize*                         pStrides) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
    }
    device_data->dispatch_table.CmdBindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthTestEnableEXT(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthTestEnable) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDepthTestEnableEXT(commandBuffer, depthTestEnable);
    }
    device_data->dispatch_table.CmdSetDepthTestEnableEXT(commandBuffer, depthTestEnable);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDepthTestEnableEXT(commandBuffer, depthTestEnable);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthWriteEnableEXT(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthWriteEnable) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDepthWriteEnableEXT(commandBuffer, depthWriteEnable);
    }
    device_data->dispatch_table.CmdSetDepthWriteEnableEXT(commandBuffer, depthWriteEnable);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDepthWriteEnableEXT(commandBuffer, depthWriteEnable);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthCompareOpEXT(
    VkCommandBuffer                             commandBuffer,
    VkCompareOp                                 depthCompareOp) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDepthCompareOpEXT(commandBuffer, depthCompareOp);
    }
    device_data->dispatch_table.CmdSetDepthCompareOpEXT(commandBuffer, depthCompareOp);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDepthCompareOpEXT(commandBuffer, depthCompareOp);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBoundsTestEnableEXT(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthBoundsTestEnable) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetDepthBoundsTestEnableEXT(commandBuffer, depthBoundsTestEnable);
    }
    device_data->dispatch_table.CmdSetDepthBoundsTestEnableEXT(commandBuffer, depthBoundsTestEnable);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetDepthBoundsTestEnableEXT(commandBuffer, depthBoundsTestEnable);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilTestEnableEXT(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    stencilTestEnable) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetStencilTestEnableEXT(commandBuffer, stencilTestEnable);
    }
    device_data->dispatch_table.CmdSetStencilTestEnableEXT(commandBuffer, stencilTestEnable);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetStencilTestEnableEXT(commandBuffer, stencilTestEnable);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilOpEXT(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    VkStencilOp                                 failOp,
    VkStencilOp                                 passOp,
    VkStencilOp                                 depthFailOp,
    VkCompareOp                                 compareOp) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdSetStencilOpEXT(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
    }
    device_data->dispatch_table.CmdSetStencilOpEXT(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdSetStencilOpEXT(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
    }
}



VKAPI_ATTR void VKAPI_CALL GetGeneratedCommandsMemoryRequirementsNV(
    VkDevice                                    device,
    const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPreprocessGeneratedCommandsNV(
    VkCommandBuffer                             commandBuffer,
    const VkGeneratedCommandsInfoNV*            pGeneratedCommandsInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
    }
    device_data->dispatch_table.CmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdExecuteGeneratedCommandsNV(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    isPreprocessed,
    const VkGeneratedCommandsInfoNV*            pGeneratedCommandsInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
    }
    device_data->dispatch_table.CmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindPipelineShaderGroupNV(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline,
    uint32_t                                    groupIndex) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
    }
    device_data->dispatch_table.CmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateIndirectCommandsLayoutNV(
    VkDevice                                    device,
    const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectCommandsLayoutNV*                 pIndirectCommandsLayout) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
    }
    VkResult result = device_data->dispatch_table.CreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyIndirectCommandsLayoutNV(
    VkDevice                                    device,
    VkIndirectCommandsLayoutNV                  indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
    }
    device_data->dispatch_table.DestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
    }
}







VKAPI_ATTR VkResult VKAPI_CALL CreatePrivateDataSlotEXT(
    VkDevice                                    device,
    const VkPrivateDataSlotCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPrivateDataSlotEXT*                       pPrivateDataSlot) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreatePrivateDataSlotEXT(device, pCreateInfo, pAllocator, pPrivateDataSlot);
    }
    VkResult result = device_data->dispatch_table.CreatePrivateDataSlotEXT(device, pCreateInfo, pAllocator, pPrivateDataSlot);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreatePrivateDataSlotEXT(device, pCreateInfo, pAllocator, pPrivateDataSlot, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPrivateDataSlotEXT(
    VkDevice                                    device,
    VkPrivateDataSlotEXT                        privateDataSlot,
    const VkAllocationCallbacks*                pAllocator) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallDestroyPrivateDataSlotEXT(device, privateDataSlot, pAllocator);
    }
    device_data->dispatch_table.DestroyPrivateDataSlotEXT(device, privateDataSlot, pAllocator);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallDestroyPrivateDataSlotEXT(device, privateDataSlot, pAllocator);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL SetPrivateDataEXT(
    VkDevice                                    device,
    VkObjectType                                objectType,
    uint64_t                                    objectHandle,
    VkPrivateDataSlotEXT                        privateDataSlot,
    uint64_t                                    data) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallSetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, data);
    }
    VkResult result = device_data->dispatch_table.SetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, data);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallSetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, data, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPrivateDataEXT(
    VkDevice                                    device,
    VkObjectType                                objectType,
    uint64_t                                    objectHandle,
    VkPrivateDataSlotEXT                        privateDataSlot,
    uint64_t*                                   pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, pData);
    }
    device_data->dispatch_table.GetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, pData);
    }
}







#ifdef VK_USE_PLATFORM_DIRECTFB_EXT

VKAPI_ATTR VkResult VKAPI_CALL CreateDirectFBSurfaceEXT(
    VkInstance                                  instance,
    const VkDirectFBSurfaceCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(instance), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateDirectFBSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
    }
    VkResult result = instance_data->dispatch_table.CreateDirectFBSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateDirectFBSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface, result);
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceDirectFBPresentationSupportEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    IDirectFB*                                  dfb) {
    instance_layer_data *instance_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), instance_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetPhysicalDeviceDirectFBPresentationSupportEXT(physicalDevice, queueFamilyIndex, dfb);
    }
    VkBool32 result = instance_data->dispatch_table.GetPhysicalDeviceDirectFBPresentationSupportEXT(physicalDevice, queueFamilyIndex, dfb);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetPhysicalDeviceDirectFBPresentationSupportEXT(physicalDevice, queueFamilyIndex, dfb);
    }
    return result;
}
#endif /* VK_USE_PLATFORM_DIRECTFB_EXT */

#ifdef VK_ENABLE_BETA_EXTENSIONS

VKAPI_ATTR VkResult VKAPI_CALL CreateAccelerationStructureKHR(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureKHR*                 pAccelerationStructure) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);
    }
    VkResult result = device_data->dispatch_table.CreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetAccelerationStructureMemoryRequirementsKHR(
    VkDevice                                    device,
    const VkAccelerationStructureMemoryRequirementsInfoKHR* pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetAccelerationStructureMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
    }
    device_data->dispatch_table.GetAccelerationStructureMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetAccelerationStructureMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructureKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildOffsetInfoKHR* const* ppOffsetInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBuildAccelerationStructureKHR(commandBuffer, infoCount, pInfos, ppOffsetInfos);
    }
    device_data->dispatch_table.CmdBuildAccelerationStructureKHR(commandBuffer, infoCount, pInfos, ppOffsetInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBuildAccelerationStructureKHR(commandBuffer, infoCount, pInfos, ppOffsetInfos);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructureIndirectKHR(
    VkCommandBuffer                             commandBuffer,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfo,
    VkBuffer                                    indirectBuffer,
    VkDeviceSize                                indirectOffset,
    uint32_t                                    indirectStride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdBuildAccelerationStructureIndirectKHR(commandBuffer, pInfo, indirectBuffer, indirectOffset, indirectStride);
    }
    device_data->dispatch_table.CmdBuildAccelerationStructureIndirectKHR(commandBuffer, pInfo, indirectBuffer, indirectOffset, indirectStride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdBuildAccelerationStructureIndirectKHR(commandBuffer, pInfo, indirectBuffer, indirectOffset, indirectStride);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BuildAccelerationStructureKHR(
    VkDevice                                    device,
    uint32_t                                    infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildOffsetInfoKHR* const* ppOffsetInfos) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallBuildAccelerationStructureKHR(device, infoCount, pInfos, ppOffsetInfos);
    }
    VkResult result = device_data->dispatch_table.BuildAccelerationStructureKHR(device, infoCount, pInfos, ppOffsetInfos);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallBuildAccelerationStructureKHR(device, infoCount, pInfos, ppOffsetInfos, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CopyAccelerationStructureKHR(
    VkDevice                                    device,
    const VkCopyAccelerationStructureInfoKHR*   pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCopyAccelerationStructureKHR(device, pInfo);
    }
    VkResult result = device_data->dispatch_table.CopyAccelerationStructureKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCopyAccelerationStructureKHR(device, pInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CopyAccelerationStructureToMemoryKHR(
    VkDevice                                    device,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCopyAccelerationStructureToMemoryKHR(device, pInfo);
    }
    VkResult result = device_data->dispatch_table.CopyAccelerationStructureToMemoryKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCopyAccelerationStructureToMemoryKHR(device, pInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CopyMemoryToAccelerationStructureKHR(
    VkDevice                                    device,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCopyMemoryToAccelerationStructureKHR(device, pInfo);
    }
    VkResult result = device_data->dispatch_table.CopyMemoryToAccelerationStructureKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCopyMemoryToAccelerationStructureKHR(device, pInfo, result);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL WriteAccelerationStructuresPropertiesKHR(
    VkDevice                                    device,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureKHR*           pAccelerationStructures,
    VkQueryType                                 queryType,
    size_t                                      dataSize,
    void*                                       pData,
    size_t                                      stride) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallWriteAccelerationStructuresPropertiesKHR(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);
    }
    VkResult result = device_data->dispatch_table.WriteAccelerationStructuresPropertiesKHR(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallWriteAccelerationStructuresPropertiesKHR(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyAccelerationStructureInfoKHR*   pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
    }
    device_data->dispatch_table.CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureToMemoryKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
    }
    device_data->dispatch_table.CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyMemoryToAccelerationStructureKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
    }
    device_data->dispatch_table.CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdTraceRaysKHR(
    VkCommandBuffer                             commandBuffer,
    const VkStridedBufferRegionKHR*             pRaygenShaderBindingTable,
    const VkStridedBufferRegionKHR*             pMissShaderBindingTable,
    const VkStridedBufferRegionKHR*             pHitShaderBindingTable,
    const VkStridedBufferRegionKHR*             pCallableShaderBindingTable,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
    }
    device_data->dispatch_table.CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRayTracingPipelinesKHR(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR*    pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCreateRayTracingPipelinesKHR(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    }
    VkResult result = device_data->dispatch_table.CreateRayTracingPipelinesKHR(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCreateRayTracingPipelinesKHR(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines, result);
    }
    return result;
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL GetAccelerationStructureDeviceAddressKHR(
    VkDevice                                    device,
    const VkAccelerationStructureDeviceAddressInfoKHR* pInfo) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetAccelerationStructureDeviceAddressKHR(device, pInfo);
    }
    VkDeviceAddress result = device_data->dispatch_table.GetAccelerationStructureDeviceAddressKHR(device, pInfo);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetAccelerationStructureDeviceAddressKHR(device, pInfo);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetRayTracingCaptureReplayShaderGroupHandlesKHR(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
    }
    VkResult result = device_data->dispatch_table.GetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData, result);
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdTraceRaysIndirectKHR(
    VkCommandBuffer                             commandBuffer,
    const VkStridedBufferRegionKHR*             pRaygenShaderBindingTable,
    const VkStridedBufferRegionKHR*             pMissShaderBindingTable,
    const VkStridedBufferRegionKHR*             pHitShaderBindingTable,
    const VkStridedBufferRegionKHR*             pCallableShaderBindingTable,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallCmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, buffer, offset);
    }
    device_data->dispatch_table.CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, buffer, offset);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallCmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, buffer, offset);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceAccelerationStructureCompatibilityKHR(
    VkDevice                                    device,
    const VkAccelerationStructureVersionKHR*    version) {
    device_layer_data *device_data = GetLayerDataPtr(get_dispatch_key(device), device_layer_data_map);
    for (auto intercept : global_interceptor_list) {
        intercept->PreCallGetDeviceAccelerationStructureCompatibilityKHR(device, version);
    }
    VkResult result = device_data->dispatch_table.GetDeviceAccelerationStructureCompatibilityKHR(device, version);
    for (auto intercept : global_interceptor_list) {
        intercept->PostCallGetDeviceAccelerationStructureCompatibilityKHR(device, version, result);
    }
    return result;
}
#endif /* VK_ENABLE_BETA_EXTENSIONS */

// Map of all APIs to be intercepted by this layer
const std::unordered_map<std::string, void*> name_to_funcptr_map = {
    {"vkCreateInstance", (void*)CreateInstance},
    {"vkDestroyInstance", (void*)DestroyInstance},
    {"vkEnumeratePhysicalDevices", (void*)EnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceFeatures", (void*)GetPhysicalDeviceFeatures},
    {"vkGetPhysicalDeviceFormatProperties", (void*)GetPhysicalDeviceFormatProperties},
    {"vkGetPhysicalDeviceImageFormatProperties", (void*)GetPhysicalDeviceImageFormatProperties},
    {"vkGetPhysicalDeviceProperties", (void*)GetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceQueueFamilyProperties", (void*)GetPhysicalDeviceQueueFamilyProperties},
    {"vkGetPhysicalDeviceMemoryProperties", (void*)GetPhysicalDeviceMemoryProperties},
    {"vkGetInstanceProcAddr", (void*)GetInstanceProcAddr},
    {"vkGetDeviceProcAddr", (void*)GetDeviceProcAddr},
    {"vkCreateDevice", (void*)CreateDevice},
    {"vkDestroyDevice", (void*)DestroyDevice},
    {"vkEnumerateInstanceExtensionProperties", (void*)EnumerateInstanceExtensionProperties},
    {"vkEnumerateDeviceExtensionProperties", (void*)EnumerateDeviceExtensionProperties},
    {"vkEnumerateInstanceLayerProperties", (void*)EnumerateInstanceLayerProperties},
    {"vkEnumerateDeviceLayerProperties", (void*)EnumerateDeviceLayerProperties},
    {"vkGetDeviceQueue", (void*)GetDeviceQueue},
    {"vkQueueSubmit", (void*)QueueSubmit},
    {"vkQueueWaitIdle", (void*)QueueWaitIdle},
    {"vkDeviceWaitIdle", (void*)DeviceWaitIdle},
    {"vkAllocateMemory", (void*)AllocateMemory},
    {"vkFreeMemory", (void*)FreeMemory},
    {"vkMapMemory", (void*)MapMemory},
    {"vkUnmapMemory", (void*)UnmapMemory},
    {"vkFlushMappedMemoryRanges", (void*)FlushMappedMemoryRanges},
    {"vkInvalidateMappedMemoryRanges", (void*)InvalidateMappedMemoryRanges},
    {"vkGetDeviceMemoryCommitment", (void*)GetDeviceMemoryCommitment},
    {"vkBindBufferMemory", (void*)BindBufferMemory},
    {"vkBindImageMemory", (void*)BindImageMemory},
    {"vkGetBufferMemoryRequirements", (void*)GetBufferMemoryRequirements},
    {"vkGetImageMemoryRequirements", (void*)GetImageMemoryRequirements},
    {"vkGetImageSparseMemoryRequirements", (void*)GetImageSparseMemoryRequirements},
    {"vkGetPhysicalDeviceSparseImageFormatProperties", (void*)GetPhysicalDeviceSparseImageFormatProperties},
    {"vkQueueBindSparse", (void*)QueueBindSparse},
    {"vkCreateFence", (void*)CreateFence},
    {"vkDestroyFence", (void*)DestroyFence},
    {"vkResetFences", (void*)ResetFences},
    {"vkGetFenceStatus", (void*)GetFenceStatus},
    {"vkWaitForFences", (void*)WaitForFences},
    {"vkCreateSemaphore", (void*)CreateSemaphore},
    {"vkDestroySemaphore", (void*)DestroySemaphore},
    {"vkCreateEvent", (void*)CreateEvent},
    {"vkDestroyEvent", (void*)DestroyEvent},
    {"vkGetEventStatus", (void*)GetEventStatus},
    {"vkSetEvent", (void*)SetEvent},
    {"vkResetEvent", (void*)ResetEvent},
    {"vkCreateQueryPool", (void*)CreateQueryPool},
    {"vkDestroyQueryPool", (void*)DestroyQueryPool},
    {"vkGetQueryPoolResults", (void*)GetQueryPoolResults},
    {"vkCreateBuffer", (void*)CreateBuffer},
    {"vkDestroyBuffer", (void*)DestroyBuffer},
    {"vkCreateBufferView", (void*)CreateBufferView},
    {"vkDestroyBufferView", (void*)DestroyBufferView},
    {"vkCreateImage", (void*)CreateImage},
    {"vkDestroyImage", (void*)DestroyImage},
    {"vkGetImageSubresourceLayout", (void*)GetImageSubresourceLayout},
    {"vkCreateImageView", (void*)CreateImageView},
    {"vkDestroyImageView", (void*)DestroyImageView},
    {"vkCreateShaderModule", (void*)CreateShaderModule},
    {"vkDestroyShaderModule", (void*)DestroyShaderModule},
    {"vkCreatePipelineCache", (void*)CreatePipelineCache},
    {"vkDestroyPipelineCache", (void*)DestroyPipelineCache},
    {"vkGetPipelineCacheData", (void*)GetPipelineCacheData},
    {"vkMergePipelineCaches", (void*)MergePipelineCaches},
    {"vkCreateGraphicsPipelines", (void*)CreateGraphicsPipelines},
    {"vkCreateComputePipelines", (void*)CreateComputePipelines},
    {"vkDestroyPipeline", (void*)DestroyPipeline},
    {"vkCreatePipelineLayout", (void*)CreatePipelineLayout},
    {"vkDestroyPipelineLayout", (void*)DestroyPipelineLayout},
    {"vkCreateSampler", (void*)CreateSampler},
    {"vkDestroySampler", (void*)DestroySampler},
    {"vkCreateDescriptorSetLayout", (void*)CreateDescriptorSetLayout},
    {"vkDestroyDescriptorSetLayout", (void*)DestroyDescriptorSetLayout},
    {"vkCreateDescriptorPool", (void*)CreateDescriptorPool},
    {"vkDestroyDescriptorPool", (void*)DestroyDescriptorPool},
    {"vkResetDescriptorPool", (void*)ResetDescriptorPool},
    {"vkAllocateDescriptorSets", (void*)AllocateDescriptorSets},
    {"vkFreeDescriptorSets", (void*)FreeDescriptorSets},
    {"vkUpdateDescriptorSets", (void*)UpdateDescriptorSets},
    {"vkCreateFramebuffer", (void*)CreateFramebuffer},
    {"vkDestroyFramebuffer", (void*)DestroyFramebuffer},
    {"vkCreateRenderPass", (void*)CreateRenderPass},
    {"vkDestroyRenderPass", (void*)DestroyRenderPass},
    {"vkGetRenderAreaGranularity", (void*)GetRenderAreaGranularity},
    {"vkCreateCommandPool", (void*)CreateCommandPool},
    {"vkDestroyCommandPool", (void*)DestroyCommandPool},
    {"vkResetCommandPool", (void*)ResetCommandPool},
    {"vkAllocateCommandBuffers", (void*)AllocateCommandBuffers},
    {"vkFreeCommandBuffers", (void*)FreeCommandBuffers},
    {"vkBeginCommandBuffer", (void*)BeginCommandBuffer},
    {"vkEndCommandBuffer", (void*)EndCommandBuffer},
    {"vkResetCommandBuffer", (void*)ResetCommandBuffer},
    {"vkCmdBindPipeline", (void*)CmdBindPipeline},
    {"vkCmdSetViewport", (void*)CmdSetViewport},
    {"vkCmdSetScissor", (void*)CmdSetScissor},
    {"vkCmdSetLineWidth", (void*)CmdSetLineWidth},
    {"vkCmdSetDepthBias", (void*)CmdSetDepthBias},
    {"vkCmdSetBlendConstants", (void*)CmdSetBlendConstants},
    {"vkCmdSetDepthBounds", (void*)CmdSetDepthBounds},
    {"vkCmdSetStencilCompareMask", (void*)CmdSetStencilCompareMask},
    {"vkCmdSetStencilWriteMask", (void*)CmdSetStencilWriteMask},
    {"vkCmdSetStencilReference", (void*)CmdSetStencilReference},
    {"vkCmdBindDescriptorSets", (void*)CmdBindDescriptorSets},
    {"vkCmdBindIndexBuffer", (void*)CmdBindIndexBuffer},
    {"vkCmdBindVertexBuffers", (void*)CmdBindVertexBuffers},
    {"vkCmdDraw", (void*)CmdDraw},
    {"vkCmdDrawIndexed", (void*)CmdDrawIndexed},
    {"vkCmdDrawIndirect", (void*)CmdDrawIndirect},
    {"vkCmdDrawIndexedIndirect", (void*)CmdDrawIndexedIndirect},
    {"vkCmdDispatch", (void*)CmdDispatch},
    {"vkCmdDispatchIndirect", (void*)CmdDispatchIndirect},
    {"vkCmdCopyBuffer", (void*)CmdCopyBuffer},
    {"vkCmdCopyImage", (void*)CmdCopyImage},
    {"vkCmdBlitImage", (void*)CmdBlitImage},
    {"vkCmdCopyBufferToImage", (void*)CmdCopyBufferToImage},
    {"vkCmdCopyImageToBuffer", (void*)CmdCopyImageToBuffer},
    {"vkCmdUpdateBuffer", (void*)CmdUpdateBuffer},
    {"vkCmdFillBuffer", (void*)CmdFillBuffer},
    {"vkCmdClearColorImage", (void*)CmdClearColorImage},
    {"vkCmdClearDepthStencilImage", (void*)CmdClearDepthStencilImage},
    {"vkCmdClearAttachments", (void*)CmdClearAttachments},
    {"vkCmdResolveImage", (void*)CmdResolveImage},
    {"vkCmdSetEvent", (void*)CmdSetEvent},
    {"vkCmdResetEvent", (void*)CmdResetEvent},
    {"vkCmdWaitEvents", (void*)CmdWaitEvents},
    {"vkCmdPipelineBarrier", (void*)CmdPipelineBarrier},
    {"vkCmdBeginQuery", (void*)CmdBeginQuery},
    {"vkCmdEndQuery", (void*)CmdEndQuery},
    {"vkCmdResetQueryPool", (void*)CmdResetQueryPool},
    {"vkCmdWriteTimestamp", (void*)CmdWriteTimestamp},
    {"vkCmdCopyQueryPoolResults", (void*)CmdCopyQueryPoolResults},
    {"vkCmdPushConstants", (void*)CmdPushConstants},
    {"vkCmdBeginRenderPass", (void*)CmdBeginRenderPass},
    {"vkCmdNextSubpass", (void*)CmdNextSubpass},
    {"vkCmdEndRenderPass", (void*)CmdEndRenderPass},
    {"vkCmdExecuteCommands", (void*)CmdExecuteCommands},
    {"vkBindBufferMemory2", (void*)BindBufferMemory2},
    {"vkBindImageMemory2", (void*)BindImageMemory2},
    {"vkGetDeviceGroupPeerMemoryFeatures", (void*)GetDeviceGroupPeerMemoryFeatures},
    {"vkCmdSetDeviceMask", (void*)CmdSetDeviceMask},
    {"vkCmdDispatchBase", (void*)CmdDispatchBase},
    {"vkEnumeratePhysicalDeviceGroups", (void*)EnumeratePhysicalDeviceGroups},
    {"vkGetImageMemoryRequirements2", (void*)GetImageMemoryRequirements2},
    {"vkGetBufferMemoryRequirements2", (void*)GetBufferMemoryRequirements2},
    {"vkGetImageSparseMemoryRequirements2", (void*)GetImageSparseMemoryRequirements2},
    {"vkGetPhysicalDeviceFeatures2", (void*)GetPhysicalDeviceFeatures2},
    {"vkGetPhysicalDeviceProperties2", (void*)GetPhysicalDeviceProperties2},
    {"vkGetPhysicalDeviceFormatProperties2", (void*)GetPhysicalDeviceFormatProperties2},
    {"vkGetPhysicalDeviceImageFormatProperties2", (void*)GetPhysicalDeviceImageFormatProperties2},
    {"vkGetPhysicalDeviceQueueFamilyProperties2", (void*)GetPhysicalDeviceQueueFamilyProperties2},
    {"vkGetPhysicalDeviceMemoryProperties2", (void*)GetPhysicalDeviceMemoryProperties2},
    {"vkGetPhysicalDeviceSparseImageFormatProperties2", (void*)GetPhysicalDeviceSparseImageFormatProperties2},
    {"vkTrimCommandPool", (void*)TrimCommandPool},
    {"vkGetDeviceQueue2", (void*)GetDeviceQueue2},
    {"vkCreateSamplerYcbcrConversion", (void*)CreateSamplerYcbcrConversion},
    {"vkDestroySamplerYcbcrConversion", (void*)DestroySamplerYcbcrConversion},
    {"vkCreateDescriptorUpdateTemplate", (void*)CreateDescriptorUpdateTemplate},
    {"vkDestroyDescriptorUpdateTemplate", (void*)DestroyDescriptorUpdateTemplate},
    {"vkUpdateDescriptorSetWithTemplate", (void*)UpdateDescriptorSetWithTemplate},
    {"vkGetPhysicalDeviceExternalBufferProperties", (void*)GetPhysicalDeviceExternalBufferProperties},
    {"vkGetPhysicalDeviceExternalFenceProperties", (void*)GetPhysicalDeviceExternalFenceProperties},
    {"vkGetPhysicalDeviceExternalSemaphoreProperties", (void*)GetPhysicalDeviceExternalSemaphoreProperties},
    {"vkGetDescriptorSetLayoutSupport", (void*)GetDescriptorSetLayoutSupport},
    {"vkCmdDrawIndirectCount", (void*)CmdDrawIndirectCount},
    {"vkCmdDrawIndexedIndirectCount", (void*)CmdDrawIndexedIndirectCount},
    {"vkCreateRenderPass2", (void*)CreateRenderPass2},
    {"vkCmdBeginRenderPass2", (void*)CmdBeginRenderPass2},
    {"vkCmdNextSubpass2", (void*)CmdNextSubpass2},
    {"vkCmdEndRenderPass2", (void*)CmdEndRenderPass2},
    {"vkResetQueryPool", (void*)ResetQueryPool},
    {"vkGetSemaphoreCounterValue", (void*)GetSemaphoreCounterValue},
    {"vkWaitSemaphores", (void*)WaitSemaphores},
    {"vkSignalSemaphore", (void*)SignalSemaphore},
    {"vkGetBufferDeviceAddress", (void*)GetBufferDeviceAddress},
    {"vkGetBufferOpaqueCaptureAddress", (void*)GetBufferOpaqueCaptureAddress},
    {"vkGetDeviceMemoryOpaqueCaptureAddress", (void*)GetDeviceMemoryOpaqueCaptureAddress},
    {"vkDestroySurfaceKHR", (void*)DestroySurfaceKHR},
    {"vkGetPhysicalDeviceSurfaceSupportKHR", (void*)GetPhysicalDeviceSurfaceSupportKHR},
    {"vkGetPhysicalDeviceSurfaceCapabilitiesKHR", (void*)GetPhysicalDeviceSurfaceCapabilitiesKHR},
    {"vkGetPhysicalDeviceSurfaceFormatsKHR", (void*)GetPhysicalDeviceSurfaceFormatsKHR},
    {"vkGetPhysicalDeviceSurfacePresentModesKHR", (void*)GetPhysicalDeviceSurfacePresentModesKHR},
    {"vkCreateSwapchainKHR", (void*)CreateSwapchainKHR},
    {"vkDestroySwapchainKHR", (void*)DestroySwapchainKHR},
    {"vkGetSwapchainImagesKHR", (void*)GetSwapchainImagesKHR},
    {"vkAcquireNextImageKHR", (void*)AcquireNextImageKHR},
    {"vkQueuePresentKHR", (void*)QueuePresentKHR},
    {"vkGetDeviceGroupPresentCapabilitiesKHR", (void*)GetDeviceGroupPresentCapabilitiesKHR},
    {"vkGetDeviceGroupSurfacePresentModesKHR", (void*)GetDeviceGroupSurfacePresentModesKHR},
    {"vkGetPhysicalDevicePresentRectanglesKHR", (void*)GetPhysicalDevicePresentRectanglesKHR},
    {"vkAcquireNextImage2KHR", (void*)AcquireNextImage2KHR},
    {"vkGetPhysicalDeviceDisplayPropertiesKHR", (void*)GetPhysicalDeviceDisplayPropertiesKHR},
    {"vkGetPhysicalDeviceDisplayPlanePropertiesKHR", (void*)GetPhysicalDeviceDisplayPlanePropertiesKHR},
    {"vkGetDisplayPlaneSupportedDisplaysKHR", (void*)GetDisplayPlaneSupportedDisplaysKHR},
    {"vkGetDisplayModePropertiesKHR", (void*)GetDisplayModePropertiesKHR},
    {"vkCreateDisplayModeKHR", (void*)CreateDisplayModeKHR},
    {"vkGetDisplayPlaneCapabilitiesKHR", (void*)GetDisplayPlaneCapabilitiesKHR},
    {"vkCreateDisplayPlaneSurfaceKHR", (void*)CreateDisplayPlaneSurfaceKHR},
    {"vkCreateSharedSwapchainsKHR", (void*)CreateSharedSwapchainsKHR},
#ifdef VK_USE_PLATFORM_XLIB_KHR
    {"vkCreateXlibSurfaceKHR", (void*)CreateXlibSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    {"vkGetPhysicalDeviceXlibPresentationSupportKHR", (void*)GetPhysicalDeviceXlibPresentationSupportKHR},
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    {"vkCreateXcbSurfaceKHR", (void*)CreateXcbSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    {"vkGetPhysicalDeviceXcbPresentationSupportKHR", (void*)GetPhysicalDeviceXcbPresentationSupportKHR},
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    {"vkCreateWaylandSurfaceKHR", (void*)CreateWaylandSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    {"vkGetPhysicalDeviceWaylandPresentationSupportKHR", (void*)GetPhysicalDeviceWaylandPresentationSupportKHR},
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    {"vkCreateAndroidSurfaceKHR", (void*)CreateAndroidSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkCreateWin32SurfaceKHR", (void*)CreateWin32SurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetPhysicalDeviceWin32PresentationSupportKHR", (void*)GetPhysicalDeviceWin32PresentationSupportKHR},
#endif
    {"vkGetPhysicalDeviceFeatures2KHR", (void*)GetPhysicalDeviceFeatures2KHR},
    {"vkGetPhysicalDeviceProperties2KHR", (void*)GetPhysicalDeviceProperties2KHR},
    {"vkGetPhysicalDeviceFormatProperties2KHR", (void*)GetPhysicalDeviceFormatProperties2KHR},
    {"vkGetPhysicalDeviceImageFormatProperties2KHR", (void*)GetPhysicalDeviceImageFormatProperties2KHR},
    {"vkGetPhysicalDeviceQueueFamilyProperties2KHR", (void*)GetPhysicalDeviceQueueFamilyProperties2KHR},
    {"vkGetPhysicalDeviceMemoryProperties2KHR", (void*)GetPhysicalDeviceMemoryProperties2KHR},
    {"vkGetPhysicalDeviceSparseImageFormatProperties2KHR", (void*)GetPhysicalDeviceSparseImageFormatProperties2KHR},
    {"vkGetDeviceGroupPeerMemoryFeaturesKHR", (void*)GetDeviceGroupPeerMemoryFeaturesKHR},
    {"vkCmdSetDeviceMaskKHR", (void*)CmdSetDeviceMaskKHR},
    {"vkCmdDispatchBaseKHR", (void*)CmdDispatchBaseKHR},
    {"vkTrimCommandPoolKHR", (void*)TrimCommandPoolKHR},
    {"vkEnumeratePhysicalDeviceGroupsKHR", (void*)EnumeratePhysicalDeviceGroupsKHR},
    {"vkGetPhysicalDeviceExternalBufferPropertiesKHR", (void*)GetPhysicalDeviceExternalBufferPropertiesKHR},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetMemoryWin32HandleKHR", (void*)GetMemoryWin32HandleKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetMemoryWin32HandlePropertiesKHR", (void*)GetMemoryWin32HandlePropertiesKHR},
#endif
    {"vkGetMemoryFdKHR", (void*)GetMemoryFdKHR},
    {"vkGetMemoryFdPropertiesKHR", (void*)GetMemoryFdPropertiesKHR},
    {"vkGetPhysicalDeviceExternalSemaphorePropertiesKHR", (void*)GetPhysicalDeviceExternalSemaphorePropertiesKHR},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkImportSemaphoreWin32HandleKHR", (void*)ImportSemaphoreWin32HandleKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetSemaphoreWin32HandleKHR", (void*)GetSemaphoreWin32HandleKHR},
#endif
    {"vkImportSemaphoreFdKHR", (void*)ImportSemaphoreFdKHR},
    {"vkGetSemaphoreFdKHR", (void*)GetSemaphoreFdKHR},
    {"vkCmdPushDescriptorSetKHR", (void*)CmdPushDescriptorSetKHR},
    {"vkCmdPushDescriptorSetWithTemplateKHR", (void*)CmdPushDescriptorSetWithTemplateKHR},
    {"vkCreateDescriptorUpdateTemplateKHR", (void*)CreateDescriptorUpdateTemplateKHR},
    {"vkDestroyDescriptorUpdateTemplateKHR", (void*)DestroyDescriptorUpdateTemplateKHR},
    {"vkUpdateDescriptorSetWithTemplateKHR", (void*)UpdateDescriptorSetWithTemplateKHR},
    {"vkCreateRenderPass2KHR", (void*)CreateRenderPass2KHR},
    {"vkCmdBeginRenderPass2KHR", (void*)CmdBeginRenderPass2KHR},
    {"vkCmdNextSubpass2KHR", (void*)CmdNextSubpass2KHR},
    {"vkCmdEndRenderPass2KHR", (void*)CmdEndRenderPass2KHR},
    {"vkGetSwapchainStatusKHR", (void*)GetSwapchainStatusKHR},
    {"vkGetPhysicalDeviceExternalFencePropertiesKHR", (void*)GetPhysicalDeviceExternalFencePropertiesKHR},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkImportFenceWin32HandleKHR", (void*)ImportFenceWin32HandleKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetFenceWin32HandleKHR", (void*)GetFenceWin32HandleKHR},
#endif
    {"vkImportFenceFdKHR", (void*)ImportFenceFdKHR},
    {"vkGetFenceFdKHR", (void*)GetFenceFdKHR},
    {"vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR", (void*)EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR},
    {"vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR", (void*)GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR},
    {"vkAcquireProfilingLockKHR", (void*)AcquireProfilingLockKHR},
    {"vkReleaseProfilingLockKHR", (void*)ReleaseProfilingLockKHR},
    {"vkGetPhysicalDeviceSurfaceCapabilities2KHR", (void*)GetPhysicalDeviceSurfaceCapabilities2KHR},
    {"vkGetPhysicalDeviceSurfaceFormats2KHR", (void*)GetPhysicalDeviceSurfaceFormats2KHR},
    {"vkGetPhysicalDeviceDisplayProperties2KHR", (void*)GetPhysicalDeviceDisplayProperties2KHR},
    {"vkGetPhysicalDeviceDisplayPlaneProperties2KHR", (void*)GetPhysicalDeviceDisplayPlaneProperties2KHR},
    {"vkGetDisplayModeProperties2KHR", (void*)GetDisplayModeProperties2KHR},
    {"vkGetDisplayPlaneCapabilities2KHR", (void*)GetDisplayPlaneCapabilities2KHR},
    {"vkGetImageMemoryRequirements2KHR", (void*)GetImageMemoryRequirements2KHR},
    {"vkGetBufferMemoryRequirements2KHR", (void*)GetBufferMemoryRequirements2KHR},
    {"vkGetImageSparseMemoryRequirements2KHR", (void*)GetImageSparseMemoryRequirements2KHR},
    {"vkCreateSamplerYcbcrConversionKHR", (void*)CreateSamplerYcbcrConversionKHR},
    {"vkDestroySamplerYcbcrConversionKHR", (void*)DestroySamplerYcbcrConversionKHR},
    {"vkBindBufferMemory2KHR", (void*)BindBufferMemory2KHR},
    {"vkBindImageMemory2KHR", (void*)BindImageMemory2KHR},
    {"vkGetDescriptorSetLayoutSupportKHR", (void*)GetDescriptorSetLayoutSupportKHR},
    {"vkCmdDrawIndirectCountKHR", (void*)CmdDrawIndirectCountKHR},
    {"vkCmdDrawIndexedIndirectCountKHR", (void*)CmdDrawIndexedIndirectCountKHR},
    {"vkGetSemaphoreCounterValueKHR", (void*)GetSemaphoreCounterValueKHR},
    {"vkWaitSemaphoresKHR", (void*)WaitSemaphoresKHR},
    {"vkSignalSemaphoreKHR", (void*)SignalSemaphoreKHR},
    {"vkGetBufferDeviceAddressKHR", (void*)GetBufferDeviceAddressKHR},
    {"vkGetBufferOpaqueCaptureAddressKHR", (void*)GetBufferOpaqueCaptureAddressKHR},
    {"vkGetDeviceMemoryOpaqueCaptureAddressKHR", (void*)GetDeviceMemoryOpaqueCaptureAddressKHR},
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCreateDeferredOperationKHR", (void*)CreateDeferredOperationKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkDestroyDeferredOperationKHR", (void*)DestroyDeferredOperationKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkGetDeferredOperationMaxConcurrencyKHR", (void*)GetDeferredOperationMaxConcurrencyKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkGetDeferredOperationResultKHR", (void*)GetDeferredOperationResultKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkDeferredOperationJoinKHR", (void*)DeferredOperationJoinKHR},
#endif
    {"vkGetPipelineExecutablePropertiesKHR", (void*)GetPipelineExecutablePropertiesKHR},
    {"vkGetPipelineExecutableStatisticsKHR", (void*)GetPipelineExecutableStatisticsKHR},
    {"vkGetPipelineExecutableInternalRepresentationsKHR", (void*)GetPipelineExecutableInternalRepresentationsKHR},
    {"vkCmdCopyBuffer2KHR", (void*)CmdCopyBuffer2KHR},
    {"vkCmdCopyImage2KHR", (void*)CmdCopyImage2KHR},
    {"vkCmdCopyBufferToImage2KHR", (void*)CmdCopyBufferToImage2KHR},
    {"vkCmdCopyImageToBuffer2KHR", (void*)CmdCopyImageToBuffer2KHR},
    {"vkCmdBlitImage2KHR", (void*)CmdBlitImage2KHR},
    {"vkCmdResolveImage2KHR", (void*)CmdResolveImage2KHR},
    {"vkCreateDebugReportCallbackEXT", (void*)CreateDebugReportCallbackEXT},
    {"vkDestroyDebugReportCallbackEXT", (void*)DestroyDebugReportCallbackEXT},
    {"vkDebugReportMessageEXT", (void*)DebugReportMessageEXT},
    {"vkDebugMarkerSetObjectTagEXT", (void*)DebugMarkerSetObjectTagEXT},
    {"vkDebugMarkerSetObjectNameEXT", (void*)DebugMarkerSetObjectNameEXT},
    {"vkCmdDebugMarkerBeginEXT", (void*)CmdDebugMarkerBeginEXT},
    {"vkCmdDebugMarkerEndEXT", (void*)CmdDebugMarkerEndEXT},
    {"vkCmdDebugMarkerInsertEXT", (void*)CmdDebugMarkerInsertEXT},
    {"vkCmdBindTransformFeedbackBuffersEXT", (void*)CmdBindTransformFeedbackBuffersEXT},
    {"vkCmdBeginTransformFeedbackEXT", (void*)CmdBeginTransformFeedbackEXT},
    {"vkCmdEndTransformFeedbackEXT", (void*)CmdEndTransformFeedbackEXT},
    {"vkCmdBeginQueryIndexedEXT", (void*)CmdBeginQueryIndexedEXT},
    {"vkCmdEndQueryIndexedEXT", (void*)CmdEndQueryIndexedEXT},
    {"vkCmdDrawIndirectByteCountEXT", (void*)CmdDrawIndirectByteCountEXT},
    {"vkGetImageViewHandleNVX", (void*)GetImageViewHandleNVX},
    {"vkGetImageViewAddressNVX", (void*)GetImageViewAddressNVX},
    {"vkCmdDrawIndirectCountAMD", (void*)CmdDrawIndirectCountAMD},
    {"vkCmdDrawIndexedIndirectCountAMD", (void*)CmdDrawIndexedIndirectCountAMD},
    {"vkGetShaderInfoAMD", (void*)GetShaderInfoAMD},
#ifdef VK_USE_PLATFORM_GGP
    {"vkCreateStreamDescriptorSurfaceGGP", (void*)CreateStreamDescriptorSurfaceGGP},
#endif
    {"vkGetPhysicalDeviceExternalImageFormatPropertiesNV", (void*)GetPhysicalDeviceExternalImageFormatPropertiesNV},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetMemoryWin32HandleNV", (void*)GetMemoryWin32HandleNV},
#endif
#ifdef VK_USE_PLATFORM_VI_NN
    {"vkCreateViSurfaceNN", (void*)CreateViSurfaceNN},
#endif
    {"vkCmdBeginConditionalRenderingEXT", (void*)CmdBeginConditionalRenderingEXT},
    {"vkCmdEndConditionalRenderingEXT", (void*)CmdEndConditionalRenderingEXT},
    {"vkCmdSetViewportWScalingNV", (void*)CmdSetViewportWScalingNV},
    {"vkReleaseDisplayEXT", (void*)ReleaseDisplayEXT},
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    {"vkAcquireXlibDisplayEXT", (void*)AcquireXlibDisplayEXT},
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    {"vkGetRandROutputDisplayEXT", (void*)GetRandROutputDisplayEXT},
#endif
    {"vkGetPhysicalDeviceSurfaceCapabilities2EXT", (void*)GetPhysicalDeviceSurfaceCapabilities2EXT},
    {"vkDisplayPowerControlEXT", (void*)DisplayPowerControlEXT},
    {"vkRegisterDeviceEventEXT", (void*)RegisterDeviceEventEXT},
    {"vkRegisterDisplayEventEXT", (void*)RegisterDisplayEventEXT},
    {"vkGetSwapchainCounterEXT", (void*)GetSwapchainCounterEXT},
    {"vkGetRefreshCycleDurationGOOGLE", (void*)GetRefreshCycleDurationGOOGLE},
    {"vkGetPastPresentationTimingGOOGLE", (void*)GetPastPresentationTimingGOOGLE},
    {"vkCmdSetDiscardRectangleEXT", (void*)CmdSetDiscardRectangleEXT},
    {"vkSetHdrMetadataEXT", (void*)SetHdrMetadataEXT},
#ifdef VK_USE_PLATFORM_IOS_MVK
    {"vkCreateIOSSurfaceMVK", (void*)CreateIOSSurfaceMVK},
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    {"vkCreateMacOSSurfaceMVK", (void*)CreateMacOSSurfaceMVK},
#endif
    {"vkSetDebugUtilsObjectNameEXT", (void*)SetDebugUtilsObjectNameEXT},
    {"vkSetDebugUtilsObjectTagEXT", (void*)SetDebugUtilsObjectTagEXT},
    {"vkQueueBeginDebugUtilsLabelEXT", (void*)QueueBeginDebugUtilsLabelEXT},
    {"vkQueueEndDebugUtilsLabelEXT", (void*)QueueEndDebugUtilsLabelEXT},
    {"vkQueueInsertDebugUtilsLabelEXT", (void*)QueueInsertDebugUtilsLabelEXT},
    {"vkCmdBeginDebugUtilsLabelEXT", (void*)CmdBeginDebugUtilsLabelEXT},
    {"vkCmdEndDebugUtilsLabelEXT", (void*)CmdEndDebugUtilsLabelEXT},
    {"vkCmdInsertDebugUtilsLabelEXT", (void*)CmdInsertDebugUtilsLabelEXT},
    {"vkCreateDebugUtilsMessengerEXT", (void*)CreateDebugUtilsMessengerEXT},
    {"vkDestroyDebugUtilsMessengerEXT", (void*)DestroyDebugUtilsMessengerEXT},
    {"vkSubmitDebugUtilsMessageEXT", (void*)SubmitDebugUtilsMessageEXT},
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    {"vkGetAndroidHardwareBufferPropertiesANDROID", (void*)GetAndroidHardwareBufferPropertiesANDROID},
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    {"vkGetMemoryAndroidHardwareBufferANDROID", (void*)GetMemoryAndroidHardwareBufferANDROID},
#endif
    {"vkCmdSetSampleLocationsEXT", (void*)CmdSetSampleLocationsEXT},
    {"vkGetPhysicalDeviceMultisamplePropertiesEXT", (void*)GetPhysicalDeviceMultisamplePropertiesEXT},
    {"vkGetImageDrmFormatModifierPropertiesEXT", (void*)GetImageDrmFormatModifierPropertiesEXT},
    {"vkCreateValidationCacheEXT", (void*)CreateValidationCacheEXT},
    {"vkDestroyValidationCacheEXT", (void*)DestroyValidationCacheEXT},
    {"vkMergeValidationCachesEXT", (void*)MergeValidationCachesEXT},
    {"vkGetValidationCacheDataEXT", (void*)GetValidationCacheDataEXT},
    {"vkCmdBindShadingRateImageNV", (void*)CmdBindShadingRateImageNV},
    {"vkCmdSetViewportShadingRatePaletteNV", (void*)CmdSetViewportShadingRatePaletteNV},
    {"vkCmdSetCoarseSampleOrderNV", (void*)CmdSetCoarseSampleOrderNV},
    {"vkCreateAccelerationStructureNV", (void*)CreateAccelerationStructureNV},
    {"vkDestroyAccelerationStructureKHR", (void*)DestroyAccelerationStructureKHR},
    {"vkDestroyAccelerationStructureNV", (void*)DestroyAccelerationStructureNV},
    {"vkGetAccelerationStructureMemoryRequirementsNV", (void*)GetAccelerationStructureMemoryRequirementsNV},
    {"vkBindAccelerationStructureMemoryKHR", (void*)BindAccelerationStructureMemoryKHR},
    {"vkBindAccelerationStructureMemoryNV", (void*)BindAccelerationStructureMemoryNV},
    {"vkCmdBuildAccelerationStructureNV", (void*)CmdBuildAccelerationStructureNV},
    {"vkCmdCopyAccelerationStructureNV", (void*)CmdCopyAccelerationStructureNV},
    {"vkCmdTraceRaysNV", (void*)CmdTraceRaysNV},
    {"vkCreateRayTracingPipelinesNV", (void*)CreateRayTracingPipelinesNV},
    {"vkGetRayTracingShaderGroupHandlesKHR", (void*)GetRayTracingShaderGroupHandlesKHR},
    {"vkGetRayTracingShaderGroupHandlesNV", (void*)GetRayTracingShaderGroupHandlesNV},
    {"vkGetAccelerationStructureHandleNV", (void*)GetAccelerationStructureHandleNV},
    {"vkCmdWriteAccelerationStructuresPropertiesKHR", (void*)CmdWriteAccelerationStructuresPropertiesKHR},
    {"vkCmdWriteAccelerationStructuresPropertiesNV", (void*)CmdWriteAccelerationStructuresPropertiesNV},
    {"vkCompileDeferredNV", (void*)CompileDeferredNV},
    {"vkGetMemoryHostPointerPropertiesEXT", (void*)GetMemoryHostPointerPropertiesEXT},
    {"vkCmdWriteBufferMarkerAMD", (void*)CmdWriteBufferMarkerAMD},
    {"vkGetPhysicalDeviceCalibrateableTimeDomainsEXT", (void*)GetPhysicalDeviceCalibrateableTimeDomainsEXT},
    {"vkGetCalibratedTimestampsEXT", (void*)GetCalibratedTimestampsEXT},
    {"vkCmdDrawMeshTasksNV", (void*)CmdDrawMeshTasksNV},
    {"vkCmdDrawMeshTasksIndirectNV", (void*)CmdDrawMeshTasksIndirectNV},
    {"vkCmdDrawMeshTasksIndirectCountNV", (void*)CmdDrawMeshTasksIndirectCountNV},
    {"vkCmdSetExclusiveScissorNV", (void*)CmdSetExclusiveScissorNV},
    {"vkCmdSetCheckpointNV", (void*)CmdSetCheckpointNV},
    {"vkGetQueueCheckpointDataNV", (void*)GetQueueCheckpointDataNV},
    {"vkInitializePerformanceApiINTEL", (void*)InitializePerformanceApiINTEL},
    {"vkUninitializePerformanceApiINTEL", (void*)UninitializePerformanceApiINTEL},
    {"vkCmdSetPerformanceMarkerINTEL", (void*)CmdSetPerformanceMarkerINTEL},
    {"vkCmdSetPerformanceStreamMarkerINTEL", (void*)CmdSetPerformanceStreamMarkerINTEL},
    {"vkCmdSetPerformanceOverrideINTEL", (void*)CmdSetPerformanceOverrideINTEL},
    {"vkAcquirePerformanceConfigurationINTEL", (void*)AcquirePerformanceConfigurationINTEL},
    {"vkReleasePerformanceConfigurationINTEL", (void*)ReleasePerformanceConfigurationINTEL},
    {"vkQueueSetPerformanceConfigurationINTEL", (void*)QueueSetPerformanceConfigurationINTEL},
    {"vkGetPerformanceParameterINTEL", (void*)GetPerformanceParameterINTEL},
    {"vkSetLocalDimmingAMD", (void*)SetLocalDimmingAMD},
#ifdef VK_USE_PLATFORM_FUCHSIA
    {"vkCreateImagePipeSurfaceFUCHSIA", (void*)CreateImagePipeSurfaceFUCHSIA},
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
    {"vkCreateMetalSurfaceEXT", (void*)CreateMetalSurfaceEXT},
#endif
    {"vkGetBufferDeviceAddressEXT", (void*)GetBufferDeviceAddressEXT},
    {"vkGetPhysicalDeviceToolPropertiesEXT", (void*)GetPhysicalDeviceToolPropertiesEXT},
    {"vkGetPhysicalDeviceCooperativeMatrixPropertiesNV", (void*)GetPhysicalDeviceCooperativeMatrixPropertiesNV},
    {"vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV", (void*)GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetPhysicalDeviceSurfacePresentModes2EXT", (void*)GetPhysicalDeviceSurfacePresentModes2EXT},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkAcquireFullScreenExclusiveModeEXT", (void*)AcquireFullScreenExclusiveModeEXT},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkReleaseFullScreenExclusiveModeEXT", (void*)ReleaseFullScreenExclusiveModeEXT},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetDeviceGroupSurfacePresentModes2EXT", (void*)GetDeviceGroupSurfacePresentModes2EXT},
#endif
    {"vkCreateHeadlessSurfaceEXT", (void*)CreateHeadlessSurfaceEXT},
    {"vkCmdSetLineStippleEXT", (void*)CmdSetLineStippleEXT},
    {"vkResetQueryPoolEXT", (void*)ResetQueryPoolEXT},
    {"vkCmdSetCullModeEXT", (void*)CmdSetCullModeEXT},
    {"vkCmdSetFrontFaceEXT", (void*)CmdSetFrontFaceEXT},
    {"vkCmdSetPrimitiveTopologyEXT", (void*)CmdSetPrimitiveTopologyEXT},
    {"vkCmdSetViewportWithCountEXT", (void*)CmdSetViewportWithCountEXT},
    {"vkCmdSetScissorWithCountEXT", (void*)CmdSetScissorWithCountEXT},
    {"vkCmdBindVertexBuffers2EXT", (void*)CmdBindVertexBuffers2EXT},
    {"vkCmdSetDepthTestEnableEXT", (void*)CmdSetDepthTestEnableEXT},
    {"vkCmdSetDepthWriteEnableEXT", (void*)CmdSetDepthWriteEnableEXT},
    {"vkCmdSetDepthCompareOpEXT", (void*)CmdSetDepthCompareOpEXT},
    {"vkCmdSetDepthBoundsTestEnableEXT", (void*)CmdSetDepthBoundsTestEnableEXT},
    {"vkCmdSetStencilTestEnableEXT", (void*)CmdSetStencilTestEnableEXT},
    {"vkCmdSetStencilOpEXT", (void*)CmdSetStencilOpEXT},
    {"vkGetGeneratedCommandsMemoryRequirementsNV", (void*)GetGeneratedCommandsMemoryRequirementsNV},
    {"vkCmdPreprocessGeneratedCommandsNV", (void*)CmdPreprocessGeneratedCommandsNV},
    {"vkCmdExecuteGeneratedCommandsNV", (void*)CmdExecuteGeneratedCommandsNV},
    {"vkCmdBindPipelineShaderGroupNV", (void*)CmdBindPipelineShaderGroupNV},
    {"vkCreateIndirectCommandsLayoutNV", (void*)CreateIndirectCommandsLayoutNV},
    {"vkDestroyIndirectCommandsLayoutNV", (void*)DestroyIndirectCommandsLayoutNV},
    {"vkCreatePrivateDataSlotEXT", (void*)CreatePrivateDataSlotEXT},
    {"vkDestroyPrivateDataSlotEXT", (void*)DestroyPrivateDataSlotEXT},
    {"vkSetPrivateDataEXT", (void*)SetPrivateDataEXT},
    {"vkGetPrivateDataEXT", (void*)GetPrivateDataEXT},
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    {"vkCreateDirectFBSurfaceEXT", (void*)CreateDirectFBSurfaceEXT},
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    {"vkGetPhysicalDeviceDirectFBPresentationSupportEXT", (void*)GetPhysicalDeviceDirectFBPresentationSupportEXT},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCreateAccelerationStructureKHR", (void*)CreateAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkGetAccelerationStructureMemoryRequirementsKHR", (void*)GetAccelerationStructureMemoryRequirementsKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdBuildAccelerationStructureKHR", (void*)CmdBuildAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdBuildAccelerationStructureIndirectKHR", (void*)CmdBuildAccelerationStructureIndirectKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkBuildAccelerationStructureKHR", (void*)BuildAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCopyAccelerationStructureKHR", (void*)CopyAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCopyAccelerationStructureToMemoryKHR", (void*)CopyAccelerationStructureToMemoryKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCopyMemoryToAccelerationStructureKHR", (void*)CopyMemoryToAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkWriteAccelerationStructuresPropertiesKHR", (void*)WriteAccelerationStructuresPropertiesKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdCopyAccelerationStructureKHR", (void*)CmdCopyAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdCopyAccelerationStructureToMemoryKHR", (void*)CmdCopyAccelerationStructureToMemoryKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdCopyMemoryToAccelerationStructureKHR", (void*)CmdCopyMemoryToAccelerationStructureKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdTraceRaysKHR", (void*)CmdTraceRaysKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCreateRayTracingPipelinesKHR", (void*)CreateRayTracingPipelinesKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkGetAccelerationStructureDeviceAddressKHR", (void*)GetAccelerationStructureDeviceAddressKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkGetRayTracingCaptureReplayShaderGroupHandlesKHR", (void*)GetRayTracingCaptureReplayShaderGroupHandlesKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkCmdTraceRaysIndirectKHR", (void*)CmdTraceRaysIndirectKHR},
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
    {"vkGetDeviceAccelerationStructureCompatibilityKHR", (void*)GetDeviceAccelerationStructureCompatibilityKHR},
#endif
};


} // namespace vulkan_layer_factory

// loader-layer interface v0, just wrappers since there is only a layer

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                                      VkExtensionProperties *pProperties) {
    return vulkan_layer_factory::EnumerateInstanceExtensionProperties(pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t *pCount,
                                                                                  VkLayerProperties *pProperties) {
    return vulkan_layer_factory::EnumerateInstanceLayerProperties(pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount,
                                                                                VkLayerProperties *pProperties) {
    // the layer command handles VK_NULL_HANDLE just fine internally
    assert(physicalDevice == VK_NULL_HANDLE);
    return vulkan_layer_factory::EnumerateDeviceLayerProperties(VK_NULL_HANDLE, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                    const char *pLayerName, uint32_t *pCount,
                                                                                    VkExtensionProperties *pProperties) {
    // the layer command handles VK_NULL_HANDLE just fine internally
    assert(physicalDevice == VK_NULL_HANDLE);
    return vulkan_layer_factory::EnumerateDeviceExtensionProperties(VK_NULL_HANDLE, pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice dev, const char *funcName) {
    return vulkan_layer_factory::GetDeviceProcAddr(dev, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *funcName) {
    return vulkan_layer_factory::GetInstanceProcAddr(instance, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_layerGetPhysicalDeviceProcAddr(VkInstance instance,
                                                                                           const char *funcName) {
    return vulkan_layer_factory::GetPhysicalDeviceProcAddr(instance, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface *pVersionStruct) {
    assert(pVersionStruct != NULL);
    assert(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

    // Fill in the function pointers if our version is at least capable of having the structure contain them.
    if (pVersionStruct->loaderLayerInterfaceVersion >= 2) {
        pVersionStruct->pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
        pVersionStruct->pfnGetDeviceProcAddr = vkGetDeviceProcAddr;
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = vk_layerGetPhysicalDeviceProcAddr;
    }

    return VK_SUCCESS;
}
