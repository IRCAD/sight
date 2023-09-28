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

#include <data/ActivitySet.hpp>

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
 * - \b activityCreated(data::Activity::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b dataRequired() : This signal is emitted when the activity can not be launch because it requires data.
 * - \b hasNext(bool): This signal is emitted on sendInfo() slot, with the information if an activity is present after
 * the current one.
 * - \b hasPrevious(bool): This signal is emitted on sendInfo() slot, with the information if an activity is present
 * before the current one.
 * - \b nextEnabled(bool): This signal is emitted when the next button is enabled and can be launched.
 *
 * @section Slots Slots
 * - \b next() : Create the next activity
 * - \b previous() : Create the next activity
 * - \b goTo(int) : Create the activity at the given index
 * - \b sendInfo() : Send the 'hasNext' and 'hasPrevious' signals for the current activity
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service  type="sight::module::ui::qt::activity::SSequencer">
        <inout key="activitySet" uid=""  autoConnect="true" />
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
 * - \b requirementOverrides [sight::data::Composite] (optional): provide some data that will be passed as activity
 *   requirements. Can be used to supply required data that cannot be created by the activity, or to override
 *   data that would normally be passed from an activity to the next.
 *
 * @subsection In-Out In-Out
 * - \b activity_set [sight::data::ActivitySet]: used to store the Activity of the managed activities
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
class MODULE_UI_QT_CLASS_API SSequencer : public QObject,
                                          public sight::ui::editor,
                                          public sight::activity::sequencer
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SSequencer, sight::ui::editor);

    /// Initialize signals and slots
    MODULE_UI_QT_API SSequencer() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~SSequencer() noexcept override = default;

    struct MODULE_UI_QT_CLASS_API Slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t GO_TO         = "goTo";
        inline static const key_t CHECK_NEXT    = "checkNext";
        inline static const key_t VALIDATE_NEXT = "validateNext";
        inline static const key_t NEXT          = "next";
        inline static const key_t PREVIOUS      = "previous";
        inline static const key_t SEND_INFO     = "sendInfo";
    };

    struct MODULE_UI_QT_CLASS_API Signals final
    {
        using key_t = sight::core::com::signals::key_t;

        inline static const key_t ACTIVITY_CREATED = "activityCreated";
        inline static const key_t DATA_REQUIRED    = "dataRequired";
        inline static const key_t HAS_PREVIOUS     = "hasPrevious";
        inline static const key_t HAS_NEXT         = "hasNext";
        inline static const key_t NEXT_ENABLED     = "nextEnabled";

        inline static const key_t NEXT_VALIDATED = "nextValidated";
        inline static const key_t NEXT_VALID     = "nextValid";
        inline static const key_t NEXT_INVALID   = "nextInvalid";

        using void_signal_t     = core::com::signal<void ()>;
        using bool_signal_t     = core::com::signal<void (bool)>;
        using activity_signal_t = core::com::signal<void (data::Activity::sptr)>;
    };

    /// Slot: Check if the next activities can be enabled
    MODULE_UI_QT_API void checkNext();

    /// Slot: Validate the next activities without enabling it. Emits nextValidated(true/false), next(In)Valid signals
    MODULE_UI_QT_API void validateNext();

    /// Slot: Create the next activity, emit 'dataRequired' signal if the activity require additional data
    MODULE_UI_QT_API void next();

    /// Slot: Create the previous activity, emit 'dataRequired' signal if the activity require additional data
    MODULE_UI_QT_API void previous();

    /// Slot: Send the 'hasNext' and 'enablePrevious' signals for the current activity
    MODULE_UI_QT_API void sendInfo() const;

public Q_SLOTS:

    /// Slot: create the activity at the given index, emit 'dataRequired' signal if the activity require additional data
    MODULE_UI_QT_API void goTo(int index);

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
    connections_t getAutoConnections() const override;

private:

    /// Invoke 'enableActivity' method in Qml file
    void enableActivity(int index);

    /// Invokes 'disableActivity' method in Qml file
    void disableActivity(int index);

    /// List of the activities
    std::vector<std::string> m_activityNames;

    QPointer<QQuickWidget> m_widget;

    /// Defines if the activities should be cleared when going backward
    bool m_clearActivities {false};

    /// Colors used to customize sequencer
    std::string m_theme;
    std::string m_accent;
    std::string m_clear;
    std::string m_foreground;
    std::string m_background;
    std::string m_primary;
    std::string m_elevation;
    std::string m_buttonWidth {"200"};
    double m_fontSize {12.0};

    const Signals::activity_signal_t::sptr m_activity_created {
        new_signal<Signals::activity_signal_t>(Signals::ACTIVITY_CREATED)
    };

    const Signals::activity_signal_t::sptr m_data_required {
        new_signal<Signals::activity_signal_t>(Signals::DATA_REQUIRED)
    };

    const Signals::bool_signal_t::sptr m_has_previous {
        new_signal<Signals::bool_signal_t>(Signals::HAS_PREVIOUS)
    };

    const Signals::bool_signal_t::sptr m_has_next {
        new_signal<Signals::bool_signal_t>(Signals::HAS_NEXT)
    };

    const Signals::bool_signal_t::sptr m_next_enabled {
        new_signal<Signals::bool_signal_t>(Signals::NEXT_ENABLED)
    };

    const Signals::bool_signal_t::sptr m_next_validated {
        new_signal<Signals::bool_signal_t>(Signals::NEXT_VALIDATED)
    };

    const Signals::void_signal_t::sptr m_next_valid {
        new_signal<Signals::void_signal_t>(Signals::NEXT_VALID)
    };

    const Signals::void_signal_t::sptr m_next_invalid {
        new_signal<Signals::void_signal_t>(Signals::NEXT_INVALID)
    };

    static constexpr std::string_view s_ACTIVITY_SET_INOUT = "activitySet";
    data::ptr<data::ActivitySet, data::Access::inout> m_activity_set {this, s_ACTIVITY_SET_INOUT, true};
};

} // namespace sight::module::ui::qt::activity
