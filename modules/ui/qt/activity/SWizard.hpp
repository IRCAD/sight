/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "modules/ui/qt/activity/DataView.hpp"
#include "modules/ui/qt/config.hpp"

#include <activity/extension/Activity.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>

#include <data/ActivitySeries.hpp>
#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This editor allows to select the data required by an activity in order to create the ActivitySeries.
 *
 * This editor displays a tab widget (one tab by data).
 * It works on a data::SeriesDB and adds the created activity series into the seriesDB.
 *
 * @section Slots Slots
 * - \b createActivity(std::string) : This slot displays the gui allowing to create a data::ActivitySeries with
 *      the required data for the given activity.
 * - \b updateActivity(data::ActivitySeries::sptr) : This slot displays the gui allowing to update the required
 *      data for the given activity.
 * - \b updateActivitySeries(data::Series::sptr) : This slot receives a data::Series, but works only if
 *   the series is an ActivitySeries, else it does nothing. It displays the gui allowing to update the required data for
 *   the given activity.
 *
 * @section Signals Signals
 * - \b activityCreated(data::ActivitySeries::sptr) : This signal is emitted when the activitySeries is built.
 * - \b activityUpdated(data::ActivitySeries::sptr) : This signal is emitted when the activitySeries is updated.
 * - \b canceled() : This signal is emitted when the cancel button is clicked.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service impl="sight::module::ui::qt::activity::SWizard">
     <inout key="seriesDB" uid="..." />
     <inout key="activitySeries" uid="..." />
     <ioSelectorConfig>config</ioSelectorConfig>
     <sdbIoSelectorConfig>config</sdbIoSelectorConfig>
     <activitySeries>seriesUid</activitySeries>
     <confirm>true</confirm>
     <cancel>true</cancel>
     <icons>
         <icon type="data::ImageSeries" icon="sight::module::ui::icons/ImageSeries.svg" />
         <icon type="data::ModelSeries" icon="sight::module::ui::icons/ModelSeries.svg" />
     </icons>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b seriesDB [sight::data::SeriesDB]: seriesDB to store the create activity series
 * - \b seriesDB [sight::data::ActivitySeries] (optional): uid of the activity series to update. It is only used when
 * the update() method is called.
 * @subsection Configuration Configuration
 * - \b ioSelectorConfig : configuration for the SSelector service used to import data in this editor.
 * - \b sdbIoSelectorConfig(optional, default: ioSelectorConfig): configuration for the SSelector service used to
 * import data in this editor from a SeriesDB.
 * - \b icons : defines the icons displayed for a type of data
 *    - \b type : type of data
 *    - \b icon : path of the icon to display
 * - \b confirm (optional, true by default): if true, the editor proposes a confirmation dialog when the activity is
 *   updated.
 * - \b cancel (optional, true by default): if true, the editor proposes a cancel button. On cancel click, the editor is
 *   emptied and the signal 'canceled' is emitted.

 */
class MODULE_UI_QT_CLASS_API SWizard : public QObject,
                                       public sight::ui::base::IEditor
{
public:

    SIGHT_DECLARE_SERVICE(SWizard, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SWizard() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~SWizard() noexcept override;

    /**
     * @name Slot API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CREATE_ACTIVITY_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_UPDATE_ACTIVITY_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_UPDATE_ACTIVITY_SERIES_SLOT;
    /// @}

    /**
     * @name Signal API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG;
    typedef core::com::Signal<void (data::ActivitySeries::sptr)> ActivityCreatedSignalType;

    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_ACTIVITY_UPDATED_SIG;
    typedef core::com::Signal<void (data::ActivitySeries::sptr)> ActivityUpdatedSignalType;

    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_CANCELED_SIG;
    typedef core::com::Signal<void ()> CanceledSignalType;
    /// @}

protected:

    ///This method creates the editor gui.
    void starting() override;

    ///This method destroys the editor gui.
    void stopping() override;

    /// Update the activity if it is defined in the configuration, else does nothing.
    void updating() override;

    /// Initialize the editor.
    void configuring() override;

private:

    enum class Mode : std::uint8_t
    {
        CREATE,
        UPDATE
    };

    /// SLOT: Displays the gui allowing to select the required data for the activity.
    void createActivity(std::string activityID);

    /// SLOT: Displays the gui allowing to update the required data for the activity.
    void updateActivity(data::ActivitySeries::sptr activitySeries);

    /// SLOT: Displays the gui allowing to update the required data for the activity.
    /**
     * @brief SLOT: It displays the gui allowing to update the required data for the given activity.
     * @note This slot receives a data::Series, but works only if the series is an ActivitySeries, else it does
     *       nothing.
     */
    void updateActivitySeries(data::Series::sptr series);

    /**
     * @brief Called when the user click on the 'apply' Button.
     *
     * if mode == CREATE : It creates the activitySeries and add it to the seriesDB.
     * else, it update the current activity series.
     */
    void onBuildActivity();

    /// Called when the tab selection changed. It check if the current data are properly selected.
    void onTabChanged(int index);

    /// Called when the user click on the 'reset' Button.
    void onReset();

    /// Called when the user click on the 'cancel' Button.
    void onCancel();

    data::ActivitySeries::sptr m_actSeries; ///< Activity series builded

    QPointer<DataView> m_DataView;        ///< view used to select required data for activity
    QPointer<QPushButton> m_okButton;     ///<  Button 'Apply' or 'Next' to validate the activity creation
    QPointer<QPushButton> m_resetButton;  ///< Button to clear the activity parameters
    QPointer<QPushButton> m_cancelButton; ///< Button to cancel the activity creation
    QPointer<QLabel> m_title;             ///< Label to show activity title
    QPointer<QLabel> m_description;       ///< Label to show activity description

    std::string m_ioSelectorConfig;    ///< configuration used to import data
    std::string m_sdbIoSelectorConfig; ///< configuration used to import data from e SeriesDB

    DataView::ObjectIconMapType m_objectIcons; ///< Map defining the icon associated to an object.

    Mode m_mode {Mode::CREATE}; ///< editor mode (CREATE or UPDATE)

    bool m_confirmUpdate {true}; ///< if true, the editor proposes a confirmation dialog when the activity is updated.
    bool m_isCancelable {true};  /// true if the cancel button is proposed

    ActivityCreatedSignalType::sptr m_sigActivityCreated; ///< Signal emitted when the activitySeries is created
    ActivityCreatedSignalType::sptr m_sigActivityUpdated; ///< Signal emitted when the activitySeries is updated
    CanceledSignalType::sptr m_sigCanceled;               /// Signal emitted when the creation is canceled.

    static constexpr std::string_view s_SERIESDB = "seriesDB";
    data::ptr<data::SeriesDB, data::Access::inout> m_seriesDB {this, s_SERIESDB};
    data::ptr<data::ActivitySeries, data::Access::inout> m_activitySeries {this, "activitySeries"};
};

} // namespace sight::module::ui::qt::activity
