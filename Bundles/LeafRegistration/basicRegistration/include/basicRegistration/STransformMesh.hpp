/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _BASICREGISTRATION_STRANSFORMMESH_HPP_
#define _BASICREGISTRATION_STRANSFORMMESH_HPP_

#include <fwServices/IController.hpp>

#include "basicRegistration/config.hpp"

namespace basicRegistration
{

class BASICREGISTRATION_CLASS_API STransformMesh : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (STransformMesh)(::fwServices::IController) ) ;


    BASICREGISTRATION_API STransformMesh();

    BASICREGISTRATION_API virtual ~STransformMesh();

protected :

    BASICREGISTRATION_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Overrides
    BASICREGISTRATION_API virtual void swapping() throw ( ::fwTools::Failed );


};



} // namespace basicRegistration

#endif /*_BASICREGISTRATION_STRANSFORMMESH_HPP_*/

