/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <app/helper/config_launcher.hpp>

#include <data/activity.hpp>
#include <data/camera_set.hpp>

#include <ui/__/editor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief This editor adds cameras to a camera series and launches configurations to calibrate them.
 *
 * @note Currently, only one or two cameras are allowed.
 *
 * This service launches an config for intrinsic camera calibration and another config for extrinsic calibration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="cameraLauncher" type="sight::module::ui::qt::calibration::camera_config_launcher">
       <inout key="cameraSet" uid="..." />
       <inout key="activity" uid="..." />
       <config>
           <intrinsic>
               <properties config="sight::navigation::calibration::cal_intrinsic_view" />
               <parameter replace="WID_PARENT" by="calibrationView" />
               <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
           </intrinsic>
           <extrinsic>
               <properties config="sight::navigation::calibration::cal_extrinsic_view" />
               <inout key="cameraSet" uid="..." />
               <parameter replace="WID_PARENT" by="calibrationView" />
               <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
           </extrinsic>
       </config>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b cameraSet [sight::data::camera_set] : stores camera calibrations.
 * - \b activity [sight::data::activity]: stores the information used to generate the calibration.
 *        It allows to re-open the activity with this information.
 *
 * @subsection Configuration Configuration:
 * - \b config: contains the appConfigs to launch for intrinsic and extrinsic calibration.
 *   - \b intrinsic: configuration to launch the intrinsic calibration config. @see
 * module::ui::action::config_launcher
 *   - \b extrinsic: configuration to launch the extrinsic calibration config. @see
 * module::ui::action::config_launcher
 *
 */
class camera_config_launcher : public QObject,
                               public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(camera_config_launcher, sight::ui::editor);

    /// Constructor. Do nothing.
    camera_config_launcher() noexcept;

    /// Destructor. Do nothing.
    ~camera_config_launcher() noexcept override;

protected:

    void configuring() override;

    ///This method launches the editor::starting method.
    void starting() override;

    ///This method launches the editor::stopping method.
    void stopping() override;

    void updating() override;

private Q_SLOTS:

    void on_add_clicked();
    void on_import_clicked();
    void on_remove_clicked();
    void on_extrinsic_toggled(bool _checked);
    void on_camera_changed(int _index);

private:

    /// Start the configuration for the intrinsic calibration
    void start_intrinsic_config(std::size_t _index);

    /// Start the configuration for the extrinsic calibration
    void start_extrinsic_config(std::size_t _index);

    /// Adds a Camera in cameraSet and the CalibrationInfo in the activity
    void add_camera();

    QPointer<QComboBox> m_camera_combo_box;
    QPointer<QPushButton> m_add_button;
    QPointer<QPushButton> m_import_button;
    QPointer<QPushButton> m_remove_button;
    QPointer<QPushButton> m_extrinsic_button;

    sight::app::helper::config_launcher m_intrinsic_launcher;
    sight::app::helper::config_launcher m_extrinsic_launcher;

    data::ptr<data::camera_set, data::access::inout> m_camera_set {this, "cameraSet"};
    data::ptr<data::activity, data::access::inout> m_activity {this, "activity"};
    data::ptr<data::map, data::access::inout> m_board_properties {this, "board_properties"};
};

} // namespace sight::module::ui::qt::calibration
