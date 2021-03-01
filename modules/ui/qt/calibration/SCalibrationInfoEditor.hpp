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

#include <data/List.hpp>
#include <data/PointList.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::calibration
{
/**
 * @brief   SCalibrationInfoEditor service is used to handle the calibration points acquisition.
 * *
 * @section Slots Slots
 * - \b remove() : removes the current selected image.
 * - \b reset() : clears all the calibration information.
 * - \b getSelection() : emits the CalibrationInfo signal 'getRecord(index)' with the current selection index.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::sight::module::ui::qt::calibration::SDisplayCalibrationInfo">
       <inout key="calInfo1" uid"..." />
       <inout key="calInfo2" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [sight::data::CalibrationInfo]: calibration information for first camera.
 * - \b calInfo2 [sight::data::CalibrationInfo] (optional): calibration information for optional second camera.
 */
class MODULE_UI_QT_CLASS_API SCalibrationInfoEditor : public QObject,
                                                      public sight::ui::base::IEditor
{
Q_OBJECT;

public:
    SIGHT_DECLARE_SERVICE(SCalibrationInfoEditor, sight::ui::base::IEditor)

    /**
     * @name Slots API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_REMOVE_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_RESET_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_GET_SELECTION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API SCalibrationInfoEditor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~SCalibrationInfoEditor() noexcept
    {
    }

protected:

    /// Initializes the editor
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
    MODULE_UI_QT_API void updating() override;

    /**
     * @brief Slot: removes the current selected image.
     */
    MODULE_UI_QT_API void remove();

    /**
     * @brief Slot:clears all the calibration information.
     */
    MODULE_UI_QT_API void reset();

    /**
     * @brief Slot: emits the CalibrationInfo signal 'getRecord(index)' with the current selection index.
     */
    MODULE_UI_QT_API void getSelection();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     */
    MODULE_UI_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

private Q_SLOTS:
    /**
     * @brief Slot called when an element is double-clicked in the list widget.
     */
    void onItemDoubleClicked(QListWidgetItem*);

private:

    /**
     * @brief Index of the last acquisition.
     */
    int m_captureIdx;

    /**
     * @brief Label displaying the number of point acquisitions.
     */
    QPointer< QLabel > m_nbCapturesLabel;

    /**
     * @brief Calibration point list.
     */
    QPointer< QListWidget > m_capturesListWidget;
};
} // namespace sight::module::ui::qt::calibration
