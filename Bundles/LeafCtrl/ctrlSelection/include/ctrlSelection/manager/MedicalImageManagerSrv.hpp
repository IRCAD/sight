/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_MANAGER_PATIENTDBWRAPPERSRV_HPP_
#define _CTRLSELECTION_MANAGER_PATIENTDBWRAPPERSRV_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IManagerSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

/**
 * @brief  This manager converts the images specified by key to medical image (with landmarks, transfer function fields)
 * @class  MedicalImageManagerSrv.
 * @author IRCAD (Research and Development Team).
 *
 * @date   2007-2009.
 */
class CTRLSELECTION_CLASS_API MedicalImageManagerSrv : public ::ctrlSelection::IManagerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MedicalImageManagerSrv)(::ctrlSelection::IManagerSrv) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API MedicalImageManagerSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~MedicalImageManagerSrv() throw() ;

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /**
     * @brief Configure the service.
     * @verbatim
     <service uid="medicalImageManager" impl="::ctrlSelection::manager::MedicalImageManagerSrv" type="::ctrlSelection::IManagerSrv" autoConnect="yes">
         <update imageCompositeKey="image" />
     </service>
      @endverbatim
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    CTRLSELECTION_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private :

    /// Converts the images specified by key to medical image (with landmarks, transfer function fields).
    void convertImages( ::fwData::Composite::sptr _composite );

    std::vector< std::string > m_imageCompositeKeys;
};

} // manager
} // ctrlSelection

#endif // _CTRLSELECTION_MANAGER_MEDICALIMAGEMANAGERSRV_HPP_
