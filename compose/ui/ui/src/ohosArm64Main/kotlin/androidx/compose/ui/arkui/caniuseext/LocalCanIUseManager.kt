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

import androidx.compose.runtime.Composable
import androidx.compose.runtime.State
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.staticCompositionLocalOf

/**
 * CompositionLocal for CanIUseManager
 * 提供 CanIUseManager 实例给 Compose 组件使用
 *
 * 使用示例:
 * ```kotlin
 * @Composable
 * fun MyScreen() {
 *     val canIUseManager = LocalCanIUseManager.current
 *     val isAvailable = canIUseManager.checkCapability("SystemCapability.ArkUI.ArkUI.Full")
 * }
 * ```
 *
 * @author ovCompose Team
 * @since 2025
 */
val LocalCanIUseManager = staticCompositionLocalOf<CanIUseManager> {
    // 默认提供一个实例，避免在未初始化时抛出异常
    CanIUseManager.create()
}

/**
 * Composable function that returns the availability of a system capability as a State.
 * The result is reactive and will update if the capability status changes.
 *
 * @param capability The name of the system capability to check
 * @return A State object holding a boolean indicating if the capability is available
 */
@Composable
fun rememberCanIUse(capability: String): State<Boolean> {
    val manager = LocalCanIUseManager.current
    return remember(capability) {
        mutableStateOf(manager.checkCapability(capability))
    }
}

/**
 * Composable function that directly checks if a system capability is available.
 * This is a non-reactive version that returns the current value directly.
 *
 * @param capability The name of the system capability to check
 * @return true if the capability is available, false otherwise
 */
@Composable
fun canIUse(capability: String): Boolean {
    val manager = LocalCanIUseManager.current
    return manager.checkCapability(capability)
}

