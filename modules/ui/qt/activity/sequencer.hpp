/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <activity/sequencer.hpp>

#include <data/activity_set.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>
#include <QQuickWidget>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This editor displays an activity stepper that allows to select the activity to launch, and display the
 * current selection
 *
 * The order of the activities is given in the configuration.
 *
 * Activity are created for each activity using the data produced by the previous activities. This activities are
 * stored in the current ActivitySet. By default all the data are stored, you can to backward and forward as you want in
 * the existing activities. Using the tag 'clearActivities', you can remove the last activities when going backward to
 * force the user to re-generate the data.
 *
 * @warning If an activity can not be launched with the existing parameters, the signal 'dataRequired' is emitted. It
 * can be connected to an activity wizard to add the missing data, or you can supplied 'requirementOverrides' composite.
 *
 * @note If the inout ActivitySet already contains activities, their are parsed and the sequencer open on the last
 * activities. Be careful to store them in the right order.
 *
 * @section Signal Signal
 * - \b activity_created(data::activity::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b data_required() : This signal is emitted when the activity can not be launch because it requires data.
 * - \b hasNext(bool): This signal is emitted on sendInfo() slot, with the information if an activity is present after
 * the current one.
 * - \b hasPrevious(bool): This signal is emitted on sendInfo() slot, with the information if an activity is present
 * before the current one.
 * - \b nextEnabled(bool): This signal is emitted when the next button is enabled and can be launched.
 *
 * @section Slots Slots
 * - \b next() : Create the next activity
 * - \b previous() : Create the next activity
 * - \b go_to(int) : Create the activity at the given index
 * - \b send_info() : Send the 'hasNext' and 'hasPrevious' signals for the current activity
 * - \b reset_requirements() : Reset requirements that have been created by the activity
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service  type="sight::module::ui::qt::activity::sequencer">
        <inout key="activitySet" uid=""  auto_connect="true" />
        <activity id="..." name="..." />
        <activity id="..." name="..." />
        <activity id="..." name="..." />
        <clearActivities>false</clearActivities>
        <clear>#FFFFFF</clear>
        <theme>#FF00FF</theme>
        <accent>#FF00FF</accent>
        <foreground>#FF00FF</foreground>
        <background>#FF00FF</background>
        <primary>#FF00FF</primary>
        <elevation>#FF00FF</elevation>
        <buttonWidth>200</buttonWidth>
        <fontSize>12</fontSize>
    </service>
   @endcode
 * @subsection In In
 * - \b requirementOverrides [sight::data::composite] (optional): provide some data that will be passed as activity
 *   requirements. Can be used to supply required data that cannot be created by the activity, or to override
 *   data that would normally be passed from an activity to the next.
 *
 * @subsection In-Out In-Out
 * - \b activity_set [sight::data::activity_set]: used to store the Activity of the managed activities
 *
 * @subsection Configuration Configuration
 * - \b activity :
 *     - \b id: id of the activities to launch. The first activity in the list is the first that will be launched.
 *     - \b name(optional): name of the activity to display in the editor. If it is empty, the the activity's will be
 *          used
 * - \b clearActivities (optional, default: false): defines if the activities and their requirements should be removed
 * when going backward.
 * - \b theme (optional, light/dark): the global theme used by the sequencer.
 * - \b clear (optional): the color of the opengl background scene.
 * - \b accent (optional): the accent color used by the sequencer.
 * - \b foreground (optional): the foreground color used by the sequencer.
 * - \b background (optional): the background color used by the sequencer.
 * - \b primary (optional): the primary color used by the sequencer.
 * - \b elevation (optional): the elevation color used by the sequencer.
 * - \b buttonWidth (optional): the width of the buttons of the sequencer.
 * - \b fontSize (optional): the size of the font used in the buttons of the sequencer.
 *
 * @todo listen the current activity data to notify when the next activity can be created
 */
class MODULE_UI_QT_CLASS_API sequencer : public QObject,
                                         public sight::ui::editor,
                                         public sight::activity::sequencer
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(sequencer, sight::ui::editor);

    /// Initialize signals and slots
    MODULE_UI_QT_API sequencer() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~sequencer() noexcept override = default;

    struct MODULE_UI_QT_CLASS_API slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t GO_TO         = "go_to";
        inline static const key_t CHECK_NEXT    = "check_next";
        inline static const key_t VALIDATE_NEXT = "validate_next";
        inline static const key_t NEXT          = "next";
        inline static const key_t PREVIOUS      = "previous";
        inline static const key_t SEND_INFO     = "send_info";

        inline static const key_t RESET_REQUIREMENTS = "reset_requirements";

        inline static const key_t ENABLE_USER_WARNING  = "enable_user_warning";
        inline static const key_t DISABLE_USER_WARNING = "disable_user_warning";
        inline static const key_t SET_USER_WARNING     = "set_user_warning";
    };

    struct MODULE_UI_QT_CLASS_API signals final
    {
        using key_t = sight::core::com::signals::key_t;

        inline static const key_t ACTIVITY_CREATED = "activity_created";
        inline static const key_t DATA_REQUIRED    = "data_required";
        inline static const key_t HAS_PREVIOUS     = "hasPrevious";
        inline static const key_t HAS_NEXT         = "hasNext";
        inline static const key_t NEXT_ENABLED     = "nextEnabled";

        inline static const key_t NEXT_VALIDATED = "next_validated";
        inline static const key_t NEXT_VALID     = "next_valid";
        inline static const key_t NEXT_INVALID   = "next_invalid";

        using void_signal_t     = core::com::signal<void ()>;
        using bool_signal_t     = core::com::signal<void (bool)>;
        using activity_signal_t = core::com::signal<void (data::activity::sptr)>;
    };

    /// Slot: Check if the next activities can be enabled
    MODULE_UI_QT_API void check_next();

    /// Slot: Validate the next activities without enabling it. Emits nextValidated(true/false), next(In)Valid signals
    MODULE_UI_QT_API void validate_next();

    /// Slot: Create the next activity, emit 'dataRequired' signal if the activity require additional data
    MODULE_UI_QT_API void next();

    /// Slot: Create the previous activity, emit 'dataRequired' signal if the activity require additional data
    MODULE_UI_QT_API void previous();

    /// Slot: Send the 'hasNext' and 'enablePrevious' signals for the current activity
    MODULE_UI_QT_API void send_info() const;

    // Slot: Reset requirements of activities
    using sight::activity::sequencer::reset_requirements;

    /// Slot: Enables the user warning dialog about possible loss of un-validated data
    /// Does nothing if "warning_message" wasn't configured.
    MODULE_UI_QT_API void enable_user_warning();

    /// Slot: Disables the user warning dialog about possible loss of un-validated data
    /// Does nothing if "warning_message" wasn't configured.
    MODULE_UI_QT_API void disable_user_warning();

    /// Slot: Configures the user warning according to _state value.
    /// Does nothing if "warning_message" wasn't configured.
    MODULE_UI_QT_API void set_user_warning(bool _state);

