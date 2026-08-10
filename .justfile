# TODO: Move as much of this into tools with Lute, so we can rid Just as a
# build dependency

GARGANTUAN_BUILD_DIRECTORY := "./build"
GARGANTUAN_BINARY := "./build/gargantuan"

# Runs the Gargantuan binary
gargantuan *args:
    -{{ GARGANTUAN_BINARY }} {{ args }}

# Rebuilds the engine, and then runs the Gargantuan binary
fresh *args: build
    -{{ GARGANTUAN_BINARY }} {{ args }}

# Pulls the latest submodules
submodules:
    git submodule update --init --recursive

# Configures the build directory
configure tracy="OFF" build_type="Debug":
    cmake -B {{ GARGANTUAN_BUILD_DIRECTORY }} -S . -G Ninja \
        -DCMAKE_BUILD_TYPE={{ build_type }} \
        -DGARGANTUAN_TRACY={{ tracy }} \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# Builds the engine
build:
    lute tools/classgen
    cmake --build {{ GARGANTUAN_BUILD_DIRECTORY }} -j={{ num_cpus() }}

# Rebuilds the engine, then runs an example inside assets
fresh_example example *flags: build
    -{{ GARGANTUAN_BINARY }} --script=./assets/examples/{{ example }} {{ flags }}

# Rebuilds the engine, then runs the Gargantuan Studio project
fresh_studio *flags: build
    -{{ GARGANTUAN_BINARY }} --project=./studio {{ flags }}

# Runs an example inside assets without rebuilding
run_example example *flags:
    -{{ GARGANTUAN_BINARY }} --script=./assets/examples/{{ example }} {{ flags }}

# Runs the Gargantuan Studio project without rebuilding
run_studio *flags:
    -{{ GARGANTUAN_BINARY }} --project=./studio {{ flags }}

# Runs tests for the core library
test_core: build
    lest run core

# Runs tests for all libraries
test: test_core
