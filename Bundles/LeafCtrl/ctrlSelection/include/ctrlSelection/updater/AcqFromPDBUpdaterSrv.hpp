/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_ACQFROMPDBUPDATERSRV_HPP_
#define _CTRLSELECTION_ACQFROMPDBUPDATERSRV_HPP_

#include <fwData/Acquisition.hpp>
#include <fwData/PatientDB.hpp>

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class   AcqFromPDBUpdaterSrv.
 * @author IRCAD (Research and Development Team).

 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API AcqFromPDBUpdaterSrv : public ::ctrlSelection::IUpdaterSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (AcqFromPDBUpdaterSrv)(::ctrlSelection::IUpdaterSrv::Baseclass) ) ;

    /// Constructor.  Do nothing.
    CTRLSELECTION_API AcqFromPDBUpdaterSrv() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~AcqFromPDBUpdaterSrv() throw() ;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

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

    ::fwData::Acquisition::sptr getAcquisition(::fwData::PatientDB::sptr patientDB);
};

} // updater
} // ctrlSelection

#endif // _CTRLSELECTION_ACQFROMPDBUPDATERSRV_HPP_
