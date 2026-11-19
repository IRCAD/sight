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

#include <data/object.hpp>

#include <QWidget>

#include <chrono>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace sight::ui::test
{

// scenario.hpp includes this header: the class it returns these expectations from is only declared here.
class scenario;

/// The type of a timeout.
using duration_t = std::chrono::steady_clock::duration;

/// How long a step waits, unless it says otherwise.
inline constexpr duration_t DEFAULT_TIMEOUT {std::chrono::seconds(30)};

/// Tells whether a Widget is in the expected state, on the GUI thread.
/* *INDENT-OFF* */
template<typename Predicate, typename Widget>
concept widget_predicate = std::derived_from<Widget, QWidget>
                           && std::predicate<std::decay_t<Predicate>&, const Widget&>;
/* *INDENT-ON* */

namespace detail
{

/// What a widget expectation waits for, besides its probe.
enum class widget_query : std::uint8_t
{
    probe,      ///< The probe, on the one visible widget.
    is_visible, ///< visible().
    is_hidden,  ///< hidden().
    gone,       ///< gone(): no widget, hidden or not, matches.
    enabled,    ///< enabled().
    disabled,   ///< disabled().
    checked,    ///< checked().
    unchecked   ///< unchecked().
};

/// The widget as the Widget a getter or predicate takes; another type fails, since its getter would not apply.
template<std::derived_from<QWidget> Widget>
const Widget& as_widget(const QWidget& _widget, std::string_view _operation, std::string_view _expression)
{
    if(const auto* const typed = dynamic_cast<const Widget*>(&_widget); typed != nullptr)
    {
        return *typed;
    }

    // A Widget without Q_OBJECT has the class name of its base.
    const std::string requested = Widget::staticMetaObject.className();
    const std::string found     = _widget.metaObject()->className();
    throw std::invalid_argument(
              std::string(_operation) + " on '" + std::string(_expression) + "' found a " + found
              + (requested == found ? ", not the requested subclass" : ", not a " + requested)
    );
}

} // namespace detail

/// An assertion on an object of the application, retried on the scenario thread, under the lock of the object.
template<std::derived_from<data::object> Data>
/**
 * @brief An assertion on an object of the application, retried on the scenario thread, under the lock of the object.
 *
 * scenario::expect_data() starts it.
 *
 * @code{.cpp}
 * ui.expect_data<data::image_series>("image").that(
 *     "has one distance",
 *     [](const data::image_series& _series){return has_one_distance(_series);});
 * @endcode
 */
class data_expectation
{
public:

    /**
     * @brief Replaces the 30 s deadline, for an operation known to be long.
     *
     * @code{.cpp}
     * ui.expect_data<data::integer>("count").within(180s).that("is 2", is_2);
     * @endcode
     *
     * @param _timeout the new deadline.
     * @return this expectation.
     */
    [[nodiscard]] data_expectation& within(duration_t _timeout);

    /**
     * @brief Waits until the predicate holds on the object.
     *
     * The deadline cannot interrupt the data lock or the predicate, which must return promptly.
     *
     * @code{.cpp}
     * ui.expect_data<data::integer>("count").that("is 2", [](const auto& _i){return _i.value() == 2;});
     * @endcode
     *
     * @param _description what holds, for the failure message.
     * @param _predicate   the condition, given the locked object.
     */
    template<std::predicate<const Data&> Predicate>
    void that(std::string_view _description, const Predicate& _predicate) const;

private:

    friend class scenario;

    data_expectation(const scenario& _scenario, std::string_view _expression);

    const scenario* m_scenario;
    std::string m_expression;
    duration_t m_timeout {DEFAULT_TIMEOUT};
};

/**
 * @brief An assertion on one visible widget, retried on the GUI thread until it holds or its deadline passes.
 *
 * scenario::expect() starts it, and each condition waits.
 *
 * @code{.cpp}
 * ui.expect("videoSliderSrv/total_duration").text("00:00:05");
 * ui.expect("annotation_srv/slider").within(180s).that<QSlider>(
 *     "is at 35",
 *     [](const QSlider& _s){return _s.value() == 35;});
 * @endcode
 */
class SIGHT_UI_TEST_CLASS_API widget_expectation
{
public:

    /**
     * @brief Replaces the 30 s deadline, for an operation known to be long.
     *
     * @code{.cpp}
     * ui.expect("progress_bar").within(180s).hidden();
     * @endcode
     *
     * @param _timeout the new deadline.
     * @return this expectation.
     */
    [[nodiscard]] SIGHT_UI_TEST_API widget_expectation& within(duration_t _timeout);

    /**
     * @brief Waits until the predicate holds on the widget, as a Widget.
     *
     * @code{.cpp}
     * ui.expect("videoSliderSrv/current_position").that<QLabel>(
     *     "plays",
     *     [](const QLabel& _label){return _label.text() != "00:00:00";});
     * @endcode
     *
     * @param _description what holds, for the failure message.
     * @param _predicate   the condition, on the GUI thread.
     */
    template<typename Widget = QWidget, typename Predicate>
    requires widget_predicate<Predicate, Widget>
    void that(std::string_view _description, Predicate&& _predicate) const
    {
        auto predicate = std::make_shared<std::decay_t<Predicate> >(std::forward<Predicate>(_predicate));
        check(
            _description,
            [predicate, expression = m_expression](const QWidget& _widget)
            {
                const bool holds = std::invoke(*predicate, detail::as_widget<Widget>(_widget, "expect", expression));
                return std::pair<bool, std::string> {holds, {}};
            });
    }

    /**
     * @brief Expects exact, case-sensitive text from a text-capable widget.
     *
     * @code{.cpp}
     * ui.expect("videoSliderSrv/total_duration").text("00:00:05");
     * @endcode
     *
     * @param _expected the text.
     */
    SIGHT_UI_TEST_API void text(std::string_view _expected) const;

    /**
     * @brief Expects the widget's text to contain this case-sensitive substring.
     *
     * @code{.cpp}
     * ui.expect("status_label").text_contains("loaded");
     * @endcode
     *
     * @param _expected the substring.
     */
    SIGHT_UI_TEST_API void text_contains(std::string_view _expected) const;

    /**
     * @brief Expects the entire widget text to match a case-sensitive regular expression.
     *
     * @code{.cpp}
     * ui.expect("videoSliderSrv/current_position").text_matches("00:00:0[1-5]");
     * @endcode
     *
     * @param _pattern the regular expression.
     */
    SIGHT_UI_TEST_API void text_matches(std::string_view _pattern) const;

    /**
     * @brief Expects a numeric value within an absolute tolerance, from the value of a numeric widget or from its text.
     *
     * @code{.cpp}
     * ui.expect("distance_label").number(42.5, 0.1);
     * @endcode
     *
     * @param _expected  the value.
     * @param _tolerance the largest accepted difference.
     */
    SIGHT_UI_TEST_API void number(double _expected, double _tolerance) const;

    /**
     * @brief Expects a slider, spin box or combo box to have exactly this value; for a combo box, its current index.
     *
     * This overload keeps value(40) unambiguous.
     *
     * @code{.cpp}
     * ui.expect("annotation_srv/size").value(10);
     * @endcode
     *
     * @param _expected the value.
     */
    SIGHT_UI_TEST_API void value(int _expected) const;

    /**
     * @brief Expects a slider, spin box, double spin box or combo box to have exactly this value.
     *
     * @code{.cpp}
     * ui.expect("settings_srv/opacity").value(0.5);
     * @endcode
     *
     * @param _expected the value.
     */
    SIGHT_UI_TEST_API void value(double _expected) const;

    /// A checked state is checked() or unchecked(), never a value of 1 or 0.
    void value(bool) const = delete;

    /**
     * @brief Expects a checkable widget or action to be checked.
     *
     * @code{.cpp}
     * ui.expect("parameter_toolbar_view/Annotation").checked();
     * @endcode
     */
    SIGHT_UI_TEST_API void checked() const;

    /**
     * @brief Expects a checkable widget or action not to be checked.
     *
     * @code{.cpp}
     * ui.expect("parameter_toolbar_view/Annotation").unchecked();
     * @endcode
     */
    SIGHT_UI_TEST_API void unchecked() const;

    /**
     * @brief Expects the widget or action to be enabled.
     *
     * @code{.cpp}
     * ui.expect("intrinsicCameraView/Add").enabled();
     * @endcode
     */
    SIGHT_UI_TEST_API void enabled() const;

    /**
     * @brief Expects the widget or action to be disabled.
     *
     * @code{.cpp}
     * ui.expect("intrinsicCameraView/Add").disabled();
     * @endcode
     */
    SIGHT_UI_TEST_API void disabled() const;

    /**
     * @brief Expects the existing widget to be visible according to the shared visibility check.
     *
     * @code{.cpp}
     * ui.expect("videoToolbarView/Pause").visible();
     * @endcode
     */
    SIGHT_UI_TEST_API void visible() const;

    /**
     * @brief Expects the existing widget not to be visible, including when clipped or unexposed.
     *
     * @code{.cpp}
     * ui.click("videoToolbarView/Start");
     * ui.expect("videoToolbarView/Start").hidden();
     * @endcode
     */
    SIGHT_UI_TEST_API void hidden() const;

    /**
     * @brief Expects no widget, hidden or not, to match any more; the locator must have matched once, now or earlier.
     *
     * @code{.cpp}
     * ui.close(dialog());
     * ui.expect(dialog()).gone();
     * @endcode
     */
    SIGHT_UI_TEST_API void gone() const;

    /**
     * @brief Expects this exact, case-sensitive tooltip.
     *
     * @code{.cpp}
     * ui.expect("top_toolbar_left/volume").tooltip("Volume rendering");
     * @endcode
     *
     * @param _expected the tooltip.
     */
    SIGHT_UI_TEST_API void tooltip(std::string_view _expected) const;

    /**
     * @brief Expects this many rows directly below the view's root index, not counting descendants.
     *
     * @code{.cpp}
     * ui.expect("intrinsicCameraView/list").rows(3);
     * @endcode
     *
     * @param _expected the number of rows.
     */
    SIGHT_UI_TEST_API void rows(int _expected) const;

    /**
     * @brief Allows gone() to pass when this locator has never matched in the scenario.
     *
     * @code{.cpp}
     * ui.expect("update_dialog").never_existed_ok().gone();
     * @endcode
     *
     * @return this expectation.
     */
    [[nodiscard]] SIGHT_UI_TEST_API widget_expectation& never_existed_ok();

private:

    friend class scenario;

    SIGHT_UI_TEST_API widget_expectation(const scenario& _scenario, std::string_view _expression);

    SIGHT_UI_TEST_API void check(
        std::string_view _description,
        std::function<std::pair<bool, std::string>(const QWidget&)> _probe,
        detail::widget_query _query = detail::widget_query::probe
    ) const;

    const scenario* m_scenario;
    std::string m_expression;
    duration_t m_timeout {DEFAULT_TIMEOUT};
    bool m_never_existed_ok {false};
};

} // namespace sight::ui::test
