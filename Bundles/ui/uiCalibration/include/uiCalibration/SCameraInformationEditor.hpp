/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiCalibration/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <gui/editor/IEditor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>

#include <list>

namespace uiCalibration
{
/**
 * @brief   SCameraInformationEditor service is used to display the intrinsic calibration of a camera.
 *
 * @subsection Configuration XML
 * @code
    <service uid="..." type="::uiCalibration::SCameraInformationEditor" autoConnect="yes">
        <in key="camera" uid="..." />
    </service>
   @endcode
 *
 * @subsection Inputs Inputs
 * - \b camera [::arData::Camera]: camera's information that will be displayed.
 *
 * @subsection Slots Slots
 * - \b updateInformations(): Updates the informations of the intrinsic calibration.
 */
class UICALIBRATION_CLASS_API SCameraInformationEditor : public QObject,
                                                         public ::gui::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceClassDefinitionsMacro((SCameraInformationEditor)(::gui::editor::IEditor))

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_INFOS_SLOT;
    typedef ::fwCom::Slot<void ()> UpdateInfosSlotType;

    /**
     * @brief Constructor.
     */
    UICALIBRATION_API SCameraInformationEditor() noexcept;

    /**
     * @brief Destructor.
     */
    UICALIBRATION_API ~SCameraInformationEditor() noexcept
    {
    }

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    UICALIBRATION_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UICALIBRATION_API void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UICALIBRATION_API void stopping() override;
    /**
     * @brief Updating method : This method is used to update the service.
     */
    UICALIBRATION_API void updating() override
    {
    }

    /**
     * @brief Swapping method : This method is used to swap the service.
     */
    UICALIBRATION_API void swapping() override;

protected:

    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Slot: Updates the informations of the intrinsic calibration.
     */
    void updateInformations();

    /**
     * @brief Clear all the labels.
     */
    void clearLabels();

    /**
     * @brief Label that displays the camera description
     */
    QLabel* m_description;

    /**
     * @brief Label that displays if the camera is calibrated.
     */
    QLabel* m_isCalibrated;

    /**
     * @brief Width of the video.
     */
    QLabel* m_width;

    /**
     * @brief Height of the video.
     */
    QLabel* m_height;

    /**
     * @brief Optical center x.
     */
    QLabel* m_cx;

    /**
     * @brief Optical center y.
     */
    QLabel* m_cy;

    /**
     * @brief Field of view x.
     */
    QLabel* m_fx;

    /**
     * @brief Field of view y.
     */
    QLabel* m_fy;

    /**
     * @brief Distortion coefficient k1.
     */
    QLabel* m_k1;

    /**
     * @brief Distortion coefficient k2.
     */
    QLabel* m_k2;

    /**
     * @brief Distortion coefficient p1.
     */
    QLabel* m_p1;

    /**
     * @brief Distortion coefficient p2.
     */
    QLabel* m_p2;

    /**
     * @brief Distortion coefficient k3.
     */
    QLabel* m_k3;

    /**
     * @brief Camera skew.
     */
    QLabel* m_skew;

};
} // namespace uiCameraSelectorQt
