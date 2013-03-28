/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_ADDMEDICALWORKSPACE_HPP_
#define _UIPATIENTDB_ACTION_ADDMEDICALWORKSPACE_HPP_

#include <fwData/Patient.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{
namespace action
{

/// Action to merge medical workspace loaded with another medical workspace
class UIPATIENTDB_CLASS_API AddMedicalWorkspace : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (AddMedicalWorkspace)( ::fwGui::IActionSrv) ) ;

    /// Does nothing
    UIPATIENTDB_API AddMedicalWorkspace() throw();

    /// Does nothing
    UIPATIENTDB_API virtual ~AddMedicalWorkspace() throw();

    /// Method to merge planning data in medical workspace
    UIPATIENTDB_API static void mergePlanningDBInfo( ::fwData::Composite::sptr _planningDBFrom, ::fwData::Composite::sptr _planningDBTo, ::fwServices::IService::sptr _msgSender );

    /// Method to merge processing data in medical workspace
    UIPATIENTDB_API static void mergeProcessingDBInfo( ::fwData::Composite::sptr _processingDBFrom, ::fwData::Composite::sptr _processingDBTo, ::fwServices::IService::sptr _msgSender );

protected:

    /**
     * @brief Configures the service to set another IOSelector config.
     * @verbatim
     <service ... >
         <IOSelectorSrvConfig name="MwzIOSelectorReaderConfig" />
     </service>
      @endverbatim
     */
    UIPATIENTDB_API void configuring() throw( ::fwTools::Failed );

    /// Calls IAction method to start action
    UIPATIENTDB_API void starting() throw (::fwTools::Failed);

    /// Calls IAction method to stop action
    UIPATIENTDB_API void stopping() throw (::fwTools::Failed);

    /// Merges in medical workspace : patient, planning and processing data
    UIPATIENTDB_API void updating() throw (::fwTools::Failed);

private:

    /// Set to use config. By default is "MwzIOSelectorReaderConfig" defined in rc/plugin.xml
    std::string m_ioSelectorSrvConfig;
};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_ACTION_ADDMEDICALWORKSPACE_HPP_
