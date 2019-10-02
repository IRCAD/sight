/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "uiActivitiesQt/config.hpp"

#include <fwActivities/IActivitySequencer.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QQuickWidget>

namespace uiActivitiesQt
{
namespace editor
{

/**
 * @brief This editor displays an activity stepper that allows to select the activity to launch, and display the
 * current selection
 *
 * The order of the activities is given in the configuration.
 *
 * ActivitySeries are created for each activity using the data produced by the previous activities. This activities are
 * stored in the current SeriesDB.
 *
 * @warning If an activity can not be launched with the existing parameters, the signal 'dataRequired' is emitted. It
 * can be connected to an activity wizard to add the missing data, or you can supplied 'requirementOverrides' composite.
 *
 * @section Signal Signal
 * - \b activityCreated(::fwMedData::ActivitySeries::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b dataRequired() : This signal is emitted when the activity can not be launch because it requires data.
 * - \b enabledNext(bool): This signal is emitted when the next button is enabled (when the activity is not the last
 *   one)
 * - \b enabledPrevious(bool): This signal is emitted when the previous button is enabled (when the activity is not the
 *   first one)
 *
 * @section Slots Slots
 * - \b next() : Create the next activity series
 * - \b previous() : Create the next activity series
 * - \b goTo(int) : Create the activity series at the given index
 * - \b sendInfo() : Send the 'enabledNext' and 'enablePrevious' signals for the current activity
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service  type="::uiActivitiesQt::editor::SActivitySequencer">
        <inout key="seriesDB" uid=""  autoConnect="yes" />
        <in    key="requirementOverrides" uid="..." />
        <activity id="..." name="..." />
        <activity id="..." name="..." />
        <activity id="..." name="..." />
        <theme>#FF00FF</theme>
        <accent>#FF00FF</accent>
        <foreground>#FF00FF</foreground>
        <background>#FF00FF</background>
        <primary>#FF00FF</primary>
        <elevation>#FF00FF</elevation>
    </service>
   @endcode
 * @subsection In In
 * - \b requirementOverrides [::fwData::Composite] (optional): provide some data that will be passed as activity
 *   requirements. Can be used to supply required data that cannot be created by the activity, or to override
 *   data that would normally be passed from an activity to the next.
 *
 * @subsection In-Out In-Out
 * - \b seriesDB [::fwMedData::SeriesDB]: used to store the ActivitySeries of the managed activities
 *
 * @subsection Configuration Configuration
 * - \b activity :
 *     - \b id: id of the activities to launch. The first activity in the list is the first that will be launched.
 *     - \b name(optional): name of the activity to display in the editor. If it is empty, the the activity's will be
 *          used
 * - \b theme (optional, lihgt/dark): the global theme used by the sequencer.
 * - \b accent (optional): the accent color used by the sequencer.
 * - \b foreground (optional): the foreground color used by the sequencer.
 * - \b background (optional): the background color used by the sequencer.
 * - \b primary (optional): the primary color used by the sequencer.
 * - \b elevation (optional): the elevation color used by the sequencer.
 *
 * @todo listen the current activity data to notify when the next activity can be created
 */
class UIACTIVITIESQT_CLASS_API SActivitySequencer : public QObject,
                                                    public ::fwGui::editor::IEditor,
                                                    public ::fwActivities::IActivitySequencer
{
Q_OBJECT

public:

    fwCoreServiceMacro(SActivitySequencer, ::fwGui::editor::IEditor)

    /// Constructor. Do nothing.
    UIACTIVITIESQT_API SActivitySequencer() noexcept;

    /// Destructor. Do nothing.
    UIACTIVITIESQT_API virtual ~SActivitySequencer() noexcept override;

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr ) > ActivityCreatedSignalType;
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr) > DataRequiredSignalType;
    typedef ::fwCom::Signal<void (bool) > EnabledPreviousSignalType;
    typedef ::fwCom::Signal<void (bool) > EnabledNextSignalType;
    /**
     * @}
     */

public Q_SLOTS:

    /// Slot: create the activity at the given index, emit 'dataRequired' signal if the activity require additional data
    void goTo(int index);

protected:

    /// Do nothing
    virtual void configuring() override;

    /// Parse the Activity ids list
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /**
     * @brief Analyse the series contained in the current seriesDB.
     *
     * - if the series is not an activity or if it is an unknown activity, it is removed
     * - else, the activity data is stored in m_requirements
     * - the last activity is launched
     */
    virtual void updating() override;

    /// Connect the service to the SeriesDB signals
    virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: Chech if the next activities can be enabled
    void checkNext();

    /// Slot: Create the next activity series, emit 'dataRequired' signal if the activity require additional data
    void next();

    /// Slot: Create the previous activity series, emit 'dataRequired' signal if the activity require additional data
    void previous();

    /// Slot: Send the 'enabledNext' and 'enablePrevious' signals for the current activity
    void sendInfo() const;

    /// Invoke 'enableActivity' method in Qml file
    void enableActivity(int index);

    ActivityCreatedSignalType::sptr m_sigActivityCreated;
    DataRequiredSignalType::sptr m_sigDataRequired;
    EnabledPreviousSignalType::sptr m_sigEnabledPrevious;
    EnabledNextSignalType::sptr m_sigEnabledNext;

    std::vector<std::string> m_activityNames;

    QPointer<QQuickWidget> m_widget;

    std::string m_theme;
    std::string m_accent;
    std::string m_foreground;
    std::string m_background;
    std::string m_primary;
    std::string m_elevation;
};

} // namespace editor
} // namespace uiActivitiesQt
