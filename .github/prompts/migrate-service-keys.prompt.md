---
name: migrate-service-keys
description: "Migrate a Sight service and all its XML usages to the hierarchical data key syntax. Use when a service still declares flat data keys (<in key=...>, <inout group=...>, <properties ...>) and should expose nested keys such as 'config.threshold' configured with <config threshold=... />."
argument-hint: "<path to the service .hpp> (optional, defaults to the active file)"
---

# Migrate a service to the hierarchical data key syntax

You migrate **one** Sight service at a time: its C++ declarations, its Doxygen block, and
**every** XML file that configures it, across the three repositories.

## Repositories

- `sight` -> `${workspaceFolder:sight}`
- `night` -> `${workspaceFolder:night}`
- `disrumpere` -> `${workspaceFolder:disrumpere}`

## Background: the two syntaxes

A service declares its data with `sight::data::ptr`, `sight::data::ptr_vector` and
`sight::data::property`. The key given to those declarations may now contain dots, and the XML
configuration mirrors that structure.

Legacy syntax:

```xml
<service uid="..." type="...">
    <in key="source" uid="${image}" />
    <inout key="target" uid="${result_image}" />
    <in key="threshold" uid="${threshold}" />
    <in key="value" value="10" />
    <in group="tracker_ip">
        <key uid="${ip1}" />
        <key uid="${ip2}" />
    </in>
    <in group="tracker_port">
        <key uid="${port1}" />
        <key uid="${port2}" />
    </in>
    <properties label="hello" />
</service>
```

Hierarchical syntax, for the keys `image.source`, `image.target`, `config.threshold`,
`config.value`, `config.tracker.ip`, `config.tracker.port`, `config.label`:

```xml
<service uid="..." type="...">
    <image source="${image}" target="${result_image}" />
    <config threshold="${threshold}" value="10" label="hello">
        <tracker ip="${ip1}" port="${port1}" />
        <tracker ip="${ip2}" port="${port2}" />
    </config>
</service>
```

Rules enforced by the parser:

- The key of an attribute is the dotted path of the enclosing tags plus the attribute name.
  Nesting is unlimited.
- For a **group** (`data::ptr_vector`), the index is the rank of the innermost tag among its
  homonym siblings. One tag per index; attributes of different groups sharing the same rank
  belong to the same index.
- For a **non-group** key, the tag may be repeated or the attributes merged, both are equivalent.
- An attribute whose value is the uid of an object declared in the application configuration
  (created **or** deferred) is bound to that object. Any other value is a literal, built by the
  service from the type of the `data::ptr` (see `create_value_objects()`).
- A tag or attribute that does not match a key declared by the service is **silently ignored** by
  the framework and left for the service to parse. A typo therefore produces no error: the
  migration must be exhaustive and verified by a build and a run.
- Reserved tag names, which can not be used as the first level of a key:
  `in`, `inout`, `out`, `properties`, `optional`.
- `optional` is also a **reserved attribute name**: on any tag it marks the keys carried by that
  tag occurrence as optional, which gives a per-element granularity inside a group.
  A sibling `<optional key="image.source" />` marks a whole key as optional instead.
- `auto_connect` per data no longer exists. Only `<service auto_connect="false">` remains.

## Step-by-step process

### 1. Locate the service

- Use the argument if provided, otherwise the active file. It must be the `.hpp` of a service.
- Read the `.hpp` and its matching `.cpp`.
- Extract the service type registered with `SIGHT_REGISTER_SERVICE`, for example
  `sight::module::filter::image::my_service`. Report it, you will need it to find the XML files.
- List every declaration, with its current key, access and kind:

  | Member | Current key | Kind | Access |
  |---|---|---|---|
  | `m_image` | `image` | `data::ptr` | `in` |
  | `m_ips` | `tracker_ip` | `data::ptr_vector` | `in` |
  | `m_label` | `label` | `data::property` | - |

- If the service already uses dotted keys, stop and report that it is already migrated.

### 2. Ask for the new key names

Ask the user, in one round of questions:

1. Whether all keys go under a **single common tag** (and which one), or whether each key must be
   named individually.
2. If individually, ask for the new key of **each** declaration listed in step 1, proposing the
   current key as default.
3. Whether the `data::property` declarations should be converted to `data::ptr`. If yes, ask
   whether to use the default `config.` prefix or another name for each of them.

Do not proceed before the user has answered.

### 3. Update the C++ declarations

