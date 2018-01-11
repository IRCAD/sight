/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPIMAGEFILTER_ACTION_STHRESHOLD_HPP__
#define __OPIMAGEFILTER_ACTION_STHRESHOLD_HPP__

#include "opImageFilter/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace opImageFilter
{

namespace action
{

/**
 * @brief   Apply a threshold on an image.
 *
 * This Service needs two image: the source and the target image.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::opImageFilter::action::SThreshold">
            <in key="source" uid="..." />
            <inout key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b source [::fwData::Image]: source image.
 * @subsection In-Out In-Out:
 * - \b target [::fwData::Image]: target image (ie the filtered image).
 */
class OPIMAGEFILTER_CLASS_API SThreshold : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SThreshold)(::fwGui::IActionSrv) );

    OPIMAGEFILTER_API SThreshold() noexcept;

    OPIMAGEFILTER_API virtual ~SThreshold() noexcept;

protected:

    OPIMAGEFILTER_API void starting() override;

    OPIMAGEFILTER_API void stopping() override;

    /// Configure the service.
    OPIMAGEFILTER_API void configuring() override;

    /// Apply the threshold.
    OPIMAGEFILTER_API void updating() override;
};

} // namespace action
} // namespace opImageFilter

#endif // __OPIMAGEFILTER_ACTION_STHRESHOLD_HPP__
