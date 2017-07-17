/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPITKREGISTRATION_SRESAMPLER_HPP__
#define __OPITKREGISTRATION_SRESAMPLER_HPP__

#include "opItkRegistration/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opItkRegistration
{

/**
 * @brief Service applying a transform to an image and outputing the resampled image.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after resampling.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::opITK::SPropagator">
       <in key="imageIn" uid="..." autoConnect="yes" />
       <in key="transform" uid="..." />
       <in key="target" uid="..." />
       <inout key="imageOut" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b imageIn [::fwData::Image]: Image to resample.
 * - \b transform [::fwData::TransformationMatrix3D]: Transform to apply.
 * - \b target [::fwData::Image] (optional): target image defining the size, spacing and origin of the output.
 * @subsection In-Out In-Out
 * - \b imageOut [::fwData::Image]: New resampled image.
 */
class OPITKREGISTRATION_CLASS_API SResampler : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SResampler)(::fwServices::IOperator) );

    /// Constructor, does nothing.
    OPITKREGISTRATION_API SResampler();

    /// Destructor, does nothing.
    OPITKREGISTRATION_API ~SResampler();

protected:

    /// Does nothing.
    OPITKREGISTRATION_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing.
    OPITKREGISTRATION_API virtual void starting() throw( ::fwTools::Failed );

    /// Does nothing.
    OPITKREGISTRATION_API virtual void stopping() throw( ::fwTools::Failed );

    /// Apply the transform and resample.
    OPITKREGISTRATION_API virtual void updating() throw( ::fwTools::Failed );

    /**
     * @brief Auto connections
     *
     * - Update service when the image or its buffer is modified.
     * - Update service when the transform matrix is modified.
     * - Update service when the target image is modified.
     */
    OPITKREGISTRATION_API virtual KeyConnectionsMap getAutoConnections() const;

};

} // namespace opItkRegistration

#endif // __OPITKREGISTRATION_SRESAMPLER_HPP__
