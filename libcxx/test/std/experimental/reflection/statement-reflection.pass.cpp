//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03 || c++11 || c++14 || c++17 || c++20
// ADDITIONAL_COMPILE_FLAGS: -freflection -fexpansion-statements

// <experimental/reflection>
//
// [reflection]

// Tests for function/statement reflection:
// body_of, statements_of, the is_*_statement predicates, return_operand_of,
// declared_variable_of, initializer_of, and the if/for/range-for role
// accessors (init_statement_of, condition_of, then_statement_of,
// else_statement_of, increment_of, loop_body_of, range_init_of,
// loop_variable_of).

#include <meta>

using namespace std::meta;

// A straight-line function to reflect.
constexpr double f(double x, double y) {
  double a = x * y;
  return a + x;
}

// body_of yields a statement reflection (a compound statement).
static_assert(is_statement(body_of(^^f)));
static_assert(!is_expression(body_of(^^f)));
static_assert(is_compound_statement(body_of(^^f)));

// statements_of walks the body's children.
static_assert(statements_of(body_of(^^f)).size() == 2);

// First child: the declaration `double a = x * y;`
static_assert(is_declaration_statement(statements_of(body_of(^^f))[0]));
static_assert(!is_return_statement(statements_of(body_of(^^f))[0]));
// Second child: the return statement.
static_assert(is_return_statement(statements_of(body_of(^^f))[1]));

// return_operand_of gives an expression reflection (`a + x`, an
// operator_expression).
namespace test_return {
  constexpr auto ret = return_operand_of(statements_of(body_of(^^f))[1]);
  static_assert(is_expression(ret));
  static_assert(is_operator_expression(ret) && operands_of(ret).size() == 2);
  static_assert(expression_operator_of(ret) == operators::op_plus);
}

// declared_variable_of -> the variable `a`; initializer_of(a) -> `x * y`.
namespace test_decl {
  constexpr auto s0 = statements_of(body_of(^^f))[0];
  constexpr auto va = declared_variable_of(s0);
  constexpr auto init = initializer_of(va);
  static_assert(is_expression(init));
  static_assert(is_operator_expression(init) && operands_of(init).size() == 2);
  static_assert(expression_operator_of(init) == operators::op_star);
}

// Expression-statement children come back as *statement* reflections (uniform
// statement model), classified by is_expression_statement; expression_of
// bridges to the underlying expression.
namespace test_expr_child {
  constexpr double g(double x) {
    x = x + 1.0;    // assignment expression statement
    return x * x;
  }
  // first child is the expression-statement `x = x + 1.0` (inlined because a
  // std::vector cannot be stored in a constexpr variable).
  static_assert(is_statement(statements_of(body_of(^^g))[0]));
  static_assert(!is_expression(statements_of(body_of(^^g))[0]));
  static_assert(is_expression_statement(statements_of(body_of(^^g))[0]));
  static_assert(!is_declaration_statement(statements_of(body_of(^^g))[0]));

  // expression_of -> the assignment `x = x + 1.0`, whose RHS is `x + 1.0`.
  constexpr auto e = expression_of(statements_of(body_of(^^g))[0]);
  static_assert(is_expression(e));
  static_assert(!is_statement(e));
  static_assert(is_operator_expression(e) && operands_of(e).size() == 2);
  static_assert(expression_operator_of(e) == operators::op_equals);
  static_assert(is_operator_expression(operands_of(e)[1]) &&
                operands_of(operands_of(e)[1]).size() == 2);
  static_assert(expression_operator_of(operands_of(e)[1]) ==
                operators::op_plus);
}

// --- if statements ---------------------------------------------------------

// Classification: an `if` is its own statement kind.
namespace test_if_kind {
  constexpr double f(double x) {
    if (x > 0.0)
      return x;
    return -x;
  }
  constexpr info ifs = statements_of(body_of(^^f))[0];
  static_assert(is_statement(ifs));
  static_assert(is_if_statement(ifs));
  static_assert(!is_for_statement(ifs));
  static_assert(!is_while_statement(ifs));
  static_assert(!is_range_for_statement(ifs));
}

// Plain `if (cond) then` with no else and no init-statement: both
// init_statement_of and else_statement_of are null. The then-branch is a
// single, unbraced return statement -- not a compound statement.
namespace test_if_plain {
  constexpr double f(double x) {
    if (x > 0.0)
      return x;
    return -x;
  }
  constexpr info ifs = statements_of(body_of(^^f))[0];
  static_assert(init_statement_of(ifs) == info{});
  static_assert(else_statement_of(ifs) == info{});
  static_assert(is_expression(condition_of(ifs)));
  static_assert(is_statement(then_statement_of(ifs)));
  static_assert(!is_compound_statement(then_statement_of(ifs)));
  static_assert(is_return_statement(then_statement_of(ifs)));
}

