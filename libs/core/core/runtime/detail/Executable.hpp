/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "core/runtime/IExecutable.hpp"

namespace sight::core::runtime::detail
{

/**
 * @brief   Provides a default core::runtime::IExecutable interface implementation.
 *
 * This class provides a default implementation of the core::runtime::IExecutable
 * interface and is intended to be used as base class for user defined executable
 * classes being exported by modules.
 *
 */
class Executable : public IExecutable
{
public:

    /**
     * @name    Overrides.
     */
    /// @{
    [[nodiscard]] std::shared_ptr<Module> getModule() const override;

    /// @}

protected:

    // Overrides
    void setModule(std::shared_ptr<Module> module) override;

private:

    /**
     * @brief   a pointer to the module that owns the executable instance
     */
    std::shared_ptr<Module> m_module;
};

} // namespace sight::core::runtime::detail
