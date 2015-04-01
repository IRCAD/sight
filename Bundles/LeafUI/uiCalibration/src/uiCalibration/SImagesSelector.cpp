/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SImagesSelector.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Vector.hpp>
#include <fwComEd/helper/Array.hpp>

#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/Base.hpp>

#include <fwTools/fwID.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

namespace uiCalibration
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SImagesSelector, ::fwData::Vector);


const ::fwCom::Slots::SlotKeyType SImagesSelector::s_REMOVE_SLOT = "remove";
const ::fwCom::Slots::SlotKeyType SImagesSelector::s_RESET_SLOT  = "reset";
const ::fwCom::Slots::SlotKeyType SImagesSelector::s_ADD_SLOT    = "add";

//------------------------------------------------------------------------------
SImagesSelector::SImagesSelector() throw() : m_captureIdx(0)
{
    m_slotRemove = ::fwCom::newSlot( &SImagesSelector::remove, this );
    m_slotReset  = ::fwCom::newSlot( &SImagesSelector::reset, this );
    m_slotAdd    = ::fwCom::newSlot( &SImagesSelector::add, this );

    ::fwCom::HasSlots::m_slots( s_REMOVE_SLOT, m_slotRemove)
        ( s_RESET_SLOT, m_slotReset)
        ( s_ADD_SLOT, m_slotAdd);



    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//------------------------------------------------------------------------------

SImagesSelector::~SImagesSelector() throw()
{
}

//------------------------------------------------------------------------------

void SImagesSelector::configuring() throw(::fwTools::Failed)
{
    fwGui::IGuiContainerSrv::initialize();

    m_frameTLUid = m_configuration->findConfigurationElement("frameTLUid")->getValue();
    SLM_ASSERT("Parameter 'frameTLUid' are not found in xml", !m_frameTLUid.empty() );

}

//------------------------------------------------------------------------------

void SImagesSelector::starting() throw(::fwTools::Failed)
{
    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_frameTLUid);
    m_frameTL                   = ::extData::FrameTL::dynamicCast(obj);
    SLM_ASSERT("Frame timeline is not found.", m_frameTL);

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();

    // Main container, VBox
    QVBoxLayout* vLayout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    QHBoxLayout* nbItemsHBox = new QHBoxLayout();

    //     Fill the nbItemsHBox
    QLabel* label = new QLabel("nb captures:", container);
    nbItemsHBox->addWidget(label);

    m_nbCapturesLabel = new QLabel("0", container);
    nbItemsHBox->addWidget(m_nbCapturesLabel);
    nbItemsHBox->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget(container);

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    container->setLayout(vLayout);

    this->updating();
}

//------------------------------------------------------------------------------

void SImagesSelector::stopping() throw(::fwTools::Failed)
{
    this->getContainer()->clean();
    ::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SImagesSelector::updating() throw(::fwTools::Failed)
{
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

    m_capturesListWidget->clear();
    unsigned int captureIdx = 0;
    for(::fwData::Object::sptr obj : vector->getContainer())
    {
        ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(obj);
        if (image)
        {
            QString countString;

            countString = QString("%1. %2").arg(captureIdx).arg(QString::fromStdString(image->getID()));
            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }
    }
    m_nbCapturesLabel->setText(QString("%1").arg(captureIdx));
}

// ----------------------------------------------------------------------------

void SImagesSelector::remove()
{
    int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

        ::fwData::Object::sptr obj = vector->getContainer()[idx];

        ::fwComEd::helper::Vector vectorHelper(vector);
        vectorHelper.remove(obj);
        vectorHelper.notify(this->getSptr());

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SImagesSelector::reset()
{
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

    ::fwComEd::helper::Vector vectorHelper(vector);
    vectorHelper.clear();
    vectorHelper.notify(this->getSptr());

    m_capturesListWidget->clear();
    m_nbCapturesLabel->setText(QString("0"));
}

//------------------------------------------------------------------------------

void SImagesSelector::add(::fwCore::HiResClock::HiResClockType timestamp)
{
    CSPTR(::extData::FrameTL::BufferType) buffer = m_frameTL->getClosestBuffer(timestamp);

    if(!buffer)
    {
        OSLM_INFO("Buffer not found with timestamp "<< timestamp);
        return;
    }

    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwData::Image::SizeType size(3);
    size[0] = m_frameTL->getWidth();
    size[1] = m_frameTL->getHeight();
    size[2] = 1;

    const ::fwData::Image::SpacingType::value_type voxelSize = 1;
    image->allocate(size, m_frameTL->getType(), m_frameTL->getNumberOfComponents());
    ::fwData::Image::OriginType origin(3,0);

    image->setOrigin(origin);
    ::fwData::Image::SpacingType spacing(3,voxelSize);
    image->setSpacing(spacing);
    image->setWindowWidth(100);
    image->setWindowCenter(0);

    ::fwData::Array::sptr array = image->getDataArray();

    ::fwComEd::helper::Array arrayHelper(array);

    const ::boost::uint8_t* frameBuff = &buffer->getElement(0);
    ::boost::uint8_t* index = arrayHelper.begin< ::boost::uint8_t >();
    std::copy( frameBuff, frameBuff+buffer->getSize(), index);

    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

    ::fwComEd::helper::Vector vectorHelper(vector);
    vectorHelper.add(image);
    vectorHelper.notify(this->getSptr());

    this->updating();
}

//------------------------------------------------------------------------------

} // namespace uiCalibration
