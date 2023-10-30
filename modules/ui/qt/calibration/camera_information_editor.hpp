/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/camera.hpp>

#include <ui/__/editor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>

#include <list>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   camera_information_editor service is used to display the intrinsic calibration of a camera.
 *
 * @subsection Configuration XML
 * @code
    <service uid="..." type="sight::module::ui::qt::calibration::camera_information_editor" auto_connect="true">
        <in key="camera" uid="..." />
    </service>
   @endcode
 *
 * @subsection Inputs Inputs
 * - \b camera [sight::data::camera]: camera's information that will be displayed.
 *
 * @subsection Slots Slots
 * - \b updateInformations(): Updates the informations of the intrinsic calibration.
 */
class MODULE_UI_QT_CLASS_API camera_information_editor : public QObject,
                                                         public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(camera_information_editor, sight::ui::editor);

    static const core::com::slots::key_t UPDATE_INFOS_SLOT;
    using update_infos_slot_t = core::com::slot<void ()>;

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API camera_information_editor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~camera_information_editor() noexcept override =
        default;

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

protected:

    service::connections_t auto_connections() const override;

    /**
     * @brief Slot: Updates the informations of the intrinsic calibration.
     */
    void update_informations();

    /**
     * @brief Clear all the labels.
     */
    void clear_labels();

    /**
     * @brief Label that displays the camera description
     */
    QLabel* m_description {};

    /**
     * @brief Label that displays if the camera is calibrated.
     */
    QLabel* m_is_calibrated {};

    /**
     * @brief Width of the video.
     */
    QLabel* m_width {};

    /**
     * @brief Height of the video.
     */
    QLabel* m_height {};

    /**
     * @brief Optical center x.
     */
    QLabel* m_cx {};

    /**
     * @brief Optical center y.
     */
    QLabel* m_cy {};

    /**
     * @brief field of view x.
     */
    QLabel* m_fx {};

    /**
     * @brief field of view y.
     */
    QLabel* m_fy {};

    /**
     * @brief Distortion coefficient k1.
     */
    QLabel* m_k1 {};

    /**
     * @brief Distortion coefficient k2.
     */
    QLabel* m_k2 {};

    /**
     * @brief Distortion coefficient p1.
     */
    QLabel* m_p1 {};

    /**
     * @brief Distortion coefficient p2.
     */
    QLabel* m_p2 {};

    /**
     * @brief Distortion coefficient k3.
     */
    QLabel* m_k3 {};

    /**
     * @brief Camera skew.
     */
    QLabel* m_skew {};

    static constexpr std::string_view CAMERA = "camera";
    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA, true};
};

} // namespace sight::module::ui::qt::calibration
