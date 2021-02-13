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

#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QVector>

#include <ui/base/editor/IEditor.hpp>

namespace sight::modules::ui::qt::calibration
{
/**
 * @brief   SCameraSeriesEditor service is used to display the extrinsic calibration of a camera series.
 *
 * @subsection Configuration XML

 * @code{.xml}
    <service uid="..." type="::sight::modules::ui::qt::calibration::SCameraSeriesEditor" >
        <in key="cameraSeries" uid="..." />
        <index>...</index>
    </service>
   @endcode

 * @subsection Configuration Configuration
 * - \b index (optional, default: 1): index of the camera in cameraSeries used to display extrinsic matrix
 *
 * @subsection Inputs Inputs
 * - \b cameraSeries [sight::data::CameraSeries]: input cameraSeries.
 *
 * @subsection Slots Slots
 * -\b updateInformations(): Updates the informations of the intrinsic calibration.
 */
class MODULE_UI_QT_CLASS_API SCameraSeriesEditor : public QObject,
                                                   public sight::ui::base::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceMacro(SCameraSeriesEditor, sight::ui::base::editor::IEditor)

    static const core::com::Slots::SlotKeyType s_UPDATE_INFOS_SLOT;
    typedef core::com::Slot<void ()> UpdateInfosSlotType;

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API SCameraSeriesEditor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~SCameraSeriesEditor() noexcept
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

    QPointer< QLabel > m_description; ///< description of camera series

    QVector< QPointer< QLabel > > m_matrixLabels; ///< Labels for matrix's elements

    /// Index of the camera in cameraSeries used to display extrinsic matrix.
    size_t m_camIndex;

};
} // namespace sight::modules::ui::qt::calibration
