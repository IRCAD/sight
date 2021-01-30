/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "uiCalibration/SCalibrationInfoEditor.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/CalibrationInfo.hpp>
#include <data/mt/ObjectReadLock.hpp>

#include <services/macros.hpp>

#include <QHBoxLayout>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <map>

namespace uiCalibration
{
fwServicesRegisterMacro(ui::base::editor::IEditor, ::uiCalibration::SCalibrationInfoEditor)

const core::com::Slots::SlotKeyType SCalibrationInfoEditor::s_REMOVE_SLOT = "remove";
const core::com::Slots::SlotKeyType SCalibrationInfoEditor::s_RESET_SLOT         = "reset";
const core::com::Slots::SlotKeyType SCalibrationInfoEditor::s_GET_SELECTION_SLOT = "getSelection";

static const std::string s_CALIBRATION_INFO_1 = "calInfo1";
static const std::string s_CALIBRATION_INFO_2 = "calInfo2";

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
    data::CalibrationInfo::sptr calInfo1 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_1);
    SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);
    data::mt::ObjectReadLock calib1Lock(calInfo1);

    data::CalibrationInfo::PointListContainerType plList1 = calInfo1->getPointListContainer();

    m_capturesListWidget->clear();

    data::CalibrationInfo::sptr calInfo2 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_2);
    if(calInfo2)
    {
        data::mt::ObjectReadLock calib2Lock(calInfo2);
        data::CalibrationInfo::PointListContainerType plList2 = calInfo2->getPointListContainer();

        size_t captureIdx = 0;

        data::CalibrationInfo::PointListContainerType::const_iterator it1, it2;

        for(it1 = plList1.begin(), it2 = plList2.begin(); it1 != plList1.end() && it2 != plList2.end(); ++it1, ++it2 )
        {
            QString countString;
            size_t count1 = (*it1)->getPoints().size();
            size_t count2 = (*it2)->getPoints().size();

            countString = QString("%1. %2 and %3 elements").arg(captureIdx).arg(count1).arg(count2);

            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }

        m_nbCapturesLabel->setText(QString().setNum(captureIdx));

        if(plList1.size() != plList2.size())
        {
            const auto errMsg = "Left and right calibration input datasets do not have the same size.\n\n"
                                "Your images may be out of sync.";

            sight::ui::base::dialog::MessageDialog::show("Inputs do not match",
                                                         errMsg, sight::ui::base::dialog::MessageDialog::WARNING);
        }
    }
    else
    {
        size_t captureIdx = 0;
        data::CalibrationInfo::PointListContainerType::const_iterator it1;
        for(it1 = plList1.begin(); it1 != plList1.end(); ++it1)
        {
            QString countString;
            size_t count = (*it1)->getPoints().size();
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
    ui::base::IGuiContainerSrv::initialize();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::starting()
{
    ui::base::IGuiContainerSrv::create();
    ui::qt::container::QtContainer::sptr qtContainer = ui::qt::container::QtContainer::dynamicCast(getContainer());

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
    QObject::connect(m_capturesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
                     SLOT(onItemDoubleClicked(QListWidgetItem*)));

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    qtContainer->setLayout(vLayout);

    this->updating();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::stopping()
{
    ui::base::IGuiContainerSrv::destroy();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::remove()
{
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const size_t idx = static_cast<size_t>(row);

        data::CalibrationInfo::sptr calInfo1 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_1);
        SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

        data::CalibrationInfo::sptr calInfo2 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_2);

        calInfo1->removeRecord(idx);

        //Notify
        {
            auto sig = calInfo1->signal< data::CalibrationInfo::RemovedRecordSignalType >(
                data::CalibrationInfo::s_REMOVED_RECORD_SIG );
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }

        if(calInfo2)
        {
            calInfo2->removeRecord(idx);

            //Notify
            {
                auto sig = calInfo2->signal< data::CalibrationInfo::RemovedRecordSignalType >(
                    data::CalibrationInfo::s_REMOVED_RECORD_SIG );
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::reset()
{
    data::CalibrationInfo::sptr calInfo1 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_1);
    SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

    data::CalibrationInfo::sptr calInfo2 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_2);

    calInfo1->resetRecords();

    //Notify
    {
        auto sig = calInfo1->signal< data::CalibrationInfo::ResetRecordSignalType >(
            data::CalibrationInfo::s_RESET_RECORD_SIG);
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    if(calInfo2)
    {
        calInfo2->resetRecords();

        //Notify
        {
            auto sig = calInfo2->signal< data::CalibrationInfo::ResetRecordSignalType >(
                data::CalibrationInfo::s_RESET_RECORD_SIG);
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
        const size_t idx = static_cast<size_t>(row);

        data::CalibrationInfo::sptr calInfo1 = this->getInOut< data::CalibrationInfo >(s_CALIBRATION_INFO_1);
        SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

        //Notify
        {
            auto sig = calInfo1->signal< data::CalibrationInfo::GetRecordSignalType >(
                data::CalibrationInfo::s_GET_RECORD_SIG);
            sig->asyncEmit(idx);
        }
    }
}

// ----------------------------------------------------------------------------

services::IService::KeyConnectionsMap SCalibrationInfoEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_CALIBRATION_INFO_1, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_CALIBRATION_INFO_2, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::onItemDoubleClicked(QListWidgetItem*)
{
    this->getSelection();
}

// ----------------------------------------------------------------------------

}
