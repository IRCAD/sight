/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SCalibrationInfoEditor.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/IController.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <arData/CalibrationInfo.hpp>

#include <QHBoxLayout>
#include <map>

namespace uiCalibration
{
fwServicesRegisterMacro(::gui::editor::IEditor, ::uiCalibration::SCalibrationInfoEditor, ::fwData::Composite);

const ::fwCom::Slots::SlotKeyType SCalibrationInfoEditor::s_REMOVE_SLOT        = "remove";
const ::fwCom::Slots::SlotKeyType SCalibrationInfoEditor::s_RESET_SLOT         = "reset";
const ::fwCom::Slots::SlotKeyType SCalibrationInfoEditor::s_GET_SELECTION_SLOT = "getSelection";

// ----------------------------------------------------------------------------

SCalibrationInfoEditor::SCalibrationInfoEditor() throw () : m_calInfo1Key(""),
                                                            m_calInfo2Key("")
{
    m_slotRemove       = ::fwCom::newSlot( &SCalibrationInfoEditor::remove, this );
    m_slotReset        = ::fwCom::newSlot( &SCalibrationInfoEditor::reset, this );
    m_slotGetSelection = ::fwCom::newSlot( &SCalibrationInfoEditor::getSelection, this );

    ::fwCom::HasSlots::m_slots( s_REMOVE_SLOT, m_slotRemove);
    ::fwCom::HasSlots::m_slots( s_RESET_SLOT, m_slotReset);
    ::fwCom::HasSlots::m_slots( s_GET_SELECTION_SLOT, m_slotGetSelection);



    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}


// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::updating() throw (fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::Composite::ConstIteratorType cmpIt1 = composite->find(m_calInfo1Key);

    SLM_ASSERT("Key : "+m_calInfo1Key+" not found !",cmpIt1 != composite->end());

    ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(cmpIt1->second);

    SLM_ASSERT("Object "+m_calInfo1Key+" is not a CalibrationInfo !",calInfo1);

    ::arData::CalibrationInfo::PointListContainerType plList1 = calInfo1->getPointListContainer();

    m_capturesListWidget->clear();

