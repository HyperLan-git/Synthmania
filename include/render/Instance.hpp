#pragma once

class Instance;

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Window.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

class Instance {
   public:
    Instance(std::string name, uint32_t version, const char* engineName,
             uint32_t engineVersion, uint32_t apiVersion,
             const std::vector<const char*> extensions,
             const std::vector<const char*> validationLayers =
                 std::vector<const char*>(),
             std::string logFile = "a.log");

    VkSurfaceKHR* createSurface(Window* window);
    void destroySurface(VkSurfaceKHR* surface);

    VkInstance* getInstance();

    ~Instance();

    bool checkValidationLayerSupport(
        const std::vector<const char*> validationLayers);

   private:
    VkInstance* instance;
    VkDebugUtilsMessengerEXT* debugMessenger;
    std::ofstream log;
};