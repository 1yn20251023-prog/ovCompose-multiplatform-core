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

#ifndef ANDROIDX_COMPOSE_UI_ARKUI_UTILS_EXPORT_H
#define ANDROIDX_COMPOSE_UI_ARKUI_UTILS_EXPORT_H

#include "napi/native_api.h"
#include "xcomponent_common.h"

EXTERN_C_START
void androidx_compose_ui_arkui_utils_init(napi_env env, napi_value exports);
napi_value androidx_compose_ui_arkui_utils_wrapped(napi_env env, void *nativeController);

Boolean androidx_compose_ui_arkui_utils_xcomponent_prepareDraw(void *render);
Boolean androidx_compose_ui_arkui_utils_xcomponent_finishDraw(void *render);
void androidx_compose_ui_arkui_utils_xcomponent_registerFrameCallback(void *render);
void androidx_compose_ui_arkui_utils_xcomponent_unregisterFrameCallback(void *render);

// ============================================
// CanIUse C API 接口（直接调用 HarmonyOS C API）
// ============================================

/**
 * 检测系统能力（SysCap）是否可用
 * 直接调用 HarmonyOS C API，绕过 ArkTS 层
 * 
 * @param capability 系统能力名称，如 "SystemCapability.ArkUI.ArkUI.Full"
 * @return true 表示可用，false 表示不可用或调用失败
 */
Boolean androidx_compose_ui_arkui_caniuseext_checkCapability(const char* capability);

/**
 * 检测 API 是否可用
 * 
 * @param apiName API 名称，如 "window.Window.getLastWindow"
 * @return true 表示可用，false 表示不可用或调用失败
 */
Boolean androidx_compose_ui_arkui_caniuseext_checkApi(const char* apiName);

/**
 * 检查 C API 是否可用
 * 用于上层判断是否需要使用回退方案
 * 
 * @return true 表示 C API 可用，false 表示不可用
 */
Boolean androidx_compose_ui_arkui_caniuseext_isAvailable();

EXTERN_C_END

#endif