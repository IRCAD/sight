/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/runtime/IPlugin.hpp"
#include <core/macros.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistry.hpp>

namespace sight::core::runtime
{

/**
 * @brief   Provides a default plugin implementation.
 *
 * This class is intended to be sub-classed. The IPlugin::start and IPlugin::stop
 * function must be implemented.
 */
class CORE_CLASS_API Plugin : public IPlugin
{
public:

    CORE_API std::shared_ptr<Module> getModule() const noexcept override;
    CORE_API virtual void setInitializationData(const SPTR(ConfigurationElement)configuration) noexcept
    override;

protected:

    CORE_API void setModule(std::shared_ptr<Module> module) noexcept override;

private:

    /**
     * @brief Pointer to the module the plugin is attached to.
     */
    std::weak_ptr<Module> m_module;
};

#define SIGHT_REGISTER_PLUGIN(class) \
    static sight::core::runtime::utils::GenericExecutableFactoryRegistry<Plugin> BOOST_PP_CAT( \
        __registry, \
        __LINE__ \
    )(class)

} // namespace sight::core::runtime
