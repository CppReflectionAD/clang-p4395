//===----------------------------------------------------------------------===//
//
// Copyright 2024 Bloomberg Finance L.P.
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 || c++11 || c++14 || c++17 || c++20
// ADDITIONAL_COMPILE_FLAGS: -freflection

// <experimental/reflection>
//
// [reflection]

#include <meta>


                             // ==================
                             // disallowed_results
                             // ==================

namespace disallowed_results {
constexpr auto v1 = std::meta::reflect_constant((const char *)"fails");
  // expected-error@-1 {{must be initialized by a constant expression}} \
  // expected-note@-1 {{provided value cannot be represented}}

struct HoldsTemporary {
  const int &tmp;
};
constexpr HoldsTemporary htmp{42};
constexpr auto v2 = std::meta::reflect_constant(htmp);
  // expected-error@-1 {{must be initialized by a constant expression}} \
  // expected-note@-1 {{provided value cannot be represented}}

}  // namespace disallowed_results

                          // ======================
                          // extract_non_structural
                          // ======================

namespace extract_non_structural {
// A private data member makes a class non-structural.
class NonStructural {
  int v = 1;
public:
  constexpr int get() const { return v; }
};
constexpr NonStructural var;

// By reference, 'extract' needs no structural type.
static_assert(std::meta::extract<const NonStructural &>(^^var).get() == 1);

// By value, 'extract' is 'extract-value<T>(constant_of(r))', and 'constant_of'
// mandates a structural type.
constexpr int v1 = std::meta::extract<NonStructural>(^^var).get();
  // expected-error@-1 {{must be initialized by a constant expression}} \
  // expected-note@-1 {{values of non-structural type}}

constexpr int v2 = std::meta::extract<NonStructural>(
                       std::meta::reflect_object(var)).get();
  // expected-error@-2 {{must be initialized by a constant expression}} \
  // expected-note@-2 {{values of non-structural type}}

consteval int fromLocal() {
  [[maybe_unused]] NonStructural local;
  return std::meta::extract<NonStructural>(^^local).get();
    // expected-note@-1 {{values of non-structural type}}
}
constexpr int v3 = fromLocal();
  // expected-error@-1 {{must be initialized by a constant expression}}
}  // namespace extract_non_structural

                              // ================
                              // self_referential
                              // ================

namespace self_referential {
struct A {
  int *const p;
  consteval A(int *p) : p(p) {}
  consteval A(const A &oth) : p(0) {
    if (oth.p) {
      ++*oth.p;
    }
  }
};

consteval int f() {
  int x = 42;
  std::meta::reflect_constant<A>(&x);
  return x;
}

static_assert(f() == 43);
  // expected-error@-1 {{not an integral constant expression}}
}  // namespace self_referential

int main() { }
