/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "uiCalibration/config.hpp"

#include <arData/CameraSeries.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwServices/helper/ConfigLauncher.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace uiCalibration
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
   <service uid="cameraLauncher" type="::uiCalibration::SCameraConfigLauncher">
       <inout key="cameraSeries" uid="..." />
       <inout key="activitySeries" uid="..." />
       <config>
           <intrinsic>
               <appConfig id="calIntrinsicView" />
               <parameter replace="WID_PARENT" by="calibrationView" />
               <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
           </intrinsic>
           <extrinsic>
               <appConfig id="calExtrinsicView" />
               <inout key="cameraSeries" uid="..." />
               <parameter replace="WID_PARENT" by="calibrationView" />
               <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
           </extrinsic>
       </config>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b cameraSeries [::arData::CameraSeries] : stores camera calibrations.
 * - \b activitySeries [::fwMedData::ActivitySeries]: stores the information used to generate the calibration.
 *        It allows to re-open the activity with this information.
 *
 * @subsection Configuration Configuration:
 * - \b config: contains the appConfigs to launch for intrinsic and extrinsic calibration.
 *   - \b intrinsic: configuration to launch the intrinsic calibration config. @see ::gui::action::SConfigLauncher
 *   - \b extrinsic: configuration to launch the extrinsic calibration config. @see ::gui::action::SConfigLauncher
 *
 */
class UICALIBRATION_CLASS_API SCameraConfigLauncher : public QObject,
                                                      public ::fwGui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SCameraConfigLauncher)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UICALIBRATION_API SCameraConfigLauncher() noexcept;

    /// Destructor. Do nothing.
    UICALIBRATION_API virtual ~SCameraConfigLauncher() noexcept;

protected:

    virtual void configuring() override;

    ///This method launches the IEditor::starting method.
    virtual void starting() override;

    ///This method launches the IEditor::stopping method.
    virtual void stopping() override;

    virtual void updating() override;

    virtual void swapping() override;

private Q_SLOTS:
    void onAddClicked();
    void onImportClicked();
    void onRemoveClicked();
    void onExtrinsicToggled(bool checked);
    void onCameraChanged(int index);

private:

    /// Start the configuration for the intrinsic calibration
    void startIntrinsicConfig(size_t index);

    /// Start the configuration for the extrinsic calibration
    void startExtrinsicConfig(size_t index);

    /// Adds a Camera in cameraSeries and the CalibrationInfo in the activitySeries
    void addCamera();

    QPointer<QComboBox> m_cameraComboBox;
    QPointer<QPushButton> m_addButton;
    QPointer<QPushButton> m_importButton;
    QPointer<QPushButton> m_removeButton;
    QPointer<QPushButton> m_extrinsicButton;

    ::fwServices::helper::ConfigLauncher m_intrinsicLauncher;
    ::fwServices::helper::ConfigLauncher m_extrinsicLauncher;

    ::arData::CameraSeries::sptr m_cameraSeries;
    ::fwMedData::ActivitySeries::sptr m_activitySeries;

};

} // namespace uiCalibration
