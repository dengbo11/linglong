/*
 * SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "util.h"

#include <sys/capability.h>
#include <sys/mount.h>

// Compatible with linux kernel which is under 5.10
// ll-box will open the symlink with O_PATH | O_NOFOLLOW and replace source by '/proc/self/fd/${fd}'
// impl: method MountNode in file host_mount.cpp
#ifndef MS_NOSYMFOLLOW
#  define LINGLONG_MS_NOSYMFOLLOW 256
#else
#  define LINGLONG_MS_NOSYMFOLLOW MS_NOSYMFOLLOW
#endif

namespace linglong {

#define LLJS_FROM(KEY) (LLJS_FROM_VARNAME(KEY, KEY))
#define LLJS_FROM_VARNAME(KEY, VAR) (o.VAR = j.at(#KEY).get<decltype(o.VAR)>())
#define LLJS_FROM_OPT(KEY) (LLJS_FROM_OPT_VARNAME(KEY, KEY))
#define LLJS_FROM_OPT_VARNAME(KEY, VAR) (o.VAR = optional<decltype(o.VAR)::value_type>(j, #KEY))

#define LLJS_TO(KEY) (LLJS_TO_VARNAME(KEY, KEY))
#define LLJS_TO_VARNAME(KEY, VAR) (j[#KEY] = o.VAR)

#define LLJS_FROM_OBJ(TYPE) inline void from_json(const nlohmann::json &j, TYPE &o)
#define LLJS_TO_OBJ(TYPE) inline void to_json(nlohmann::json &j, const TYPE &o)

#undef linux

struct Root
{
    std::string path;
    std::optional<bool> readonly;
};

LLJS_FROM_OBJ(Root)
{
    LLJS_FROM(path);
    LLJS_FROM_OPT(readonly);
}

LLJS_TO_OBJ(Root)
{
    LLJS_TO(path);
    LLJS_TO(readonly);
}

const static std::map<std::string_view, int> capsMap{
    { "CAP_CHOWN", CAP_CHOWN },
    { "CAP_DAC_OVERRIDE", CAP_DAC_OVERRIDE },
    { "CAP_DAC_READ_SEARCH", CAP_DAC_READ_SEARCH },
    { "CAP_FOWNER", CAP_FOWNER },
    { "CAP_FSETID", CAP_FSETID },
    { "CAP_KILL", CAP_KILL },
    { "CAP_SETGID", CAP_SETGID },
    { "CAP_SETUID", CAP_SETUID },
    { "CAP_SETPCAP", CAP_SETPCAP },
    { "CAP_LINUX_IMMUTABLE", CAP_LINUX_IMMUTABLE },
    { "CAP_NET_BIND_SERVICE", CAP_NET_BIND_SERVICE },
    { "CAP_NET_BROADCAST", CAP_NET_BROADCAST },
    { "CAP_NET_ADMIN", CAP_NET_ADMIN },
    { "CAP_NET_RAW", CAP_NET_RAW },
    { "CAP_IPC_LOCK", CAP_IPC_LOCK },
    { "CAP_IPC_OWNER", CAP_IPC_OWNER },
    { "CAP_SYS_MODULE", CAP_SYS_MODULE },
    { "CAP_SYS_RAWIO", CAP_SYS_RAWIO },
    { "CAP_SYS_CHROOT", CAP_SYS_CHROOT },
    { "CAP_SYS_PTRACE", CAP_SYS_PTRACE },
    { "CAP_SYS_PACCT", CAP_SYS_PACCT },
    { "CAP_SYS_ADMIN", CAP_SYS_ADMIN },
    { "CAP_SYS_BOOT", CAP_SYS_BOOT },
    { "CAP_SYS_NICE", CAP_SYS_NICE },
    { "CAP_SYS_RESOURCE", CAP_SYS_RESOURCE },
    { "CAP_SYS_TIME", CAP_SYS_TIME },
    { "CAP_SYS_TTY_CONFIG", CAP_SYS_TTY_CONFIG },
    { "CAP_MKNOD", CAP_MKNOD },
    { "CAP_LEASE", CAP_LEASE },
    { "CAP_AUDIT_WRITE", CAP_AUDIT_WRITE },
    { "CAP_AUDIT_CONTROL", CAP_AUDIT_CONTROL },
    { "CAP_SETFCAP", CAP_SETFCAP },
    { "CAP_MAC_OVERRIDE", CAP_MAC_OVERRIDE },
    { "CAP_MAC_ADMIN", CAP_MAC_ADMIN },
    { "CAP_SYSLOG", CAP_SYSLOG },
    { "CAP_WAKE_ALARM", CAP_WAKE_ALARM },
    { "CAP_BLOCK_SUSPEND", CAP_BLOCK_SUSPEND },
    { "CAP_AUDIT_READ", CAP_AUDIT_READ }
};

struct Capabilities
{
    util::str_vec bounding;
    util::str_vec permitted;
    util::str_vec inheritable;
    util::str_vec effective;
    util::str_vec ambient;
};

inline void from_json(const nlohmann::json &j, Capabilities &o)
{
    o.bounding = j.at("bounding").get<util::str_vec>();
    o.permitted = j.at("permitted").get<util::str_vec>();
    o.inheritable = j.at("inheritable").get<util::str_vec>();
    o.effective = j.at("effective").get<util::str_vec>();
    o.ambient = j.at("ambient").get<util::str_vec>();
}

inline void to_json(nlohmann::json &j, const Capabilities &o)
{
    j["bounding"] = o.bounding;
    j["permitted"] = o.permitted;
    j["inheritable"] = o.inheritable;
    j["effective"] = o.effective;
    j["ambient"] = o.ambient;
}

struct Process
{
    util::str_vec args;
    util::str_vec env;
    std::string cwd;
    std::optional<Capabilities> capabilities;
};

inline void from_json(const nlohmann::json &j, Process &o)
{
    o.args = j.at("args").get<util::str_vec>();
    o.env = j.at("env").get<util::str_vec>();
    o.cwd = j.at("cwd").get<std::string>();
    if (j.find("capabilities") != j.end()) {
        o.capabilities = j.at("capabilities").get<Capabilities>();
    }
}

inline void to_json(nlohmann::json &j, const Process &o)
{
    j["args"] = o.args;
    j["env"] = o.env;
    j["cwd"] = o.cwd;
    if (o.capabilities) {
        j["capabilities"] = o.capabilities;
    }
}

struct Mount
{
    enum Type {
        Unknown,
        Bind,
        Proc,
        Sysfs,
        Devpts,
        Mqueue,
        Tmpfs,
        Cgroup,
        Cgroup2,
    };

    std::string destination;
    std::string type;
    std::string source;
    util::str_vec data;

    Type fsType;
    uint32_t flags{ 0 };
    uint32_t propagationFlags{ 0 };
    uint32_t extensionFlags{ 0 };
};

enum Extension { COPY_SYMLINK = 1 };

inline void from_json(const nlohmann::json &j, Mount &o)
{
    const static std::map<std::string_view, Mount::Type> fsTypes = {
        { "bind", Mount::Bind },     { "proc", Mount::Proc },       { "devpts", Mount::Devpts },
        { "mqueue", Mount::Mqueue }, { "tmpfs", Mount::Tmpfs },     { "sysfs", Mount::Sysfs },
        { "cgroup", Mount::Cgroup }, { "cgroup2", Mount::Cgroup2 },
    };

    struct mountFlag
    {
        bool clear{ false };
        uint32_t flag{ 0 };
    };

    const static std::map<std::string_view, mountFlag> optionFlags = {
        { "acl", { false, MS_POSIXACL } },
        { "async", { true, MS_SYNCHRONOUS } },
        { "atime", { true, MS_NOATIME } },
        { "bind", { false, MS_BIND } },
        { "defaults", { false, 0 } },
        { "dev", { true, MS_NODEV } },
        { "diratime", { true, MS_NODIRATIME } },
        { "dirsync", { false, MS_DIRSYNC } },
        { "exec", { true, MS_NOEXEC } },
        { "iversion", { false, MS_I_VERSION } },
        { "lazytime", { false, MS_LAZYTIME } },
        { "loud", { true, MS_SILENT } },
        { "mand", { false, MS_MANDLOCK } },
        { "noacl", { true, MS_POSIXACL } },
        { "noatime", { false, MS_NOATIME } },
        { "nodev", { false, MS_NODEV } },
        { "nodiratime", { false, MS_NODIRATIME } },
        { "noexec", { false, MS_NOEXEC } },
        { "noiversion", { true, MS_I_VERSION } },
        { "nolazytime", { true, MS_LAZYTIME } },
        { "nomand", { true, MS_MANDLOCK } },
        { "norelatime", { true, MS_RELATIME } },
        { "nostrictatime", { true, MS_STRICTATIME } },
        { "nosuid", { false, MS_NOSUID } },
        { "nosymfollow", { false, LINGLONG_MS_NOSYMFOLLOW } }, // since kernel 5.10
        { "rbind", { false, MS_BIND | MS_REC } },
        { "relatime", { false, MS_RELATIME } },
        { "remount", { false, MS_REMOUNT } },
        { "ro", { false, MS_RDONLY } },
        { "rw", { true, MS_RDONLY } },
        { "silent", { false, MS_SILENT } },
        { "strictatime", { false, MS_STRICTATIME } },
        { "suid", { true, MS_NOSUID } },
        { "sync", { false, MS_SYNCHRONOUS } },
        // {"symfollow",{true, MS_NOSYMFOLLOW}}, // since kernel 5.10
    };

    const static std::map<std::string_view, uint32_t> propagationFlags{
        { "rprivate", MS_PRIVATE | MS_REC },       { "private", MS_PRIVATE },
        { "rslave", MS_SLAVE | MS_REC },           { "slave", MS_SLAVE },
        { "rshared", MS_SHARED | MS_REC },         { "shared", MS_SHARED },
        { "runbindable", MS_UNBINDABLE | MS_REC }, { "unbindable", MS_UNBINDABLE }
    };

    const static std::map<std::string_view, mountFlag> extensionFlags{
        { "copy-symlink", { false, Extension::COPY_SYMLINK } }
    };

    o.destination = j.at("destination").get<std::string>();
    o.type = j.at("type").get<std::string>();
    o.fsType = fsTypes.find(o.type)->second;
    if (o.fsType == Mount::Bind) {
        o.flags = MS_BIND;
    }
    o.source = j.at("source").get<std::string>();
    o.data = {};

    // Parse options to data and flags.
    // FIXME: support "recursive mount attrs" in the future
    // https://github.com/opencontainers/runc/blob/c83abc503de7e8b3017276e92e7510064eee02a8/libcontainer/specconv/spec_linux.go#L958
    auto options = j.value("options", util::str_vec());
    for (auto const &opt : options) {
        if (auto it = optionFlags.find(opt); it != optionFlags.cend() && it->second.flag != 0) {
            if (it->second.clear) {
                o.flags &= ~it->second.flag;
            } else {
                o.flags |= it->second.flag;
            }
        } else if (auto it = propagationFlags.find(opt); it != propagationFlags.cend()) {
            o.propagationFlags |= it->second;
        } else if (auto it = extensionFlags.find(opt); it != extensionFlags.cend()) {
            if (it->second.clear) {
                o.extensionFlags &= ~it->second.flag;
            } else {
                o.extensionFlags |= it->second.flag;
            }
        } else {
            o.data.push_back(opt);
        }
    }
}

inline void to_json(nlohmann::json &j, const Mount &o)
{
    j["destination"] = o.destination;
    j["source"] = o.source;
    j["type"] = o.type;
    j["options"] = o.data; // FIXME: this data is not original options, some of them have been
                           // prased to flags.
}

struct Namespace
{
    int type;
};

static std::map<std::string, int> namespaceType = {
    { "pid", CLONE_NEWPID },       { "uts", CLONE_NEWUTS },     { "mount", CLONE_NEWNS },
    { "cgroup", CLONE_NEWCGROUP }, { "network", CLONE_NEWNET }, { "ipc", CLONE_NEWIPC },
    { "user", CLONE_NEWUSER },
};

inline void from_json(const nlohmann::json &j, Namespace &o)
{
    o.type = namespaceType.find(j.at("type").get<std::string>())->second;
}

inline void to_json(nlohmann::json &j, const Namespace &o)
{
    auto matchPair = std::find_if(std::begin(namespaceType),
                                  std::end(namespaceType),
                                  [&](const std::pair<std::string, int> &pair) {
                                      return pair.second == o.type;
                                  });
    j["type"] = matchPair->first;
}

struct IDMap
{
    uint64_t containerID = 0u;
    uint64_t hostID = 0u;
    uint64_t size = 0u;
};

inline void from_json(const nlohmann::json &j, IDMap &o)
{
    o.hostID = j.value("hostID", 0);
    o.containerID = j.value("containerID", 0);
    o.size = j.value("size", 0);
}

inline void to_json(nlohmann::json &j, const IDMap &o)
{
    j["hostID"] = o.hostID;
    j["containerID"] = o.containerID;
    j["size"] = o.size;
}

typedef std::string SeccompAction;
typedef std::string SeccompArch;

struct SyscallArg
{
    u_int index;        // require
    u_int64_t value;    // require
    u_int64_t valueTwo; // optional
    std::string op;     // require
};

inline void from_json(const nlohmann::json &j, SyscallArg &o)
{
    o.index = j.at("index").get<u_int>();
    o.value = j.at("value").get<u_int64_t>();
    o.valueTwo = j.value("valueTwo", u_int64_t());
    o.op = j.at("op").get<std::string>();
}

inline void to_json(nlohmann::json &j, const SyscallArg &o)
{
    j["index"] = o.index;
    j["value"] = o.value;
    j["valueTwo"] = o.valueTwo;
    j["op"] = o.op;
}

struct Syscall
{
    util::str_vec names;
    SeccompAction action;
    std::vector<SyscallArg> args;
};

inline void from_json(const nlohmann::json &j, Syscall &o)
{
    o.names = j.at("names").get<util::str_vec>();
    o.action = j.at("action").get<SeccompAction>();
    o.args = j.value("args", std::vector<SyscallArg>());
}

inline void to_json(nlohmann::json &j, const Syscall &o)
{
    j["names"] = o.names;
    j["action"] = o.action;
    j["args"] = o.args;
}

struct Seccomp
{
    SeccompAction defaultAction = "INVALID_ACTION";
    std::vector<SeccompArch> architectures;
    std::vector<Syscall> syscalls;
};

inline void from_json(const nlohmann::json &j, Seccomp &o)
{
    o.defaultAction = j.at("defaultAction").get<std::string>();
    o.architectures = j.value("architectures", std::vector<SeccompArch>{});
    o.syscalls = j.value("syscalls", std::vector<Syscall>{});
}

inline void to_json(nlohmann::json &j, const Seccomp &o)
{
    j["defaultAction"] = o.defaultAction;
    j["architectures"] = o.architectures;
    j["syscalls"] = o.syscalls;
}

// https://github.com/containers/crun/blob/main/crun.1.md#memory-controller
struct ResourceMemory
{
    int64_t limit = -1;
    int64_t reservation = -1;
    int64_t swap = -1;
};

inline void from_json(const nlohmann::json &j, ResourceMemory &o)
{
    o.limit = j.value("limit", -1);
    o.reservation = j.value("reservation", -1);
    o.swap = j.value("swap", -1);
}

inline void to_json(nlohmann::json &j, const ResourceMemory &o)
{
    j["limit"] = o.limit;
    j["reservation"] = o.reservation;
    j["swap"] = o.swap;
}

// https://github.com/containers/crun/blob/main/crun.1.md#cpu-controller
// support v1 and v2 with conversion
struct ResourceCPU
{
    u_int64_t shares = 1024;
    int64_t quota = 100000;
    u_int64_t period = 100000;
    //    int64_t realtimeRuntime;
    //    int64_t realtimePeriod;
    //    std::string cpus;
    //    std::string mems;
};

inline void from_json(const nlohmann::json &j, ResourceCPU &o)
{
    o.shares = j.value("shares", 1024);
    o.quota = j.value("quota", 100000);
    o.period = j.value("period", 100000);
}

inline void to_json(nlohmann::json &j, const ResourceCPU &o)
{
    j["shares"] = o.shares;
    j["quota"] = o.quota;
    j["period"] = o.period;
}

struct Resources
{
    ResourceMemory memory;
    ResourceCPU cpu;
};

inline void from_json(const nlohmann::json &j, Resources &o)
{
    o.cpu = j.value("cpu", ResourceCPU());
    o.memory = j.value("memory", ResourceMemory());
}

inline void to_json(nlohmann::json &j, const Resources &o)
{
    j["cpu"] = o.cpu;
    j["memory"] = o.memory;
}

struct Linux
{
    std::vector<Namespace> namespaces;
    std::vector<IDMap> uidMappings;
    std::vector<IDMap> gidMappings;
    std::optional<Seccomp> seccomp;
    std::string cgroupsPath;
    Resources resources;
};

inline void from_json(const nlohmann::json &j, Linux &o)
{
    o.namespaces = j.at("namespaces").get<std::vector<Namespace>>();
    o.uidMappings = j.value("uidMappings", std::vector<IDMap>{});
    o.gidMappings = j.value("gidMappings", std::vector<IDMap>{});
    o.seccomp = optional<decltype(o.seccomp)::value_type>(j, "seccomp");
    o.cgroupsPath = j.value("cgroupsPath", "");
    o.resources = j.value("resources", Resources());
}

inline void to_json(nlohmann::json &j, const Linux &o)
{
    j["namespaces"] = o.namespaces;
    j["uidMappings"] = o.uidMappings;
    j["gidMappings"] = o.gidMappings;
    j["seccomp"] = o.seccomp;
    j["cgroupsPath"] = o.cgroupsPath;
    j["resources"] = o.resources;
}

/*
    "hooks": {
        "prestart": [
            {
                "path": "/usr/bin/fix-mounts",
                "args": ["fix-mounts", "arg1", "arg2"],
                "env":  [ "key1=value1"]
            },
            {
                "path": "/usr/bin/setup-network"
            }
        ],
        "poststart": [
            {
                "path": "/usr/bin/notify-start",
                "timeout": 5
            }
        ],
        "poststop": [
            {
                "path": "/usr/sbin/cleanup.sh",
                "args": ["cleanup.sh", "-f"]
            }
        ]
    }
 */

