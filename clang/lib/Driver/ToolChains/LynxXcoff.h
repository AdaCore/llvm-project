//===--- LynxXcoff.h - Lynx XCOFF ToolChain Implementations  -*- C++  -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LYNXXCOFF_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LYNXXCOFF_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

/// lynxxcoff -- Directly call GNU Binutils assembler and linker.
namespace lynxxcoff {

class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
  Assembler(const ToolChain &TC)
      : Tool("lynxxcoff::Assembler", "assembler", TC) {}

  bool hasIntegratedCPP() const override { return false; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &TCArgs,
                    const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
  Linker(const ToolChain &TC) : Tool("lynxxcoff::Linker", "linker", TC) {}

  bool hasIntegratedCPP() const override { return false; }
  bool isLinkJob() const override { return true; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &TCArgs,
                    const char *LinkingOutput) const override;
};

} // end namespace lynxxcoff
} // end namespace tools

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY LynxXcoff : public ToolChain {
public:
  LynxXcoff(const Driver &D, const llvm::Triple &Triple,
            const llvm::opt::ArgList &Args);

  bool isPICDefault() const override { return false; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }
  bool isPICDefaultForced() const override { return false; }
  bool HasNativeLLVMSupport() const override { return false; }

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;

  // Set default DWARF version to 4 for now as latest LynxXcoff OS supports
  // version 4.
  unsigned GetDefaultDwarfVersion() const override { return 4; }

  RuntimeLibType GetDefaultRuntimeLibType() const override;

protected:
  Tool *buildAssembler() const override;
  Tool *buildLinker() const override;

private:
  bool ParseInlineAsmUsingAsmParser;
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_LYNXXCOFF_H
