---
name: sight-gui-test
description: Write or fix GUI functional tests of Sight applications (`test/uit`, `SIGHT_GUI_TEST_CASE`) from the application XML. Use for a new scenario, or a failing or flaky GUI test.
metadata:
  short-description: Sight GUI functional tests
---

# Sight GUI functional tests

A scenario drives a real application as a user does, and is written from its XML. Read the XML first, then
`lib/ui/test/scenario.hpp`, `expectation.hpp` and `locator.hpp`: they document every verb, with an example. An example
elsewhere is not a reference. `app/sight_viewer/test/uit/` shows complete scenarios.

## The target

```cmake
sight_add_target(my_app_uit TYPE GUI_TEST)  # PROFILE defaults to the target name without _uit, TIMEOUT to 300 s
target_link_libraries(my_app_uit PRIVATE ui_test my_app)
```

```cpp
#include <ui/test/gui_test.hpp>

SIGHT_GUI_TEST_CASE("my_scenario")
{
    ui.click("top_toolbar_left/volume");
    ui.expect("top_toolbar_left/volume").checked();
}
```

- The name is `snake_case`, in CTest `<app>::my_scenario`. A new file needs CMake to run again: check with
  `ctest -N -R my_scenario`, since "no test found" is not a pass.
- `ui` is a `sight::ui::test::scenario`; the builders (`at`, `loc`, `item`...) are in that namespace. Shared code goes
  in a header of the folder.
- Each scenario has its own process and display (Xvfb on Linux, a desktop on Windows), a client area of 1280x800 and a
  font DPI of 96. `SIGHT_UI_TEST_VISIBLE=1` shows it.
- A unit test of a library that needs a real display (the native platform of each system, or a real OpenGL window)
  goes in `test/uit`: a `GUI_TEST` target with a display of its own, while `test/ut` runs offscreen.

## Names

Take every name from the XML, never from a screenshot.

| Element | Name |
|---|---|
| Service with a widget | its `uid` |
| Sub-view | its `sid` or `wid` |
| Toolbar or menu item | `<toolbar uid>/<menu_item name>`; `menu_item()` escapes a `/` of the XML |
| Parameter of a `settings` service | `<uid>/<key>`, or `<uid>/<key>/<part>` |
| Section of a toolbox layout | `<sid or wid>/toggle` |
| Render window | `<render uid>/view` |
| Dialog of a service | `<uid>/dialog`, then `<uid>/dialog/<key>` or `<uid>/dialog/ok` |
| Other widget of a service | `<uid>/<role>`: `button`, `slider`, `label`... |
| Explicit part | `<uid>/<part>`, given in the service by `ui::qt::name_part(widget, "new_group")` |

`ui.dump()` prints a locator for every visible object and marks what breaks the naming contract. A name unknown to the
XML fails at once, with the closest names. Every scenario ends with `ui.expect_valid_names()`, called by the test
fixture. Qt's own dialogs are out of the contract. Add a part with `ui::qt::name_part()` only when a test needs a name
the role does not give.

## Exploring a running application

An MCP client can drive the application: see "Exploring with an AI agent" in `lib/ui/test/README.md`. When its `ui`
tool is available, try each step of a new scenario on the running application before writing it, so that every name
and verb is checked. The tool takes the same locators, but its verbs and states are a subset of the API, and a call
that acts then waits is several scenario lines: check each verb and assertion in the headers before writing it.

```json
{"locator": "properties1_srv/bool_1", "do": "click", "expect": {"checked": true}}
```

```cpp
ui.click("properties1_srv/bool_1");
ui.expect("properties1_srv/bool_1").checked();
```

## Locators

```cpp
"top_toolbar_left/volume"                         // exact, visible and unique
"axial_scene_view >> negato_slicer_srv"           // inside a scope: the same uid is in several views
"annotation_srv/slider#2"                         // the second of a role: weak, prefer an explicit part
menu_item("top_toolbar_left", "Show/hide mesh")   // escapes the XML components
name("properties_srv", "nonlin", "slider")        // the same for any name
loc("view") >> "service/label"                    // a builder, same as a string
dialog() >> "fileTypeCombo"                       // inside the innermost modal dialog
dialog().type<QPushButton>().text("&Yes")         // Qt's own dialogs, which have no names
loc("label").hidden()                             // not visible: for read and expect only
item("annotation_srv/tree", "Group_0/Point_0")    // an item, by its displayed texts
item_at("captures_srv/list", "0")                 // by rows, when the order is the subject
item("tree", "Group_0").column(1)                 // a cell, or the widget in it
```

Locators are strict: `type<>()` and `text()` are for what is outside the naming contract, never a way around it.

## Acting

- An action first waits until a user could reach the target (shown, enabled, not covered, not behind a modal dialog),
  then fails naming what is in the way.
- Qt widgets are acted on through their Qt API or get their events directly, so Qt emits what a user's action emits:
  focus, active window and popup timing do not matter. Render windows, `drag`, `wheel`, `pinch`, `press` and
  `right_click` are real input.
- What reached its target is never sent again.
- Verbs: `click` (`at(x, y)` in fractions, `px(x, y)`), `double_click`, `right_click`, `drag`, `wheel`, `pinch`,
  `scroll_to`, `type`, `press("Ctrl+S")`, `set(loc, value)`, `select`, `close`, `choose_file(path, filter)`,
  `choose_files`, `choose_folder`, `cancel_file_dialog`. A filter is the exact text the dialog shows, `name (*.ext)`.
- A 3D scene has no locators: act on its render window at fractions of its size. Check the effect on the data
  (`expect_data`) or the image (`expect_image_changes`): the effect of `wheel` depends on the interactor.
