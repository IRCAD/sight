/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/runtime/module_element.hpp"
#include "core/runtime/runtime_exception.hpp"

#include <string>

namespace sight::core::runtime
{

class executable;

} // namespace sight::core::runtime

namespace sight::core::runtime
{

/**
 * @brief   Defines the abstract executable factory class.
 *
 */
class SIGHT_CORE_CLASS_API executable_factory : public module_element
{
public:

    /**
     * @brief       Constructor
     *
     * @param[in]   _type    a string containing a type identifier
     */
    SIGHT_CORE_API executable_factory(const std::string& _type);

    /**
     * @brief   Destructor : does nothing
     */
    SIGHT_CORE_API ~executable_factory() override;

    /**
     * @brief   Creates an executable object instance.
     *
     * @return  a pointer to an executable instance
     */
    [[nodiscard]] virtual executable* create_executable() const = 0;

    /**
     * @brief   Retrieves the type of executable the factory is able to create.
     *
     * @return  a string containing an executable type
     */
    [[nodiscard]] SIGHT_CORE_API std::string type() const;

private:

    std::string m_type; ///< a string containing the type identifier managed by the factory
};

} // namespace sight::core::runtime
