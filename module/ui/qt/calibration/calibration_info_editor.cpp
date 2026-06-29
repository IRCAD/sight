/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "module/ui/qt/calibration/calibration_info_editor.hpp"

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>

namespace sight::module::ui::qt::calibration
{

// ----------------------------------------------------------------------------

calibration_info_editor::calibration_info_editor() noexcept
{
    new_slot(slots::REMOVE, &calibration_info_editor::remove, this);
    new_slot(slots::RESET, &calibration_info_editor::reset, this);
    new_slot(slots::GET_SELECTION, &calibration_info_editor::get_selection, this);
}

// ----------------------------------------------------------------------------

void calibration_info_editor::updating()
{
    const auto cal_info1 = m_calibration_info1.lock();
    SIGHT_ASSERT("Object " << CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

    const auto pl_list1 = cal_info1->get_point_list_container();

    m_captures_list_widget->clear();

    const auto cal_info2 = m_calibration_info2.lock();
    if(cal_info2)
    {
        const auto pl_list2 = cal_info2->get_point_list_container();

        std::size_t capture_idx = 0;
        auto it1                = pl_list1.begin();
        auto it2                = pl_list2.begin();

        for( ; it1 != pl_list1.end() && it2 != pl_list2.end() ; ++it1, ++it2)
        {
            QString count_string;
            std::size_t count1 = (*it1)->size();
            std::size_t count2 = (*it2)->size();

            count_string = QString("%1. %2 and %3 elements").arg(capture_idx).arg(count1).arg(count2);

            m_captures_list_widget->addItem(count_string);
            ++capture_idx;
        }

        m_nb_captures_label->setText(QString().setNum(capture_idx));

        if(pl_list1.size() != pl_list2.size())
        {
            const auto* const err_msg = "Left and right calibration input datasets do not have the same size.\n\n"
                                        "Your images may be out of sync.";

            sight::ui::dialog::message::show(
                "Inputs do not match",
                err_msg,
                sight::ui::dialog::message::warning
            );
        }
    }
    else
    {
        std::size_t capture_idx = 0;
        for(const auto& it1 : pl_list1)
        {
            QString count_string;
            std::size_t count = it1->size();
            count_string = QString("%1. %2 element%3").arg(capture_idx).arg(count).arg(count > 1 ? "s" : "");

            m_captures_list_widget->addItem(count_string);
            ++capture_idx;
        }

        m_nb_captures_label->setText(QString().setNum(capture_idx));
    }
}

// ----------------------------------------------------------------------------

void calibration_info_editor::configuring()
{
    sight::ui::service::initialize();
}

// ----------------------------------------------------------------------------

void calibration_info_editor::starting()
{
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    // Creation of the Qt elements

    // Main container, VBox
    auto* v_layout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    auto* nb_items_h_box = new QHBoxLayout();

    //     Fill the nbItemsHBox
    auto* label = new QLabel("nb captures:");
    nb_items_h_box->addWidget(label);

    const QString service_id = QString::fromStdString(base_id());

    m_nb_captures_label = new QLabel("-");
    m_nb_captures_label->setObjectName(service_id + "/nbCapturesLabel");
    nb_items_h_box->addWidget(m_nb_captures_label);
    nb_items_h_box->addStretch();

    //   The ListWidget
    m_captures_list_widget = new QListWidget();
    m_captures_list_widget->setObjectName(service_id + "/capturesListWidget");
    QObject::connect(
        m_captures_list_widget,
        SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this,
        SLOT(on_item_double_clicked(QListWidgetItem*))
    );

    // Fill the main VBox
    v_layout->addLayout(nb_items_h_box);
    v_layout->addWidget(m_captures_list_widget);

    qt_container->set_layout(v_layout);

    this->updating();
}

// ----------------------------------------------------------------------------

void calibration_info_editor::stopping()
{
    sight::ui::service::destroy();
}

// ----------------------------------------------------------------------------

void calibration_info_editor::remove()
{
    int row = m_captures_list_widget->currentRow();

    if(row >= 0)
    {
        const auto idx = static_cast<std::size_t>(row);

        {
            const auto cal_info1 = m_calibration_info1.lock();
            SIGHT_ASSERT("Object " << CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

            const auto cal_info2 = m_calibration_info2.lock();

            cal_info1->remove_record(idx);

            cal_info1->async_emit(this, data::calibration_info::signals::REMOVED_RECORD);

            if(cal_info2)
            {
                cal_info2->remove_record(idx);
                cal_info2->async_emit(this, data::calibration_info::signals::REMOVED_RECORD);
            }
        }
        this->updating();
    }
}

// ----------------------------------------------------------------------------

void calibration_info_editor::reset()
{
    const auto cal_info1 = m_calibration_info1.lock();
    SIGHT_ASSERT("Object " << CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

    const auto cal_info2 = m_calibration_info2.lock();

    cal_info1->reset_records();
    cal_info1->async_emit(this, data::calibration_info::signals::RESET_RECORD);

    if(cal_info2)
    {
        cal_info2->reset_records();

        cal_info2->async_emit(this, data::calibration_info::signals::RESET_RECORD);
    }

    m_captures_list_widget->clear();
    m_nb_captures_label->setText("0");
}

// ----------------------------------------------------------------------------

void calibration_info_editor::get_selection()
{
    int row = m_captures_list_widget->currentRow();

    if(row >= 0)
    {
        const auto idx = static_cast<std::size_t>(row);

        const auto cal_info1 = m_calibration_info1.lock();
        SIGHT_ASSERT("Object " << CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

        //Notify
        {
            cal_info1->async_emit(data::calibration_info::signals::GET_RECORD, idx);
        }
    }
}

// ----------------------------------------------------------------------------

service::connections_t calibration_info_editor::auto_connections() const
{
    connections_t connections;
    connections.push(CALIBRATION_INFO_1, data::signals::MODIFIED, service::slots::UPDATE);
    connections.push(CALIBRATION_INFO_2, data::signals::MODIFIED, service::slots::UPDATE);
    return connections;
}

// ----------------------------------------------------------------------------

void calibration_info_editor::on_item_double_clicked(QListWidgetItem* /*unused*/)
{
    this->get_selection();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
