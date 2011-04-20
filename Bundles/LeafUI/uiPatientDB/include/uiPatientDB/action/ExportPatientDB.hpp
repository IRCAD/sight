/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_EXPORTPATIENTDB_HPP_
#define _UIPATIENTDB_ACTION_EXPORTPATIENTDB_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"


namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to export the PatientDB. Asks the user if he wants to anonymize the patient.
 * @class   ExportPatientDB.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * The available writer can be configured
 * @verbatim
    <service uid="action_exportPatientDB" type="::fwGui::IActionSrv" implementation="::uiPatientDB::action::ExportPatientDB" autoComChannel="no">
        <IOSelectorSrvConfig name="myPatientDBExporterConfig" />
    </service>
   @endverbatim
 * With :
 *  - \b myPatientDBExporterConfig : the id of a configuration for IOSelectorService
 *  for example
 *  @verbatim
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>myPatientDBExporterConfig</id>
        <service>::uiIO::editor::IOSelectorService</service>
        <desc>IOSelector config to export patientDB</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="::ioXML::FwXMLGenericWriterService" />
        </config>
    </extension>
    @endverbatim
 */
class UIPATIENTDB_CLASS_API ExportPatientDB : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ExportPatientDB)( ::fwGui::IActionSrv) ) ;
    UIPATIENTDB_API ExportPatientDB() throw() ;

    UIPATIENTDB_API virtual ~ExportPatientDB() throw() ;

protected:

    /// Configure action. Retrieves the IOSelector config
    void configuring() throw (::fwTools::Failed);

    /// Start action.
    void starting() throw (::fwTools::Failed);

    /// Show the PatientDB writer selector and export the patientDB
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
} // namespace uiPatientDb

#endif // _UIPATIENTDB_ACTION_EXPORTPATIENTDB_HPP_
