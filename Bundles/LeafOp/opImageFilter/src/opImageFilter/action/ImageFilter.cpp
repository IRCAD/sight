/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwData/Image.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/helper/Image.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include "opImageFilter/action/ImageFilter.hpp"

namespace opImageFilter
{

namespace action
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE(  ::fwGui::IActionSrv, ::opImageFilter::action::ImageFilter, ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

ImageFilter::ImageFilter() throw() :
    m_image1UID(""),
    m_image2UID("")
{}

//-----------------------------------------------------------------------------

ImageFilter::~ImageFilter() throw()
{}

//-----------------------------------------------------------------------------

void ImageFilter::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void ImageFilter::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void ImageFilter::updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageFilter::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();

    m_image1UID = m_configuration->findConfigurationElement("imageIn")->getExistingAttributeValue("uid");
    m_image2UID = m_configuration->findConfigurationElement("imageOut")->getExistingAttributeValue("uid");

    OSLM_INFO( "ImageIn UID = " << m_image1UID);
    OSLM_INFO( "ImageOut UID = " << m_image2UID);
}

//-----------------------------------------------------------------------------

struct ThresholdFilter
{
    struct Parameter
    {
        double thresholdValue;
        ::fwData::Image::sptr  imageIn;
        ::fwData::Image::sptr  imageOut;
    };

    template<class PIXELTYPE>
    void operator()(Parameter &param)
    {
        ::fwData::Image::sptr imageIn = param.imageIn;
        ::fwData::Image::sptr imageOut = param.imageOut;
        SLM_ASSERT("Sorry, image must be 3D", imageIn->getNumberOfDimensions() == 3 );
        imageOut->copyInformation(imageIn);
        imageOut->allocate();

        ::fwComEd::helper::Image imageInHelper(imageIn);
        ::fwComEd::helper::Image imageOutHelper(imageOut);
        PIXELTYPE *buffer1 = (PIXELTYPE *)imageInHelper.getBuffer();
        PIXELTYPE *buffer2 = (PIXELTYPE *)imageOutHelper.getBuffer();
        const unsigned int NbPixels = imageIn->getSize()[0] * imageIn->getSize()[1] * imageIn->getSize()[2];
        const PIXELTYPE ThresholdValue = ( PIXELTYPE )param.thresholdValue;
        unsigned int i;
        for( i=0 ; i<NbPixels ; ++i , ++buffer1, ++buffer2 )
        {
            *buffer2 = ( *buffer1 < ThresholdValue ) ? 0 : std::numeric_limits<PIXELTYPE>::max();
        }
    }
};

//-----------------------------------------------------------------------------

void ImageFilter::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    const double Threshold = 50.0;

    ThresholdFilter::Parameter param;
    OSLM_ASSERT("Image 1 not found. UID : " << m_image1UID, ::fwTools::fwID::exist(m_image1UID)) ;
    param.imageIn = ::fwData::Image::dynamicCast( ::fwTools::fwID::getObject(m_image1UID) ) ;
    OSLM_ASSERT("Image 2 not found. UID : " << m_image2UID, ::fwTools::fwID::exist(m_image2UID)) ;
    param.imageOut = ::fwData::Image::dynamicCast( ::fwTools::fwID::getObject(m_image2UID) ) ;
    param.thresholdValue = Threshold;

    ::fwTools::DynamicType type = param.imageIn->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , ThresholdFilter >::invoke( type , param );

    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) ;
//  msg->addEvent( ::fwComEd::ImageMsg::BUFFER ) ;

    // Notify message to all service listeners
    ::fwServices::IEditionService::notify(this->getSptr(), param.imageOut, msg);
}

//-----------------------------------------------------------------------------

void ImageFilter::info ( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace opImageFilter
