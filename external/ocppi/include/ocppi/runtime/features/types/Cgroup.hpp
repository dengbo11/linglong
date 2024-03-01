// Thish file is generated by /tools/codegen
// DO NOT EDIT IT.

// clang-format off

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Cgroup.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include "ocppi/runtime/features/types/helper.hpp"

namespace ocppi {
namespace runtime {
namespace features {
namespace types {
using nlohmann::json;

struct Cgroup {
std::optional<bool> rdma;
std::optional<bool> systemd;
std::optional<bool> systemdUser;
std::optional<bool> v1;
std::optional<bool> v2;
};
}
}
}
}

// clang-format on
