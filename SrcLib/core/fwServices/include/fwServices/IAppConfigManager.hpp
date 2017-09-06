/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_IAPPCONFIGMANAGER_HPP__
#define __FWSERVICES_IAPPCONFIGMANAGER_HPP__

#include "fwServices/config.hpp"
#include "fwServices/registry/AppInfo.hpp"

#include <fwData/Composite.hpp>

#include <fwTools/Object.hpp>

namespace fwServices
{
/**
 * @brief   This class provides an interface to manage configurations template.
 */
class FWSERVICES_CLASS_API IAppConfigManager : public ::fwTools::Object
{
public:
    typedef registry::FieldAdaptorType FieldAdaptorType;

    fwCoreNonInstanciableClassDefinitionsMacro((IAppConfigManager)(::fwTools::Object));

    /// Constructor. Do nothing.
    FWSERVICES_API IAppConfigManager();

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~IAppConfigManager();

    /// Return a new AppConfigManager implementation. Should be used for all the IAppConfigManager of the application,
    /// bexcept the first one which must be explicitly called.
    FWSERVICES_API static SPTR(IAppConfigManager) New();

    /// Return state
    bool isCreated() const;
    bool isStarted() const;
    bool isStopped() const;
    bool isDestroyed() const;

    /// Set config param
    void setConfig(::fwRuntime::ConfigurationElement::csptr cfgElem);

    /**
     * @brief Set configuration
     * @param configId the identifier of the requested config.
     * @param replaceFields the associations between the value and the pattern to replace in the config.
     */
    FWSERVICES_API virtual void setConfig(const std::string& configId,
                                          const FieldAdaptorType& replaceFields = FieldAdaptorType()) = 0;

    /**
     * @brief Set configuration
     * @param configId the identifier of the requested config.
     * @param replaceFields composite of association between the value and the pattern to replace in the config.
     */
    FWSERVICES_API virtual void setConfig(const std::string& configId,
                                          const ::fwData::Composite::csptr& replaceFields) = 0;

    /// Get config root
    FWSERVICES_API virtual ::fwData::Object::sptr getConfigRoot() const = 0;

    /// Calls methods : create, start then update.
    FWSERVICES_API virtual void launch() = 0;

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    FWSERVICES_API virtual void stopAndDestroy() = 0;

    /// Creates objects and services from config
    FWSERVICES_API virtual void create() = 0;

    /// Starts services specified in config
    FWSERVICES_API virtual void start() = 0;

    /// Updates services specified in config
    FWSERVICES_API virtual void update() = 0;

    /// Stops services specified in config
    FWSERVICES_API virtual void stop() = 0;

    /// Destroys services specified in config
    FWSERVICES_API virtual void destroy() = 0;

protected:
    enum ConfigState
    {
        STATE_CREATED,
        STATE_STARTED,
        STATE_STOPPED,
        STATE_DESTROYED,
    };

    /// XML Configuration tree
    ::fwRuntime::ConfigurationElement::csptr m_cfgElem;

    /// Running state of the app config manager
    ConfigState m_state;
};

//------------------------------------------------------------------------------

inline IAppConfigManager::IAppConfigManager() :
    m_state(STATE_DESTROYED)
{
}

//------------------------------------------------------------------------------

inline IAppConfigManager::~IAppConfigManager()
{

}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isCreated() const
{
    return m_state == STATE_CREATED;
}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isStarted() const
{
    return m_state == STATE_STARTED;
}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isStopped() const
{
    return m_state == STATE_STOPPED;
}

//------------------------------------------------------------------------------

inline bool IAppConfigManager::isDestroyed() const
{
    return m_state == STATE_DESTROYED;
}

//------------------------------------------------------------------------------

inline void IAppConfigManager::setConfig(fwRuntime::ConfigurationElement::csptr cfgElem)
{
    m_cfgElem = cfgElem;
}

//------------------------------------------------------------------------------

} // namespace fwServices

#endif // __FWSERVICES_IAPPCONFIGMANAGER_HPP__
