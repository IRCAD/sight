/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/ui/qt/calibration/calibration_info_editor.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>

#include <map>

namespace sight::module::ui::qt::calibration
{

const core::com::slots::key_t calibration_info_editor::REMOVE_SLOT        = "remove";
const core::com::slots::key_t calibration_info_editor::RESET_SLOT         = "reset";
const core::com::slots::key_t calibration_info_editor::GET_SELECTION_SLOT = "getSelection";

// ----------------------------------------------------------------------------

calibration_info_editor::calibration_info_editor() noexcept
{
    new_slot(REMOVE_SLOT, &calibration_info_editor::remove, this);
    new_slot(RESET_SLOT, &calibration_info_editor::reset, this);
    new_slot(GET_SELECTION_SLOT, &calibration_info_editor::getSelection, this);
}

// ----------------------------------------------------------------------------

void calibration_info_editor::updating()
{
    const auto cal_info1 = m_calibrationInfo1.lock();
    SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

    const auto pl_list1 = cal_info1->getPointListContainer();

    m_capturesListWidget->clear();

    const auto cal_info2 = m_calibrationInfo2.lock();
    if(cal_info2)
    {
        const auto pl_list2 = cal_info2->getPointListContainer();

        std::size_t capture_idx = 0;
        auto it1                = pl_list1.begin();
        auto it2                = pl_list2.begin();

        for( ; it1 != pl_list1.end() && it2 != pl_list2.end() ; ++it1, ++it2)
        {
            QString count_string;
            std::size_t count1 = (*it1)->getPoints().size();
            std::size_t count2 = (*it2)->getPoints().size();

            count_string = QString("%1. %2 and %3 elements").arg(capture_idx).arg(count1).arg(count2);

            m_capturesListWidget->addItem(count_string);
            ++capture_idx;
        }

        m_nbCapturesLabel->setText(QString().setNum(capture_idx));

        if(pl_list1.size() != pl_list2.size())
        {
            const auto* const err_msg = "Left and right calibration input datasets do not have the same size.\n\n"
                                        "Your images may be out of sync.";

            sight::ui::dialog::message::show(
                "Inputs do not match",
                err_msg,
                sight::ui::dialog::message::WARNING
            );
        }
    }
    else
    {
        std::size_t capture_idx = 0;
        for(const auto& it1 : pl_list1)
        {
            QString count_string;
            std::size_t count = it1->getPoints().size();
            count_string = QString("%1. %2 element%3").arg(capture_idx).arg(count).arg(count > 1 ? "s" : "");

            m_capturesListWidget->addItem(count_string);
            ++capture_idx;
        }

        m_nbCapturesLabel->setText(QString().setNum(capture_idx));
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
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    // Creation of the Qt elements

    // Main container, VBox
    auto* v_layout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    auto* nb_items_h_box = new QHBoxLayout();

    //     Fill the nbItemsHBox
    auto* label = new QLabel("nb captures:");
    nb_items_h_box->addWidget(label);

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    m_nbCapturesLabel = new QLabel("-");
    m_nbCapturesLabel->setObjectName(service_id + "/nbCapturesLabel");
    nb_items_h_box->addWidget(m_nbCapturesLabel);
    nb_items_h_box->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget();
    m_capturesListWidget->setObjectName(service_id + "/capturesListWidget");
    QObject::connect(
        m_capturesListWidget,
        SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this,
        SLOT(onItemDoubleClicked(QListWidgetItem*))
    );

    // Fill the main VBox
    v_layout->addLayout(nb_items_h_box);
    v_layout->addWidget(m_capturesListWidget);

    qt_container->setLayout(v_layout);

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
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const auto idx = static_cast<std::size_t>(row);

        {
            const auto cal_info1 = m_calibrationInfo1.lock();
            SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

            const auto cal_info2 = m_calibrationInfo2.lock();

            cal_info1->removeRecord(idx);

            //Notify
            {
                auto sig = cal_info1->signal<data::calibration_info::removed_record_signal_t>(
                    data::calibration_info::REMOVED_RECORD_SIG
                );
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }

            if(cal_info2)
            {
                cal_info2->removeRecord(idx);

                //Notify
                {
                    auto sig = cal_info2->signal<data::calibration_info::removed_record_signal_t>(
                        data::calibration_info::REMOVED_RECORD_SIG
                    );
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }
        }
        this->updating();
    }
}

// ----------------------------------------------------------------------------

void calibration_info_editor::reset()
{
    const auto cal_info1 = m_calibrationInfo1.lock();
    SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

    const auto cal_info2 = m_calibrationInfo2.lock();

    cal_info1->resetRecords();

    //Notify
    {
        auto sig = cal_info1->signal<data::calibration_info::Reset_record_signal_type>(
            data::calibration_info::RESET_RECORD_SIG
        );
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }

    if(cal_info2)
    {
        cal_info2->resetRecords();

        //Notify
        {
            auto sig = cal_info2->signal<data::calibration_info::Reset_record_signal_type>(
                data::calibration_info::RESET_RECORD_SIG
            );
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }

    m_capturesListWidget->clear();
    m_nbCapturesLabel->setText("0");
}

// ----------------------------------------------------------------------------

void calibration_info_editor::getSelection()
{
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const auto idx = static_cast<std::size_t>(row);

        const auto cal_info1 = m_calibrationInfo1.lock();
        SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", cal_info1);

        //Notify
        {
            auto sig = cal_info1->signal<data::calibration_info::get_record_signal_t>(
                data::calibration_info::GET_RECORD_SIG
            );
            sig->async_emit(idx);
        }
    }
}

// ----------------------------------------------------------------------------

service::connections_t calibration_info_editor::auto_connections() const
{
    connections_t connections;
    connections.push(s_CALIBRATION_INFO_1, data::object::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_CALIBRATION_INFO_2, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

// ----------------------------------------------------------------------------

void calibration_info_editor::onItemDoubleClicked(QListWidgetItem* /*unused*/)
{
    this->getSelection();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
