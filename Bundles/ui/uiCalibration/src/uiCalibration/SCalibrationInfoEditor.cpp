/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SCalibrationInfoEditor.hpp"

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>

#include <map>

namespace uiCalibration
{
fwServicesRegisterMacro(::gui::editor::IEditor, ::uiCalibration::SCalibrationInfoEditor, ::fwData::Object);

const ::fwCom::Slots::SlotKeyType SCalibrationInfoEditor::s_REMOVE_SLOT        = "remove";
const ::fwCom::Slots::SlotKeyType SCalibrationInfoEditor::s_RESET_SLOT         = "reset";
const ::fwCom::Slots::SlotKeyType SCalibrationInfoEditor::s_GET_SELECTION_SLOT = "getSelection";

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
    ::arData::CalibrationInfo::sptr calInfo1 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_1);
    SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

    ::arData::CalibrationInfo::PointListContainerType plList1 = calInfo1->getPointListContainer();

    m_capturesListWidget->clear();

    ::arData::CalibrationInfo::sptr calInfo2 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_2);
    if(calInfo2)
    {
        ::arData::CalibrationInfo::PointListContainerType plList2 = calInfo2->getPointListContainer();

        size_t captureIdx = 0;

        ::arData::CalibrationInfo::PointListContainerType::const_iterator it1, it2;

        for(it1 = plList1.begin(), it2 = plList2.begin(); it1 != plList1.end() && it2 != plList2.end(); ++it1, ++it2 )
        {
            QString countString;
            size_t count1 = (*it1)->getCRefPoints().size();
            size_t count2 = (*it2)->getCRefPoints().size();

            countString = QString("%1. %2 and %3 elements").arg(captureIdx).arg(count1).arg(count2);

            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }
    }
    else
    {
        size_t captureIdx = 0;
        ::arData::CalibrationInfo::PointListContainerType::const_iterator it1;
        for(it1 = plList1.begin(); it1 != plList1.end(); ++it1)
        {
            QString countString;
            size_t count = (*it1)->getCRefPoints().size();
            countString = QString("%1. %2 element%3").arg(captureIdx).arg(count).arg(count > 1 ? "s" : "");

            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::configuring()
{
    fwGui::IGuiContainerSrv::initialize();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::starting()
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

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
    ::fwGui::IGuiContainerSrv::destroy();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::remove()
{
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const size_t idx = static_cast<size_t>(row);

        ::arData::CalibrationInfo::sptr calInfo1 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_1);
        SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

        ::arData::CalibrationInfo::sptr calInfo2 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_2);

        calInfo1->removeRecord(idx);

        //Notify
        {
            auto sig = calInfo1->signal< ::arData::CalibrationInfo::RemovedRecordSignalType >(
                ::arData::CalibrationInfo::s_REMOVED_RECORD_SIG );
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }

        if(calInfo2)
        {
            calInfo2->removeRecord(idx);

            //Notify
            {
                auto sig = calInfo2->signal< ::arData::CalibrationInfo::RemovedRecordSignalType >(
                    ::arData::CalibrationInfo::s_REMOVED_RECORD_SIG );
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::reset()
{
    ::arData::CalibrationInfo::sptr calInfo1 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_1);
    SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

    ::arData::CalibrationInfo::sptr calInfo2 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_2);

    calInfo1->resetRecords();

    //Notify
    {
        auto sig = calInfo1->signal< ::arData::CalibrationInfo::ResetRecordSignalType >(
            ::arData::CalibrationInfo::s_RESET_RECORD_SIG);
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    if(calInfo2)
    {
        calInfo2->resetRecords();

        //Notify
        {
            auto sig = calInfo2->signal< ::arData::CalibrationInfo::ResetRecordSignalType >(
                ::arData::CalibrationInfo::s_RESET_RECORD_SIG);
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }

    m_capturesListWidget->clear();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::getSelection()
{
    int row = m_capturesListWidget->currentRow();

    if(row >= 0)
    {
        const size_t idx = static_cast<size_t>(row);

        ::arData::CalibrationInfo::sptr calInfo1 = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INFO_1);
        SLM_ASSERT("Object "+s_CALIBRATION_INFO_1+" is not a CalibrationInfo !", calInfo1);

        //Notify
        {
            auto sig = calInfo1->signal< ::arData::CalibrationInfo::GetRecordSignalType >(
                ::arData::CalibrationInfo::s_GET_RECORD_SIG);
            sig->asyncEmit(idx);
        }
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::onItemDoubleClicked(QListWidgetItem*)
{
    this->getSelection();
}

// ----------------------------------------------------------------------------

}
