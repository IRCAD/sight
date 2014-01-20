/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIPATIENTDB_ACTION_EXPORTACQUISITION_HPP__
#define __UIPATIENTDB_ACTION_EXPORTACQUISITION_HPP__

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{
namespace action
{
/**
 * @brief   Action to export the selected acquisition of a patientDB.
 * @class   ExportAcquisition
 *
 * The available writer can be configured
 * @verbatim
    <service uid="action_ExportAcquisition" type="::fwGui::IActionSrv" impl="::uiPatientDB::action::ExportAcquisition" autoConnect="no">
        <IOSelectorSrvConfig name="acquisitionExporterConfig" />
    </service>
   @endverbatim
 * With :
 *  - \b acquisitionExporterConfig : the id of a configuration for IOSelectorService
 *  for example
 *  @verbatim
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>acquisitionExporterConfig</id>
        <service>::uiIO::editor::IOSelectorService</service>
        <desc>IOSelector config to export an acquisition</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="::ioXML::FwXMLGenericWriterService" />
        </config>
    </extension>
    @endverbatim
 */
class UIPATIENTDB_CLASS_API ExportAcquisition : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro ( (ExportAcquisition)(::fwGui::IActionSrv) ) ;
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Constructor. Do nothing (Just initialize parameters).
    UIPATIENTDB_API ExportAcquisition() throw() ;

    /// Destructor. Do nothing.
    UIPATIENTDB_API virtual ~ExportAcquisition() throw() ;

protected:

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     * @note    Call action base class configuring() method.
     */
    UIPATIENTDB_API void configuring() throw( ::fwTools::Failed ) ;

    /**
     * @brief  Starting method. Call base class starting() method.
     */
    UIPATIENTDB_API void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief  Stopping method. Call base class stopping() method.
     */
    UIPATIENTDB_API void stopping() throw( ::fwTools::Failed ) ;

    /**
     * @brief  Export the acquition.
     *
     * Use the IOSelectorService which propose all writer services for acquisition.
     */
    UIPATIENTDB_API void updating() throw(::fwTools::Failed) ;

    /**
     * @brief React on modifications : does nothing.
     */
    UIPATIENTDB_API void receiving(CSPTR(::fwServices::ObjectMsg) _msg) throw(::fwTools::Failed) ;

    /// Gives a short information about the action.
    UIPATIENTDB_API void info(std::ostream &_sstream ) ;

private:

    std::string m_ioSelectorSrvConfig;
};

} // namespace action
} // namespace uiPatientDB

#endif // __UIPATIENTDB_ACTION_EXPORTACQUISITION_HPP__
