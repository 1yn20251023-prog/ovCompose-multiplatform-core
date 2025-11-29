/*
 * Tencent is pleased to support the open source community by making ovCompose available.
 * Copyright (C) 2025 Tencent. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * CanIUse C API 实现
 * 直接调用 HarmonyOS C API，绕过 ArkTS 层和 Messenger
 * 
 * @author ovCompose Team
 * @since 2025
 */

#include "../arkui_utils_export.h"
#include "../xcomponent_log.h"
#include <string>
#include <cstring>
#include <dlfcn.h>

// HarmonyOS C API 函数指针类型
// 根据 HarmonyOS SDK 文档，canIUse 函数签名可能是：
// bool canIUse(const char* syscap);
typedef bool (*CanIUseFunc)(const char*);

// 全局变量：动态库句柄和函数指针
static void* g_libHandle = nullptr;
static CanIUseFunc g_canIUseFunc = nullptr;
static bool g_apiInitialized = false;

/**
 * 初始化 HarmonyOS C API
 * 尝试加载动态库并获取 canIUse 函数指针
 * 
 * @return true 表示初始化成功，false 表示失败
 */
static bool InitCanIUseAPI() {
    if (g_apiInitialized) {
        return g_canIUseFunc != nullptr;
    }
    
    g_apiInitialized = true;
    
    // 尝试加载 HarmonyOS 系统库
    // 可能的库名列表，按优先级尝试
    const char* libNames[] = {
        "libdeviceinfo.so",       // 设备信息库（可能包含 canIUse）
        "libsyscap.so",           // 系统能力库
        "libdeviceinfo_ndk.z.so", // NDK 版本
        nullptr
    };
    
    for (int i = 0; libNames[i] != nullptr; i++) {
        g_libHandle = dlopen(libNames[i], RTLD_LAZY);
        if (g_libHandle != nullptr) {
            // 尝试获取 canIUse 函数指针
            g_canIUseFunc = (CanIUseFunc)dlsym(g_libHandle, "canIUse");
            if (g_canIUseFunc != nullptr) {
                LOGI("InitCanIUseAPI: success, loaded %s", libNames[i]);
                return true;
            }
            
            // 如果 canIUse 不存在，尝试其他可能的函数名
            g_canIUseFunc = (CanIUseFunc)dlsym(g_libHandle, "OH_SYSCAP_CanIUse");
            if (g_canIUseFunc != nullptr) {
                LOGI("InitCanIUseAPI: success with OH_SYSCAP_CanIUse, loaded %s", libNames[i]);
                return true;
            }
            
            // 关闭这个库，尝试下一个
            dlclose(g_libHandle);
            g_libHandle = nullptr;
        }
    }
    
    LOGE("InitCanIUseAPI: failed to load any library with canIUse function");
    return false;
}

/**
 * 清理 C API 资源
 * 关闭动态库句柄
 */
static void CleanupCanIUseAPI() {
    if (g_libHandle != nullptr) {
        dlclose(g_libHandle);
        g_libHandle = nullptr;
        g_canIUseFunc = nullptr;
        g_apiInitialized = false;
        LOGI("CleanupCanIUseAPI: cleaned up");
    }
}

EXTERN_C_START

/**
 * 检测系统能力（SysCap）是否可用
 * 直接调用 HarmonyOS C API，绕过 ArkTS 层
 * 
 * @param capability 系统能力名称，如 "SystemCapability.ArkUI.ArkUI.Full"
 * @return true 表示可用，false 表示不可用或调用失败
 */
Boolean androidx_compose_ui_arkui_caniuseext_checkCapability(const char* capability) {
    if (capability == nullptr) {
        LOGE("checkCapability: capability is null");
        return false;
    }
    
    if (strlen(capability) == 0) {
        LOGE("checkCapability: capability is empty");
        return false;
    }
    
    // 尝试初始化 C API
    if (!InitCanIUseAPI()) {
        LOGE("checkCapability: InitCanIUseAPI failed for %s", capability);
        // C API 不可用时返回 false
        // 上层可以考虑使用 Messenger 回退方案
        return false;
    }
    
    if (g_canIUseFunc != nullptr) {
        bool isAvailable = g_canIUseFunc(capability);
        LOGI("checkCapability: %s = %s", capability, isAvailable ? "true" : "false");
        return isAvailable;
    }
    
    LOGE("checkCapability: C API function not available for %s", capability);
    return false;
}

/**
 * 检测 API 是否可用
 * 根据 HarmonyOS 文档，API 检测也使用 canIUse 函数
 * 
 * @param apiName API 名称，如 "window.Window.getLastWindow"
 * @return true 表示可用，false 表示不可用或调用失败
 */
Boolean androidx_compose_ui_arkui_caniuseext_checkApi(const char* apiName) {
    if (apiName == nullptr) {
        LOGE("checkApi: apiName is null");
        return false;
    }
    
    if (strlen(apiName) == 0) {
        LOGE("checkApi: apiName is empty");
        return false;
    }
    
    // API 检测使用相同的 canIUse 函数
    return androidx_compose_ui_arkui_caniuseext_checkCapability(apiName);
}

/**
 * 检查 C API 是否可用
 * 用于上层判断是否需要使用回退方案
 * 
 * @return true 表示 C API 可用，false 表示不可用
 */
Boolean androidx_compose_ui_arkui_caniuseext_isAvailable() {
    return InitCanIUseAPI();
}

EXTERN_C_END

