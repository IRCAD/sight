/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_PLUGIN_HPP__
#define __FWRUNTIME_PLUGIN_HPP__


#include "fwRuntime/config.hpp"
#include "fwRuntime/IPlugin.hpp"

namespace fwRuntime
{



/**
 * @brief   Provides a default plugin implementation.
 * @struct  Plugin
 *
 * This class is intended to be sub-classed. The IPlugin::start and IPlugin::stop
 * function must be implemented.
 *
 * @date    2004-2009
 *
 */
struct FWRUNTIME_CLASS_API Plugin : public IPlugin
{

    /**
     * @name    Overrides
     */
    //@{
    FWRUNTIME_API std::shared_ptr<Bundle> getBundle() const noexcept;
    FWRUNTIME_API virtual void setInitializationData(const SPTR(ConfigurationElement)configuration) noexcept;
    //@}


    protected:

        // Overrides
        FWRUNTIME_API void setBundle( std::shared_ptr<Bundle> bundle) noexcept;


    private:

        /**
         * @brief   a pointer to the bundle the plugin is attached to
         */
        std::shared_ptr<Bundle> m_bundle;

};


} // namespace fwRuntime


#endif //__FWRUNTIME_PLUGIN_HPP__
