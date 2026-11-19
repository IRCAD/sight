/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include <sight/ui/test/config.hpp>

#include <data/mt/locked_ptr.hpp>
#include <data/object.hpp>

#include <ui/test/expectation.hpp>
#include <ui/test/image.hpp>
#include <ui/test/locator.hpp>

#include <QApplication>
#include <QColor>
#include <QImage>
#include <QMetaObject>
#include <QPointer>
#include <QWidget>

#include <chrono>
#include <concepts>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace sight::ui::test
{

namespace detail
{

// Defined with the scenario: a signal_watch only holds it.
struct signal_watch_state;

class locator_catalog;

} // namespace detail

/// Names a service of the application for watch_signal(): it gives no access to the service.
class service_locator
{
public:

    //------------------------------------------------------------------------------

    /// The locator expression of the service.
    [[nodiscard]] const std::string& expression() const noexcept
    {
        return m_expression;
    }

private:

    friend class scenario;

    explicit service_locator(std::string _expression) :
        m_expression(std::move(_expression))
    {
    }

    std::string m_expression;
};

/**
 * @brief A one-shot observation of a signal, armed before the action that emits it.
 *
 * An emission before wait() counts, and one emission satisfies one wait. A timeout leaves it armed.
 *
 * @code{.cpp}
 * auto watch = ui.watch_signal("annotation_srv/button", &QPushButton::clicked);
 * ui.click("annotation_srv/button");
 * watch.wait();
 * @endcode
 */
class SIGHT_UI_TEST_CLASS_API signal_watch
{
public:

    signal_watch(const signal_watch&)                = delete;
    signal_watch& operator=(const signal_watch&)     = delete;
    signal_watch(signal_watch&&) noexcept            = default; ///< Moves the armed watch.
    signal_watch& operator=(signal_watch&&) noexcept = default; ///< Moves the armed watch.
    ~signal_watch()                                  = default;

    /**
     * @brief Waits for the emission.
     *
     * Fails on timeout, when the target is destroyed first or when the event loop stops.
     *
     * @param _timeout how long to wait for the emission.
     */
    SIGHT_UI_TEST_API void wait(duration_t _timeout = DEFAULT_TIMEOUT);

private:

    friend class scenario;

    explicit signal_watch(std::shared_ptr<detail::signal_watch_state> _state) :
        m_state(std::move(_state))
    {
    }

    std::shared_ptr<detail::signal_watch_state> m_state;
};

/// What a getter returns from a widget of type Widget.
template<typename Getter, typename Widget>
using widget_value_t = std::remove_cvref_t<std::invoke_result_t<std::decay_t<Getter>&, const Widget&> >;

/* *INDENT-OFF* */
/**
 * @brief Reads a copy from a Widget on the GUI thread.
 *
 * The copy refers to nothing in the GUI, so no pointer, and no QObject, which cannot be copied.
 */
template<typename Getter, typename Widget>
concept widget_getter = std::derived_from<Widget, QWidget>
                        && std::invocable<std::decay_t<Getter>&, const Widget&>
                        && std::copy_constructible<widget_value_t<Getter, Widget> >
                        && !std::is_pointer_v<widget_value_t<Getter, Widget> >;
/* *INDENT-ON* */

/// A point expressed as fractions of the target's rectangle, from its top-left (0, 0) to its bottom-right (1, 1).
struct relative_position
{
    double x {0.5}; ///< From 0 at the left edge to 1 at the right edge.
    double y {0.5}; ///< From 0 at the top edge to 1 at the bottom edge.
};

/// A click position in the target's own pixels. No default: `{}` is the centre, a relative_position.
struct pixel_position
{
    /// The point, from the top-left corner.
    constexpr pixel_position(int _x, int _y) noexcept :
        x(_x),
        y(_y)
    {
    }

    int x; ///< Pixels from the left edge.
    int y; ///< Pixels from the top edge.
};

//------------------------------------------------------------------------------

/// Asks set() to move a slider with a real drag of its handle instead of its Qt API.
struct by_drag_t
{
    explicit by_drag_t() = default;
};

/// The tag of set(loc, 35, by_drag). Lower case like the standard tags (std::in_place): it reads as part of the call.
inline constexpr by_drag_t by_drag {}; // NOLINT(readability-identifier-naming)

//------------------------------------------------------------------------------

/**
 * @brief A point of the target in fractions of its rectangle.
 *
 * @code{.cpp}
 * ui.click("main_scene_view >> scene_srv/view", at(0.25, 0.5));
 * ui.drag("main_scene_view >> scene_srv/view", at(0.2, 0.5), at(0.8, 0.5));
 * @endcode
 *
 * @param _x from 0 at the left edge to 1 at the right edge.
 * @param _y from 0 at the top edge to 1 at the bottom edge.
 * @return the position.
 */
[[nodiscard]] inline relative_position at(double _x, double _y)
{
    return {.x = _x, .y = _y};
}

//------------------------------------------------------------------------------

/**
 * @brief A point of the target in its own pixels.
 *
 * @code{.cpp}
 * ui.click("main_scene_view >> scene_srv/view", px(10, 20));
 * @endcode
 *
 * @param _x pixels from the left edge.
 * @param _y pixels from the top edge.
 * @return the position.
 */
[[nodiscard]] inline pixel_position px(int _x, int _y)
{
    return {_x, _y};
}

/**
 * @brief Drives a running application from a thread of its own, as a user does.
 *
 * Each verb waits, up to its deadline of 30 s, for one stable, visible target; a target is located by the object names
 * the application gives from its XML. A SIGHT_GUI_TEST_CASE receives it as `ui`. Once a user could reach the target, a
 * Qt widget is acted on through its Qt API or gets its events handed to it directly, so that Qt emits what a user's
 * action emits whatever the focus or a popup's timing; render windows get real input. Data and signals are for
 * observation, not for changing state or calling service slots.
 *
 * From sight_viewer/test/uit/axial_negato.cpp, after loading an image and resetting the negatos. Its negato() helper
 * builds the scoped names of the slider and label:
 * @code{.cpp}
 * const auto label = negato("axial_scene_view", "label");
 * ui.set(negato("axial_scene_view"), 133);
 * ui.expect(label).text("133 / 133");
 * @endcode
 *
 * Prefer an observable condition to a fixed wait. For a full scenario, see app/sight_viewer/test/uit and the GUI
 * test skill in .agents/skills/sight-gui-test/SKILL.md.
 */
class SIGHT_UI_TEST_CLASS_API scenario
{
public:

    /**
     * @brief Drives the windows of an application from the scenario thread.
     *
     * SIGHT_GUI_TEST_CASE builds it: a test rarely does.
     *
     * @param _application            the running application.
     * @param _image_output_directory where image failures and candidates go.
     */
    SIGHT_UI_TEST_API explicit scenario(
        QApplication& _application,
        std::filesystem::path _image_output_directory = {});

    /**
     * @brief Same, and a locator naming nothing the catalog knows fails at once, with the closest known names.
     *
     * @param _application            the running application.
     * @param _catalog                the names declared in the XML of the application.
     * @param _image_output_directory where image failures and candidates go.
     */
    SIGHT_UI_TEST_API scenario(
        QApplication& _application,
        const detail::locator_catalog& _catalog,
        std::filesystem::path _image_output_directory = {});
    SIGHT_UI_TEST_API ~scenario();

    scenario(const scenario&)            = delete;
    scenario(scenario&&)                 = delete;
    scenario& operator=(const scenario&) = delete;
    scenario& operator=(scenario&&)      = delete;

    /**
     * @brief Clicks a point of one stable, visible widget or action, its centre by default, retrying until it is
     * reachable; a point that cannot be reached is never traded for another one.
     *
     * A button is clicked through QAbstractButton::click(), which emits what a mouse click emits. An action is
     * triggered once what shows it is reachable: its toolbar button is clicked, an open menu activates it as with
     * Return, and an action of a closed menu is triggered without opening the menu. Other widgets get a mouse click.
     *
     * @code{.cpp}
     * ui.click("top_toolbar_left/volume");
     * ui.click("main_scene_view >> scene_srv/view", at(0.25, 0.5));
     * ui.click("update_dialog", {}, 2s);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _position   the point, in fractions of the target's rectangle.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void click(
        std::string_view _expression,
        relative_position _position = {},
        duration_t _timeout         = DEFAULT_TIMEOUT
    );

    /**
     * @brief Clicks one fixed point of the target, in its own pixels; a point that cannot be reached fails.
     *
     * @code{.cpp}
     * ui.click("main_scene_view >> scene_srv/view", px(10, 20));
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _position   the point, in pixels of the target.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void click(
        std::string_view _expression,
        pixel_position _position,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Double-clicks the centre of one stable, visible widget.
     *
     * If the first click opens a modal dialog, the second one is only sent once the dialog closes: such a target
     * cannot be double-clicked.
     *
     * @code{.cpp}
     * ui.double_click("main_scene_view >> scene_srv/view");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void double_click(std::string_view _expression, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Right-clicks the centre of one stable, visible widget.
     *
     * A context menu it opens is then a target: `ui.click(menu_item(...))`.
     *
     * @code{.cpp}
     * ui.right_click("series_list_srv");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void right_click(std::string_view _expression, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Drags with the left button from a point inside the target; the destination may lie outside it.
     *
     * This does not drive Qt's platform QDrag loop.
     *
     * @code{.cpp}
     * ui.drag("main_scene_view >> scene_srv/view", at(0.2, 0.5), at(0.8, 0.5));
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _from       where the button is pressed, inside the target.
     * @param _to         where it is released.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void drag(
        std::string_view _expression,
        relative_position _from,
        relative_position _to,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Scrolls the target widget by wheel notches, at its centre.
     *
     * Sends no keyboard modifier. The effect depends on the target's interactor; for example, a negato may change
     * slices rather than zoom.
     *
     * From sight_viewer/test/uit/zoom_out.cpp, where MAIN_SCENE names the render window:
     * @code{.cpp}
     * ui.expect_image_changes(MAIN_SCENE, [&]{ui.wheel(MAIN_SCENE, -7);});
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _notches    from -255 to 255, not 0; positive turns the wheel away from the user.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void wheel(std::string_view _expression, int _notches, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Brings a widget scrolled out of view into view as a user does.
     *
     * One wheel notch at a time on the vertical scroll bar of its scroll area, towards it, until it is visible. An
     * action is reached through its toolbar button. Fails when it is hidden or the bar cannot move on.
     *
     * @code{.cpp}
     * ui.scroll_to("settings_srv/threshold");
     * ui.set("settings_srv/threshold", 40);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void scroll_to(std::string_view _expression, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Pinches the target widget by a scale factor, two fingers about its centre.
     *
     * A gesture the platform dropped after delivery is not retried: only its effect tells, so the scenario checks it.
     *
     * From sight_viewer/test/uit/zoom_out_gesture.cpp, where MAIN_SCENE names the render window:
     * @code{.cpp}
     * ui.expect_image_changes(MAIN_SCENE, [&]{ui.pinch(MAIN_SCENE, 0.3);});
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _scale      the ratio of the final to the initial distance of the fingers, in ]0, 10]: below 1 brings them
     *                    together.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void pinch(std::string_view _expression, double _scale, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Types UTF-8 text into the target widget without committing it.
     *
     * The keys go to the target itself, whatever has the focus, and a completer's popup does not take them. To replace
     * and commit the text of a line edit, use set().
     *
     * @code{.cpp}
     * ui.type("settings_srv/name", "Liver");
     * ui.press("settings_srv/name", "Return");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _text       the text, typed key by key.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void type(
        std::string_view _expression,
        std::string_view _text,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Presses a key sequence on the target widget.
     *
     * The keys are released as part of the call; this does not hold a modifier for a later mouse action.
     *
     * @code{.cpp}
     * ui.press("main_scene_view >> scene_srv/view", "Ctrl+Z");
     * @endcode
     *
     * @param _expression   the locator of the target: a name, a scoped string or a locator_expression.
     * @param _key_sequence a Qt portable-text key sequence, such as "Ctrl+S" or "Escape".
     * @param _timeout      how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void press(
        std::string_view _expression,
        std::string_view _key_sequence,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Sets a checkable button or action to the requested state, with a click only when needed.
     *
     * The click goes through the Qt API, as for click().
     *
     * @code{.cpp}
     * ui.set("parameter_toolbar_view/Annotation", true);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _checked    the state to reach.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(std::string_view _expression, bool _checked, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Sets a slider through QSlider::setValue(), or types into a spin box.
     *
     * A slider emits valueChanged() only, as at the end of a user's keys: pass by_drag for a service that waits for
     * the release of the handle. The keys of a spin box go to it directly, ending with Return.
     *
     * From sight_viewer/test/uit/axial_negato.cpp, after loading an image and resetting the negatos:
     * @code{.cpp}
     * const auto label = negato("axial_scene_view", "label");
     * ui.set(negato("axial_scene_view"), 133);
     * ui.expect(label).text("133 / 133");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _value      the value to reach.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(std::string_view _expression, int _value, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Sets a slider with a real drag of its handle.
     *
     * @code{.cpp}
     * ui.set("annotation_srv/slider", 35, by_drag);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _value      the value to reach.
     * @param _by_drag    by_drag.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(
        std::string_view _expression,
        int _value,
        by_drag_t _by_drag,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Sets the value of a double spin box by typing and committing it, the keys going to it directly.
     *
     * @code{.cpp}
     * ui.set("settings_srv/opacity", 0.5);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _value      the value to reach.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(std::string_view _expression, double _value, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Replaces and commits the text of a line edit.
     *
     * The keys go to the line edit itself; a text its validator does not accept at Return fails. On a combo box,
     * picks the item with this text, as select() does.
     *
     * @code{.cpp}
     * const std::string name = "Liver";
     * ui.set("settings_srv/name", name);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _value      the new text.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(
        std::string_view _expression,
        std::string_view _value,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Replaces and commits the text of a line edit: a string literal is text, not a bool.
     *
     * @code{.cpp}
     * ui.set("settings_srv/name", "Liver");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _value      the new text.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(std::string_view _expression, const char* _value, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Sets the color button embedded in a view item through its real color dialog.
     *
     * @code{.cpp}
     * ui.set(item("organ_list", "Liver").column(1), QColor(Qt::red));
     * @endcode
     *
     * @param _target  the item that holds the button.
     * @param _color   the color to choose.
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(
        const item_locator& _target,
        const QColor& _color,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Sets a color button through its real color dialog.
     *
     * @code{.cpp}
     * ui.set("settings_srv/color", QColor(255, 0, 0));
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _color      the color to choose.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void set(
        std::string_view _expression,
        const QColor& _color,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Expands the tree ancestors of an item, then selects it with a click.
     *
     * Each ancestor is expanded through QTreeView::expand(), once a user's click on its branch indicator would land;
     * the click on the item is handed to the view, which emits what a user's click emits.
     *
     * @code{.cpp}
     * ui.select(item("series_list_srv", "Patient/Study/CT"));
     * @endcode
     *
     * @param _target  the item to select.
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void select(const item_locator& _target, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Selects one combo box item by its displayed text.
     *
     * The item is chosen through the Qt API of the combo, as Qt does once a user picked it in the popup: the current
     * index changes, then activated() and textActivated() are emitted. Qt's popup is not opened, since a scenario
     * tests the application, not the timing of the popup. Selecting the current item does nothing; a disabled item
     * is waited for like a disabled widget.
     *
     * @code{.cpp}
     * ui.select("videoSelectorSrv/device_selector", "File...");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _text       the exact text of the item.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void select(
        std::string_view _expression,
        std::string_view _text,
        duration_t _timeout = DEFAULT_TIMEOUT
    );

    /**
     * @brief Chooses an absolute path in the active real file dialog.
     *
     * The action that opens the dialog comes first. Qt's dialog is then driven through its Qt API, since a scenario
     * tests the application, not Qt. A question about overwriting a file is answered Yes.
     *
     * From the open_image() and open_file() helpers in sight_viewer/test/uit/helper.hpp:
     * @code{.cpp}
     * ui.set("parameter_toolbar_view/Data", true);
     * ui.click("data_tools_row_1/Load Files");
     * ui.choose_file(utest_data::dir() / "sight/image/inr/image.inr.gz", "Inr (.inr) (*.inr *.inr.gz)");
     * ui.expect("top_toolbar_left/volume").within(std::chrono::seconds(180)).enabled();
     * @endcode
     *
     * @param _path    the absolute path of the file.
     * @param _filter  the displayed text of the file type to choose first, or empty to keep the current one.
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void choose_file(
        const std::filesystem::path& _path,
        std::string_view _filter = {},
        duration_t _timeout      = DEFAULT_TIMEOUT
    );

    /**
     * @brief Chooses several absolute paths, in one folder, in an ExistingFiles dialog.
     *
     * @code{.cpp}
     * ui.choose_files({folder / "a.png", folder / "b.png"});
     * @endcode
     *
     * @param _paths   the absolute paths of the files.
     * @param _filter  the displayed text of the file type to choose first, or empty to keep the current one.
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void choose_files(
        const std::vector<std::filesystem::path>& _paths,
        std::string_view _filter = {},
        duration_t _timeout      = DEFAULT_TIMEOUT
    );

    /**
     * @brief Chooses an absolute directory in the active real folder dialog.
     *
     * @code{.cpp}
     * ui.choose_folder(single_image.path());
     * @endcode
     *
     * @param _path    the absolute path of the folder.
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void choose_folder(const std::filesystem::path& _path, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Cancels the active real file or folder dialog with its Cancel button.
     *
     * @code{.cpp}
     * ui.cancel_file_dialog();
     * @endcode
     *
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void cancel_file_dialog(duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Sends a platform close request to the target's window, as its title bar button does.
     *
     * A confirmation dialog opened by the window stays pending for the next actions, like one opened by a click.
     *
     * @code{.cpp}
     * ui.close(dialog());
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void close(std::string_view _expression, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Starts an assertion on one widget, retried until its condition holds or the deadline expires.
     *
     * @code{.cpp}
     * ui.expect("distance_list_srv").rows(1);
     * ui.expect("progress_bar").within(180s).hidden();
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @return the expectation: the condition called on it waits.
     */
    [[nodiscard]] SIGHT_UI_TEST_API widget_expectation expect(std::string_view _expression) const;

    /**
     * @brief Captures two identical consecutive RGB frames from the scene's QOpenGLWidget, never the screen.
     *
     * Assert that the application is ready first: a stable frame alone can still be stale.
     *
     * @code{.cpp}
     * const QImage frame = ui.image("main_scene_view >> scene_srv/view");
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _timeout    how long to retry before the step fails.
     * @return the stable frame.
     */
    [[nodiscard]] SIGHT_UI_TEST_API QImage image(std::string_view _expression, duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief The folder of this execution, where image failures go and where a scenario may write its own files.
     *
     * @code{.cpp}
     * ui.choose_file(ui.output_directory() / "snapshot.png");
     * @endcode
     *
     * @return the folder, created if needed.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::filesystem::path output_directory();

    /**
     * @brief Retries stable captures until one matches a reference image within a threshold.
     *
     * Assert that the application is ready first: a stable frame alone can still be stale.
     * Update mode, SIGHT_UI_TEST_UPDATE_REFERENCES=1, saves a candidate only, and the scenario fails when it finishes.
     *
     * @code{.cpp}
     * ui.expect_image(
     *     "main_scene_view >> scene_srv/view",
     *     utest_data::dir() / "sight/ui/sight_viewer/load_itk.png"
     * );
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _reference  the PNG to match.
     * @param _threshold  how far a capture may differ.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void expect_image(
        std::string_view _expression,
        const std::filesystem::path& _reference,
        const image_threshold& _threshold = {},
        duration_t _timeout               = DEFAULT_TIMEOUT
    );

    /**
     * @brief Captures a baseline, invokes the action once on the scenario thread, then waits for a significant change.
     *
     * The timeout applies to the baseline and to the observation, not to the action, which has its own.
     * A change alone does not establish that the resulting state is correct; use expect_image() or a data assertion
     * when the result matters.
     *
     * @code{.cpp}
     * ui.expect_image_changes("main_scene_view >> scene_srv/view", [&]{ui.click("top_toolbar_view/reset");});
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _action     what changes the image.
     * @param _threshold  how far the image must change.
     * @param _timeout    how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void expect_image_changes(
        std::string_view _expression,
        const std::function<void()>& _action,
        const image_threshold& _threshold = {},
        duration_t _timeout               = DEFAULT_TIMEOUT
    );

    /**
     * @brief Retries a predicate without Qt inside, on the scenario thread, until it holds or the deadline expires.
     *
     * The predicate must return promptly: the deadline cannot interrupt it. For a widget, use expect(); for an object,
     * expect_data().
     *
     * @code{.cpp}
     * ui.eventually("the file exists", [&]{return std::filesystem::exists(path);});
     * @endcode
     *
     * @param _description what holds, for the failure message.
     * @param _predicate   the condition.
     * @param _timeout     how long to retry before the step fails.
     */
    template<typename Predicate>
    requires std::predicate<const Predicate&>
    void eventually(
        std::string_view _description,
        const Predicate& _predicate,
        duration_t _timeout = DEFAULT_TIMEOUT
    ) const
    {
        // It runs here before this returns: a reference to the predicate is enough.
        eventually_impl(
            _description,
            [&_predicate](std::chrono::steady_clock::time_point /*_deadline*/)
            {
                return static_cast<bool>(std::invoke(_predicate));
            },
            _timeout
        );
    }

    /**
     * @brief Waits until the application has written a complete PNG file, and returns its image.
     *
     * A writer may be asynchronous: the file is read only once its final IEND chunk exists.
     *
     * @code{.cpp}
     * ui.click("top_toolbar_view/Snapshot");
     * ui.choose_file(ui.output_directory() / "snapshot.png");
     * const QImage snapshot = ui.read_png(ui.output_directory() / "snapshot.png", 60s);
     * @endcode
     *
     * @param _path    the file the application writes.
     * @param _timeout how long to wait before the step fails.
     * @return the image of the file.
     */
    [[nodiscard]] SIGHT_UI_TEST_API QImage read_png(
        const std::filesystem::path& _path,
        duration_t _timeout = DEFAULT_TIMEOUT
    ) const;

    /**
     * @brief Waits for one visible widget, of type Widget, and returns what a short, read-only getter copies from it on
     * the GUI thread.
     *
     * @code{.cpp}
     * const QSize size = ui.read("main_scene_view >> scene_srv/view", &QWidget::size);
     * const int value  = ui.read<QSlider>("annotation_srv/slider", &QSlider::value);
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _getter     a member function or a callable taking the widget.
     * @param _timeout    how long to retry before the step fails.
     * @return the copy.
     */
    template<typename Widget = QWidget, typename Getter>
    requires widget_getter<Getter, Widget>
    [[nodiscard]] widget_value_t<Getter, Widget> read(
        std::string_view _expression,
        Getter&& _getter,
        duration_t _timeout = DEFAULT_TIMEOUT
    ) const
    {
        // A GUI call can still run after its deadline: the getter and the result live as long as the call.
        auto value  = std::make_shared<std::optional<widget_value_t<Getter, Widget> > >();
        auto getter = std::make_shared<std::decay_t<Getter> >(std::forward<Getter>(_getter));
        read_widget(
            _expression,
            [value, getter, expression = std::string(_expression)](const QWidget& _widget)
            {
                value->emplace(std::invoke(*getter, detail::as_widget<Widget>(_widget, "read", expression)));
            },
            _timeout
        );
        return std::move(**value);
    }

    /**
     * @brief Arms a one-shot watch on a signal of a visible widget, before the action that emits it.
     *
     * @code{.cpp}
     * auto watch = ui.watch_signal("annotation_srv/button", &QPushButton::clicked);
     * ui.click("annotation_srv/button");
     * watch.wait();
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _signal     the signal of the widget.
     * @param _timeout    how long to retry before the step fails.
     * @return the armed watch.
     */
    template<std::derived_from<QWidget> Widget, typename ... Args>
    [[nodiscard]] signal_watch watch_signal(
        std::string_view _expression,
        void (Widget::*_signal)(Args ...),
        duration_t _timeout = DEFAULT_TIMEOUT
    ) const
    {
        return arm_signal_watch(
            _expression,
            [_signal, expression = std::string(_expression)](const QWidget& _widget, std::function<void()> _notify)
            {
                return QObject::connect(
                    &detail::as_widget<Widget>(_widget, "watch_signal", expression),
                    _signal,
                    std::move(_notify)
                );
            },
            _timeout
        );
    }

    /**
     * @brief Names a service, scoped like a locator, for watch_signal(). An unknown name fails at once.
     *
     * @code{.cpp}
     * auto watch = ui.watch_signal(ui.service("reader"), "changed");
     * @endcode
     *
     * @param _expression the uid of the service, scoped like a locator.
     * @return the name of the service.
     */
    [[nodiscard]] SIGHT_UI_TEST_API service_locator service(std::string_view _expression) const;

    /**
     * @brief Arms a one-shot watch on a signal of a service, before the action that emits it. Its arguments are
     * ignored.
     *
     * @code{.cpp}
     * auto watch = ui.watch_signal(ui.service("reader"), "changed");
     * ui.click("toolbar/load");
     * watch.wait();
     * @endcode
     *
     * @param _service the service, from service().
     * @param _signal  the key of the signal.
     * @param _timeout how long to retry before the step fails.
     * @return the armed watch.
     */
    [[nodiscard]] SIGHT_UI_TEST_API signal_watch watch_signal(
        const service_locator& _service,
        std::string_view _signal,
        duration_t _timeout = DEFAULT_TIMEOUT
    ) const;

    /**
     * @brief Waits for an object of the application, scoped like a locator.
     *
     * `axial_scene_view >> image` is the object of the configuration launched in that view. Observation only: lock it
     * as usual, and never modify it.
     *
     * @code{.cpp}
     * const auto camera = ui.data<data::matrix4>("camera");
     * const data::mt::locked_ptr lock(camera);
     * @endcode
     *
     * @param _expression the uid of the object, scoped like a locator.
     * @param _timeout    how long to retry before the step fails.
     * @return the object; another type than Data fails.
     */
    template<std::derived_from<data::object> Data = data::object>
    [[nodiscard]] Data::csptr data(std::string_view _expression, duration_t _timeout = DEFAULT_TIMEOUT) const
    {
        auto object = std::dynamic_pointer_cast<const Data>(wait_for_object(_expression, _timeout));
        if(object == nullptr)
        {
            throw std::invalid_argument(
                      "object '" + std::string(_expression) + "' is not a " + Data::classname()
            );
        }

        return object;
    }

    /**
     * @brief Starts an assertion on an object of the application, retried under its lock until it holds or the deadline
     * expires.
     *
     * @code{.cpp}
     * ui.expect_data<data::integer>("count").that("is 2", [](const auto& _i){return _i.value() == 2;});
     * @endcode
     *
     * @param _expression the uid of the object, scoped like a locator.
     * @return the expectation: that() waits.
     */
    template<std::derived_from<data::object> Data>
    [[nodiscard]] data_expectation<Data> expect_data(std::string_view _expression) const
    {
        return data_expectation<Data>(*this, _expression);
    }

    /**
     * @brief Tells whether the target appears within this time, without failing: for what the application shows only
     * sometimes.
     *
     * How long to wait is the question, so it has no default. A GUI too busy to answer extends the wait: "no" is an
     * answer of the GUI, and a target found late is there, so the scenario must handle it.
     *
     * @code{.cpp}
     * if(ui.exists("update_dialog", 2s))
     * {
     *     ui.close("update_dialog");
     * }
     * @endcode
     *
     * @param _expression the locator of the target: a name, a scoped string or a locator_expression.
     * @param _timeout    how long the target may take to appear.
     * @return whether it appeared.
     */
    [[nodiscard]] SIGHT_UI_TEST_API bool exists(std::string_view _expression, duration_t _timeout) const;

    /**
     * @brief Waits a fixed time, when time itself is the subject.
     *
     * Anything else waits on a condition. Not scaled, since the time is what is being tested, and it requires a reason.
     *
     * @code{.cpp}
     * ui.click("videoToolbarView/Pause");
     * ui.hold(1s, "a paused video must not move");
     * @endcode
     *
     * @param _duration the time to wait.
     * @param _why      what the wait tests, never empty.
     */
    SIGHT_UI_TEST_API void hold(duration_t _duration, std::string_view _why);

    /**
     * @brief Waits for mouse handlers that opened a modal dialog or popup to return.
     *
     * In update mode, it then fails with the candidates expect_image() saved. gui_session calls it after the body.
     *
     * @param _timeout how long to retry before the step fails.
     */
    SIGHT_UI_TEST_API void finish(duration_t _timeout = DEFAULT_TIMEOUT);

    /**
     * @brief Prints and returns the visible windows reduced to their named, textual or interactive objects, each with a
     * locator that resolves to it.
     *
     * A widget breaking the naming contract or a weak ordinal is marked. Call it to find the name of a target.
     *
     * @code{.cpp}
     * ui.click("top_toolbar_view/settings");
     * ui.dump();
     * @endcode
     *
     * @return the tree, as printed.
     */
    SIGHT_UI_TEST_API std::string dump();

    /**
     * @brief Returns one diagnostic for each visible object that breaks the naming contract: no name, or a name no
     * sanctioned path gave it.
     *
     * Qt's file, color and font dialogs are out of the contract.
     *
     * @code{.cpp}
     * CHECK(ui.naming_violations().empty());
     * @endcode
     *
     * @return the diagnostics, empty when every name is sanctioned.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::vector<std::string> naming_violations();

    /**
     * @brief Fails with the violations when a visible object breaks the naming contract.
     *
     * Call it again in each state that shows other widgets: a panel opened, a dialog shown.
     *
     * @code{.cpp}
     * ui.expect_valid_names();
     * ui.click("top_toolbar_view/settings");
     * ui.expect_valid_names();
     * @endcode
     */
    SIGHT_UI_TEST_API void expect_valid_names();

    /**
     * @brief One line per step so far, what kept it waiting included; also written to the test log.
     *
     * @return the lines, oldest first.
     */
    [[nodiscard]] SIGHT_UI_TEST_API const std::vector<std::string>& step_log() const noexcept;

    /**
     * @brief Writes failure.png, ui_tree.txt and steps.txt into a directory.
     *
     * Called by gui_session on the scenario thread, after a failure.
     *
     * @param _directory where the files go.
     * @param _what      the failure.
     * @return what to report: the step, the failure and where these files are.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::string report_failure(
        const std::filesystem::path& _directory,
        std::string_view _what
    ) const;

private:

    friend class widget_expectation;
    friend class gui_session;
    template<std::derived_from<data::object> Data>
    friend class data_expectation;

    /**
     * @brief The object now, nullptr when it does not exist yet.
     *
     * Several objects, or an unknown uid, fail at once; a GUI too busy to answer before the deadline fails with a
     * timeout.
     *
     * @param _expression the uid of the object, scoped like a locator.
     * @param _deadline   when to stop waiting for the GUI.
     * @return the object, or nullptr.
     */
    [[nodiscard]] SIGHT_UI_TEST_API data::object::csptr try_object(
        std::string_view _expression,
        std::chrono::steady_clock::time_point _deadline
    ) const;

    [[nodiscard]] SIGHT_UI_TEST_API data::object::csptr wait_for_object(
        std::string_view _expression,
        duration_t _timeout
    ) const;

    SIGHT_UI_TEST_API void prepare_window(QPointer<QWidget> _window);

    /// Connects the watched signal of a widget, on the GUI thread, to a notification.
    using signal_connector = std::function<QMetaObject::Connection(const QWidget&, std::function<void()>)>;

    /// Arms a signal_watch on the unique visible widget of _expression, through _connect.
    [[nodiscard]] SIGHT_UI_TEST_API signal_watch arm_signal_watch(
        std::string_view _expression,
        signal_connector _connect,
        duration_t _timeout
    ) const;

    /// Runs _reader once on the unique visible widget of _expression.
    SIGHT_UI_TEST_API void read_widget(
        std::string_view _expression,
        std::function<void(const QWidget&)> _reader,
        duration_t _timeout
    ) const;

    /**
     * @brief Retries a probe on the unique visible widget of a locator until it holds.
     *
     * @param _expression       the locator.
     * @param _description      what holds, for the failure message.
     * @param _probe            whether it holds, and the observed value for the failure message, or nothing.
     * @param _timeout          how long to retry.
     * @param _query            what the expectation waits for, besides the probe.
     * @param _never_existed_ok whether gone() passes for a locator that never matched.
     */
    SIGHT_UI_TEST_API void expect_widget(
        std::string_view _expression,
        std::string_view _description,
        std::function<std::pair<bool, std::string>(const QWidget&)> _probe,
        duration_t _timeout,
        detail::widget_query _query = detail::widget_query::probe,
        bool _never_existed_ok      = false
    ) const;

    /// Retries _predicate, given the deadline of the whole wait for its own GUI calls.
    SIGHT_UI_TEST_API void eventually_impl(
        std::string_view _description,
        std::function<bool(std::chrono::steady_clock::time_point)> _predicate,
        duration_t _timeout
    ) const;

    struct implementation;
    std::unique_ptr<implementation> m_implementation;
};

// Defined once scenario is complete, which these members call.

//------------------------------------------------------------------------------

template<std::derived_from<data::object> Data>
data_expectation<Data>::data_expectation(const scenario& _scenario, std::string_view _expression) :
    m_scenario(&_scenario),
    m_expression(_expression)
{
}

//------------------------------------------------------------------------------

template<std::derived_from<data::object> Data>
data_expectation<Data>& data_expectation<Data>::within(duration_t _timeout)
{
    m_timeout = _timeout;
    return *this;
}

//------------------------------------------------------------------------------

template<std::derived_from<data::object> Data>
template<std::predicate<const Data&> Predicate>
void data_expectation<Data>::that(std::string_view _description, const Predicate& _predicate) const
{
    m_scenario->eventually_impl(
        _description,
        [this, &_predicate](std::chrono::steady_clock::time_point _deadline)
        {
            const data::object::csptr found = m_scenario->try_object(m_expression, _deadline);
            if(found == nullptr)
            {
                return false;
            }

            // As data<Data>(): another type is a mistake of the test, not an object still to come.
            const auto object = std::dynamic_pointer_cast<const Data>(found);
            if(object == nullptr)
            {
                throw std::invalid_argument("object '" + m_expression + "' is not a " + Data::classname());
            }

            const data::mt::locked_ptr lock(object);
            return static_cast<bool>(std::invoke(_predicate, *lock));
        },
        m_timeout
    );
}

} // namespace sight::ui::test
