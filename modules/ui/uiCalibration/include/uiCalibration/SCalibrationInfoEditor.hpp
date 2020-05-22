/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "uiCalibration/config.hpp"

#include <fwData/List.hpp>
#include <fwData/PointList.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

namespace uiCalibration
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
   <service type="::uiCalibration::SDisplayCalibrationInfo">
       <inout key="calInfo1" uid"..." />
       <inout key="calInfo2" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [::arData::CalibrationInfo]: calibration information for first camera.
 * - \b calInfo2 [::arData::CalibrationInfo] (optional): calibration information for optional second camera.
 */
class UICALIBRATION_CLASS_API SCalibrationInfoEditor : public QObject,
                                                       public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceMacro(SCalibrationInfoEditor, ::fwGui::editor::IEditor);

    /**
     * @name Slots API
     * @{
     */
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_RESET_SLOT;
    UICALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_GET_SELECTION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    UICALIBRATION_API SCalibrationInfoEditor() noexcept;

    /**
     * @brief Destructor.
     */
    UICALIBRATION_API ~SCalibrationInfoEditor() noexcept
    {
    }

protected:

    /// Initializes the editor
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
    UICALIBRATION_API void updating() override;

    /**
     * @brief Slot: removes the current selected image.
     */
    UICALIBRATION_API void remove();

    /**
     * @brief Slot:clears all the calibration information.
     */
    UICALIBRATION_API void reset();

    /**
     * @brief Slot: emits the CalibrationInfo signal 'getRecord(index)' with the current selection index.
     */
    UICALIBRATION_API void getSelection();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     */
    UICALIBRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

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
} // namespace uiCalibration
