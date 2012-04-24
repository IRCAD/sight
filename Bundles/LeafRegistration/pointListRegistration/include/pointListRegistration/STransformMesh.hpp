/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _POINTLISTREGISTRATION_STRANSFORMMESH_HPP_
#define _POINTLISTREGISTRATION_STRANSFORMMESH_HPP_

#include <fwServices/IController.hpp>

#include "pointListRegistration/config.hpp"

namespace pointListRegistration
{

class POINTLISTREGISTRATION_CLASS_API STransformMesh : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (STransformMesh)(::fwServices::IController) ) ;

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
    POINTLISTREGISTRATION_API STransformMesh();

    /// Destructor
    POINTLISTREGISTRATION_API virtual ~STransformMesh();

};



} // namespace pointListRegistration

#endif /*_POINTLISTREGISTRATION_STRANSFORMMESH_HPP_*/

