#include "Instance.hpp"
Instance::Instance(const char* name, uint32_t version, const char* engineName,
                   uint32_t engineVersion, uint32_t apiVersion,
                   const std::vector<const char*> extensions) {
    instance = new VkInstance();
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = name;
    appInfo.applicationVersion = version;
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = apiVersion;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;

    if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    this->debugMessenger = nullptr;
}

Instance::Instance(const char* name, uint32_t version, const char* engineName,
                   uint32_t engineVersion, uint32_t apiVersion,
                   const std::vector<const char*> extensions,
                   const std::vector<const char*> validationLayers) {
    instance = new VkInstance();
    debugMessenger = new VkDebugUtilsMessengerEXT();
    if (!checkValidationLayerSupport(validationLayers)) {
        throw std::runtime_error(
            "validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = name;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;

    createInfo.pNext = &debugCreateInfo;

    if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo2 = {};
    createInfo2.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo2.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo2.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo2.pfnUserCallback = debugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        *instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        if (func(*instance, &createInfo2, nullptr, debugMessenger))
            throw std::runtime_error("failed to set up debug messenger!");
    } else {
        throw std::runtime_error(
            "failed to set up debug messenger because extension is not "
            "present!");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkInstance* Instance::getInstance() { return instance; }

Instance::~Instance() {
    if (debugMessenger != nullptr) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            *instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(*instance, *debugMessenger, nullptr);
        }
        delete debugMessenger;
    }
    vkDestroyInstance(*instance, nullptr);
    delete instance;
}

VkSurfaceKHR* Instance::createSurface(Window* window) {
    VkSurfaceKHR* surface = new VkSurfaceKHR();
    if (window->createSurface(this, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

void Instance::destroySurface(VkSurfaceKHR* surface) {
    vkDestroySurfaceKHR(*instance, *surface, nullptr);
    delete surface;
}

bool Instance::checkValidationLayerSupport(
    const std::vector<const char*> validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}