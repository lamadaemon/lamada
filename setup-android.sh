#!/usr/bin/bash

if [ -z "${ANDROID_HOME}" ]; then
    if [ -z "$1" ]; then
        echo "Error: unable to find ANDROID_HOME. Set environment variable or pass as the first parameter."
        exit 1
    else
        export ANDROID_HOME="$1"
    fi
fi

if [ -z "${TARGET_SDK}" ]; then
    if [ -z "$1" ]; then
        echo "TARGET_SDK not specified! Using default: 33 (Android 13, usage >50%)"
        export TARGET_SDK=33
    fi
fi

export CC="${ANDROID_HOME}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android$TARGET_SDK-clang"

$CC -v
echo ""
echo "CC: $CC"
echo "CC Configured to target $TARGET_SDK"
