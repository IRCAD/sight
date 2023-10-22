/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "core/runtime/executable.hpp"

#include <core/macros.hpp>
#include <core/runtime/utils/generic_executable_factory_registry.hpp>

namespace sight::core::runtime
{

/**
 * @brief   Provides a default plugin implementation.
 *
 * This class is intended to be sub-classed. The plugin::start and plugin::stop
 * function must be implemented.
 */
class CORE_CLASS_API plugin : public executable
{
public:

    [[nodiscard]] CORE_API std::shared_ptr<module> get_module() const noexcept override;

    /**
     * @brief   Notifies the plugin about its start.
     */
    virtual void start() = 0;

    /**
     * @brief   Notifies the plugin about its stop.
     */
    virtual void stop() noexcept = 0;

protected:

    CORE_API void set_module(std::shared_ptr<module> _module) noexcept override;

private:

    /**
     * @brief Pointer to the module the plugin is attached to.
     */
    std::weak_ptr<module> m_module;
};

#define SIGHT_REGISTER_PLUGIN(class) \
    static sight::core::runtime::utils::generic_executable_factory_registry<plugin> BOOST_PP_CAT( \
        __registry, \
        __LINE__ \
    )(class)

} // namespace sight::core::runtime
