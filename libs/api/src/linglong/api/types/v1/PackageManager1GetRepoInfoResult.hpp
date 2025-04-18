// This file is generated by tools/codegen.sh
// DO NOT EDIT IT.

// clang-format off

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     PackageManager1GetRepoInfoResult.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include "linglong/api/types/v1/helper.hpp"

#include "linglong/api/types/v1/PackageManager1GetRepoInfoResultRepoInfo.hpp"

namespace linglong {
namespace api {
namespace types {
namespace v1 {
/**
* result of package manager get repo info
*
* this is common error result of ll-cli command --json
*/

using nlohmann::json;

/**
* result of package manager get repo info
*
* this is common error result of ll-cli command --json
*/
struct PackageManager1GetRepoInfoResult {
/**
* result of package manager get repo info
*/
PackageManager1GetRepoInfoResultRepoInfo repoInfo;
/**
* We do not use DBus error. We return an error code instead. Non-zero code indicated errors
* occurs and message should be displayed to user.
*/
int64_t code;
/**
* Human readable result message.
*/
std::string message;
/**
* error type, to indicate client what should be done.
*/
std::string type;
};
}
}
}
}

// clang-format on
