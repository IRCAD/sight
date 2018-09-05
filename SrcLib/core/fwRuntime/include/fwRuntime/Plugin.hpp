/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRuntime/config.hpp"
#include "fwRuntime/IPlugin.hpp"

namespace fwRuntime
{

/**
 * @brief   Provides a default plugin implementation.
 *
 * This class is intended to be sub-classed. The IPlugin::start and IPlugin::stop
 * function must be implemented.
 */
struct FWRUNTIME_CLASS_API Plugin : public IPlugin
{
    public:
        FWRUNTIME_API std::shared_ptr<Bundle> getBundle() const noexcept override;
        FWRUNTIME_API virtual void setInitializationData(const SPTR(ConfigurationElement)configuration) noexcept
        override;

    protected:
        FWRUNTIME_API void setBundle( std::shared_ptr<Bundle> bundle) noexcept override;

    private:
        /**
         * @brief Pointer to the bundle the plugin is attached to.
         */
        std::weak_ptr<Bundle> m_bundle;
};

} // namespace fwRuntime
