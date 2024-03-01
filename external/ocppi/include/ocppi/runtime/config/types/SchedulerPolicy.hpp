// Thish file is generated by /tools/codegen
// DO NOT EDIT IT.

// clang-format off

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     SchedulerPolicy.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include "ocppi/runtime/config/types/helper.hpp"

namespace ocppi {
namespace runtime {
namespace config {
namespace types {
using nlohmann::json;

enum class SchedulerPolicy : int { SchedBatch, SchedDeadline, SchedFIFO, SchedISO, SchedIdle, SchedOther, SchedRr };
}
}
}
}

// clang-format on
