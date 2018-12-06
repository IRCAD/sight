/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
