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

package androidx.compose.ui.arkui.caniuseext

import androidx.annotation.MainThread
import androidx.compose.ui.arkui.utils.androidx_compose_ui_arkui_caniuseext_checkApi
import androidx.compose.ui.arkui.utils.androidx_compose_ui_arkui_caniuseext_checkCapability
import androidx.compose.ui.arkui.utils.androidx_compose_ui_arkui_caniuseext_isAvailable

/**
 * CanIUse 管理器
 * 直接调用 HarmonyOS C API，提供最优性能
 * 完全绕过 ArkTS 层和 Messenger，直接调用系统 C API
 *
 * @author ovCompose Team
 * @since 2025
 */
class CanIUseManager internal constructor() {
    
    private val capabilityCache = mutableMapOf<String, Boolean>()
    
    /**
     * 检测系统能力（SysCap）是否可用
     * 直接调用 HarmonyOS C API，无中间层开销
     * 
     * @param capability 系统能力名称，如 "SystemCapability.ArkUI.ArkUI.Full"
     * @return true 表示可用，false 表示不可用
     */
    @MainThread
    fun checkCapability(capability: String): Boolean {
        // 1. 检查缓存
        capabilityCache[capability]?.let { return it }
        
        // 2. 直接调用 HarmonyOS C API（通过 cinterop 生成的 Kotlin 绑定）
        val isAvailable = try {
            androidx_compose_ui_arkui_caniuseext_checkCapability(capability)
        } catch (e: Exception) {
            // C API 调用失败，返回 false
            androidx.compose.ui.graphics.kLog("CanIUseManager: checkCapability failed for $capability: ${e.message}")
            false
        }
        
        // 3. 更新缓存
        capabilityCache[capability] = isAvailable
        return isAvailable
    }
    
    /**
     * 检测 API 是否可用
     * 
     * @param apiName API 名称，如 "window.Window.getLastWindow"
     * @return true 表示可用，false 表示不可用
     */
    @MainThread
    fun checkApi(apiName: String): Boolean {
        return try {
            androidx_compose_ui_arkui_caniuseext_checkApi(apiName)
        } catch (e: Exception) {
            androidx.compose.ui.graphics.kLog("CanIUseManager: checkApi failed for $apiName: ${e.message}")
            false
        }
    }
    
    /**
     * 批量检测系统能力
     * 
     * @param capabilities 系统能力名称列表
     * @return 检测结果 Map，key 为能力名称，value 为是否可用
     */
    @MainThread
    fun checkCapabilities(capabilities: List<String>): Map<String, Boolean> {
        return capabilities.associateWith { checkCapability(it) }
    }
    
    /**
     * 检查 C API 是否可用
     * 用于判断是否需要使用回退方案（如 Messenger）
     * 
     * @return true 表示 C API 可用，false 表示不可用
     */
    fun isCApiAvailable(): Boolean {
        return try {
            androidx_compose_ui_arkui_caniuseext_isAvailable()
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * 清理缓存
     */
    fun clearCache() {
        capabilityCache.clear()
    }
    
    companion object {
        /**
         * 创建 CanIUseManager 实例
         */
        fun create(): CanIUseManager = CanIUseManager()
    }
}