// `if (cond) {...} else {...}`: else_statement_of is present and braced.
namespace test_if_else {
  constexpr double f(double x) {
    if (x > 0.0) {
      return x;
    } else {
      return -x;
    }
  }
  constexpr info ifs = statements_of(body_of(^^f))[0];
  static_assert(else_statement_of(ifs) != info{});
  static_assert(is_compound_statement(then_statement_of(ifs)));
  static_assert(is_compound_statement(else_statement_of(ifs)));
}

// `if (init; cond)`: init_statement_of gives the declaration.
namespace test_if_init {
  constexpr double f(double x) {
    if (double y = x * x; y > 1.0)
      return y;
    return 0.0;
  }
  constexpr info ifs = statements_of(body_of(^^f))[0];
  constexpr info init = init_statement_of(ifs);
  static_assert(init != info{});
  static_assert(is_declaration_statement(init));
  static_assert(identifier_of(declared_variable_of(init)) == "y");
}

// Nesting: an `if` inside the then-branch of another `if`.
namespace test_if_nested {
  constexpr double f(double x, double y) {
    if (x > 0.0) {
      if (y > 0.0)
        return x + y;
      return x - y;
    }
    return -x;
  }
  constexpr info outer = statements_of(body_of(^^f))[0];
  constexpr info inner = statements_of(then_statement_of(outer))[0];
  static_assert(is_if_statement(inner));
  static_assert(else_statement_of(inner) == info{});
}

// --- for statements ----------------------------------------------------

// Classification: a `for` is its own statement kind.
namespace test_for_kind {
  constexpr double f() {
    double s = 0.0;
    for (int i = 0; i < 10; ++i)
      s = s + i;
    return s;
  }
  constexpr info fs = statements_of(body_of(^^f))[1];
  static_assert(is_statement(fs));
  static_assert(is_for_statement(fs));
  static_assert(!is_if_statement(fs));
  static_assert(!is_while_statement(fs));
  static_assert(!is_range_for_statement(fs));
}

// Full `for (init; cond; inc) body`, with an unbraced body.
namespace test_for_full {
  constexpr double f() {
    double s = 0.0;
    for (int i = 0; i < 10; ++i)
      s = s + i;
    return s;
  }
  constexpr info fs = statements_of(body_of(^^f))[1];
  constexpr info init = init_statement_of(fs);
  static_assert(init != info{});
  static_assert(is_declaration_statement(init));
  static_assert(identifier_of(declared_variable_of(init)) == "i");
  static_assert(is_expression(condition_of(fs)));
  static_assert(increment_of(fs) != info{});
  static_assert(is_statement(loop_body_of(fs)));
  static_assert(!is_compound_statement(loop_body_of(fs)));
  static_assert(is_expression_statement(loop_body_of(fs)));
}

// `for (;;)` -- every optional slot (init, condition, increment) absent.
namespace test_for_bare {
  constexpr int g() {
    for (;;) {
      return 1;
    }
  }
  constexpr info fs = statements_of(body_of(^^g))[0];
  static_assert(init_statement_of(fs) == info{});
  static_assert(condition_of(fs) == info{});
  static_assert(increment_of(fs) == info{});
  static_assert(is_compound_statement(loop_body_of(fs)));
}

// Nesting: a `for` inside a `for`.
namespace test_for_nested {
  constexpr double f() {
    double s = 0.0;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        s = s + 1.0;
      }
    }
    return s;
  }
  constexpr info outer = statements_of(body_of(^^f))[1];
  constexpr info inner = statements_of(loop_body_of(outer))[0];
  static_assert(is_for_statement(inner));
}

// --- range-for statements ------------------------------------------------

// Classification: a range-for is its own statement kind, distinct from a
// classic `for`.
namespace test_rangefor_kind {
  constexpr double f() {
    double arr[3] = {1.0, 2.0, 3.0};
    double s = 0.0;
    for (const auto& x : arr) {
      s = s + x;
    }
    return s;
  }
  constexpr info rfs = statements_of(body_of(^^f))[2];
  static_assert(is_statement(rfs));
  static_assert(is_range_for_statement(rfs));
  static_assert(!is_for_statement(rfs));
}

// Role accessors: the range, the loop variable, and the (braced) body.
namespace test_rangefor_accessors {
  constexpr double f() {
    double arr[3] = {1.0, 2.0, 3.0};
    double s = 0.0;
    for (const auto& x : arr) {
      s = s + x;
    }
    return s;
  }
  constexpr info rfs = statements_of(body_of(^^f))[2];
  static_assert(is_expression(range_init_of(rfs)));
  static_assert(identifier_of(loop_variable_of(rfs)) == "x");
  static_assert(is_compound_statement(loop_body_of(rfs)));
}

// Unbraced body is a single statement, not a compound one.
namespace test_rangefor_unbraced {
  constexpr double f() {
    double arr[3] = {1.0, 2.0, 3.0};
    double s = 0.0;
    for (const auto& x : arr)
      s = s + x;
    return s;
  }
  constexpr info rfs = statements_of(body_of(^^f))[2];
  static_assert(!is_compound_statement(loop_body_of(rfs)));
  static_assert(is_expression_statement(loop_body_of(rfs)));
}


int main() {}
