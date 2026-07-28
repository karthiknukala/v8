// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if V8_TARGET_ARCH_ARM64

#include "src/codegen/arm64/decoder-arm64.h"
#include "src/common/globals.h"
#include "src/utils/utils.h"

namespace v8 {
namespace internal {

namespace {

bool IsExpectedVisitorInstruction(Instruction* instr, Instr fmask,
                                  Instr fixed) {
  if (instr->Mask(fmask) == fixed) return true;
#if V8_TARGET_CHERI
  // A CHERI build decodes both Morello capability-register branches and the
  // ordinary A64 X-register branches used by the Benchmark ABI through the
  // same visitor.
  return (fmask == UnconditionalBranchToRegisterFMask) &&
         (fixed == UnconditionalBranchToRegisterFixed) &&
         (instr->Mask(UnconditionalBranchToRegisterXFMask) ==
          UnconditionalBranchToRegisterXFixed);
#else
  return false;
#endif
}

}  // namespace

void DispatchingDecoderVisitor::AppendVisitor(DecoderVisitor* new_visitor) {
  visitors_.remove(new_visitor);
  visitors_.push_back(new_visitor);
}

void DispatchingDecoderVisitor::PrependVisitor(DecoderVisitor* new_visitor) {
  visitors_.remove(new_visitor);
  visitors_.push_front(new_visitor);
}

void DispatchingDecoderVisitor::InsertVisitorBefore(
    DecoderVisitor* new_visitor, DecoderVisitor* registered_visitor) {
  visitors_.remove(new_visitor);
  std::list<DecoderVisitor*>::iterator it;
  for (it = visitors_.begin(); it != visitors_.end(); it++) {
    if (*it == registered_visitor) {
      visitors_.insert(it, new_visitor);
      return;
    }
  }
  // We reached the end of the list. The last element must be
  // registered_visitor.
  DCHECK(*it == registered_visitor);
  visitors_.insert(it, new_visitor);
}

void DispatchingDecoderVisitor::InsertVisitorAfter(
    DecoderVisitor* new_visitor, DecoderVisitor* registered_visitor) {
  visitors_.remove(new_visitor);
  std::list<DecoderVisitor*>::iterator it;
  for (it = visitors_.begin(); it != visitors_.end(); it++) {
    if (*it == registered_visitor) {
      it++;
      visitors_.insert(it, new_visitor);
      return;
    }
  }
  // We reached the end of the list. The last element must be
  // registered_visitor.
  DCHECK(*it == registered_visitor);
  visitors_.push_back(new_visitor);
}

void DispatchingDecoderVisitor::RemoveVisitor(DecoderVisitor* visitor) {
  visitors_.remove(visitor);
}

#define DEFINE_VISITOR_CALLERS(A)                                \
  void DispatchingDecoderVisitor::Visit##A(Instruction* instr) { \
    if (!IsExpectedVisitorInstruction(instr, A##FMask,           \
                                      A##Fixed)) {                \
      DCHECK(IsExpectedVisitorInstruction(instr, A##FMask,       \
                                          A##Fixed));             \
    }                                                            \
    std::list<DecoderVisitor*>::iterator it;                     \
    for (it = visitors_.begin(); it != visitors_.end(); it++) {  \
      (*it)->Visit##A(instr);                                    \
    }                                                            \
  }
VISITOR_LIST(DEFINE_VISITOR_CALLERS)
#undef DEFINE_VISITOR_CALLERS

}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_ARM64
