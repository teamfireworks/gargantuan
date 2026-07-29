# Introduction

_Note: Gargantuan will have it's own docsite eventually._

FYI: Gargantuan is in it's infancy and should not be used yet. This page is for
people who want to work on Gargantuan's core.

Before you contribute, read the [TODO.md](../TODO.md)!! Also, please ask
godmothersfire in the Discord server before you try any big changes. I don't
want you wasting time to get a PR closed :(

## Prerequisites

- cmake
- ninja
- glslc for compiling shaders
- spirv-cross and the XCode toolchain IF compiling Metal shaders

## Configure

```sh
rm -rf build
mkdir build
cmake -B build
```

## Testing

```sh
cmake -B build
```

Create a `Testbed.luau` script in the repository root (it is not tracked by
Git), write some code (there's a few `examples` to use!), and then run the
program:

```sh
./build/gargantuan
```

For now, set Luau LSP's `platform` to Roblox. Eventually, Gargantuan will have
procedurally generated type definitions for consumption, alongside proper
project management and a test framework.

## Adding Data Types

It's easy!

- Make a datatype class that extends Userdata
- Implement a StackValue (theres the G_UD_STACKVALUE macro to simplify this)
- Add a matching UserdataTag
- If you're writing a Lib for the data type, edit ScriptEngine.hpp to include a OpenLib function for it, then implement it under scripting
- Call your datatypes CreateUserdataMetatable inside ScriptEngine.hpp
- It's done
