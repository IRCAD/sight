/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImageText.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

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

SImageText::SImageText() noexcept
{
    this->installTFSlots(this);
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
    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction >(s_TF_INOUT);

    this->setOrCreateTF(tf, image);

    this->updateImageInfos(image);
    this->updating();
}

//-----------------------------------------------------------------------------

void SImageText::stopping()
{
    this->removeTFConnections();
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

        ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

        double min = tf->getLevel() - tf->getWindow()/2.0;
        double max = tf->getLevel() + tf->getWindow()/2.0;

        double window = max - min;
        double level  = min + window*0.5;

        ss <<  ( ::boost::format("[% 3li,% 3li]") % min % max ) << std::endl;
        ss <<  ( ::boost::format("W:% 3lg L:% 3lg") % window % level ) << std::endl;
        ss <<  ( ::boost::format("(% 4li,% 4li,% 4li): %s") % m_sagittalIndex % m_frontalIndex % m_axialIndex %
                 imageHelper.getPixelAsString(m_sagittalIndex, m_frontalIndex, m_axialIndex ));
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
        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction >(s_TF_INOUT);
        ::fwData::Image::sptr image         = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        this->setOrCreateTF(tf, image);
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SImageText::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex    = axial;
    m_frontalIndex  = frontal;
    m_sagittalIndex = sagittal;

    this->updating();
}

//------------------------------------------------------------------------------

void SImageText::updateTFPoints()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SImageText::updateTFWindowing(double /*window*/, double /*level*/)
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
