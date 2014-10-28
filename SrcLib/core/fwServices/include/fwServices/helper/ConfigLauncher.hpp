/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_HELPER_CONFIGLAUNCHER_HPP_
#define _FWSERVICES_HELPER_CONFIGLAUNCHER_HPP_

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwActivities/registry/Activities.hpp>

#include "fwServices/AppConfigManager.hpp"
#include "fwServices/helper/SigSlotConnection.hpp"
#include "fwServices/config.hpp"

namespace fwServices
{

class IService;

namespace helper
{


/**
 * @class   ConfigLauncher
 * @brief   This class provides few methods to manage AppConfig (parsing, starting, stopping...).
 * @date    2013.
 */
class FWSERVICES_CLASS_API ConfigLauncher : public ::fwCore::BaseObject
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (ConfigLauncher)(::fwCore::BaseObject),
                                            (()),
                                            ::boost::make_shared< ConfigLauncher > );

    /// Constructor. Do nothing.
    FWSERVICES_API ConfigLauncher();

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~ConfigLauncher();

    /**
     * @brief Parse a ConfigLauncher configuration
     *
     * Example of configuration
     * @verbatim
       <service>
           <config>
                <appConfig id="Visu2DID" >
                    <parameters>
                        <parameter replace="SERIESDB" by="medicalData"  />
                        <parameter replace="IMAGE" by="@values.image"  />
                    </parameters>
                </appConfig>
            </config>
       </service>
        @endverbatim
     */
    FWSERVICES_API virtual void parseConfig(const ::fwServices::IService::ConfigType& config);

    /// Launch Appconfig and connect service with config root object
    FWSERVICES_API virtual void startConfig( SPTR(::fwServices::IService) srv );

    /// Stop/destroy AppConfig and disconnect connection with config root object
    FWSERVICES_API virtual void stopConfig();

    /// Check if AppConfig can be launched.
    FWSERVICES_API virtual bool isExecutable(::fwData::Object::sptr currentObj);

protected:

    /// To manage connection to the config root
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    ::fwActivities::registry::ActivityAppConfig m_appConfig;

    /// to know if AppConfig is running
    bool m_configIsRunning;

    // config manager
    ::fwServices::AppConfigManager::sptr m_appConfigManager;

private:

    /// helper to connect config root
    void connectToConfigRoot(SPTR(::fwServices::IService) srv);

    /// helper to disconnect config root
    void disconnectToConfigRoot();

    /// Defines a special key to defines the associated object him self
    static const std::string s_SELF_KEY;
    /// Defines a special key to defines the generated uid
    static const std::string s_GENERIC_UID_KEY;

};

} // helper
} // fwServices


#endif // _FWSERVICES_HELPER_CONFIGLAUNCHER_HPP_


