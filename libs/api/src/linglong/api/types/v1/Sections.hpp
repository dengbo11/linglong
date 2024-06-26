// This file is generated by tools/codegen.sh
// DO NOT EDIT IT.

// clang-format off

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Sections.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include "linglong/api/types/v1/helper.hpp"

namespace linglong {
namespace api {
namespace types {
namespace v1 {
using nlohmann::json;

struct Sections {
/**
* Name of the section contains the read only filesystem image. It SHOULD always be
* 'linglong.bundle'.
*/
std::string bundle;
/**
* Name of the section contains the icon of this UAB file. It SHOULD always be
* 'linglong.icon'.
*/
std::optional<std::string> icon;
};
}
}
}
}

// clang-format on