- A click that opens a modal dialog does not wait for it: go on with the dialog, and end with `ui.finish()` if the
  scenario ends there.
- `set` checks its effect; a refused value fails. `by_drag` asks for a real drag of a slider, for a service that waits
  for the release.
- Never write data, call a slot or reach a service object: the test would no longer prove what a user can do.

## Checking

```cpp
ui.expect(loc).text("133 / 133");                    // retried until the deadline, then fails with what it saw
ui.expect(loc).within(180s).enabled();               // a long operation says so
ui.expect(loc).that<QSlider>("is at 35", [](const QSlider& _s){return _s.value() == 35;});
const auto size = ui.read(scene, &QWidget::size);    // a copy, for REQUIRE
ui.eventually("the file exists", [&]{return std::filesystem::exists(path);});  // no Qt inside
```

Matchers: `text`, `text_contains`, `text_matches`, `number`, `value`, `checked`, `unchecked`, `enabled`, `disabled`,
`visible`, `hidden`, `gone`, `tooltip`, `rows`.

- UI state goes through `ui.expect`. A failed `REQUIRE` stops the scenario and leaves its artifacts; a failed `CHECK`
  goes on in a state nobody expected.
- Callbacks of `that` and `read` run on the GUI thread: short, read-only, capturing by value, no `ui.*`. A scenario
  never holds a Qt object.
- A deadline bounds a hang, it is not an assertion. The first look of a step waits at least 30 s for the GUI to answer,
  however short its deadline: a GUI slowed by load does not fail it, a GUI blocked longer does. The deadline cannot
  interrupt a predicate or its data lock: keep them short.
- **No fixed wait**: wait on a condition (a label, an enabled button, data, a signal). A stable image proves nothing
  about asynchronous data. `ui.hold(1s, "why")` only when time is the subject; `ui.exists(loc, 2s)` only for what the
  application shows sometimes.

## Data and signals

Observation only:

```cpp
ui.expect_data<sight::data::image_series>("image").that("has one fiducial", has_one_fiducial);
const auto series = ui.data<sight::data::image_series>("axial_scene_view >> image");

auto watch = ui.watch_signal(ui.service("reader_srv"), "updated");  // armed before the action
ui.click("toolbar/run");
watch.wait();
```

An uid in several launched configurations needs the scope of its view. A watch counts one emission.
`ui.watch_signal(loc, &QAbstractButton::clicked)` watches a widget.

## Images

```cpp
ui.expect(ready_label).text("loaded");   // readiness first
ui.expect_image("main_scene_view >> scene_srv/view", utest_data::dir() / "sight/ui/my_app/my_state.png");
ui.expect_image_changes(scene, [&]{ui.wheel(scene, -7);});
```

- The scene's framebuffer is compared, never the screen, once two frames are identical. Default: a pixel differs above
  16 on a channel, 0.1% of pixels may differ; sizes must match. Override with `threshold(channel, ratio, "why")` only
  for a reason you can state, keep the ratio below 0.4%, and prove with the negative control that a wrong state fails.
- A reference holds on every GPU (Mesa on Linux CI, NVIDIA on Windows) and shows the scene alone, not a panel sized by
  fonts.
- `SIGHT_UI_TEST_UPDATE_REFERENCES=1` writes a `candidate.png` per `expect_image` in `ui.output_directory()`. **A human
  reviews it** before it is added to the test data at a new path; then move `TEST_DATA_REF` in `.gitlab-ci.yml`.

## When is it done

Passing once is not done; a missing platform or dataset is a blocker, never a pass.

1. **Negative control**: break the action alone (never a value the expectation also reads), read the failing step and
   its artifacts, restore. An effect smaller than the image threshold is checked on the data.
2. **Stress**, once a scenario is written or changed, before it is pushed (not at each edit): idle, then next to one
   CPU burner per core with two runs at once, on Windows and Linux:

   ```shell
   SIGHT_TEST_LOG_KEEP=1 ctest --preset <preset> -R "^<app>::<scenario>$" --repeat until-fail:3 --output-on-failure
   ```

   A duration above half the CTest timeout needs an explanation. A flaky test is a bug: find its cause, never raise a
   deadline. `SIGHT_UI_TEST_TIMEOUT_SCALE` is for a slow machine, never to hide a race.
3. **Build and lint** on Linux (clang and gcc) and Windows, as `.agents/skills/sight-service/SKILL.md` says.

## When it fails

A failure names the step and what kept it waiting, and leaves `failure.png`, `ui_tree.txt` and `steps.txt` (plus
actual, expected, diff and metrics for an image) in the folder it reports. Read them first.

Never guess. "Timed out: <message>" tells what the last look saw; if the step log stops, take the stack of the GUI
thread with a debugger. Without one, instrument; a cause read in logs alone is a guess. A failure under load only is a
race: reproduce it under the same load, prove the cause, and fix it with a test that fails without the fix.

Traps:

- On Linux CI the display is an Xvfb without window manager: windows have no frame and nothing activates them, so a
  scenario never relies on window positions or decorations. The offscreen platform draws widgets in another style: a
  size or a reference image taken there does not hold elsewhere.
- A target hidden for a user is shown as a user does (open its panel first). One a layout leaves 0 px wide is a bug of
  the layout.
- A checkable toolbar item is a `QAction`: `set` and `expect(...).checked()` treat it as a button.
- Qt's file and color dialogs are driven through their Qt API once the application opened them; the action that opens
  them stays a user's.
- Keys go to the field itself, whatever the focus. An action in a closed menu is triggered without opening it: open a
  menu filled in `aboutToShow` by a click on its title first.
