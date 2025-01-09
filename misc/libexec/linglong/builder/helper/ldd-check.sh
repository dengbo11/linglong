#!/bin/bash

# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

#ldd check binary script

declare -a dependLibs=()
declare -a blackList=()
declare -r ErrColor="\033[0;31m"
declare -r WarningColor="\033[1;33m"
declare -r NoColor="\033[0m"

logErr() {
        echo -e "${ErrColor}Error:$1${NoColor}\n"
        exit 255
}

logWarn() {
        echo -e "${WarningColor}Warning:$1${NoColor}\n"
}

logDbg() {
        echo -e "Debug:$1\n"
}

processExecBin() {
        local filePath="$1"
        if [[ -z ${filePath} || ! -f ${filePath} ]]; then
                logErr "Invalid file path: ${filePath}"
        fi

        logDbg "${filePath} is an executable binary, processing ..."

        declare lddInfos
        if ! lddInfos=$(ldd "${filePath}"); then
                logErr "Failed to execute ldd on ${filePath}"
        fi

        # Trim leading and trailing whitespace
        lddInfos=$(echo "${lddInfos}" | awk '{$1=$1;print}')
        readarray -t lddInfoList <<<"${lddInfos}"

        IFS=" "
        for line in "${lddInfoList[@]}"; do
                read -ra elements <<<"${line}"
                elementSize=${#elements[@]}

                if [[ ${elementSize} -gt 4 || ${elementSize} -lt 2 ]]; then
                        logWarn "Unexpected line format: '${line}' split into ${elementSize} parts"
                        continue
                fi

                if [[ ${elementSize} -eq 2 ]]; then
                        # https://regex101.com/r/sPAsBt/1
                        # https://man7.org/linux/man-pages/man7/vdso.7.html
                        if [[ ${elements[0]:0:1} == "/" ]]; then
                                continue
                        elif [[ ${elements[0]} =~ linux-(vdso|gate){1}(32|64)?\.so\.[0-9]+ ]]; then
                                continue
                                #ignore virtual ELF dynamic shared object
                        else
                                rawStr="${elements[0]} ${elements[1]}"
                                if [[ ${rawStr} == "statically linked" ]]; then
                                        continue
                                fi
                                logWarn "unexpected conditions, unkonwn line: ${line}"
                        fi
                elif [[ ${elementSize} -eq 4 ]]; then
                        rawStr="${elements[2]} ${elements[3]}"
                        if [[ ${rawStr} == "not found" ]]; then
                                logErr "couldn't find dependency \"${elements[0]}\" of ${filePath}"
                        fi

                         # clean path
                        dependLibs+=("${elements[2]}")
                else
                        logWarn "unexpected conditions, unkonwn line: ${line}"
                fi
        done
}

collectDependsLibs() {
        #support multiple path, split by ':'
        declare paths
        paths=$(echo "$1" | tr ':' ' ')

        if [[ -z ${paths} ]]; then
                logErr "No paths provided"
        fi

        filePaths=$(find "${paths}" -type f)

        IFS=$'\n'
        for filePath in ${filePaths}; do
                local has_start
                # We can't find executable binaries just by mimeType. Some elf files are of type “shared object file” or otherwise
                # and it also can be executed. Therefore, try to find the executable binary with the symbol “__libc_start_main”
                has_start="$(nm -D ${filePath} /dev/null 2>&1 | grep "__libc_start_main" || true)"
                if [[ -n ${has_start} ]]; then
                        processExecBin "${filePath}"
                fi
        done

        # remove duplicates
        local oldIFS="${IFS}"
        IFS=" " read -r -a dependLibs <<<"$(echo "${dependLibs[@]}" | tr ' ' '\n' | sort -u | tr '\n' ' ' || true)"
        IFS="${oldIFS}"
}

main() {
        declare arg1="$1"
        if [[ -z ${arg1} ]]; then
                echo "usage:
                        ldd-check.sh path
                        ldd-check.sh [path:...]"
                return 0
        fi

        logDbg "start collecting library dependencies..."
        # 更新ld.so.cache
        if [[ -n "${LINGLONG_LD_SO_CACHE}" ]]; then
                ldconfig -C "${LINGLONG_LD_SO_CACHE}"
                logDbg "updated ld.so.cache to ${LINGLONG_LD_SO_CACHE}"
        fi

        # Collect the needed dynamic libraries for the specified binaries
        collectDependsLibs "${arg1}"

        # remove all library which from /runtime
        local oldIFS="${IFS}"
        IFS=" " read -r -a dependLibs <<<"$(echo "${dependLibs[@]}" | tr ' ' '\n' | grep -v "^/opt\|^/runtime" | tr '\n' ' ' || true)"
        IFS="${oldIFS}"

        # the rest of the library may from /usr/[lib|lib32|lib64]
        yamlContent="# DO NOT EDIT THIS FILE, GENERATED BY ldd-check.sh\n\n"
        yamlContent+="depends:\n"
        yamlContent+=$(printf " - %s\n" "${dependLibs[@]}")
        echo -e "${yamlContent}" > "/project/linglong/depends.yaml"

        logDbg "finish collecting library dependencies..."
        return 0
}

set -eo pipefail
main "$@"
