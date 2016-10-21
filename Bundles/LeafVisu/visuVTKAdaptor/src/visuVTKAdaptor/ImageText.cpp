/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "visuVTKAdaptor/ImageText.hpp"

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

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include <boost/format.hpp>

#include <sstream>


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageText, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";

//-----------------------------------------------------------------------------

ImageText::ImageText() throw()
{
    this->installTFSlots(this);
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &ImageText::updateSliceIndex, this);
}

//-----------------------------------------------------------------------------

ImageText::~ImageText() throw()
{
}

//-----------------------------------------------------------------------------

void ImageText::doStart() throw(::fwTools::Failed)
{
    ::fwData::Composite::wptr tfSelection = this->getSafeInOut< ::fwData::Composite>(this->getTFSelectionFwID());
    this->setTransferFunctionSelection(tfSelection);

    this->Text::doStart();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    this->updateTransferFunction(image);
    this->installTFConnections();
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void ImageText::doStop() throw(fwTools::Failed)
{
    this->removeTFConnections();
    this->Text::doStop();
}

//-----------------------------------------------------------------------------

void ImageText::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->Text::doConfigure();

    this->parseTFConfig( m_configuration );
}


//-----------------------------------------------------------------------------

void ImageText::doUpdate() throw(::fwTools::Failed)
{
    std::stringstream ss;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwDataTools::helper::Image imageHelper(image);
        unsigned int axialIndex    = m_axialIndex->value();
        unsigned int frontalIndex  = m_frontalIndex->value();
        unsigned int sagittalIndex = m_sagittalIndex->value();

        int min = this->getLevel() - this->getWindow()/2.0;
        int max = this->getLevel() + this->getWindow()/2.0;

        double window = max - min;
        double level  = min + window*0.5;

        ss <<  ( ::boost::format("[% 3li,% 3li]") % min % max ) << std::endl;
        ss <<  ( ::boost::format("W:% 3lg L:% 3lg") % window % level ) << std::endl;
        ss <<  ( ::boost::format("(% 4li,% 4li,% 4li): %s") % sagittalIndex % frontalIndex % axialIndex %
                 imageHelper.getPixelAsString(sagittalIndex, frontalIndex, axialIndex ));
    }

    this->setText(ss.str());

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageText::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    this->updating();
}

//------------------------------------------------------------------------------

void ImageText::doSwap() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->removeTFConnections();
    this->updateImageInfos(image);
    this->updateTransferFunction(image);
    this->doUpdate();
    this->installTFConnections();
}

//------------------------------------------------------------------------------

void ImageText::updatingTFPoints()
{
    this->updating();
}

//------------------------------------------------------------------------------

void ImageText::updatingTFWindowing(double window, double level)
{
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImageText::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
