//===--- LynxXcoff.cpp - Lynx XCOFF ToolChain Implementations  -*- C++  -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LynxXcoff.h"
#include "clang/Driver/CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/SanitizerArgs.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Path.h"

using LynxXcoff = clang::driver::toolchains::LynxXcoff;
using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;

using namespace llvm::opt;
using namespace llvm::sys;

void lynxxcoff::Assembler::ConstructJob(Compilation &C, const JobAction &JA,
                                        const InputInfo &Output,
                                        const InputInfoList &Inputs,
                                        const ArgList &Args,
                                        const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // Only support 32 bit.
  if (getToolChain().getTriple().isArch64Bit())
    llvm_unreachable("Unsupported bit width value.");

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA, options::OPT_Xassembler);

  CmdArgs.push_back("-a32");

  // Accept any mixture of instructions.
  // On Power for AIX and Linux, this behaviour matches that of GCC for both the
  // user-provided assembler source case and the compiler-produced assembler
  // source case. Yet XL with user-provided assembler source would not add this.
  CmdArgs.push_back("-many");

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (const auto &II : Inputs)
    CmdArgs.push_back(II.getFilename());

  const char *Exec = Args.MakeArgString(
      getToolChain().GetProgramPath("powerpc-xcoff-lynxos178-as"));
  C.addCommand(std::make_unique<Command>(JA, *this, ResponseFileSupport::None(),
                                         Exec, CmdArgs, Inputs, Output));
}

void lynxxcoff::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                     const InputInfo &Output,
                                     const InputInfoList &Inputs,
                                     const ArgList &Args,
                                     const char *LinkingOutput) const {
  const LynxXcoff &ToolChain = static_cast<const LynxXcoff &>(getToolChain());
  const Driver &D = ToolChain.getDriver();
  ArgStringList CmdArgs;

  // Only support 32 bit.
  if (getToolChain().getTriple().isArch64Bit())
    llvm_unreachable("Unsupported bit width value.");

  // Force static linking when "-static" is present.
  if (Args.hasArg(options::OPT_static))
    CmdArgs.push_back("-bnso");

  // Specify linker output file.
  assert((Output.isFilename() || Output.isNothing()) && "Invalid output.");
  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  }

  // Set linking mode (i.e., 32/64-bit) and the address of
  // text and data sections based on arch bit width.
  CmdArgs.push_back("-b32");
  CmdArgs.push_back("-bpT:0x10000000");
  CmdArgs.push_back("-bpD:0x20000000");

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles)) {
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("init.o")));
  }

  // Specify linker input file(s).
  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs, JA);

  // Add directory to library search path.
  Args.AddAllArgs(CmdArgs, options::OPT_L);
  ToolChain.AddFilePathLibArgs(Args, CmdArgs);
  ToolChain.addProfileRTLibs(Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs)) {
    CmdArgs.push_back("-lc");

    // Libgcc must come after lc.
    // Don't call AddRuntTimeLibs with libgcc as it will try to
    // pull shared libgcc which doesn't exist.
    // TODO: this could be a linker issue. To investigate.
    auto RtLib = ToolChain.GetRuntimeLibType(Args);
    if (RtLib == ToolChain::RLT_Libgcc)
      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("libgcc.a")));
    else
      AddRunTimeLibs(ToolChain, D, CmdArgs, Args);
  }

  const char *Exec = Args.MakeArgString(
      getToolChain().GetProgramPath("powerpc-xcoff-lynxos178-ld"));
  C.addCommand(std::make_unique<Command>(JA, *this, ResponseFileSupport::None(),
                                         Exec, CmdArgs, Inputs, Output));
}

/// LynxXcoff - LynxXcoff tool chain .
LynxXcoff::LynxXcoff(const Driver &D, const llvm::Triple &Triple,
                     const ArgList &Args)
    : ToolChain(D, Triple, Args) {
  getFilePaths().push_back(getDriver().SysRoot + "/lib");
}

void LynxXcoff::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                          ArgStringList &CC1Args) const {
  if (DriverArgs.hasArg(options::OPT_nostdinc))
    return;

  if (!DriverArgs.hasArg(options::OPT_nobuiltininc)) {
    SmallString<128> P(getDriver().ResourceDir);
    llvm::sys::path::append(P, "include");
    addSystemInclude(DriverArgs, CC1Args, P);
  }

  if (!DriverArgs.hasArg(options::OPT_nostdlibinc))
    addSystemInclude(DriverArgs, CC1Args, getDriver().SysRoot + "/usr/include");
}

ToolChain::RuntimeLibType LynxXcoff::GetDefaultRuntimeLibType() const {
  // CompilerRT has an issue so fallbacks on libgcc.a provided within
  // the sysroot.
  return ToolChain::RLT_Libgcc;
}

auto LynxXcoff::buildAssembler() const -> Tool * {
  return new lynxxcoff::Assembler(*this);
}

auto LynxXcoff::buildLinker() const -> Tool * {
  return new lynxxcoff::Linker(*this);
}
