# Sight repository guidance

Sight is a C++20 surgical image guidance and healthcare toolkit based on an
object/service architecture. Objects and services communicate through signals
and slots, and applications and components are described with XML.

## General conventions

- Write code and comments in English.
- Use four spaces for indentation; never use tabs.
- Use `snake_case` for variables, functions, and class names.
- Class data members use `m_` followed by snake_case, e.g. `m_image_size`.
- Keep lines at or below 120 characters.
- Use `#pragma once` for header guards.
- Keep includes minimal and follow the existing `.clang-tidy` configuration.
- Use namespaces matching the repository tree.

## Repository layout

- `app/`: XML applications
- `config/`: shared XML configurations
- `example/`: elaborate examples
- `lib/`: shared libraries
- `module/`: dynamically loadable modules and most services
- `tutorial/`: progressive tutorials
- `util/`: standalone executables

## Tests

Unit tests live below the target's `test/ut` directory and use Doctest.
Use the following order for build and test work:

1. If the project has no configured build, ask the user to configure it once
   with the CMake Tools extension. Do not silently choose a preset, compiler,
   or build type for the initial configuration.
2. Inspect the active CMake Tools configuration before building: selected
   project, configure preset (if any), build directory, generator, and build
   type. Use the selected configuration rather than assuming one.
3. Before running a data-dependent test, look for `FWTEST_DATA_DIR` in the
   selected project's `.vscode/launch.json` and relevant CMake Tools test or
   debug settings. Do not print or commit the value.
4. If it is not configured, ask the user to provide `FWTEST_DATA_DIR` for the
   selected repository. Each repository has its own data directory; never
   reuse a value from another repository without confirmation.
5. During development, build only the affected target and run only the
   relevant CTest test or test pattern. At the end of the change, run the
   broader or complete test suite when practical.
6. Validate with the relevant build, tests, and lint checks when available.
   Report checks that ran separately from environment blockers.

The repository-specific data path and the username are machine-specific. Never
guess them or hard-code them into committed project files. Prefer CMake Tools
or direct `cmake --build` and `ctest` commands using the active configuration.
Never delete a build directory to make a test pass.

For service tests, instantiate services through the factory, monitor their
input/inout data or signals, stop them before removal, and remove every service
they create. Test observable behaviour rather than protected member variables.

For service-specific creation and testing guidance, use the repository skill
at `.agents/skills/sight-service/SKILL.md`.

For filling an existing GitLab merge request, use the repository skill at
`.agents/skills/fill-mr/SKILL.md`. It must resolve repository roots from the
current workspace and must not contain machine-specific paths or user data.

For creating a GitLab issue from a project template, use the repository skill
at `.agents/skills/create-issue/SKILL.md`. It must resolve the project
from the current workspace and require confirmation before remote creation.

Useful documentation:

- https://sight.pages.ircad.fr/sight-doc/
- https://sight.pages.ircad.fr/sight-doc/HowTos/ServiceCreation.html
