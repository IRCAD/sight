/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <data/Activity.hpp>
#include <data/CameraSet.hpp>

#include <service/helper/ConfigLauncher.hpp>

#include <ui/base/IEditor.hpp>

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
 * This service launches an AppConfig for intrinsic camera calibration and another AppConfig for extrinsic calibration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="cameraLauncher" type="sight::module::ui::qt::calibration::SCameraConfigLauncher">
       <inout key="cameraSet" uid="..." />
       <inout key="activity" uid="..." />
       <config>
           <intrinsic>
               <appConfig id="calIntrinsicView" />
               <parameter replace="WID_PARENT" by="calibrationView" />
               <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
           </intrinsic>
           <extrinsic>
               <appConfig id="calExtrinsicView" />
               <inout key="cameraSet" uid="..." />
               <parameter replace="WID_PARENT" by="calibrationView" />
               <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
           </extrinsic>
       </config>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b cameraSet [sight::data::CameraSet] : stores camera calibrations.
 * - \b activity [sight::data::Activity]: stores the information used to generate the calibration.
 *        It allows to re-open the activity with this information.
 *
 * @subsection Configuration Configuration:
 * - \b config: contains the appConfigs to launch for intrinsic and extrinsic calibration.
 *   - \b intrinsic: configuration to launch the intrinsic calibration config. @see
 * module::ui::base::action::SConfigLauncher
 *   - \b extrinsic: configuration to launch the extrinsic calibration config. @see
 * module::ui::base::action::SConfigLauncher
 *
 */
class MODULE_UI_QT_CLASS_API SCameraConfigLauncher : public QObject,
                                                     public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SCameraConfigLauncher, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SCameraConfigLauncher() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~SCameraConfigLauncher() noexcept override;

protected:

    void configuring() override;

    ///This method launches the IEditor::starting method.
    void starting() override;

    ///This method launches the IEditor::stopping method.
    void stopping() override;

    void updating() override;

private Q_SLOTS:

    void onAddClicked();
    void onImportClicked();
    void onRemoveClicked();
    void onExtrinsicToggled(bool checked);
    void onCameraChanged(int index);

private:

    /// Start the configuration for the intrinsic calibration
    void startIntrinsicConfig(std::size_t index);

    /// Start the configuration for the extrinsic calibration
    void startExtrinsicConfig(std::size_t index);

    /// Adds a Camera in cameraSet and the CalibrationInfo in the activity
    void addCamera();

    QPointer<QComboBox> m_cameraComboBox;
    QPointer<QPushButton> m_addButton;
    QPointer<QPushButton> m_importButton;
    QPointer<QPushButton> m_removeButton;
    QPointer<QPushButton> m_extrinsicButton;

    service::helper::ConfigLauncher m_intrinsicLauncher;
    service::helper::ConfigLauncher m_extrinsicLauncher;

    data::ptr<data::CameraSet, data::Access::inout> m_camera_set {this, "cameraSet"};
    data::ptr<data::Activity, data::Access::inout> m_activity {this, "activity"};
};

} // namespace sight::module::ui::qt::calibration
