# Clang/P2996: An LLVM fork

Welcome to Clang/P2996, a fork of the [`llvm/llvm-project`](
https://github.com/llvm/llvm-project) implementing experimental support for ISO
C++ proposal [P2996](https://wg21.link/p2996) in `clang`. Please see [P2996.md](
P2996.md) for more about this project.

## CppReflectionAD/clang-p4395

This fork's `p4395` branch adds expression reflection, the C++ proposal P4395
(`body_of`, `statements_of`, `operands_of`, ...), to Clang/P2996.
[cpp_reflection_ad](https://github.com/CppReflectionAD/cpp_reflection_ad)
builds and tests with it, and it is on [Compiler Explorer](https://godbolt.org/)
as **x86-64 clang (expr-reflect)**.

This repository has issues turned off. Report a bug in the reflection support
as an
[issue in cpp_reflection_ad](https://github.com/CppReflectionAD/cpp_reflection_ad/issues/new),
with a small reproducer. Bugs in the forks are tracked there under the
[`CompilerGap`](https://github.com/CppReflectionAD/cpp_reflection_ad/issues?q=label%3ACompilerGap)
label. A fix is a pull request into `p4395`, merged with a merge commit (see
[Bumping a compiler pin](https://github.com/CppReflectionAD/cpp_reflection_ad/blob/main/CONTRIBUTING.md#bumping-a-compiler-pin)).

# The LLVM Compiler Infrastructure

[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/llvm/llvm-project/badge)](https://securityscorecards.dev/viewer/?uri=github.com/llvm/llvm-project)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/8273/badge)](https://www.bestpractices.dev/projects/8273)
[![libc++](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml/badge.svg?branch=main&event=schedule)](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml?query=event%3Aschedule)

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](https://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362),
[LLVM Office Hours](https://llvm.org/docs/GettingInvolved.html#office-hours) or
[Regular sync-ups](https://llvm.org/docs/GettingInvolved.html#online-sync-ups).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.
