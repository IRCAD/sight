/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPITKREGISTRATION_SFASTREGISTRATION_HPP__
#define __OPITKREGISTRATION_SFASTREGISTRATION_HPP__

#include "opItkRegistration/config.hpp"

#include <arServices/IRegisterer.hpp>

namespace opItkRegistration
{
/**
 * @brief This service uses a fast registration algorithm to quickly register two images together if the transform
 * between both is pure translation.
 *
 * @section Signals Signals
 *
 * @section Slots Slots
 *   - \b update: compute the registration
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::opItkRegistration::SFastRegistration">
       <in key="source" uid="..." />
       <in key="target" uid="..." />
       <in key="transform" uid="..." />
       <inout key="flipTransform" uid="..." />
   </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b source [::fwData::Image]: Input image that will be aligned to the target.
 * - \b target [::fwData::Image]: Input image to which the source will be aligned.
 * - \b flipTransform [::fwData::TransformationMatrix3D]: temporary hack allowing to flip the image according to the
 *   flipping applied in the RegistrationActivity.
 *
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: initial registration. The computed registration will be
 *   concatenated to the initial registration matrix.
 * @note The initial registration in currently not applied, and the output consists purely of translation.
 *
 * @todo Remove the need for a `flipTransform` parameter and resample the source image properly based on the `transform`
 * matrix.
 */
OPITKREGISTRATION_CLASS_API class SFastRegistration : public ::arServices::IRegisterer
{
public:
    fwCoreServiceClassDefinitionsMacro((SFastRegistration)(::arServices::IRegisterer));

    /**
     * @brief Constructor
     */
    OPITKREGISTRATION_API SFastRegistration() noexcept;
    /**
     * @brief Destructor
     */
    OPITKREGISTRATION_API virtual ~SFastRegistration() noexcept;

protected:
    /**
     * @brief Starts the service.
     *
     * @throw ::fwTools::Failed
     */
    OPITKREGISTRATION_API virtual void starting() override;

    /**
     * @brief Stops the service.
     *
     * @throw ::fwTools::Failed
     */
    OPITKREGISTRATION_API virtual void stopping() override;

    /**
     * @brief Configures the service.
     *
     * @throw ::fwTools::Failed
     */
    OPITKREGISTRATION_API virtual void configuring() override;

    /**
     * @brief Compute the registration.
     *
     * @throw ::fwTools::Failed
     */
    OPITKREGISTRATION_API virtual void updating() override;

    /**
     * @brief Compute the registration by calling the `update` slot. The timestamp parameter is discarded.
     */
    OPITKREGISTRATION_API virtual void computeRegistration (::fwCore::HiResClock::HiResClockType timestamp) override;

private:
};

} // namespace opItkRegistration

#endif // __OPITKREGISTRATION_SFASTREGISTRATION_HPP__

