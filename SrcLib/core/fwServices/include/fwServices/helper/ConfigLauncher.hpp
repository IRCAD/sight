/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_CONFIGLAUNCHER_HPP__
#define __FWSERVICES_HELPER_CONFIGLAUNCHER_HPP__

#include "fwServices/config.hpp"
#include "fwServices/IAppConfigManager.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwTools/Failed.hpp>

namespace fwServices
{

namespace helper
{

/**
 * @brief   This class provides few methods to manage AppConfig (parsing, starting, stopping...).
 */
class FWSERVICES_CLASS_API ConfigLauncher
{

public:

    typedef std::unique_ptr<ConfigLauncher> uptr;
    typedef ::fwServices::registry::FieldAdaptorType FieldAdaptorType;

    /// Constructor. Do nothing.
    FWSERVICES_API ConfigLauncher();

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~ConfigLauncher();

    /**
     * @brief Parse a ConfigLauncher configuration
     *
     * Example of configuration
     * @code{.xml}
       <service>
           <config>
                <appConfig id="Visu2DID" />
                <parameter replace="SERIESDB" by="medicalData" />
                <parameter replace="IMAGE" by="@values.image" />
            </config>
       </service>
        @endcode
     */
    FWSERVICES_API virtual void parseConfig(const ::fwServices::IService::ConfigType& config,
                                            const ::fwServices::IService::sptr& service);

    /**
     * @brief Launch Appconfig
     * @param[in] srv  service to connect with config root object
     * @param[in] optReplaceMap optional replace map used to replace patterns (concatenated with parsed parameter)
     */
    FWSERVICES_API virtual void startConfig( SPTR(::fwServices::IService) srv,
                                             const FieldAdaptorType& optReplaceMap = FieldAdaptorType() );

    /// Stop/destroy AppConfig and disconnect connection with config root object
    FWSERVICES_API virtual void stopConfig();

    /// Check if AppConfig can be launched.
    FWSERVICES_API virtual bool isExecutable(::fwData::Object::sptr currentObj);

    //------------------------------------------------------------------------------

    virtual bool configIsRunning() const
    {
        return m_configIsRunning;
    }

protected:

    /// To manage connection to the config root
    ::fwCom::helper::SigSlotConnection m_connections;

    ::fwActivities::registry::ActivityAppConfig m_appConfig;

    /// to know if AppConfig is running
    bool m_configIsRunning;

    // config manager
    ::fwServices::IAppConfigManager::sptr m_appConfigManager;

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

#endif // __FWSERVICES_HELPER_CONFIGLAUNCHER_HPP__
