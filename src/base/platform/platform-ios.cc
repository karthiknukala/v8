// Copyright 2023 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/base/base-export.h"
#include "src/base/build_config.h"

// pthread_jit_write_protect_np is marked as not available in the iOS
// SDK but it is there for the iOS simulator. So we provide a thunk
// and a forward declaration in a compilation target that doesn't
// include pthread.h to avoid the compiler error.
extern "C" void pthread_jit_write_protect_np(int enable);

namespace v8::base {
#ifdef __CHERI_PURE_CAPABILITY__
#error "This file has not been ported to CHERI."
#endif
struct OS::C18n::TrustedFrameState::TrustedFrameStateImpl {};
OS::C18n::TrustedFrameState::TrustedFrameState() {}
void* OS::C18n::GetTrustedStack(void* pc) { return nullptr; }
void* OS::C18n::GetNextTrustedFrame(
    OS::C18n::TrustedFrameState& trusted_frame_state, void* trusted_frame) {
  return nullptr;
}
size_t OS::C18n::TrustedFrameState::NumRegisters() const { return 0; }
const void* const* OS::C18n::TrustedFrameState::Registers() const {
  return nullptr;
}
const void* OS::C18n::TrustedFrameState::StackStart() const { return nullptr; }
const void* OS::C18n::TrustedFrameState::StackTop() const { return nullptr; }
bool OS::C18n::ShouldIterateStack(const void* top, const void* start) {
  return true;
}
bool OS::C18n::Enabled() { return false; }
ptraddr_t OS::C18n::Reflect(uintptr_t ptr) { return ptr; }
ptraddr_t OS::C18n::InvalidReflectedAddress() { return 0; }
bool OS::C18n::IsValidReflectedAddress(ptraddr_t addr) { return addr != 0; }

#if V8_HAS_PTHREAD_JIT_WRITE_PROTECT && defined(V8_OS_IOS)
V8_BASE_EXPORT void SetJitWriteProtected(int enable) {
  pthread_jit_write_protect_np(enable);
}
#endif

}  // namespace v8::base
