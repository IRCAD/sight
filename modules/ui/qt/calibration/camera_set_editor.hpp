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

#include "data/camera_set.hpp"

#include "modules/ui/qt/config.hpp"

#include <ui/__/editor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QVector>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   camera_set_editor service is used to display the extrinsic calibration of a camera series.
 *
 * @subsection Configuration XML

 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::calibration::camera_set_editor" >
        <in key="cameraSet" uid="..." />
        <index>...</index>
    </service>
   @endcode

 * @subsection Configuration Configuration
 * - \b index (optional, default: 1): index of the camera in cameraSet used to display extrinsic matrix
 *
 * @subsection Inputs Inputs
 * - \b cameraSet [sight::data::camera_set]: input cameraSet.
 *
 * @subsection Slots Slots
 * -\b updateInformations(): Updates the informations of the intrinsic calibration.
 */
class MODULE_UI_QT_CLASS_API camera_set_editor : public QObject,
                                                 public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(camera_set_editor, sight::ui::editor);

    static const core::com::slots::key_t UPDATE_INFOS_SLOT;
    typedef core::com::slot<void ()> UpdateInfosSlotType;

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API camera_set_editor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~camera_set_editor() noexcept
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

protected:

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     */
    service::connections_t auto_connections() const override;

    /**
     * @brief Slot: Updates the informations of the intrinsic calibration.
     */
    void updateInformations();

    /**
     * @brief Clear all the labels.
     */
    void clearLabels();

    QPointer<QLabel> m_description; ///< description of camera series

    QVector<QPointer<QLabel> > m_matrixLabels; ///< Labels for matrix's elements

    /// Index of the camera in cameraSet used to display extrinsic matrix.
    std::size_t m_camIndex;

    static constexpr std::string_view s_CAMERASET = "cameraSet";
    data::ptr<data::camera_set, data::Access::in> m_camera_set {this, s_CAMERASET, true};
};

} // namespace sight::module::ui::qt::calibration