- Rewrite the key string of each `data::ptr` / `data::ptr_vector`.
- For each `data::property` to convert:
  - `data::property<data::integer> m_x {this, "x", 42};`
    becomes `data::ptr<data::integer> m_x {this, "config.x", 42};`
  - `data::property<data::string> m_s {this, "s", {"txt"}};`
    becomes `data::ptr<data::string> m_s {this, "config.s", {"txt"}};`
  - The access stays `inout`, which is the default of `data::ptr`.
  - **`data::ptr` has no `value()` nor `operator*`.** Every call site must be adapted:
    - `*m_x` and `m_x.value()` become `*m_x`
    - `m_x.lock()` and `m_x.const_lock()` are unchanged
  - The property slot disappears: `on_property_set()` is no longer called for that key, and
    `MAKE_PROPERTY_SLOT_NAME` no longer applies. If the service overrides `on_property_set()` and
    reacts to this key, **do not convert it**: report it to the user and keep the property.
  - If the key appears in `auto_connections()`, the entry stays valid, the member is unchanged.
- Search the whole repository for the member name and for the old key string, and fix every use.

### 4. Update the Doxygen block

Rewrite the `@section XML XML Configuration` code block of the service with the new syntax, and
update the `@subsection In-Out` / `@subsection Properties` / `@subsection Configuration` lists so
the key names match. Keep the wording style of the file.

### 5. Update the XML files

Search the three repositories for the service type:

```
grep -rn "<service type" --include=\*.xml . | grep "<the service type>"
```

Search for both `type="..."` occurrences and for the service used inside a
`sight::app::extension::config` extension. Do not forget `.xml` files under `rc/configurations/`.

For each occurrence, apply the transformation:

| Legacy | Hierarchical |
|---|---|
| `<in key="a" uid="${x}" />` for key `foo.a` | `<foo a="${x}" />` |
| `<in key="a" value="10" />` for key `foo.a` | `<foo a="10" />` |
| `<in key="a" uid="${x}" optional="true" />` | `<foo a="${x}" optional="true" />` or `<foo a="${x}" /><optional key="foo.a" />` |
| `<in key="a" uid="${x}" auto_connect="false" />` | `<foo a="${x}" />` plus `auto_connect="false"` on `<service>` if it must really be disabled |
| `<in group="g"><key uid="${x}" /><key uid="${y}" optional="true" /></in>` for key `foo.g` | `<foo g="${x}" /><foo g="${y}" optional="true" />` |
| `<properties p="v" q="${o}" />` for keys `config.p` / `config.q` | `<config p="v" q="${o}" />` |
| `<properties><property p="v" /></properties>` | `<config p="v" />` |

Additional rules:

- The uid in the attribute must always use the ${...} in the new syntax, so for example:
 `<in key="a" uid="x" />` becomes `<foo a="${x}" />`
- Merge the attributes of non-group keys into as few tags as possible, keeping the reading order
  of the original file.
- Group keys sharing the same index must end up in the **same** tag occurrence, in the original
  index order. Never reorder them.
- Never merge tags that carry a different `optional` flag.
- `<properties from="${map}" />` has no hierarchical equivalent, keep it as is and report it.
- If the same tag name is already used by the service for its own configuration (a tag that is not
  a declared key), pick a different first level and warn the user.
- Preserve the indentation, the comments and the blank lines of each file.

### 6. Verify

1. Build:
   ```
  cd ${workspaceFolder:disrumpere}/build/Debug/sight && ninja && ninja install
   ```
   Never delete a build directory. If the build fails, fix the code or stop and ask.
2. Run the tests:
   ```
  cd ${workspaceFolder:disrumpere}/build/Debug/sight && \
     FWTEST_DATA_DIR=${workspaceFolder:disrumpere}/../data/sight-data ctest -j8
   ```
3. Lint every modified C++ file, one file per invocation:
   ```
  ${workspaceFolder:sight}/sight-git/hooks/sheldon -f -i <file> \
     --hooks crlf,tab,filesize,codingstyle,doxygen,badwords,cspell \
     -b ${workspaceFolder:disrumpere}/build/Debug/sight
   ```
   Rebuild if a file was reformatted.
4. Re-grep the three repositories for the **old** key strings to make sure no XML occurrence was
   missed. This is the only safety net, since unknown keys are silently ignored at runtime.

### 7. Report

Give a short summary:

- the service type and the list of `old key -> new key`
- the properties converted, and the ones deliberately kept
- the list of modified XML files, with a count per repository
- anything left to the user: manual checks, `<properties from>` occurrences, behaviour changes on
  `optional` or `auto_connect`
