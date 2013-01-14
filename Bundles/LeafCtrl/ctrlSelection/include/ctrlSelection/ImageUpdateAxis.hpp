/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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
 * @brief  This service updates a fwData::Float given in configuration with the image axis.
 * @author IRCAD (Research and Development Team).

 * @date   2011.
 */
class CTRLSELECTION_CLASS_API ImageUpdateAxis : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ImageUpdateAxis)(::fwServices::IController) ) ;

    CTRLSELECTION_API ImageUpdateAxis() throw() ;

    CTRLSELECTION_API virtual ~ImageUpdateAxis() throw() ;

protected:

    /// Calls updating().
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Does nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing.
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configures the service.
     *
     * @verbatim
       <service type="::fwServices::IController" impl="::ctrlSelection::ImageUpdateAxis" autoConnect="yes">
           <axis uid="imageAxis" orientation="axial" />
       </service>
       @endverbatim
     * - \b uid : fwID of the ::fwData::Float to update
     * - \b orientation : image orientation (axial, frontal or sagittal)
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Does nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Updates the float from the image axis orientation.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    /// Receives image event to change axis orientation.
    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private:

    /// fwID of the float to update.
    std::string m_floatID;

    /// Orientation of the image axis.
    ::fwComEd::helper::MedicalImageAdaptor::Orientation m_orientation;
};

} // ctrlSelection

#endif // _CTRLSELECTION_IMAGEUPDATEAXIS_HPP_
