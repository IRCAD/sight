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

#include "uiCalibration/SImagesSelector.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/tools/fwID.hpp>

#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/helper/Vector.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace uiCalibration
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiCalibration::SImagesSelector, ::fwData::Vector)

const core::com::Slots::SlotKeyType SImagesSelector::s_ADD_SLOT = "add";
const core::com::Slots::SlotKeyType SImagesSelector::s_REMOVE_SLOT = "remove";
const core::com::Slots::SlotKeyType SImagesSelector::s_RESET_SLOT  = "reset";

const ::fwServices::IService::KeyType s_SELECTION_INOUT = "selection";

//------------------------------------------------------------------------------
SImagesSelector::SImagesSelector() noexcept :
    m_captureIdx(0)
{
    newSlot( s_ADD_SLOT, &SImagesSelector::add, this );
    newSlot( s_REMOVE_SLOT, &SImagesSelector::remove, this );
    newSlot( s_RESET_SLOT, &SImagesSelector::reset, this );
}

//------------------------------------------------------------------------------

SImagesSelector::~SImagesSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SImagesSelector::configuring()
{
    fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void SImagesSelector::starting()
{
    m_frameTL = this->getInput< ::arData::FrameTL>("frameTL");
    SLM_ASSERT("Frame timeline is not found.", m_frameTL);

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    // Main container, VBox
    QVBoxLayout* vLayout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    QHBoxLayout* nbItemsHBox = new QHBoxLayout();

    //     Fill the nbItemsHBox
    QLabel* label = new QLabel("nb captures:");
    nbItemsHBox->addWidget(label);

    m_nbCapturesLabel = new QLabel("0");
    nbItemsHBox->addWidget(m_nbCapturesLabel);
    nbItemsHBox->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget();

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    qtContainer->setLayout(vLayout);

    this->updating();
}

//------------------------------------------------------------------------------

void SImagesSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SImagesSelector::updating()
{
    ::fwData::Vector::sptr vector = this->getInOut< ::fwData::Vector >(s_SELECTION_INOUT);

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
        ::fwData::Vector::sptr vector = this->getInOut< ::fwData::Vector >(s_SELECTION_INOUT);
        ::fwData::Object::sptr obj    = vector->getContainer()[idx];

        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.remove(obj);
        vectorHelper.notify();

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SImagesSelector::reset()
{
    ::fwData::Vector::sptr vector = this->getInOut< ::fwData::Vector >(s_SELECTION_INOUT);

    ::fwDataTools::helper::Vector vectorHelper(vector);
    vectorHelper.clear();
    vectorHelper.notify();

    m_capturesListWidget->clear();
    m_nbCapturesLabel->setText(QString("0"));
}

//------------------------------------------------------------------------------

void SImagesSelector::add(core::HiResClock::HiResClockType timestamp)
{
    CSPTR(::arData::FrameTL::BufferType) buffer = m_frameTL->getClosestBuffer(timestamp);

    if(!buffer)
    {
        SLM_INFO("Buffer not found with timestamp "<< timestamp);
        return;
    }

    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwData::Image::Size size;
    size[0] = m_frameTL->getWidth();
    size[1] = m_frameTL->getHeight();
    size[2] = 1;

    ::fwData::Image::PixelFormat format;
    // FIXME since frameTL does not have format information, we assume that image are Grayscale, RGB or RGBA according
    // to the number of components.
    switch ( m_frameTL->getNumberOfComponents())
    {
        case 1:
            format = ::fwData::Image::GRAY_SCALE;
            break;
        case 3:
            format = ::fwData::Image::RGB;
            break;
        case 4:
            format = ::fwData::Image::RGBA;
            break;
        default:
            format = ::fwData::Image::UNDEFINED;
    }

    image->resize(size, m_frameTL->getType(), format);
    const ::fwData::Image::Origin origin = {0., 0., 0.};
    image->setOrigin2(origin);
    const ::fwData::Image::Spacing spacing = {1., 1., 1.};
    image->setSpacing2(spacing);
    image->setWindowWidth(100);
    image->setWindowCenter(0);

    const auto dumpLock = image->lock();

    const std::uint8_t* frameBuff = &buffer->getElement(0);
    std::uint8_t* imgBuffer       = static_cast< std::uint8_t* >(image->getBuffer());
    std::copy( frameBuff, frameBuff+buffer->getSize(), imgBuffer);

    ::fwData::Vector::sptr vector = this->getInOut< ::fwData::Vector >(s_SELECTION_INOUT);

    ::fwDataTools::helper::Vector vectorHelper(vector);
    vectorHelper.add(image);
    vectorHelper.notify();

    this->updating();
}

//------------------------------------------------------------------------------

} // namespace uiCalibration
