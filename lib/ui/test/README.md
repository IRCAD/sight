# sight::ui::test

Library used to write GUI functional tests (`test/uit`): doctest tests that drive a whole Sight application from the
outside, as a user does, one scenario per process, written from the XML of the application.

To write one, read `.agents/skills/sight-gui-test/SKILL.md`, which is meant for humans too.

```cpp
#include <ui/test/gui_test.hpp>

SIGHT_GUI_TEST_CASE("my_scenario")
{
    ui.click("top_toolbar_left/volume");
    ui.expect("top_toolbar_left/volume").checked();
}
```

## Files

- **`gui_test.hpp`**: `SIGHT_GUI_TEST_CASE("name")`, the entry point of a scenario. It loads the `PROFILE` of the
  target, builds a locator catalog from the loaded modules, runs the body on a thread of its own and reports its doctest
  assertions on the test thread.
- **`scenario.hpp`/`scenario.cpp`**: the verbs (`click`, `set`, `select`, `choose_file`...), `expect`, `read`, `data`,
  `watch_signal`, images, `dump`, `step_log`. Each operation has a deadline of 30 s, or what it says; its first look
  waits at least 30 s for the GUI to answer, however short the deadline, so a GUI slowed by load does not fail it.
- **`expectation.hpp`**: what `expect` and `expect_data` return, the assertions retried until they hold.
- **`locator.hpp`/`locator.cpp`**: locators and their builders (`loc`, `dialog`, `item`, `item_at`, `name`,
  `menu_item`); the parser and `locator_catalog`, the XML names known before anything waits, are in
  `detail/locator.hpp`.
- **`image.hpp`/`image.cpp`**: the metric of images and its failure artifacts.
- **`adaptors.cpp`**: how each kind of widget is acted on once a user could reach it, through its Qt API or events
  handed to it directly, so that Qt emits what a user's action emits; Qt's own color dialog through its Qt API.
  **`file_dialog.cpp`**: the real `QFileDialog`, through its Qt API: a scenario tests the application, not Qt.
  **`application_objects.cpp`**: what `data` and `service` find, by XML uid and scope.
- **`detail/`**: the GUI gateway and dispatcher, window-level input, deadlines, the test window. Not an API.
- **`gui_session.hpp`/`gui_session.cpp`**: what runs a `SIGHT_GUI_TEST_CASE`: the profile, the scenario thread, the
  teardown.
- **`gui_fixture.hpp`/`gui_fixture.cpp`**: a lighter fixture to test GUI **services** alone, without an application
  profile. Several tests can share one process.
- **`dialog/`**: test doubles of the Sight dialogs, for unit tests of services.

## Environment

| Variable | Effect |
|---|---|
| `SIGHT_UI_TEST_TIMEOUT_SCALE` | Multiplies every deadline, for a machine known to be slow. Anything but a positive number fails the test |
| `SIGHT_UI_TEST_VISIBLE=1` | Runs a GUI test on the display of the user, to watch it; empty or `0` does not |
| `SIGHT_UI_TEST_UPDATE_REFERENCES=1` | Each `expect_image` writes a candidate, and the scenario fails at its end; it never replaces a reference |
| `SIGHT_TEST_LOG_KEEP=1` | Needed by several `ctest` runs at once in one build tree |
| `SIGHT_TEST_DUMP_DIR` | Where a test writes the dump of a crash |
| `IMAGE_OUTPUT_PATH` | Where failure artifacts and image candidates go, by binary, scenario and run. By default `sight_ui_test` in the temporary directory of the system, kept after the run |

## Engine tests

- `ui_test_ut`: the parts of the engine that need no real window, on the offscreen platform.
- `ui_test_uit` (`test/uit`), tests named `native::*`: the scenario, its input and dialogs, window stacking and
  OpenGL captures, on the real platform of each system, where a window has a real stacking, focus and activation.
- `ui_test_app_uit` (`test/ui_test_app`): the scenario framework itself, `SIGHT_GUI_TEST_CASE`, its fixture and its
  reports, on `ui_test_app`, a small XML application with a scene and the same panel in two views.

On Windows, a plain unit test runs offscreen and never shows a window. One that needs a real window is
`TYPE GUI_TEST PROFILE NONE`, on a desktop of its own, or it takes the foreground of the applications of the user. A CRT
assertion or a `qFatal` ends the test with its message on stderr: nothing waits for a click.

## Naming diagnostics

Each sanctioned naming path records the name it gives (`ui/qt/naming.hpp`): XML names, the roles of the namer, parts
given by `ui::qt::name_part()`, and the parts of shared widgets, bound to their owner's name. A name set any other way,
or changed afterwards, has no matching record. `scenario::naming_violations()` lists, on the GUI thread, each visible
object without a valid name, with its reason (`no name`, `explicit name`) and where it is; `expect_valid_names()` fails
with that list. `dump()` marks the same objects.

## Running the tests

Each scenario is its own CTest entry (`ctest -L gui`), and each entry runs the binary in its own
process. A binary invoked directly without a `--test-case` filter (e.g. `./sight_viewer_uit`)
re-executes itself once per matching scenario and aggregates the results: a scenario owns a
QApplication and a Sight profile lifecycle, and two of those in a single process crash.

## Known limits

- A handler running a nested modal loop returns only once its dialog closes: such a target is never double-clicked.
- A `QDrag` source runs the platform drag loop, which injected input cannot drive.
- A role ordinal (`#2`) follows `children()` order, which `raise()` changes.
- A dialog is named after a service only when opened during its start, update or stop; otherwise it takes the name of a
  parent service, or stays unnamed, which `ui.dump()` marks.
- Custom, text and value matchers of `expect` take widgets only; `enabled()` and `disabled()` also take actions.
- The step log records neither a window activation nor the item path of `select(item(...))`.
- A dialog a service opens from its own worker after the last step outlives the scenario: the process ends after 120 s.
- Every desktop of a Windows session shares the clipboard: a test that copies and pastes is not isolated.
- A locked Windows session slows the creation of windows: keep `WINDOWS_PARALLEL_GUI_TESTS` at 20 or less.
