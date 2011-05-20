/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_EXPORTIMAGE_HPP_
#define _UIPATIENTDB_ACTION_EXPORTIMAGE_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to export the selected image in PatientDB
 * @class   ExportImage.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * The available writer can be configured
 * @verbatim
    <service uid="action_exportImage" type="::fwGui::IActionSrv" implementation="::uiPatientDB::action::ExportImage" autoComChannel="no">
        <IOSelectorSrvConfig name="myImageExporterConfig" />
    </service>
   @endverbatim
 * With :
 *  - \b myImageExporterConfig : the id of a configuration for IOSelectorService
 *  for example
 *  @verbatim
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>myImageExporterConfig</id>
        <service>::uiIO::editor::IOSelectorService</service>
        <desc>IOSelector config to export image</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="::ioXML::FwXMLGenericWriterService" />
        </config>
    </extension>
    @endverbatim
 */
class UIPATIENTDB_CLASS_API ExportImage : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ExportImage)( ::fwGui::IActionSrv) ) ;
    UIPATIENTDB_API ExportImage() throw() ;

    UIPATIENTDB_API virtual ~ExportImage() throw() ;

protected:

    /// Configure action. Retrieves the IOSelector config
    void configuring() throw (::fwTools::Failed);

    /// Start action.
    void starting() throw (::fwTools::Failed);

    /// Show the Image writer selector and export the selected image
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

#endif // _UIPATIENTDB_ACTION_EXPORTIMAGE_HPP_
