# doctest

Test-only dependency, vendored without local patches. It is not linked into the
engine or Sandbox, and configuration/build never downloads it.

- Upstream: https://github.com/doctest/doctest
- Release: **v2.5.3**
- Commit: `2d0a9359a60c51affe2a9bebb1be1dca47868151`
- License: MIT; preserved in [LICENSE.txt](LICENSE.txt).
- Files: upstream `doctest/doctest.h` and `LICENSE.txt`, copied verbatim.
- Platforms: headless tests target the project's Linux and Windows toolchains.

The interface target in `tests/CMakeLists.txt` supplies the header and enables
`DOCTEST_CONFIG_USE_STD_HEADERS` to avoid standard-library forward declarations.
The implementation/main is compiled once in `tests/TestMain.cpp`. Unit tests are
registered through CTest and built only when `BUILD_TESTING` is enabled.

The single-header framework keeps the initial test setup small and self-contained.
Version 2.5.3 is pinned for its C++23 standard-library compatibility, including
modern Clang/libstdc++ detection through `<version>`.

SHA-256 of the unmodified upstream files:

```text
cfd518a3ef90f67e1f3ba514df23fb3627437de1a2feeba78cf5062a40021421  doctest.h
0fe0b331fa1513dcce8604ff1fa925f32d1cea17d8aeb1c2471fad40d291adc5  LICENSE.txt
```

For an update, select a release and resolve its immutable commit. Download both
files from `https://raw.githubusercontent.com/doctest/doctest/<commit>/`, inspect
upstream changes/license, and update the revision/hashes here. Run Debug, Release,
and sanitizer core tests, including the Windows CI checks, before accepting the
update. Do not patch or suppress framework warnings without investigating them.
