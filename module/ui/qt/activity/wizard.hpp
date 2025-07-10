/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

#include "module/ui/qt/activity/data_view.hpp"

#include <activity/extension/activity.hpp>

#include <data/activity.hpp>
#include <data/activity_set.hpp>
#include <data/vector.hpp>

#include <ui/__/editor.hpp>

#include <QLabel>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This editor allows to select the data required by an activity in order to create the Activity.
 *
 * This editor displays a tab widget (one tab by data).
 * It works on a data::activity_set and adds the created activity into the activity_set.
 *
 * @section Slots Slots
 * - \b create_activity(std::string) : This slot displays the gui allowing to create a data::activity with
 *      the required data for the given activity.
 * - \b update_activity(data::activity::sptr) : This slot displays the gui allowing to update the required
 *      data for the given activity.
 *
 * @section Signals Signals
 * - \b activity_created(data::activity::sptr) : This signal is emitted when the activity is built.
 * - \b activity_updated(data::activity::sptr) : This signal is emitted when the activity is updated.
 * - \b canceled() : This signal is emitted when the cancel button is clicked.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service impl="sight::module::ui::qt::activity::wizard">
     <inout key="activitySet" uid="..." />
     <inout key="activity" uid="..." />
     <ioSelectorConfig>config</ioSelectorConfig>
     <sdbIoSelectorConfig>config</sdbIoSelectorConfig>
     <activity>activityUid</activity>
     <confirm>true</confirm>
     <cancel>true</cancel>
     <icons>
         <icon type="data::image_series" icon="sight::module::ui::icons/image_series.svg" />
         <icon type="data::model_series" icon="sight::module::ui::icons/model_series.svg" />
     </icons>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b activity_set [sight::data::activity_set]: activity_set to store the create activity
 * - \b activity [sight::data::activity] (optional): uid of the activity to update. It is only used when
 * the update() method is called.
 * @subsection Configuration Configuration
 * - \b ioSelectorConfig : configuration for the selector service used to import data in this editor.
 * - \b sdbIoSelectorConfig(optional, default: ioSelectorConfig): configuration for the selector service used to
 * import data in this editor from a ActivitySet.
 * - \b icons : defines the icons displayed for a type of data
 *    - \b type : type of data
 *    - \b icon : path of the icon to display
 * - \b confirm (optional, true by default): if true, the editor proposes a confirmation dialog when the activity is
 *   updated.
 * - \b cancel (optional, true by default): if true, the editor proposes a cancel button. On cancel click, the editor is
 *   emptied and the signal 'canceled' is emitted.

 */
class wizard : public QObject,
               public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(wizard, sight::ui::editor);

    /// Constructor. Do nothing.
    wizard() noexcept;

    /// Destructor. Do nothing.
    ~wizard() noexcept override;

    /**
     * @name Slot API
     * @{
     */
    static const core::com::slots::key_t CREATE_ACTIVITY_SLOT;
    static const core::com::slots::key_t UPDATE_ACTIVITY_SLOT;
    /// @}

    /**
     * @name Signal API
     * @{
     */
    static const core::com::signals::key_t ACTIVITY_CREATED_SIG;
    using activity_created_signal_t = core::com::signal<void (data::activity::sptr)>;

    static const core::com::signals::key_t ACTIVITY_UPDATED_SIG;
    using activity_updated_signal_t = core::com::signal<void (data::activity::sptr)>;

    static const core::com::signals::key_t CANCELED_SIG;
    using canceled_signal_t = core::com::signal<void ()>;
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

    enum class mode : std::uint8_t
    {
        create,
        update
    };

    /// SLOT: Displays the gui allowing to select the required data for the activity.
    void create_activity(std::string _activity_id);

    /// SLOT: Displays the gui allowing to update the required data for the activity.
    void update_activity(data::activity::sptr _activity);

    /**
     * @brief Called when the user click on the 'apply' Button.
     *
     * if mode == CREATE : It creates the activity and add it to the activity_set.
     * else, it update the current activity.
     */
    void on_build_activity();

    /// Called when the tab selection changed. It check if the current data are properly selected.
    void on_tab_changed(int _index);

    /// Called when the user click on the 'reset' Button.
    void on_reset();

    /// Called when the user click on the 'cancel' Button.
    void on_cancel();

    data::activity::sptr m_new_activity; ///< Activity builded

    QPointer<data_view> m_data_view;       ///< view used to select required data for activity
    QPointer<QPushButton> m_ok_button;     ///<  Button 'Apply' or 'Next' to validate the activity creation
    QPointer<QPushButton> m_reset_button;  ///< Button to clear the activity parameters
    QPointer<QPushButton> m_cancel_button; ///< Button to cancel the activity creation
    QPointer<QLabel> m_title;              ///< Label to show activity title
    QPointer<QLabel> m_description;        ///< Label to show activity description

    std::string m_io_selector_config;     ///< configuration used to import data
    std::string m_sdb_io_selector_config; ///< configuration used to import data from e ActivitySet

    data_view::object_icon_map_t m_object_icons; ///< Map defining the icon associated to an object.

    mode m_mode {mode::create}; ///< editor mode (CREATE or UPDATE)

    bool m_confirm_update {true}; ///< if true, the editor proposes a confirmation dialog when the activity is updated.
    bool m_is_cancelable {true};  /// true if the cancel button is proposed

    activity_created_signal_t::sptr m_sig_activity_created; ///< Signal emitted when the activity is created
    activity_created_signal_t::sptr m_sig_activity_updated; ///< Signal emitted when the activity is updated
    canceled_signal_t::sptr m_sig_canceled;                 /// Signal emitted when the creation is canceled.

    static constexpr std::string_view ACTIVITY_SET = "activitySet";
    data::ptr<data::activity_set, data::access::inout> m_activity_set {this, ACTIVITY_SET};
    data::ptr<data::activity, data::access::inout> m_activity {this, "activity"};
};

} // namespace sight::module::ui::qt::activity
