#include "Instance.hpp"

Instance::Instance(const char* name, uint32_t version, const char* engineName,
                   uint32_t engineVersion, uint32_t apiVersion,
                   const std::vector<const char*> extensions,
                   const std::vector<const char*> validationLayers,
                   std::string logFile) {
    bool debug = !validationLayers.empty();
    instance = new VkInstance();

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = name;
    appInfo.applicationVersion = version;
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = apiVersion;
    appInfo.pNext = NULL;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.flags = 0;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (!debug) {
        debugMessenger = NULL;

        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    } else {
        debugMessenger = new VkDebugUtilsMessengerEXT();
        log = std::ofstream(logFile, std::ios::out);

        if (!checkValidationLayerSupport(validationLayers))
            throw std::runtime_error(
                "validation layers requested, but not available!");

        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.pNext = NULL;
    }

    if (vkCreateInstance(&createInfo, NULL, instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");

    if (debug) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.flags = 0;
        createInfo.pUserData = &log;
        createInfo.pNext = NULL;

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            *instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            if (func(*instance, &createInfo, nullptr, debugMessenger))
                throw std::runtime_error("failed to set up debug messenger!");
        } else {
            throw std::runtime_error(
                "failed to set up debug messenger because extension is not "
                "present!");
        }
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
    std::ofstream* logFile = static_cast<std::ofstream*>(pUserData);
    std::string severity;
    switch (messageSeverity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severity = "INFO";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severity = "ERROR";
            std::cerr << pCallbackData->pMessage << std::endl;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severity = "VERBOSE";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severity = "WARNING";
            break;
    }
    (*logFile) << ("[" + severity + "] ") << pCallbackData->pMessage
               << std::endl;
    std::cout << ("[" + severity + "] ") << pCallbackData->pMessage
              << std::endl;

    return VK_FALSE;
}

VkInstance* Instance::getInstance() { return instance; }

Instance::~Instance() {
    if (debugMessenger != NULL) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                *instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != NULL) func(*instance, *debugMessenger, NULL);
        delete debugMessenger;
    }
    vkDestroyInstance(*instance, NULL);
    delete instance;
}

VkSurfaceKHR* Instance::createSurface(Window* window) {
    VkSurfaceKHR* surface = new VkSurfaceKHR();
    if (window->createSurface(this, NULL, surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

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

        for (const auto& layerProperties : availableLayers)
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }

        if (!layerFound) return false;
    }

    return true;
}