    if(m_calInfo2Key != "")
    {
        ::fwData::Composite::ConstIteratorType cmpIt2 = composite->find(m_calInfo2Key);
        SLM_ASSERT("Key : "+m_calInfo2Key+" not found !",cmpIt2 != composite->end());
        ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::dynamicCast(cmpIt2->second);
        SLM_ASSERT("Object "+m_calInfo2Key+" is not a CalibrationInfo !",calInfo2);

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

void SCalibrationInfoEditor::configuring() throw (fwTools::Failed)
{
    fwGui::IGuiContainerSrv::initialize();

    SLM_ASSERT("Parameter CalInfo1 are not found in xml", m_configuration->findConfigurationElement("CalInfo1") );
    m_calInfo1Key = m_configuration->findConfigurationElement("CalInfo1")->getValue();

    if(m_configuration->findConfigurationElement("CalInfo2"))
    {
        m_calInfo2Key = m_configuration->findConfigurationElement("CalInfo2")->getValue();
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::starting() throw (fwTools::Failed)
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();

    // Creation of the Qt elements

    // Main container, VBox
    QVBoxLayout* vLayout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    QHBoxLayout* nbItemsHBox = new QHBoxLayout();

    //     Fill the nbItemsHBox
    QLabel* label = new QLabel("nb captures:", container);
    nbItemsHBox->addWidget(label);

    m_nbCapturesLabel = new QLabel("-", container);
    nbItemsHBox->addWidget(m_nbCapturesLabel);
    nbItemsHBox->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget(container);
    QObject::connect(m_capturesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
                     SLOT(onItemDoubleClicked(QListWidgetItem*)));

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    container->setLayout(vLayout);

    this->updating();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::stopping() throw (fwTools::Failed)
{
    this->getContainer()->clean();
    ::fwGui::IGuiContainerSrv::destroy();
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::receiving(fwServices::ObjectMsg::csptr msg) throw (fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::remove()
{
    int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        ::fwData::Composite::ConstIteratorType cmpIt1 = composite->find(m_calInfo1Key);
        SLM_ASSERT("Key : "+m_calInfo1Key+" not found !",cmpIt1 != composite->end());
        ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(cmpIt1->second);
        SLM_ASSERT("Object "+m_calInfo1Key+" is not a CalibrationInfo !",calInfo1);

        calInfo1->removeRecord(idx);

        //Notify
        {
            ::arData::CalibrationInfo::RemovedRecordSignalType::sptr sig1;
            sig1 = calInfo1->signal< ::arData::CalibrationInfo::RemovedRecordSignalType >(
                ::arData::CalibrationInfo::s_REMOVED_RECORD_SIG );
            ::fwCom::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
            sig1->asyncEmit();
        }

        if(m_calInfo2Key != "")
        {
            ::fwData::Composite::ConstIteratorType cmpIt2 = composite->find(m_calInfo2Key);
            SLM_ASSERT("Key : "+m_calInfo2Key+" not found !",cmpIt2 != composite->end());

            ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::dynamicCast(cmpIt2->second);
            SLM_ASSERT("Object "+m_calInfo1Key+" is not a CalibrationInfo !",calInfo2);

            calInfo2->removeRecord(idx);

            //Notify
            {
                ::arData::CalibrationInfo::RemovedRecordSignalType::sptr sig2;
                sig2 = calInfo2->signal< ::arData::CalibrationInfo::RemovedRecordSignalType >(
                    ::arData::CalibrationInfo::s_REMOVED_RECORD_SIG );
                ::fwCom::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
                sig2->asyncEmit();
            }
        }

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::reset()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::Composite::ConstIteratorType cmpIt1 = composite->find(m_calInfo1Key);
    SLM_ASSERT("Key : "+m_calInfo1Key+" not found !",cmpIt1 != composite->end());

    ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(cmpIt1->second);
    SLM_ASSERT("Object "+m_calInfo1Key+" is not a CalibrationInfo !",calInfo1);

    calInfo1->resetRecords();

    //Notify
    {
        ::arData::CalibrationInfo::ResetRecordSignalType::sptr sig1;
        sig1 = calInfo1->signal< ::arData::CalibrationInfo::ResetRecordSignalType >(
            ::arData::CalibrationInfo::s_RESET_RECORD_SIG);
        ::fwCom::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
        sig1->asyncEmit();
    }

    if(m_calInfo2Key != "")
    {
        ::fwData::Composite::ConstIteratorType cmpIt2 = composite->find(m_calInfo2Key);
        SLM_ASSERT("Key : "+m_calInfo2Key+" not found !",cmpIt2 != composite->end());

        ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::dynamicCast(cmpIt2->second);
        SLM_ASSERT("Object "+m_calInfo1Key+" is not a CalibrationInfo !",calInfo2);

        calInfo2->resetRecords();

        //Notify
        {
            ::arData::CalibrationInfo::ResetRecordSignalType::sptr sig2;
            sig2 = calInfo2->signal< ::arData::CalibrationInfo::ResetRecordSignalType >(
                ::arData::CalibrationInfo::s_RESET_RECORD_SIG);
            ::fwCom::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
            sig2->asyncEmit();
        }
    }

    m_capturesListWidget->clear();
}


// ----------------------------------------------------------------------------

void SCalibrationInfoEditor::getSelection()
{
    int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        ::fwData::Composite::ConstIteratorType cmpIt1 = composite->find(m_calInfo1Key);
        SLM_ASSERT("Key : "+m_calInfo1Key+" not found !",cmpIt1 != composite->end());

        ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(cmpIt1->second);
        SLM_ASSERT("Object "+m_calInfo1Key+" is not a CalibrationInfo !",calInfo1);

        //Notify
        {
            ::arData::CalibrationInfo::GetRecordSignalType::sptr sig;
            sig = calInfo1->signal< ::arData::CalibrationInfo::GetRecordSignalType >(
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
