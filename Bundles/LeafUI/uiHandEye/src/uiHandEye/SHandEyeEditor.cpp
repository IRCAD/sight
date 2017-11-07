/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiHandEye/SHandEyeEditor.hpp"

#include <arData/BufferTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/List.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QDialog>
#include <QHBoxLayout>

#include <map>

namespace uiHandEye
{
fwServicesRegisterMacro(::gui::editor::IEditor, ::uiHandEye::SHandEyeEditor);

const ::fwCom::Slots::SlotKeyType SHandEyeEditor::s_ADD_SLOT           = "add";
const ::fwCom::Slots::SlotKeyType SHandEyeEditor::s_REMOVE_SLOT        = "remove";
const ::fwCom::Slots::SlotKeyType SHandEyeEditor::s_RESET_SLOT         = "reset";
const ::fwCom::Slots::SlotKeyType SHandEyeEditor::s_GET_SELECTION_SLOT = "getSelection";

static const ::fwServices::IService::KeyType s_MATRIXLIST1_INOUT = "matrixList1";
static const ::fwServices::IService::KeyType s_MATRIXLIST2_INOUT = "matrixList2";

static const ::fwServices::IService::KeyType s_MATRIXTL1_INPUT = "matrixTL1";
static const ::fwServices::IService::KeyType s_MATRIXTL2_INPUT = "matrixTL2";

// ----------------------------------------------------------------------------

SHandEyeEditor::SHandEyeEditor() noexcept
{
    newSlot( s_ADD_SLOT,  &SHandEyeEditor::add, this );
    newSlot( s_REMOVE_SLOT, &SHandEyeEditor::remove, this );
    newSlot( s_RESET_SLOT,  &SHandEyeEditor::reset, this );
    newSlot( s_GET_SELECTION_SLOT, &SHandEyeEditor::getSelection, this );

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::updating()
{
    ::fwData::List::sptr matrixList1 = this->getInOut< ::fwData::List>(s_MATRIXLIST1_INOUT);
    ::fwData::List::sptr matrixList2 = this->getInOut< ::fwData::List>(s_MATRIXLIST2_INOUT);

    m_capturesListWidget->clear();

    const size_t size1 = matrixList1->getContainer().size();
    const size_t size2 = matrixList1->getContainer().size();

    SLM_ASSERT("The two ::fwData::List have not the same size", size1 == size2);

    for(size_t i = 0; i < size1; ++i )
    {
        QString countString;
        countString = QString("%1. Matrix 1 and Matrix 2").arg(i);
        m_capturesListWidget->addItem(countString);
    }
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::configuring()
{
    fwGui::IGuiContainerSrv::initialize();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::starting()
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

void SHandEyeEditor::stopping()
{
    this->destroy();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::add()
{
    ::fwData::List::sptr matrixList1 = this->getInOut< ::fwData::List>(s_MATRIXLIST1_INOUT);
    ::fwData::List::sptr matrixList2 = this->getInOut< ::fwData::List>(s_MATRIXLIST2_INOUT);

    if(this->isStarted())
    {

        // Get the timelines
        ::arData::MatrixTL::csptr matrixTL1 = this->getInput< ::arData::MatrixTL>(s_MATRIXTL1_INPUT);
        ::arData::MatrixTL::csptr matrixTL2 = this->getInput< ::arData::MatrixTL>(s_MATRIXTL2_INPUT);

        const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

        if (timestamp > m_lastTimestamp)
        {
            ::fwCore::HiResClock::HiResClockType newerTimestamp =
                std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

            newerTimestamp  = std::min(timestamp, newerTimestamp);
            m_lastTimestamp = newerTimestamp;

            const CSPTR(::arData::MatrixTL::BufferType) buffer1 = matrixTL1->getClosestBuffer(newerTimestamp);

            const CSPTR(::arData::MatrixTL::BufferType) buffer2 = matrixTL2->getClosestBuffer(newerTimestamp);

            if(buffer1 && buffer2)
            {

                const float* matBuffer1 = buffer1->getElement(0);
                const float* matBuffer2 = buffer2->getElement(0);

                ::fwData::TransformationMatrix3D::sptr matrix1 = ::fwData::TransformationMatrix3D::New();
                ::fwData::TransformationMatrix3D::sptr matrix2 = ::fwData::TransformationMatrix3D::New();

                for(unsigned int i = 0; i < 4; ++i)
                {
                    for(unsigned int j = 0; j < 4; ++j)
                    {
                        matrix1->setCoefficient(i, j, static_cast<double>(matBuffer1[4*i+j]));
                        matrix2->setCoefficient(i, j, static_cast<double>(matBuffer2[4*i+j]));
                    }
                }

                matrixList1->getContainer().push_back(matrix1);
                matrixList2->getContainer().push_back(matrix2);

                // send data modified signals
                {
                    auto sig1 = matrixList1->signal< ::fwData::List::ModifiedSignalType >(
                        ::fwData::List::s_MODIFIED_SIG);
                    ::fwCom::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
                    sig1->asyncEmit();
                }

                {
                    auto sig2 = matrixList2->signal< ::fwData::List::ModifiedSignalType >(
                        ::fwData::List::s_MODIFIED_SIG);
                    ::fwCom::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
                    sig2->asyncEmit();
                }
            }
        }
    }

    this->updating();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::remove()
{
    const int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        ::fwData::List::sptr matrixList1 = this->getInOut< ::fwData::List>(s_MATRIXLIST1_INOUT);
        ::fwData::List::sptr matrixList2 = this->getInOut< ::fwData::List>(s_MATRIXLIST2_INOUT);

        ::fwData::List::IteratorType it1 = matrixList1->begin();
        ::fwData::List::IteratorType it2 = matrixList2->begin();

        std::advance(it1, idx);
        std::advance(it2, idx);

        if(it1 != matrixList1->end() &&
           it2 != matrixList2->end())
        {
            matrixList1->getContainer().remove(*it1);
            matrixList2->getContainer().remove(*it2);

            // send data modified signals

            {
                auto sig1 = matrixList1->signal< ::fwData::List::ModifiedSignalType >( ::fwData::List::s_MODIFIED_SIG);
                ::fwCom::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
                sig1->asyncEmit();
            }

            {
                auto sig2 = matrixList2->signal< ::fwData::List::ModifiedSignalType >( ::fwData::List::s_MODIFIED_SIG);
                ::fwCom::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
                sig2->asyncEmit();
            }
        }

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::reset()
{
    ::fwData::List::sptr matrixList1 = this->getInOut< ::fwData::List>(s_MATRIXLIST1_INOUT);
    ::fwData::List::sptr matrixList2 = this->getInOut< ::fwData::List>(s_MATRIXLIST2_INOUT);

    matrixList1->getContainer().clear();
    matrixList2->getContainer().clear();

    // send data modified signals
    {
        auto sig1 = matrixList1->signal< ::fwData::List::ModifiedSignalType >( ::fwData::List::s_MODIFIED_SIG);
        ::fwCom::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
        sig1->asyncEmit();
    }

    {
        auto sig2 = matrixList2->signal< ::fwData::List::ModifiedSignalType >( ::fwData::List::s_MODIFIED_SIG);
        ::fwCom::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
        sig2->asyncEmit();
    }

    m_capturesListWidget->clear();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::getSelection()
{
    const int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        ::fwData::List::sptr matrixList1 = this->getInOut< ::fwData::List>(s_MATRIXLIST1_INOUT);
        ::fwData::List::sptr matrixList2 = this->getInOut< ::fwData::List>(s_MATRIXLIST2_INOUT);

        ::fwData::List::IteratorType it1 = matrixList1->begin();
        ::fwData::List::IteratorType it2 = matrixList2->begin();
        std::advance(it1, idx);
        std::advance(it2, idx);

        auto matrix1 = ::fwData::TransformationMatrix3D::dynamicCast(*it1);

        SLM_ASSERT("This element of the list is not a TransformationMatrix3D", matrix1);

        auto matrix2 = ::fwData::TransformationMatrix3D::dynamicCast(*it2);

        SLM_ASSERT("This element of the list is not a TransformationMatrix3D", matrix2);

        // TODO : Launch a dialog with a view of the two corresponding matrices
        QPointer<QDialog> dialog     = new QDialog();
        QPointer<QGridLayout> layout = new QGridLayout();

        QBoxLayout* m1Layout = new QBoxLayout(QBoxLayout::TopToBottom);
        m1Layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QBoxLayout* m2Layout = new QBoxLayout(QBoxLayout::TopToBottom);
        m2Layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QGridLayout* gridLayout1 = new QGridLayout();
        QGridLayout* gridLayout2 = new QGridLayout();
        QVector< QPointer< QLabel > > matrix1Labels, matrix2Labels;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                QLabel* label1 = new QLabel("");
                QLabel* label2 = new QLabel("");
                // matrix1
                matrix1Labels.push_back(label1);
                matrix1Labels[i*4 + j]->setText(QString("%1").arg(matrix1->getCoefficient(i, j)));
                gridLayout1->addWidget(label1, i, j);
                // matrix2
                matrix2Labels.push_back(label2);
                matrix2Labels[i*4 + j]->setText(QString("%1").arg(matrix2->getCoefficient(i, j)));
                gridLayout2->addWidget(label2, i, j);

            }
        }
        m1Layout->addLayout(gridLayout1);
        m2Layout->addLayout(gridLayout2);

        QPointer<QPushButton> okButton = new QPushButton("OK");
        okButton->setDefault(true);

        QPointer<QHBoxLayout> buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(okButton);
        QLabel* m1 = new QLabel("matrix 1 :");
        QLabel* m2 = new QLabel("matrix 2 :");

        layout->addWidget(m1, 0, 0);
        layout->addWidget(m2, 0, 2);

        layout->addLayout(m1Layout, 1, 0);
        layout->addLayout(m2Layout, 1, 2);

        layout->addLayout(buttonLayout, 2, 1, 2, 2 );

        QObject::connect(okButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

        dialog->setLayout(layout);

        dialog->exec();

    }
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::onItemDoubleClicked(QListWidgetItem*)
{
    this->getSelection();
}

// ----------------------------------------------------------------------------

}
