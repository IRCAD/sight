/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SCAMERACONFIGLAUNCHER_HPP__
#define __UICALIBRATION_SCAMERACONFIGLAUNCHER_HPP__

#include "uiCalibration/config.hpp"

#include <arData/CameraSeries.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwServices/helper/ConfigLauncher.hpp>

#include <gui/editor/IEditor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace uiCalibration
{

/**
 * @brief   SCameraConfigLauncher This editors allows calibrate one or more camera.
 *
 * @note Currently, only two cameras are allowed.
 *
 * This service launches an AppConfig for intrinsic camera calibration and another AppConfig for extrinsic calibration.
 */
class UICALIBRATION_CLASS_API SCameraConfigLauncher : public QObject,
                                                      public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SCameraConfigLauncher)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UICALIBRATION_API SCameraConfigLauncher() noexcept;

    /// Destructor. Do nothing.
    UICALIBRATION_API virtual ~SCameraConfigLauncher() noexcept;

protected:

    ///This method launches the IEditor::starting method.
    virtual void starting();

    ///This method launches the IEditor::stopping method.
    virtual void stopping();

    virtual void updating();

    virtual void swapping();

    /**
     * @brief This method is used to configure the service parameters:
     * @code{.xml}
            <registry>
                <parent wid="myView" />
            </registry>
            <config>
                 <intrinsic>
                    <appConfig id="calCameraView" >
                        <parameters>
                            <parameter replace="SERIESDB" by="medicalData"  />
                            <parameter replace="IMAGE" by="@values.image"  />
                        </parameters>
                    </appConfig>
                </intrinsic>
                <extrinsic>
                    <appConfig id="calExtrinsicView" >
                        <parameters>
                            <parameter replace="SERIESDB" by="medicalData"  />
                            <parameter replace="IMAGE" by="@values.image"  />
                        </parameters>
                    </appConfig>
                </extrinsic>
                <cameraSeriesKey>cameraSeries</cameraSeriesKey>
                <activitySeriesKey>activitySeries</activitySeriesKey>
            </config>
       @endcode
     * @note The registry section can be empty.
     *
     * - \b config: contains the configuration to launch for intrinsic or extrinsic calibration.
     *   - \b intrinsic: configuration for intrinsic calibration.
     *     - \b appConfig id: identifier of the AppConfig used for the intrinsic camera calibration.
     *     - \b parameters: parameters of the configuration (pattern to replace).
     *   - \b extrinsic: configuration for intrinsic calibration.
     *     - \b appConfig id: identifier of the AppConfig used for the extrinsic camera calibration.
     *     - \b parameters: parameters of the configuration (pattern to replace).
     *   - \b cameraSeriesKey: key of the cameraSeries, it is used to store camera calibration.
     *   - \b activitySeriesKey: key of the activitySeries, it stores the information used to generate the calibration.
     *        It allows to re-open the activity with this informations.
     *
     * @see fwServices::helper::ConfigLauncher
     */
    virtual void configuring();

private Q_SLOTS:
    void onAddClicked();
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
    QPointer<QPushButton> m_removeButton;
    QPointer<QPushButton> m_extrinsicButton;

    ::fwServices::helper::ConfigLauncher m_intrinsicLauncher;
    ::fwServices::helper::ConfigLauncher m_extrinsicLauncher;

    std::string m_cameraSeriesKey;
    std::string m_activitySeriesKey;

    ::arData::CameraSeries::sptr m_cameraSeries;
    ::fwMedData::ActivitySeries::sptr m_activitySeries;

};

} // namespace uiCalibration

#endif // __UICALIBRATION_SCAMERACONFIGLAUNCHER_HPP__

