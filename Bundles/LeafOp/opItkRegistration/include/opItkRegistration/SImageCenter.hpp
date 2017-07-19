/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPITKREGISTRATION_SIMAGECENTER_HPP__
#define __OPITKREGISTRATION_SIMAGECENTER_HPP__

#include "opItkRegistration/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opItkRegistration
{

/**
 * @brief Service computing the center of a 3D image, it outputs the translation going from origin to the center.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after computing center.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::opItkRegistration::SImageCenter">
       <in key="image" uid="..." />
       <inout key="transform" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [::fwData::Image]: Input image on which the center will be computed.
 *
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: Will be first reset to identity and then will contain translation
 * from world origin(0,0,0) to the image center (in mm).
 *
 */
class OPITKREGISTRATION_CLASS_API SImageCenter : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SImageCenter)(::fwServices::IOperator) );

    /// Constructor, does nothing.
    OPITKREGISTRATION_API SImageCenter();

    /// Destructor, does nothing.
    OPITKREGISTRATION_API ~SImageCenter();

protected:

    /// Configure registration parameters.
    OPITKREGISTRATION_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing.
    OPITKREGISTRATION_API virtual void starting() throw( ::fwTools::Failed );

    /// Do the registration.
    OPITKREGISTRATION_API virtual void updating() throw( ::fwTools::Failed );

    /// Does nothing.
    OPITKREGISTRATION_API virtual void stopping() throw( ::fwTools::Failed );

    /**
     * @brief Auto connections
     *
     * - Update service when one of the two images is modified.
     * - Update service when the transform matrix is modified.
     */
    OPITKREGISTRATION_API virtual KeyConnectionsMap getAutoConnections() const;

};

} // namespace opItkRegistration

#endif // __OPITKREGISTRATION_SIMAGECENTER_HPP__
