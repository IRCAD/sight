/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_EXPORTPATIENT_HPP_
#define _UIPATIENTDB_ACTION_EXPORTPATIENT_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to export the selected patient in PatientDB. Asks the user if he wants to anonymize the patient.
 * @class   ExportPatient.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * The available writer can be configured
 * @verbatim
    <service uid="action_exportPatient" type="::fwGui::IActionSrv" implementation="::uiPatientDB::action::ExportPatient" autoComChannel="no">
        <IOSelectorSrvConfig name="myPatientExporterConfig" />
    </service>
   @endverbatim
 * With :
 *  - \b myPatientExporterConfig : the id of a configuration for IOSelectorService
 *  for example
 *  @verbatim
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>myPatientExporterConfig</id>
        <service>::uiIO::editor::IOSelectorService</service>
        <desc>IOSelector config to export patient</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="::ioXML::FwXMLGenericWriterService" />
        </config>
    </extension>
    @endverbatim
 */
class UIPATIENTDB_CLASS_API ExportPatient : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ExportPatient)( ::fwGui::IActionSrv) ) ;
    UIPATIENTDB_API ExportPatient() throw() ;

    UIPATIENTDB_API virtual ~ExportPatient() throw() ;

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

    void info(std::ostream &_sstream ) ;

private:
    std::string m_ioSelectorSrvConfig;
};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_UI_ACTION_EXPORTPATIENT_HPP_
