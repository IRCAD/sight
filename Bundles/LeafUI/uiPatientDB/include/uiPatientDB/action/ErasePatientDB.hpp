/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_UI_ACTION_ERASEPATIENTDB_HPP_
#define _UIPATIENTDB_UI_ACTION_ERASEPATIENTDB_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to clear the PatientDB
 * @class   ErasePatientDB
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIPATIENTDB_CLASS_API ErasePatientDB : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ErasePatientDB)( ::fwGui::IActionSrv) ) ;
    UIPATIENTDB_API ErasePatientDB() throw() ;

    UIPATIENTDB_API virtual ~ErasePatientDB() throw() ;

protected:

    /// Configure action
    void configuring() throw( ::fwTools::Failed ) ;

    /// Start action
    void starting() throw (::fwTools::Failed);

    ///  Clear the PatientDB
    void updating() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stop action.
    void stopping() throw (::fwTools::Failed);
    void info(std::ostream &_sstream ) ;

};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_ACTION_ERASEPATIENTDB_HPP_