public Q_SLOTS:

    /// Slot: create the activity at the given index, emit 'dataRequired' signal if the activity require additional data
    MODULE_UI_QT_API void go_to(int _index);

protected:

    /// Parses the configuration
    void configuring() override;

    /// Create the sequencer widgets: launch Qml file
    void starting() override;

    /// Destroy the container
    void stopping() override;

    /**
     * @brief Analyse the contained in the current activity_set.
     *
     * - if the is an unknown activity, it is removed
     * - else, the activity data is stored in m_requirements
     * - the last activity is launched
     */
    void updating() override;

    /// Connect the service to the ActivitySet signals
    connections_t auto_connections() const override;

private:

    /// Invoke 'enableActivity' method in Qml file
    void enable_activity(int _index);

    /// Invokes 'disableActivity' method in Qml file
    void disable_activity(int _index);

    /// List of the activities
    std::vector<std::string> m_activity_names;

    QPointer<QQuickWidget> m_widget;

    /// Defines if the activities should be cleared when going backward
    bool m_clear_activities {false};

    /// Display a warning message if changing activities without validating it.
    bool m_warn_user {false};
    /// Configured warning message, if empty no warnings are displayed.
    std::string m_warning_message;

    /// Colors used to customize sequencer
    std::string m_theme;
    std::string m_accent;
    std::string m_clear;
    std::string m_foreground;
    std::string m_background;
    std::string m_primary;
    std::string m_elevation;
    std::string m_button_width {"200"};
    double m_font_size {12.0};

    const signals::activity_signal_t::sptr m_activity_created {
        new_signal<signals::activity_signal_t>(signals::ACTIVITY_CREATED)
    };

    const signals::activity_signal_t::sptr m_data_required {
        new_signal<signals::activity_signal_t>(signals::DATA_REQUIRED)
    };

    const signals::bool_signal_t::sptr m_has_previous {
        new_signal<signals::bool_signal_t>(signals::HAS_PREVIOUS)
    };

    const signals::bool_signal_t::sptr m_has_next {
        new_signal<signals::bool_signal_t>(signals::HAS_NEXT)
    };

    const signals::bool_signal_t::sptr m_next_enabled {
        new_signal<signals::bool_signal_t>(signals::NEXT_ENABLED)
    };

    const signals::bool_signal_t::sptr m_next_validated {
        new_signal<signals::bool_signal_t>(signals::NEXT_VALIDATED)
    };

    const signals::void_signal_t::sptr m_next_valid {
        new_signal<signals::void_signal_t>(signals::NEXT_VALID)
    };

    const signals::void_signal_t::sptr m_next_invalid {
        new_signal<signals::void_signal_t>(signals::NEXT_INVALID)
    };

    static constexpr std::string_view ACTIVITY_SET_INOUT = "activitySet";
    data::ptr<data::activity_set, data::access::inout> m_activity_set {this, ACTIVITY_SET_INOUT, true};
};

} // namespace sight::module::ui::qt::activity
