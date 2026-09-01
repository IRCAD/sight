---
name: create-service
description: "Create or modify a Sight C++ service and its factory registration, XML configuration, and unit test."
argument-hint: "<service name or .hpp path>"
---

# Create or modify a Sight service

Use the repository guidance in `AGENTS.md` and the detailed workflow in
`.agents/skills/sight-service/SKILL.md`.

Apply the workflow to the requested service only. Inspect the existing module,
registration, XML configurations, CMake test target, and neighbouring tests
before editing. Preserve established conventions and unrelated configuration.

Implement the requested change, add or update the service unit test, and run
the narrowest relevant build and test commands available. Report files changed
and validation results, distinguishing checks that ran from checks that were
blocked by the environment.
