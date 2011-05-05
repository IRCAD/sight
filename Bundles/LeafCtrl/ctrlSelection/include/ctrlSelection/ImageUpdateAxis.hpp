/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IMAGEUPDATEAXIS_HPP_
#define _CTRLSELECTION_IMAGEUPDATEAXIS_HPP_

#include <fwServices/IService.hpp>
#include <fwServices/IController.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "ctrlSelection/config.hpp"

namespace ctrlSelection
{


/**
 * @class  ImageUpdateAxis.
 * @brief  This service update a fwData::Float given in configuration with the image axis.
 * @author IRCAD (Research and Development Team).

 * @date   2011.
 */
class CTRLSELECTION_CLASS_API ImageUpdateAxis : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ImageUpdateAxis)(::fwServices::IController) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API ImageUpdateAxis() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~ImageUpdateAxis() throw() ;

    /// Implements starting method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements swapping method derived from IService. Convert the image.
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    /// Implements configuring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private:
    std::string m_floatID;
    ::fwComEd::helper::MedicalImageAdaptor::Orientation m_orientation;
};

} // ctrlSelection

#endif // _CTRLSELECTION_IMAGEUPDATEAXIS_HPP_
