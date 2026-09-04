---
name: migrate-service-keys
description: Migrate one Sight service from flat data keys to hierarchical keys and update every XML configuration across the Sight workspace. Use when C++ service declarations and their XML usages must be migrated together.
metadata:
  short-description: Migrate a service to hierarchical data keys
---

# Migrate a service to hierarchical data keys

Migrate one Sight service at a time: its C++ declarations, Doxygen block, and
every XML file that configures it across all relevant repositories in the
current workspace. Preserve unrelated worktree changes.

## Before editing

Use the optional argument as the service `.hpp`; otherwise use the active file.
Read its matching `.cpp`, identify the `SIGHT_REGISTER_SERVICE` type, and list
every declaration with its current key, kind, and access. If the service
already uses dotted keys, stop and report that it is already migrated.

Ask the user in one round of questions, and do not edit before receiving the
answers:

1. Should all keys use one common tag, and if so which one, or should each key
   be named individually?
2. If keys are named individually, what is the new key for each declaration?
   Propose the current key as the default.
3. Should each `data::property` be converted to `data::ptr`? If yes, ask
   whether to use the default `config.` prefix or another prefix per property.

## Key and parser rules

- A key is the dotted path of enclosing XML tags plus the attribute name;
  nesting is unlimited.
- A `data::ptr_vector` index is the rank of the innermost tag among homonymous
  siblings. Attributes of groups sharing a rank belong to the same index;
  preserve index and reading order.
- Repeated tags or merged attributes are equivalent for non-group keys.
- An attribute matching an application object UID, including a deferred UID,
  binds to that object. Other values are literals created from the `data::ptr`
  type.
- Unknown tags and attributes are silently ignored by the framework. The
  migration must therefore be exhaustive and verified by build and runtime
  checks.
- Reserved first-level tags are `in`, `inout`, `out`, `properties`, and
  `optional`. `optional` is also a reserved attribute: it applies to keys on
  that tag occurrence; `<optional key="..." />` applies to a whole key.
- Per-data `auto_connect` no longer exists. Only
  `<service auto_connect="false">` remains.

The target shape is, for example:

```xml
<service uid="..." type="...">
    <image source="${image}" target="${result_image}" />
    <config threshold="${threshold}" value="10">
        <tracker ip="${ip1}" />
        <tracker ip="${ip2}" />
    </config>
</service>
```

## C++ and Doxygen

- Rewrite every `data::ptr` and `data::ptr_vector` key string and search the
  whole workspace for each member and old key string.
- A property conversion changes, for example,
  `data::property<data::integer> m_x {this, "x", 42};` to
  `data::ptr<data::integer> m_x {this, "config.x", 42};`. Its access is the
  default `inout` access of `data::ptr`.
- Adapt converted-property call sites: property `value()`/dereference access
  becomes pointer dereference (`*m_x`); `lock()` and `const_lock()` stay
  unchanged. Remove its property slot and `MAKE_PROPERTY_SLOT_NAME` use. If
  `on_property_set()` reacts to that key, do not convert it; report it and keep
  the property. An `auto_connections()` entry remains valid when the member is
  unchanged.
- Rewrite the service Doxygen `@section XML XML Configuration` example and
  the `In-Out`, `Properties`, and `Configuration` lists with the new keys,
  keeping the file's wording style.

## XML

Resolve the relevant repository roots with `git rev-parse --show-toplevel`.
Search all of them for the registered service type, including `type="..."`,
services inside the application configuration extension, and XML under
`rc/configurations/`.

Transform legacy entries as follows:

| Legacy | Hierarchical |
|---|---|
| `<in key="a" uid="${x}" />` for `foo.a` | `<foo a="${x}" />` |
| `<in key="a" value="10" />` for `foo.a` | `<foo a="10" />` |
| `<in key="a" uid="${x}" optional="true" />` | `<foo a="${x}" optional="true" />` or whole-key `<optional key="foo.a" />` |
| `<in key="a" uid="${x}" auto_connect="false" />` | `<foo a="${x}" />`, plus service-level `auto_connect="false"` if needed |
| `<in group="g"><key uid="${x}" /><key uid="${y}" optional="true" /></in>` for `foo.g` | `<foo g="${x}" /><foo g="${y}" optional="true" />` |
| `<properties p="v" q="${o}" />` for `config.p`/`config.q` | `<config p="v" q="${o}" />` |
| `<properties><property p="v" /></properties>` | `<config p="v" />` |

Always write object UIDs as `${...}` in new attributes. Merge non-group
attributes into as few tags as possible in original reading order, without
merging different `optional` flags. Put group attributes sharing an index in
the same tag occurrence and never reorder them. Preserve indentation, comments,
and blank lines. If a first-level tag conflicts with service-owned undeclared
configuration, choose another and warn the user. Keep and report
`<properties from="${map}" />`, which has no hierarchical equivalent.

## Verification and report

Use the active CMake Tools configuration and its build directory; never choose
a preset silently or delete a build directory. If no build is configured, ask
the user to configure it once. For data-dependent tests, use the selected
repository's configured `FWTEST_DATA_DIR` without printing or committing it;
ask the user if it is absent.

Build and test the affected target first, then the broader suite when practical.
For the standard build, the commands are:

```text
cd <configured-build-dir> && ninja && ninja install
cd <configured-build-dir> && FWTEST_DATA_DIR=<configured-test-data> ctest -j8
```

Lint each modified C++ file separately. Prefer the user's configured VS Code
task named `Lint format` when it is available. Codex cannot press Run Task in
the VS Code UI, but can read the task definition and execute its shell command
with `${file}` replaced by the file being checked and
`${command:cmake.buildDirectory}` replaced by the active CMake build
directory. Preserve the task's configured hook list and options. If the task
definition is unavailable or cannot be resolved, use the repository's
configured Sheldon hook directly instead of guessing a private path.

Rebuild if lint reformats a file. Finally re-search all relevant repositories
for old key strings. Report executed checks separately from blocked checks, and
summarize the service type, every `old key -> new key`, converted and retained
properties, modified XML files with counts per repository, and remaining
manual work including `<properties from>`, `optional`, and `auto_connect`.
