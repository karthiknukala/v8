// Copyright 2026 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Compatibility shim for std::source_location (C++20) for compilers/libc++
// that lack the <source_location> header. Uses compiler builtins instead.

#ifndef V8_BASE_SOURCE_LOCATION_H_
#define V8_BASE_SOURCE_LOCATION_H_

#include <cstddef>

namespace v8 {
namespace base {

struct source_location {
  static constexpr source_location current(
      const char* file = __builtin_FILE(), int line = __builtin_LINE(),
      const char* func = __builtin_FUNCTION()) noexcept {
    source_location loc;
    loc.file_ = file;
    loc.line_ = line;
    loc.func_ = func;
    return loc;
  }

  constexpr const char* file_name() const noexcept { return file_; }
  constexpr int line() const noexcept { return line_; }
  constexpr int column() const noexcept { return 0; }
  constexpr const char* function_name() const noexcept { return func_; }

 private:
  const char* file_ = "";
  int line_ = 0;
  const char* func_ = "";
};

}  // namespace base
}  // namespace v8

#endif  // V8_BASE_SOURCE_LOCATION_COMPAT_H_
