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

#include "data/CameraSet.hpp"

#include "modules/ui/qt/config.hpp"

#include <ui/base/IEditor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QVector>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   SCameraSetEditor service is used to display the extrinsic calibration of a camera series.
 *
 * @subsection Configuration XML

 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::calibration::SCameraSetEditor" >
        <in key="cameraSet" uid="..." />
        <index>...</index>
    </service>
   @endcode

 * @subsection Configuration Configuration
 * - \b index (optional, default: 1): index of the camera in cameraSet used to display extrinsic matrix
 *
 * @subsection Inputs Inputs
 * - \b cameraSet [sight::data::CameraSet]: input cameraSet.
 *
 * @subsection Slots Slots
 * -\b updateInformations(): Updates the informations of the intrinsic calibration.
 */
class MODULE_UI_QT_CLASS_API SCameraSetEditor : public QObject,
                                                public sight::ui::base::IEditor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(SCameraSetEditor, sight::ui::base::IEditor);

    static const core::com::Slots::SlotKeyType s_UPDATE_INFOS_SLOT;
    typedef core::com::Slot<void ()> UpdateInfosSlotType;

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API SCameraSetEditor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~SCameraSetEditor() noexcept
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
    service::IService::KeyConnectionsMap getAutoConnections() const override;

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
    data::ptr<data::CameraSet, data::Access::in> m_camera_set {this, s_CAMERASET, true};
};

} // namespace sight::module::ui::qt::calibration
