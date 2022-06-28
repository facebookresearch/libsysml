# libsysml

A collection of minimal yet performant utilities that are useful for various forms of research at the intersection of machine learning and systems.

### Benchmarking

The `measure.hpp` header provides utilities for measuring the mean, median and minimum runtime of arbitrary functions.
Also useful is the ability to limit the execution time of the benchmark.

```cpp
#include "sysml/measure.hpp"

unsigned iterations = 1000;
double limit_in_seconds = 0.01;
auto time = measure_fastest_time_limited([]() {
  // do something
}, iterations, limit_in_seconds);

std::cout << "fastest run " << time << " seconds";
```

### Parallelism

Threading utilities can be found in the `sysml/thread/` folder,
with fast primitives for `parallel_for` and barriers.

### Code generation

An X86_64/ARM64 codegenerator based on `xbyak`/`xbyak_aarch64` can be found in `sysml/code_generator/code_generator.hpp`.
There are functions to simplify the use of generated functions, such as automatic `shared_ptr` wrapping and improved executable memory mapping.

### Fast N-dimensional Arrays

Create `ndarray_ref`s from underlying data.

```cpp
std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7, 8};
std::array<std::size_t, 2> shape{3, 3};
sysml::const_ndarray_ref<int, 2> nda(v.data(), shape);
```

### Numerics

Determine the availability of numeric types on the system.

```cpp
#include "sysml/numeric.hpp"

using namespace sysml;

CHECK(is_arithmetic_v<char32_t>);
CHECK(is_arithmetic_v<uint16x4_t>);
```

### Small array support

Fast stack-allocated arrays with the `vek.hpp` header:

```cpp
#include "sysml/ndloop.hpp"
#include "sysml/vek.hpp"

using namespace sysml;

vek<int, 3> range{2, 3, 4};
// Equivalent to three nested loops:
ndloop(range, [&](auto const& v) { me[v[0]][v[1]][v[2]] = 0xdeadbeef; });

for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j)
        for (int k = 0; k < 4; ++k)
            CHECK(me[i][j][k] == 0xdeadbeef);
            
// Alternatively:
ndloop(range, [&](auto a, auto b, auto c) { me[a][b][c] = 0x12341234; });

for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j)
        for (int k = 0; k < 4; ++k)
            CHECK(me[i][j][k] == 0x12341234);

```

### Iteration

Easily iterate over multiple dimensions with `sysml/ndloop.hpp`.

## Contribution

To contribute, please see the CONTRIBUTING.md file.

## License

libsysml is MIT licensed, as found in the LICENSE file.
