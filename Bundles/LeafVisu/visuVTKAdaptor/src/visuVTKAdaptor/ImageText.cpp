/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/format.hpp>

#include <fwServices/macros.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include "visuVTKAdaptor/ImageText.hpp"

#include <sstream>


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageText, ::fwData::Image ) ;

namespace visuVTKAdaptor
{

//-----------------------------------------------------------------------------

ImageText::ImageText() throw()
{
    this->installTFSelectionEventHandler(this);
    this->addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::MODIFIED_POINTS );
}

//-----------------------------------------------------------------------------

ImageText::~ImageText() throw()
{}

//-----------------------------------------------------------------------------

void ImageText::doStart() throw(::fwTools::Failed)
{
    this->Text::doStart();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    this->installTFObserver( this->getSptr() );
}

//-----------------------------------------------------------------------------

void ImageText::doStop() throw(fwTools::Failed)
{
    this->removeTFObserver();
    this->Text::doStop();
}

//-----------------------------------------------------------------------------

void ImageText::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->Text::configuring();

    this->parseTFConfig( m_configuration );
}


//-----------------------------------------------------------------------------

void ImageText::doUpdate() throw(::fwTools::Failed)
{
    std::stringstream ss;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if (::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        unsigned int axialIndex    = m_axialIndex->value();
        unsigned int frontalIndex  = m_frontalIndex->value();
        unsigned int sagittalIndex = m_sagittalIndex->value();

        int min = this->getLevel() - this->getWindow()/2.0;
        int max = this->getLevel() + this->getWindow()/2.0;

        double window = max - min;
        double level = min + window*0.5;

        ss <<  ( ::boost::format("[% 3li,% 3li]") % min % max ) << std::endl;
        ss <<  ( ::boost::format("W:% 3lg L:% 3lg") % window % level ) << std::endl;
        ss <<  ( ::boost::format("(% 4li,% 4li,% 4li): %s") % sagittalIndex % frontalIndex % axialIndex % image->getPixelAsString(sagittalIndex, frontalIndex, axialIndex ));
    }

    setText(ss.str());

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageText::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    // update only if new LandMarks
    ::fwComEd::ImageMsg::csptr imgMsg =  ::fwComEd::ImageMsg::dynamicConstCast( msg );
    ::fwComEd::TransferFunctionMsg::csptr tfMsg =  ::fwComEd::TransferFunctionMsg::dynamicConstCast( msg );

    if ( imgMsg )
    {
        if( imgMsg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ))
        {
            imgMsg->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
        }
        this->doUpdate();
    }
    else  if ( tfMsg || this->upadteTFObserver(msg, this->getSptr()))
    {
        this->doUpdate();
    }
}

//------------------------------------------------------------------------------

void ImageText::doSwap() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->removeTFObserver();
    this->updateImageInfos(image);
    this->doUpdate();
    this->installTFObserver( this->getSptr() );
}


} //namespace visuVTKAdaptor
