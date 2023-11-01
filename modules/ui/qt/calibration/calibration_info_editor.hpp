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

#include <data/calibration_info.hpp>
#include <data/point_list.hpp>

#include <ui/__/editor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   calibration_info_editor service is used to handle the calibration points acquisition.
 * *
 * @section Slots Slots
 * - \b remove() : removes the current selected image.
 * - \b reset() : clears all the calibration information.
 * - \b get_selection() : emits the CalibrationInfo signal 'getRecord(index)' with the current selection index.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::calibration::display_calibration_info">
       <inout key="calInfo1" uid"..." />
       <inout key="calInfo2" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b calInfo1 [sight::data::calibration_info]: calibration information for first camera.
 * - \b calInfo2 [sight::data::calibration_info] (optional): calibration information for optional second camera.
 */
class MODULE_UI_QT_CLASS_API calibration_info_editor : public QObject,
                                                       public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(calibration_info_editor, sight::ui::editor);

    /**
     * @name Slots API
     * @{
     */
    MODULE_UI_QT_API static const core::com::slots::key_t REMOVE_SLOT;
    MODULE_UI_QT_API static const core::com::slots::key_t RESET_SLOT;
    MODULE_UI_QT_API static const core::com::slots::key_t GET_SELECTION_SLOT;
    ///@}

    /**
     * @brief Constructor.
     */
    MODULE_UI_QT_API calibration_info_editor() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_UI_QT_API ~calibration_info_editor() noexcept override =
        default;

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
    MODULE_UI_QT_API void get_selection();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     */
    MODULE_UI_QT_API connections_t auto_connections() const override;

private Q_SLOTS:

    /**
     * @brief Slot called when an element is double-clicked in the list widget.
     */
    void on_item_double_clicked(QListWidgetItem* /*unused*/);

private:

    /**
     * @brief Index of the last acquisition.
     */
    int m_capture_idx {};

    /**
     * @brief Label displaying the number of point acquisitions.
     */
    QPointer<QLabel> m_nb_captures_label;

    /**
     * @brief Calibration point list.
     */
    QPointer<QListWidget> m_captures_list_widget;

    static constexpr std::string_view CALIBRATION_INFO_1 = "calInfo1";
    static constexpr std::string_view CALIBRATION_INFO_2 = "calInfo2";
    data::ptr<data::calibration_info, data::access::inout> m_calibration_info1 {this, CALIBRATION_INFO_1, true};
    data::ptr<data::calibration_info, data::access::inout> m_calibration_info2 {this, CALIBRATION_INFO_2, true, true};
};

} // namespace sight::module::ui::qt::calibration
