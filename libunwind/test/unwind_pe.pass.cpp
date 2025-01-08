//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <unwind-pe.h>

const uint8_t TestData[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

int main() {
  struct {
    uint8_t Encoding;
    ptrdiff_t Length;
    _Unwind_Ptr Result;
  } Tests[] = {{DW_EH_PE_absptr, sizeof(void *), 0x0706050403020100},
               {DW_EH_PE_udata2, 2, 0x0100},
               {DW_EH_PE_udata4, 4, 0x03020100},
               {DW_EH_PE_udata8, 8, 0x0706050403020100},
               {DW_EH_PE_sdata2, 2, 0x0100},
               {DW_EH_PE_sdata4, 4, 0x03020100},
               {DW_EH_PE_sdata8, 8, 0x0706050403020100}};

  for (auto &T : Tests) {
    uint64_t Result = 0xaa;
    const auto *NewData =
        read_encoded_value_with_base(T.Encoding, 0, TestData, &Result);
    if (NewData - TestData != T.Length || Result != T.Result)
      return 1;

    if (size_of_encoded_value(T.Encoding) != T.Length)
      return 2;
  }

  {
    // ULEB128 example from Figure 22 of the DWARF4 stardard
    const uint8_t ULEB128Data[] = {57 + 0x80, 100};
    _uleb128_t Result = 0xaa;
    const auto *NewData = read_uleb128(ULEB128Data, &Result);
    if (NewData - ULEB128Data != 2 || Result != 12857)
      return 3;
  }

  {
    // SLEB128 example from Figure 23 of the DWARF4 stardard
    const uint8_t SLEB128Data[] = {0x7f + 0x80, 0x7e};
    _sleb128_t Result = 0xaa;
    const auto *NewData = read_sleb128(SLEB128Data, &Result);
    if (NewData - SLEB128Data != 2 || Result != -129)
      return 4;
  }

  return 0;
}
