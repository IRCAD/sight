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

#include "core/runtime/executable.hpp"
#include "core/runtime/executable_factory.hpp"
#include "core/runtime/module.hpp"

#include <sstream>
#include <string>

namespace sight::core::runtime::utils
{

/**
 * @brief   Defines a generic template executable factory class.
 */
template<typename E>
class generic_executable_factory : public executable_factory
{
public:

    /**
     * @brief       Constructor.
     *
     * @param[in]   _type    a string containing the type supported by the factory
     */
    generic_executable_factory(const std::string& _type) :
        executable_factory(_type)
    {
    }

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment forbidden for this class.
     */
    void operator=(const generic_executable_factory&) noexcept = delete;

    /**
     * @brief   Destructor : does nothing.
     */
    ~generic_executable_factory() override
    = default;

    /**
     * @brief   Creates an executable object instance.
     *
     * @return  a pointer to an executable instance
     */
    [[nodiscard]] executable* create_executable() const override
    {
        executable* result = nullptr;
        try
        {
            result = new E();
            result->set_module(this->get_module());
        }
        catch(std::exception& exception)
        {
            std::ostringstream buffer;
            buffer << "Unexpected exception caught while creating an core::runtime::executable instance. ";
            buffer << exception.what();
            throw runtime_exception(buffer.str());
        }
        catch(...)
        {
            throw runtime_exception(
                      "Unexpected exception caught while creating an core::runtime::executable instance."
            );
        }
        return result;
    }
};

} // namespace sight::core::runtime::utils
