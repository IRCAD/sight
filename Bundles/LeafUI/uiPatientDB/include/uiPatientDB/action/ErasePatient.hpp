/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_ERASEPATIENT_HPP_
#define _UIPATIENTDB_ACTION_ERASEPATIENT_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{

namespace action
{

/**
 * @brief   This action allows to erase the selected Image in PatientDB
 * @class   ErasePatient.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIPATIENTDB_CLASS_API ErasePatient : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ErasePatient)( ::fwGui::IActionSrv) ) ;
    UIPATIENTDB_API ErasePatient() throw() ;

    UIPATIENTDB_API virtual ~ErasePatient() throw() ;

protected:

    /// Configure action
    void configuring() throw( ::fwTools::Failed ) ;

    /// Start action
    void starting() throw (::fwTools::Failed);

    ///  Erase the selected Image in PatientDB
    void updating() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stop action.
    void stopping() throw (::fwTools::Failed);
    void info(std::ostream &_sstream ) ;

};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_ACTION_ERASEPATIENT_HPP_
