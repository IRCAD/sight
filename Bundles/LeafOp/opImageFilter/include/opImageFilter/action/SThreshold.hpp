/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPIMAGEFILTER_ACTION_STHRESHOLD_HPP__
#define __OPIMAGEFILTER_ACTION_STHRESHOLD_HPP__

#include <fwGui/IActionSrv.hpp>

#include "opImageFilter/config.hpp"

namespace opImageFilter
{

namespace action
{

/**
 * @brief Applies a threshold on an image.
 *
 * This Service needs two image: the source and the target image.
 */
class OPIMAGEFILTER_CLASS_API SThreshold : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SThreshold)(::fwGui::IActionSrv) );

    OPIMAGEFILTER_API SThreshold() throw();

    OPIMAGEFILTER_API virtual ~SThreshold() throw();

protected:

    OPIMAGEFILTER_API void starting() throw ( ::fwTools::Failed );

    OPIMAGEFILTER_API void stopping() throw ( ::fwTools::Failed );

    /**
     * @brief Configure the service:
     *
     * @verbatim
        <service uid="actionImageFilter" impl="::opImageFilter::action::ImageFilter">
            <imageIn uid="myImage1" />
            <imageOut uid="myImage2" />
        </service>
       @endverbatim
     * - \b imageIn: uid of the source image
     * - \b imageOut: uid of the target image (ie the filtered image)
     */
    OPIMAGEFILTER_API void configuring() throw ( ::fwTools::Failed );

    /// Applies the threshold
    OPIMAGEFILTER_API void updating() throw ( ::fwTools::Failed );

private:

    std::string m_imageSrcUID;
    std::string m_imageTgtUID;

};


} // namespace action
} // namespace opImageFilter


#endif // __OPIMAGEFILTER_ACTION_STHRESHOLD_HPP__
