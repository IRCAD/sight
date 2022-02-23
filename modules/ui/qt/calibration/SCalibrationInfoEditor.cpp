/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/ui/qt/calibration/SCalibrationInfoEditor.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>

#include <map>

namespace sight::module::ui::qt::calibration
{

const core::com::Slots::SlotKeyType SCalibrationInfoEditor::s_REMOVE_SLOT        = "remove";
const core::com::Slots::SlotKeyType SCalibrationInfoEditor::s_RESET_SLOT         = "reset";
const core::com::Slots::SlotKeyType SCalibrationInfoEditor::s_GET_SELECTION_SLOT = "getSelection";

// ----------------------------------------------------------------------------

SCalibrationInfoEditor::SCalibrationInfoEditor() noexcept
{
    newSlot(s_REMOVE_SLOT, &SCalibrationInfoEditor::remove, this);
    newSlot(s_RESET_SLOT, &SCalibrationInfoEditor::reset, this);
    newSlot(s_GET_SELECTION_SLOT, &SCalibrationInfoEditor::getSelection, this);
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::updating()
{
    const auto calInfo1 = m_calibrationInfo1.lock();
    SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", calInfo1);

    const auto plList1 = calInfo1->getPointListContainer();

    m_capturesListWidget->clear();

    const auto calInfo2 = m_calibrationInfo2.lock();
    if(calInfo2)
    {
        const auto plList2 = calInfo2->getPointListContainer();

        std::size_t captureIdx = 0;
        auto it1               = plList1.begin();
        auto it2               = plList2.begin();

        for( ; it1 != plList1.end() && it2 != plList2.end() ; ++it1, ++it2)
        {
            QString countString;
            std::size_t count1 = (*it1)->getPoints().size();
            std::size_t count2 = (*it2)->getPoints().size();

            countString = QString("%1. %2 and %3 elements").arg(captureIdx).arg(count1).arg(count2);

            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }

        m_nbCapturesLabel->setText(QString().setNum(captureIdx));

        if(plList1.size() != plList2.size())
        {
            const auto errMsg = "Left and right calibration input datasets do not have the same size.\n\n"
                                "Your images may be out of sync.";

            sight::ui::base::dialog::MessageDialog::show(
                "Inputs do not match",
                errMsg,
                sight::ui::base::dialog::MessageDialog::WARNING
            );
        }
    }
    else
    {
        std::size_t captureIdx = 0;
        for(auto it1 = plList1.begin() ; it1 != plList1.end() ; ++it1)
        {
            QString countString;
            std::size_t count = (*it1)->getPoints().size();
            countString = QString("%1. %2 element%3").arg(captureIdx).arg(count).arg(count > 1 ? "s" : "");

            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }

        m_nbCapturesLabel->setText(QString().setNum(captureIdx));
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::configuring()
{
    sight::ui::base::IGuiContainer::initialize();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::starting()
{
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    // Creation of the Qt elements

    // Main container, VBox
    QVBoxLayout* vLayout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    QHBoxLayout* nbItemsHBox = new QHBoxLayout();

    //     Fill the nbItemsHBox
    QLabel* label = new QLabel("nb captures:");
    nbItemsHBox->addWidget(label);

    m_nbCapturesLabel = new QLabel("-");
    nbItemsHBox->addWidget(m_nbCapturesLabel);
    nbItemsHBox->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget();
    QObject::connect(
        m_capturesListWidget,
        SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this,
        SLOT(onItemDoubleClicked(QListWidgetItem*))
    );

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    qtContainer->setLayout(vLayout);

    this->updating();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::stopping()
{
    sight::ui::base::IGuiContainer::destroy();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::remove()
{
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const std::size_t idx = static_cast<std::size_t>(row);

        {
            const auto calInfo1 = m_calibrationInfo1.lock();
            SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", calInfo1);

            const auto calInfo2 = m_calibrationInfo2.lock();

            calInfo1->removeRecord(idx);

            //Notify
            {
                auto sig = calInfo1->signal<data::CalibrationInfo::RemovedRecordSignalType>(
                    data::CalibrationInfo::s_REMOVED_RECORD_SIG
                );
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }

            if(calInfo2)
            {
                calInfo2->removeRecord(idx);

                //Notify
                {
                    auto sig = calInfo2->signal<data::CalibrationInfo::RemovedRecordSignalType>(
                        data::CalibrationInfo::s_REMOVED_RECORD_SIG
                    );
                    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
        }
        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::reset()
{
    const auto calInfo1 = m_calibrationInfo1.lock();
    SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", calInfo1);

    const auto calInfo2 = m_calibrationInfo2.lock();

    calInfo1->resetRecords();

    //Notify
    {
        auto sig = calInfo1->signal<data::CalibrationInfo::ResetRecordSignalType>(
            data::CalibrationInfo::s_RESET_RECORD_SIG
        );
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    if(calInfo2)
    {
        calInfo2->resetRecords();

        //Notify
        {
            auto sig = calInfo2->signal<data::CalibrationInfo::ResetRecordSignalType>(
                data::CalibrationInfo::s_RESET_RECORD_SIG
            );
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }

    m_capturesListWidget->clear();
    m_nbCapturesLabel->setText("0");
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::getSelection()
{
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const std::size_t idx = static_cast<std::size_t>(row);

        const auto calInfo1 = m_calibrationInfo1.lock();
        SIGHT_ASSERT("Object " << s_CALIBRATION_INFO_1 << " is not a CalibrationInfo !", calInfo1);

        //Notify
        {
            auto sig = calInfo1->signal<data::CalibrationInfo::GetRecordSignalType>(
                data::CalibrationInfo::s_GET_RECORD_SIG
            );
            sig->asyncEmit(idx);
        }
    }
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SCalibrationInfoEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_CALIBRATION_INFO_1, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_CALIBRATION_INFO_2, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::onItemDoubleClicked(QListWidgetItem*)
{
    this->getSelection();
}

// ----------------------------------------------------------------------------

} // namespace sight::module
