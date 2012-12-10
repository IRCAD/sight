/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_ANONYMISEPATIENT_HPP_
#define _UIPATIENTDB_ACTION_ANONYMISEPATIENT_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to anonymise the selected patient in PatientDB.
 * @class   AnonymisePatient.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 */
class UIPATIENTDB_CLASS_API AnonymisePatient : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro ( (AnonymisePatient)( ::fwGui::IActionSrv) );

    UIPATIENTDB_API AnonymisePatient() throw() ;

    UIPATIENTDB_API virtual ~AnonymisePatient() throw() ;

protected:

    /// Configure action. Retrieves the IOSelector config
    void configuring() throw (::fwTools::Failed);

    /// Start action.
    void starting() throw (::fwTools::Failed);

    /// Show the Patient writer selector and export the selected patient
    void updating() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stop action.
    void stopping() throw (::fwTools::Failed);

    void info(std::ostream &_sstream );

};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_ACTION_ANONYMISEPATIENT_HPP_
