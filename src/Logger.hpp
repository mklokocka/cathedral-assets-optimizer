/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <plog/Log.h>

#include <filesystem>

namespace cao {
[[nodiscard]] auto init_logging(const std::filesystem::path &log_directory) noexcept -> bool;
} // namespace cao
