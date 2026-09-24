//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// Equivalent of libgcc's unwind-pe.h, containing DWARF helpers
//
//===----------------------------------------------------------------------===//

#ifndef __UNWIND_PE_H__

#include <unwind.h>

#include <inttypes.h>

#ifndef NO_DWARF_CONSTANTS

#define DW_EH_PE_omit 0xff

#define DW_EH_PE_absptr 0x00
#define DW_EH_PE_uleb128 0x01
#define DW_EH_PE_udata2 0x02
#define DW_EH_PE_udata4 0x03
#define DW_EH_PE_udata8 0x04
#define DW_EH_PE_sleb128 0x09
#define DW_EH_PE_sdata2 0x0A
#define DW_EH_PE_sdata4 0x0B
#define DW_EH_PE_sdata8 0x0C

#define DW_EH_PE_pcrel 0x10
#define DW_EH_PE_textrel 0x20
#define DW_EH_PE_datarel 0x30
#define DW_EH_PE_funcrel 0x40
#define DW_EH_PE_aligned 0x50
#define DW_EH_PE_indirect 0x80

#endif // NO_DWARF_CONSTANTS

#ifdef __cplusplus
extern "C" {
#endif

// Names in this file follow the convention dictated by unwind-pe.h and
// required for compatibility with existing code:
//
// NOLINTBEGIN(readability-identifier-naming)

//
// Internal implementation
//

const uint8_t *__read_uleb128(const uint8_t *Data, _uleb128_t *Result);
const uint8_t *__read_sleb128(const uint8_t *Data, _sleb128_t *Result);
const uint8_t *__read_encoded_value_with_base(uint8_t Encoding,
                                              _Unwind_Ptr Base,
                                              const uint8_t *Data,
                                              _Unwind_Ptr *Result);
unsigned int __size_of_encoded_value(uint8_t Encoding);

//
// Functions exported from libgcc's unwind-pe.h
//

static inline unsigned int size_of_encoded_value(uint8_t encoding) {
  return __size_of_encoded_value(encoding);
}

static const uint8_t *read_encoded_value_with_base(uint8_t encoding,
                                                   _Unwind_Ptr base,
                                                   const uint8_t *p,
                                                   _Unwind_Ptr *val) {
  return __read_encoded_value_with_base(encoding, base, p, val);
}

static uintptr_t base_of_encoded_value(uint8_t encoding __attribute__((unused)),
                                       struct _Unwind_Context *context
                                       __attribute__((unused))) {
  // Not actually supported; read_encoded_value_with_base will abort if the
  // base is needed but 0.
  return 0;
}

static inline const uint8_t *read_encoded_value(struct _Unwind_Context *context,
                                                uint8_t encoding,
                                                const uint8_t *p,
                                                _Unwind_Ptr *val) {
  return read_encoded_value_with_base(
      encoding, base_of_encoded_value(encoding, context), p, val);
}

static inline const uint8_t *read_uleb128(const uint8_t *p, _uleb128_t *val) {
  return __read_uleb128(p, val);
}

static inline const uint8_t *read_sleb128(const uint8_t *p, _sleb128_t *val) {
  return __read_sleb128(p, val);
}

// NOLINTEND(readability-identifier-naming)

#ifdef __cplusplus
}
#endif

#endif // __UNWIND_PE_H__
