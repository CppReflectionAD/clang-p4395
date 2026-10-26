//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 || c++11 || c++14 || c++17 || c++20
// ADDITIONAL_COMPILE_FLAGS: -freflection
// ADDITIONAL_COMPILE_FLAGS: -fparameter-reflection

// <experimental/reflection>
//
// [reflection]
//
// variable_of works outside the function's body (an extension to P3096), but
// what it returns is still a local of that function: splicing it elsewhere is
// ill-formed, as for any other function's local.

#include <meta>

int fn(int p) { return p; }

int read_outside() {
  return [:std::meta::variable_of(std::meta::parameters_of(^^fn)[0]):];
  // expected-error@-1 2 {{reference to local variable 'p' declared in enclosing function 'fn'}}
}

// A function with no definition has no parameter variables.
int declared_only(int q);
constexpr auto bad = std::meta::variable_of(std::meta::parameters_of(^^declared_only)[0]);
// expected-error@-1 {{constexpr variable 'bad' must be initialized by a constant expression}}
// expected-note-re@* {{expected a reflection of a parameter of a function with a definition{{.*}}}}
// expected-note@-3 {{in call to}}

int main() {}
