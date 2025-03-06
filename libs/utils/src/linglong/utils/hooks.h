/*
 * SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "linglong/utils/error/error.h"

#include <string>
#include <vector>

namespace linglong::utils {

class InstallHookManager final
{
public:
    InstallHookManager() = default;
    InstallHookManager(const InstallHookManager &) = delete;
    InstallHookManager(const InstallHookManager &&) = delete;
    InstallHookManager &operator=(const InstallHookManager &) = delete;
    ~InstallHookManager() = default;

    utils::error::Result<void> parseInstallHooks();
    utils::error::Result<void> executeInstallHooks(int fd);
    utils::error::Result<void> executePostInstallHooks(const std::string &,
                                                       const std::string &) noexcept;

private:
    std::vector<std::string> preInstallCommands;
    std::vector<std::string> postInstallCommands;
};

} // namespace linglong::utils
