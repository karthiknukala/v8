// Copyright 2022 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_SNAPSHOT_EMBEDDED_EMBEDDED_DATA_INL_H_
#define V8_SNAPSHOT_EMBEDDED_EMBEDDED_DATA_INL_H_

#include "src/snapshot/embedded/embedded-data.h"
// Include the non-inl header before the rest of the headers.

namespace v8 {
namespace internal {

Address EmbeddedData::InstructionStartOf(Builtin builtin) const {
  DCHECK(Builtins::IsBuiltinId(builtin));
  const struct LayoutDescription& desc = LayoutDescription(builtin);
#ifdef __CHERI_PURE_CAPABILITY__
  // Since all the code is placed in a .text section, we will end up with
  // sentries on CHERI when we deserialize the heap, but regular RX capabilities
  // in mksnapshot. If we are manipulating sentries, re-derive an unsealed RX
  // capability from the PCC and compute the new sentry.
  uintptr_t sentry = reinterpret_cast<uintptr_t>(RawCode());
  uintptr_t result_cap = sentry + desc.instruction_offset;
  if (V8_CHERI_SEALED(sentry)) {
    using namespace base;
    ptraddr_t instruction_start =
        V8_CHERI_ADDR_GET(sentry) + desc.instruction_offset;
#ifdef __aarch64__
    instruction_start |= 1;  // C64 LSB
#endif
    result_cap = reinterpret_cast<uintptr_t>(V8_CHERI_ADDR_SET(
        reinterpret_cast<uintptr_t>(V8_CHERI_PCC), instruction_start));
    result_cap = V8_CHERI_TO_SENTRY(result_cap);
    DCHECK_NE(reinterpret_cast<void*>(result_cap), nullptr);
  }
  const uint8_t* result = reinterpret_cast<const uint8_t*>(result_cap);
  DCHECK(V8_CHERI_TAG_GET(result));
#else
  const uint8_t* result = RawCode() + desc.instruction_offset;
#endif
  DCHECK_LT(result, code_ + code_size_);
  return reinterpret_cast<Address>(result);
}

Address EmbeddedData::InstructionEndOf(Builtin builtin) const {
  DCHECK(Builtins::IsBuiltinId(builtin));
  const struct LayoutDescription& desc = LayoutDescription(builtin);
  const uint8_t* result =
      RawCode() + desc.instruction_offset + desc.instruction_length;
  DCHECK_LT(result, code_ + code_size_);
  return reinterpret_cast<Address>(result);
}

uint32_t EmbeddedData::InstructionSizeOf(Builtin builtin) const {
  DCHECK(Builtins::IsBuiltinId(builtin));
  const struct LayoutDescription& desc = LayoutDescription(builtin);
  return desc.instruction_length;
}

Address EmbeddedData::MetadataStartOf(Builtin builtin) const {
  DCHECK(Builtins::IsBuiltinId(builtin));
  const struct LayoutDescription& desc = LayoutDescription(builtin);
  const uint8_t* result = RawMetadata() + desc.metadata_offset;
  DCHECK_LE(desc.metadata_offset, data_size_);
  return reinterpret_cast<Address>(result);
}

Address EmbeddedData::InstructionStartOfBytecodeHandlers() const {
  return InstructionStartOf(Builtin::kFirstBytecodeHandler);
}

Address EmbeddedData::InstructionEndOfBytecodeHandlers() const {
  static_assert(Builtins::kBytecodeHandlersAreSortedLast);
  // Note this also includes trailing padding, but that's fine for our purposes.
  return reinterpret_cast<Address>(code_ + code_size_);
}

uint32_t EmbeddedData::PaddedInstructionSizeOf(Builtin builtin) const {
  uint32_t size = InstructionSizeOf(builtin);
  CHECK_NE(size, 0);
  return PadAndAlignCode(size);
}

}  // namespace internal
}  // namespace v8

#endif  // V8_SNAPSHOT_EMBEDDED_EMBEDDED_DATA_INL_H_
