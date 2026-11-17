# sight::ui::test

Library used to write GUI (`uit`) tests: doctest-based tests that drive a whole Sight application
through its Qt widgets, one scenario per process.

## Files

- **`test.hpp`/`test.cpp`** (`sight::ui::test::base`): base class for a GUI test fixture. Loads and
  starts the application profile given at construction, and exposes `start()` to run a scenario
  against it. `start()` returns the failure message as a `std::string` (empty on success) rather
  than asserting itself - the caller (a `TEST_CASE_FIXTURE` body) must do the doctest assertion,
  since this library must never include `<doctest/doctest.h>` (see the class-level comment on
  `base` in `test.hpp` for why).
- **`tester.hpp`/`tester.cpp`** (`sight::ui::test::tester`): the actual driver. Finds widgets,
  sends synthetic Qt events, waits for asynchronous UI state, takes screenshots, and compares
  images. `tester::fail()` is how a scenario reports a failure - see below.
- **`interaction.hpp`/`interaction.cpp`**: the concrete interactions (`mouse_click`, `mouse_drag`,
  `keyboard_sequence`, ...) that `tester::interact()` accepts.
- **`gui_fixture.hpp`/`gui_fixture.cpp`** (`sight::ui::test::gui_fixture`): a lighter doctest
  fixture for testing individual GUI **services** in isolation, without a full application
  profile. Several `TEST_CASE_FIXTURE`s can share one process, unlike `base`-derived tests.
- **`helper/`**: higher-level helpers built on `tester` for common widgets (buttons, combo boxes,
  sliders, file dialogs, ...).

## Writing a GUI test

Each application under test needs one fixture deriving from `base`, giving it the path to that
application's `profile.xml`:

```cpp
struct fixture : sight::ui::test::base
{
    fixture() :
        sight::ui::test::base(sight::core::runtime::working_path() / "share/sight/my_app/profile.xml")
    {
    }
};
```

Each scenario is a `TEST_CASE_FIXTURE` in a suite named after the application:

```cpp
TEST_SUITE("my_app")
{

TEST_CASE_FIXTURE(fixture, "my_scenario")
{
    const std::string failure_message = start(
        "my_scenario",
        [](sight::ui::test::tester& _tester)
        {
            // drive the UI through _tester and the helper/ functions
        }
    );

    INFO(failure_message);
    REQUIRE(failure_message.empty());
}

} // TEST_SUITE
```

## The one rule that matters

The scenario lambda passed to `start()` runs on a **secondary thread**, while the Qt event loop
runs on the main thread. **Never call a throwing doctest assertion** (`REQUIRE`, `FAIL`, ...) from
that thread, or from any helper it calls: doctest does not catch it there, and the process
terminates instead of the test failing cleanly. Report failures from the scenario through
`tester::fail()` instead, which throws an exception that `tester::start()` catches on that same
thread, taking a failure screenshot and composing a `GIVEN/WHEN/THEN` message. The single
`INFO(...); REQUIRE(...);` shown above, after `start()` has returned, is the only place a doctest
assertion belongs.

## Recording a scenario

`sight::module::ui::test::macro_saver` (`module/ui/test/macro_saver.hpp`) can record Qt
interactions on a running application and generate a skeleton `TEST_CASE_FIXTURE` from them - a
starting point to hand-tune into a real scenario, not a finished test.

## Running the tests

Each scenario is its own CTest entry (`ctest -L gui`), and each entry runs the binary in its own
process. A binary invoked directly without a `--test-case` filter (e.g. `./sight_viewer_uit`)
re-executes itself once per matching scenario and aggregates the results: a scenario owns a
QApplication and a Sight profile lifecycle, and two of those in a single process crash.
