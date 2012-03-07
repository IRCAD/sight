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
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    addNewHandledEvent( ::fwComEd::ImageMsg::WINDOWING );
    addNewHandledEvent( ::fwComEd::ImageMsg::TRANSFERTFUNCTION );
}

//-----------------------------------------------------------------------------

ImageText::~ImageText() throw()
{}

//-----------------------------------------------------------------------------

void ImageText::doStart() throw(::fwTools::Failed)
{
    Text::doStart();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

void ImageText::doUpdate() throw(::fwTools::Failed)
{
    std::stringstream ss;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if (::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        unsigned int axialIndex    = m_axialIndex->value();
        unsigned int frontalIndex  = m_frontalIndex->value();
        unsigned int sagittalIndex = m_sagittalIndex->value();

        int min = m_windowMin->value();
        int max = m_windowMax->value();

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
    if ( imgMsg )
    {
        if( imgMsg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ))
        {
            imgMsg->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
        }
        doUpdate();
    }
}

//------------------------------------------------------------------------------

void ImageText::doSwap() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    this->doUpdate();
}


} //namespace visuVTKAdaptor
