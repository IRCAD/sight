---
name: sight-service
description: Create, modify, register, configure, and test C++ services in the Sight framework. Use for service lifecycle code, data-key declarations, XML registration, or service unit tests.
metadata:
  short-description: Sight service development
---

# Sight service development

Use this skill when a task creates or changes a Sight service. First inspect
the target module, neighbouring services, its CMake target, registrations, XML
configurations, and existing tests. Keep the change scoped to the requested
service and preserve unrelated configuration.

## Service implementation

- Place the service in the appropriate `module/<module>/` namespace.
- Derive from `sight::service::base` and declare it with
  `SIGHT_DECLARE_SERVICE(service_name, sight::service::base)`.
- Implement the lifecycle methods that the service needs:
  `configuring(const config_t&)`, `starting()`, `stopping()`, and `updating()`.
- Use `sight::data::ptr<T>` for one data object and
  `sight::data::ptr_vector<T>` for groups of data objects. Use the exact data
  keys required by the task; dotted keys represent the XML hierarchy.
- Keep includes minimal, use `#pragma once`, and follow the surrounding code's
  namespace and formatting conventions.
- Do not expose or weaken protected service state merely to make it testable.

## Registration and XML

- Register the service in the module's `rc/plugin.xml` using the existing
  `sight::service::extension::factory` pattern and the actual fully qualified
  service type.
- Check every XML consumer of the service, including configurations under
  `rc/configurations/` and consumers in sibling repositories when the task
  spans the Disrumpere workspace.
- Keep XML keys consistent with the C++ declarations and the registration.
- When migrating flat keys to dotted keys, update declarations, Doxygen,
  registrations, tests, and all XML consumers together. Search again for the
  old keys because unknown XML keys may be silently ignored.

## Tests and validation

Follow the general test and validation rules in the repository's `AGENTS.md`.
For this service specifically, put the test in the target's
`test/ut/<service_name>_test.cpp`, use a Doctest `TEST_SUITE` named after the
service, and preserve existing scenarios and invalid-to-valid transitions.

For asynchronous behaviour, bind a local slot to the service signal and use
`SIGHT_TEST_WAIT` where appropriate. Set data inputs/inouts through the public
service API and test observable data, outputs, and signals.
