// This file is generated by tools/codegen.sh
// DO NOT EDIT IT.

// clang-format off

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     BuilderProjectSource.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include "linglong/api/types/v1/helper.hpp"

namespace linglong {
namespace api {
namespace types {
namespace v1 {
/**
* items of sources of builder project
*/

using nlohmann::json;

/**
* items of sources of builder project
*/
struct BuilderProjectSource {
/**
* commit of source
*/
std::optional<std::string> commit;
/**
* digest of source
*/
std::optional<std::string> digest;
/**
* kind of source
*/
std::string kind;
/**
* name of source
*/
std::optional<std::string> name;
/**
* url of source
*/
std::optional<std::string> url;
/**
* version of source
*/
std::optional<std::string> version;
};
}
}
}
}

// clang-format on
