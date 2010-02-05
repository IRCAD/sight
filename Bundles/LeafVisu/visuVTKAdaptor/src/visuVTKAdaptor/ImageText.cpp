/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/format.hpp>

#include <fwServices/macros.hpp>
#include <fwData/Image.hpp>
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
}

//-----------------------------------------------------------------------------

ImageText::~ImageText() throw()
{}

//-----------------------------------------------------------------------------

void ImageText::doUpdate() throw(::fwTools::Failed)
{
    std::stringstream ss;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if (::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        unsigned int axialIndex    = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId )->value();
        unsigned int frontalIndex  = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId )->value();
        unsigned int sagittalIndex = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId )->value();
        int min = image->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMinId )->value();
        int max = image->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMaxId )->value();
        double window = max - min;
        double level = min + window*0.5;

        ss <<  ( ::boost::format("[% 3li,% 3li]") % min % max ) << std::endl;
        ss <<  ( ::boost::format("W:% 3lg L:% 3lg") % window % level ) << std::endl;
        ss <<  ( ::boost::format("(% 4li,% 4li,% 4li): %s") % sagittalIndex % frontalIndex % axialIndex % getPixelAsString(image, sagittalIndex, frontalIndex, axialIndex ));
    }

    setText(ss.str());

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void ImageText::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    // update only if new LandMarks
     ::fwComEd::ImageMsg::csptr imgMsg =  ::fwComEd::ImageMsg::dynamicConstCast( msg );
    if ( imgMsg &&
               ( imgMsg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX )
                 || imgMsg->hasEvent( ::fwComEd::ImageMsg::WINDOWING )))
    {
        doUpdate();
    }
}

//------------------------------------------------------------------------------

void ImageText::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}


} //namespace visuVTKAdaptor
