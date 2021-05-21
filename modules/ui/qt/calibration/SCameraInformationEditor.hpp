/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <ui/base/IEditor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>

#include <list>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   SCameraInformationEditor service is used to display the intrinsic calibration of a camera.
 *
 * @subsection Configuration XML
 * @code
    <service uid="..." type="sight::module::ui::qt::calibration::SCameraInformationEditor" autoConnect="true">
        <in key="camera" uid="..." />
    </service>
   @endcode
 *
 * @subsection Inputs Inputs
 * - \b camera [sight::data::Camera]: camera's information that will be displayed.
 *
 * @subsection Slots Slots
 * - \b updateInformations(): Updates the informations of the intrinsic calibration.
 */
class MODULE_UI_QT_CLASS_API SCameraInformationEditor : public QObject,
                                                        public sight::ui::base::IEditor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(SCameraInformationEditor, sight::ui::base::IEditor)

    static const core::com::Slots::SlotKeyType s_UPDATE_INFOS_SLOT;
    typedef core::com::Slot<void ()> UpdateInfosSlotType;

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API SCameraInformationEditor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~SCameraInformationEditor() noexcept
    {
    }

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    MODULE_UI_QT_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    MODULE_UI_QT_API void stopping() override;
    /**
     * @brief Updating method : This method is used to update the service.
     */
    MODULE_UI_QT_API void updating() override
    {
    }

    /**
     * @brief Swapping method : This method is used to swap the service.
     */
    MODULE_UI_QT_API void swapping() override;

protected:

    service::IService::KeyConnectionsMap getAutoConnections() const override;

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