struct Hook
{
    std::string path;
    std::optional<util::str_vec> args;
    std::optional<std::vector<std::string>> env;
};

inline void from_json(const nlohmann::json &j, Hook &o)
{
    LLJS_FROM(path);
    LLJS_FROM_OPT(args);
    LLJS_FROM_OPT(env);
}

inline void to_json(nlohmann::json &j, const Hook &o)
{
    j["path"] = o.path;
    j["args"] = o.args;
    j["env"] = o.env;
}

struct Hooks
{
    std::optional<std::vector<Hook>> prestart;
    std::optional<std::vector<Hook>> poststart;
    std::optional<std::vector<Hook>> poststop;
    std::optional<std::vector<Hook>> startContainer;
};

inline void from_json(const nlohmann::json &j, Hooks &o)
{
    LLJS_FROM_OPT(prestart);
    LLJS_FROM_OPT(poststart);
    LLJS_FROM_OPT(poststop);
    LLJS_FROM_OPT(startContainer);
}

inline void to_json(nlohmann::json &j, const Hooks &o)
{
    j["poststop"] = o.poststop;
    j["poststart"] = o.poststart;
    j["prestart"] = o.prestart;
    j["startContainer"] = o.startContainer;
}

struct Runtime
{
    std::string version;
    Root root;
    Process process;
    std::string hostname;
    Linux linux;
    std::optional<std::vector<Mount>> mounts;
    std::optional<Hooks> hooks;
};

inline void from_json(const nlohmann::json &j, Runtime &o)
{
    o.version = j.at("ociVersion").get<std::string>();
    o.hostname = j.at("hostname").get<std::string>();
    LLJS_FROM(process);
    o.mounts = optional<decltype(o.mounts)::value_type>(j, "mounts");
    LLJS_FROM(linux);
    // maybe optional
    LLJS_FROM(root);
    o.hooks = optional<decltype(o.hooks)::value_type>(j, "hooks");
}

inline void to_json(nlohmann::json &j, const Runtime &o)
{
    j["ociVersion"] = o.version;
    j["hostname"] = o.hostname;
    j["process"] = o.process;
    j["mounts"] = o.mounts;
    j["linux"] = o.linux;
    j["root"] = o.root;
    j["hooks"] = o.hooks;
}

inline static Runtime fromFile(const std::string &filepath)
{
    return util::json::fromFile(filepath).get<Runtime>();
}

inline static Runtime fromString(const std::string &content)
{
    return util::json::fromByteArray(content).get<Runtime>();
}

} // namespace linglong
