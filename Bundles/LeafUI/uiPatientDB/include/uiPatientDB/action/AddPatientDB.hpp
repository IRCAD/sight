/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_ADDPATIENTDB_HPP_
#define _UIPATIENTDB_ACTION_ADDPATIENTDB_HPP_

#include "uiPatientDB/config.hpp"

#include <fwData/Patient.hpp>
#include <fwGui/IActionSrv.hpp>


namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to load a new PatientDB and merge it with the current PatientDB
 * @class   AddPatientDB.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * The available reader can be configured
 * @verbatim
    <service uid="action_addPatient" type="::fwGui::IActionSrv" implementation="::uiPatientDB::action::AddPatientDB" autoComChannel="no">
        <IOSelectorSrvConfig name="myPatientDBImporterConfig" />
    </service>
   @endverbatim
 * With :
 *  - \b myPatientDBImporterConfig : the id of a configuration for IOSelectorService
 *  for example
 *  @verbatim
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>myPatientDBImporterConfig</id>
        <service>::uiIO::editor::IOSelectorService</service>
        <desc>IOSelector config to import PatientDB</desc>
        <config>
            <type mode="reader" />
            <selection mode="include" />
            <addSelection service="::ioXML::FwXMLGenericReaderService" />
        </config>
    </extension>
    @endverbatim
 */
class UIPATIENTDB_CLASS_API AddPatientDB : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (AddPatientDB)( ::fwGui::IActionSrv) ) ;

    UIPATIENTDB_API AddPatientDB() throw() ;

    UIPATIENTDB_API virtual ~AddPatientDB() throw() ;

protected:

    /// Configure action. Retrieves the IOSelector config
    void configuring() throw( ::fwTools::Failed ) ;

    /// Start action.
    void starting() throw (::fwTools::Failed);

    /// Show the PatientDB reader selector, load the new PatientDB and merge it the the current PatientDB
    void updating() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stop action.
    void stopping() throw (::fwTools::Failed);
    void info(std::ostream &_sstream ) ;

private:
    std::string m_ioSelectorSrvConfig;

};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_ACTION_ADDPATIENTDB_HPP_
