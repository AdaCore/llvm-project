// Check that ENV_PREFIX is used to specify the sysroot for cross Linux.
//
// REQUIRES: host=x86_64-{{.*}}, aarch64-registered-target

// Native compilation shouldn't use ENV_PREFIX.
//
// RUN: ENV_PREFIX=/foo/bar %clang -### -target x86_64-linux-gnu %s 2>&1 | \
// RUN:   FileCheck %s -check-prefix=NATIVE
// NATIVE-NOT: /foo/bar

// Compiling from x86_64 to x86 shouldn't be considered cross compilation.
//
// RUN: ENV_PREFIX=/foo/bar %clang -### -target i686-linux-gnu %s 2>&1 | \
// RUN:   FileCheck %s -check-prefix=NATIVE32
// NATIVE32-NOT: /foo/bar

// Cross-compilation to bare metal shouldn't use ENV_PREFIX.
//
// RUN: ENV_PREFIX=/foo/bar %clang -### -target aarch64-none-elf %s 2>&1 | \
// RUN:   FileCheck %s -check-prefix=CROSS_BARE
// CROSS_BARE-NOT: /foo/bar

// Cross-compilation to Linux should use ENV_PREFIX.
//
// RUN: ENV_PREFIX=/foo/bar %clang -### -target aarch64-linux-gnu %s 2>&1 | \
// RUN:   FileCheck %s -check-prefix=CROSS_LINUX
// CROSS_LINUX: "-cc1"
// CROSS_LINUX-SAME: "/foo/bar/include"
// CROSS_LINUX-NEXT: "--sysroot=/foo/bar"

// Without ENV_PREFIX and --sysroot, cross-compilation to Linux shouldn't set a
// sysroot.
//
// RUN: %clang -### -target aarch64-linux-gnu %s 2>&1 | \
// RUN:   FileCheck %s -check-prefix=CROSS_LINUX_NO_ENV_PREFIX
// CROSS_LINUX_NO_ENV_PREFIX-NOT: --sysroot

// When both ENV_PREFIX and --sysroot are specified, ENV_PREFIX should be
// ignored.
//
// RUN: ENV_PREFIX=/foo/bar %clang -### -target aarch64-linux-gnu --sysroot=/baz %s 2>&1 | \
// RUN:   FileCheck %s -check-prefix=CROSS_LINUX_SYSROOT
// CROSS_LINUX_SYSROOT: "-cc1"
// CROSS_LINUX_SYSROOT-NOT: /foo/bar
// CROSS_LINUX_SYSROOT-SAME: "/baz/include"
// CROSS_LINUX_SYSROOT-NOT: /foo/bar
// CROSS_LINUX_SYSROOT-NEXT: "--sysroot=/baz"
