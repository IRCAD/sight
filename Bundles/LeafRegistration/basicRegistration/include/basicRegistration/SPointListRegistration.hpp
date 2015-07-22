/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BASICREGISTRATION_SPOINTLISTREGISTRATION_HPP__
#define __BASICREGISTRATION_SPOINTLISTREGISTRATION_HPP__

#include <fwServices/IController.hpp>

#include "basicRegistration/config.hpp"

namespace basicRegistration
{

class BASICREGISTRATION_CLASS_API SPointListRegistration : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SPointListRegistration)(::fwServices::IController) );

    BASICREGISTRATION_API SPointListRegistration();

    BASICREGISTRATION_API virtual ~SPointListRegistration();

protected:

    /** Overrides
     *
       @verbatim
       <registeredPoints compositeKey="pointList2" />
       <referencePoints compositeKey="pointList1" />
       <matrix compositeKey="registrationMatrix" />
       @endverbatim
     **/
    BASICREGISTRATION_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void swapping() throw ( ::fwTools::Failed );




    /// Key source point list
    std::string m_registeredPointsKey;

    /// Key dest point list
    std::string m_referencePointsKey;

    /// Key for computed registration matrix
    std::string m_matrixKey;
};



} // namespace basicRegistration

#endif /*__BASICREGISTRATION_SPOINTLISTREGISTRATION_HPP__*/

