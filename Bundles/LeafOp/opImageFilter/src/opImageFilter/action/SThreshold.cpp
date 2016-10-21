/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opImageFilter/action/SThreshold.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/fwID.hpp>

namespace opImageFilter
{

namespace action
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro(  ::fwGui::IActionSrv, ::opImageFilter::action::SThreshold, ::fwData::Object );

//-----------------------------------------------------------------------------

SThreshold::SThreshold() throw()
{
}

//-----------------------------------------------------------------------------

SThreshold::~SThreshold() throw()
{
}

//-----------------------------------------------------------------------------

void SThreshold::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SThreshold::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SThreshold::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();

    m_imageSrcUID = m_configuration->findConfigurationElement("imageIn")->getExistingAttributeValue("uid");
    m_imageTgtUID = m_configuration->findConfigurationElement("imageOut")->getExistingAttributeValue("uid");

    OSLM_INFO( "ImageIn UID = " << m_imageSrcUID);
    OSLM_INFO( "ImageOut UID = " << m_imageTgtUID);
}

//-----------------------------------------------------------------------------

/**
 * Functor to apply a threshold filter.
 *
 * The pixel with the value less than the threshold value will be set to 0, else the value is set to the maximum
 * value of the image pixel type.
 *
 * The functor provides a template method operator(param) to apply the filter
 */
struct ThresholdFilter
{
    struct Parameter
    {
        double thresholdValue; ///< threshold value.
        ::fwData::Image::sptr imageIn; ///< image source
        ::fwData::Image::sptr imageOut; ///< image target: contains the result of the filter
    };

    /**
     * @brief Applies the filter
     * @tparam PIXELTYPE image pixel type (uint8, uint16, int8, int16, float, double, ....)
     */
    template<class PIXELTYPE>
    void operator()(Parameter& param)
    {
        const PIXELTYPE thresholdValue = static_cast<PIXELTYPE>(param.thresholdValue);
        ::fwData::Image::sptr imageIn  = param.imageIn;
        ::fwData::Image::sptr imageOut = param.imageOut;
        SLM_ASSERT("Sorry, image must be 3D", imageIn->getNumberOfDimensions() == 3 );
        imageOut->copyInformation(imageIn); // Copy image size, type... without copying the buffer
        imageOut->allocate(); // Allocate the image buffer

        ::fwDataTools::helper::Image imageInHelper(imageIn); // helper used to access the image source buffer
        ::fwDataTools::helper::Image imageOutHelper(imageOut); // helper used to access the image target buffer

        // Get image buffers
        PIXELTYPE* buffer1 = (PIXELTYPE*)imageInHelper.getBuffer();
        PIXELTYPE* buffer2 = (PIXELTYPE*)imageOutHelper.getBuffer();

        // Get number of pixels
        const size_t NbPixels = imageIn->getSize()[0] * imageIn->getSize()[1] * imageIn->getSize()[2];

        // Fill the target buffer considering the thresholding
        for( size_t i = 0; i<NbPixels; ++i, ++buffer1, ++buffer2 )
        {
            * buffer2 = ( *buffer1 < thresholdValue ) ? 0 : std::numeric_limits<PIXELTYPE>::max();
        }
    }
};

//-----------------------------------------------------------------------------

void SThreshold::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // threshold value: the pixel with the value less than 50 will be set to 0, else the value is set to the maximum
    // value of the image pixel type.
    const double threshold = 50.0;

    ThresholdFilter::Parameter param; // filter parameters: threshold value, image source, image target

    // Get source image
    OSLM_ASSERT("Image 1 not found. UID : " << m_imageSrcUID, ::fwTools::fwID::exist(m_imageSrcUID));
    param.imageIn = ::fwData::Image::dynamicCast( ::fwTools::fwID::getObject(m_imageSrcUID) );

    // Get target image
    OSLM_ASSERT("Image 2 not found. UID : " << m_imageTgtUID, ::fwTools::fwID::exist(m_imageTgtUID));
    param.imageOut = ::fwData::Image::dynamicCast( ::fwTools::fwID::getObject(m_imageTgtUID) );

    param.thresholdValue = threshold;

    /*
     * The dispatcher allows to apply the filter on any type of image.
     * It invokes the template functor ThresholdFilter using the image type.
     */
    ::fwTools::DynamicType type = param.imageIn->getPixelType(); // image type

    // Invoke filter functor
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, ThresholdFilter >::invoke( type, param );

    // Notify that the image target is modified
    auto sig = param.imageOut->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace opImageFilter
