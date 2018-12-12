/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/SImageText.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwServices/macros.hpp>

#include <boost/format.hpp>

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include <sstream>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImageText);

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";
static const ::fwServices::IService::KeyType s_TF_INOUT    = "tf";

//-----------------------------------------------------------------------------

SImageText::SImageText() noexcept :
    m_helperTF(std::bind(&SImageText::updateTF, this))
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SImageText::updateSliceIndex, this);
}

//-----------------------------------------------------------------------------

SImageText::~SImageText() noexcept
{
}

//-----------------------------------------------------------------------------

void SImageText::starting()
{
    this->SText::starting();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    const ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    m_helperImg.updateImageInfos(image);
    this->updating();
}

//-----------------------------------------------------------------------------

void SImageText::stopping()
{
    m_helperTF.removeTFConnections();
    this->SText::stopping();
}

//-----------------------------------------------------------------------------

void SImageText::configuring()
{
    this->SText::configuring();
}

//-----------------------------------------------------------------------------

void SImageText::updating()
{
    std::stringstream ss;
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwDataTools::helper::Image imageHelper(image);
        ::fwData::Integer::sptr indexesPtr[3];
        m_helperImg.getSliceIndex(indexesPtr);
        const size_t axialIndex    = static_cast<size_t>(indexesPtr[2]->value());
        const size_t frontalIndex  = static_cast<size_t>(indexesPtr[1]->value());
        const size_t sagittalIndex = static_cast<size_t>(indexesPtr[0]->value());

        const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        double min = tf->getLevel() - tf->getWindow()/2.0;
        double max = tf->getLevel() + tf->getWindow()/2.0;

        double window = max - min;
        double level  = min + window*0.5;

        ss <<  ( ::boost::format("[% 3li,% 3li]") % min % max ) << std::endl;
        ss <<  ( ::boost::format("W:% 3lg L:% 3lg") % window % level ) << std::endl;
        ss <<  ( ::boost::format("(% 4li,% 4li,% 4li): %s") % sagittalIndex % frontalIndex % axialIndex %
                 imageHelper.getPixelAsString(sagittalIndex, frontalIndex, axialIndex ));
    }

    this->setText(ss.str());

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageText::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void SImageText::updateSliceIndex(int axial, int frontal, int sagittal)
{
    const int indexes[] = {sagittal, frontal, axial};
    m_helperImg.setSliceIndex(indexes);

    this->updating();
}

//------------------------------------------------------------------------------

void SImageText::updateTF()
{
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageText::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
