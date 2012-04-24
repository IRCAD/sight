/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _POINTLISTREGISTRATION_SPOINTLISTREGISTRATION_HPP_
#define _POINTLISTREGISTRATION_SPOINTLISTREGISTRATION_HPP_

#include <fwServices/IController.hpp>

#include "pointListRegistration/config.hpp"

namespace pointListRegistration
{

class POINTLISTREGISTRATION_CLASS_API SPointListRegistration : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SPointListRegistration)(::fwServices::IController) ) ;

    /** Overrides
     *
     @verbatim
     <registeredPoints compositeKey="pointList2" />
     <referencePoints compositeKey="pointList1" />
     <matrix compositeKey="registrationMatrix" />
     @endverbatim
     **/
    POINTLISTREGISTRATION_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides
    POINTLISTREGISTRATION_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    POINTLISTREGISTRATION_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    POINTLISTREGISTRATION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    POINTLISTREGISTRATION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Overrides
    POINTLISTREGISTRATION_API virtual void swapping() throw ( ::fwTools::Failed );

protected :

    /// Constructor
    POINTLISTREGISTRATION_API SPointListRegistration();

    /// Destructor
    POINTLISTREGISTRATION_API virtual ~SPointListRegistration();

    /// Key source point list
    std::string m_registeredPointsKey;

    /// Key dest point list
    std::string m_referencePointsKey;

    /// Key for computed registration matrix
    std::string m_matrixKey;
};



} // namespace pointListRegistration

#endif /*_POINTLISTREGISTRATION_SPOINTLISTREGISTRATION_HPP_*/

