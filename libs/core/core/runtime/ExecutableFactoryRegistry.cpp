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

#include "core/runtime/ExecutableFactoryRegistry.hpp"

#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/Runtime.hpp"

namespace sight::core::runtime
{

ExecutableFactoryRegistry::ExecutableFactoryRegistry(std::shared_ptr<ExecutableFactory> factory)
{
    // Pre-condition
    SIGHT_ASSERT("No module module currently loaded", detail::Module::getLoadingModule() != nullptr);

    // Retrieves the module that is currently loading.
    std::shared_ptr<detail::Module> loadingModule(detail::Module::getLoadingModule());

    // Stores the factory into that module and the default runtime instance.
    loadingModule->addExecutableFactory(factory);

    detail::Runtime& runtime = detail::Runtime::get();
    runtime.addExecutableFactory(factory);
}

} // namespace sight::core::runtime
