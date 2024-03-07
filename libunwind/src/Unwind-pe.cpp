//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// Implementation of public functions in unwind-pe.h
//
//===----------------------------------------------------------------------===//

#define NO_DWARF_CONSTANTS
#include <unwind-pe.h>

#include "AddressSpace.hpp"
#include "dwarf2.h"

using libunwind::LocalAddressSpace;

const uint8_t *__read_uleb128(const uint8_t *Data, _uleb128_t *Result) {
  auto Addr = reinterpret_cast<uintptr_t>(Data);
  *Result = LocalAddressSpace::getULEB128(Addr, 0);
  return reinterpret_cast<uint8_t *>(Addr);
}

const uint8_t *__read_sleb128(const uint8_t *Data, _sleb128_t *Result) {
  auto Addr = reinterpret_cast<uintptr_t>(Data);
  *Result = LocalAddressSpace::getSLEB128(Addr, 0);
  return reinterpret_cast<uint8_t *>(Addr);
}

const uint8_t *__read_encoded_value_with_base(uint8_t Encoding,
                                              _Unwind_Ptr Base,
                                              const uint8_t *Data,
                                              _Unwind_Ptr *Result) {
  LocalAddressSpace AS;
  auto Addr = reinterpret_cast<uintptr_t>(Data);
  *Result = AS.getEncodedP(Addr, 0, Encoding, Base);
  return reinterpret_cast<uint8_t *>(Addr);
}

unsigned int __size_of_encoded_value(uint8_t Encoding) {
  if (Encoding == DW_EH_PE_omit)
    return 0;

  switch (Encoding & 0x0F) {
  case DW_EH_PE_absptr:
    return sizeof(uintptr_t);
  case DW_EH_PE_udata2:
    return sizeof(uint16_t);
  case DW_EH_PE_udata4:
    return sizeof(uint32_t);
  case DW_EH_PE_udata8:
    return sizeof(uint64_t);
  case DW_EH_PE_sdata2:
    return sizeof(int16_t);
  case DW_EH_PE_sdata4:
    return sizeof(int32_t);
  case DW_EH_PE_sdata8:
    return sizeof(int64_t);
  default:
    _LIBUNWIND_ABORT("unsupported encoding");
  }
}
