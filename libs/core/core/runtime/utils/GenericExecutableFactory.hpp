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

#include "core/runtime/ExecutableFactory.hpp"
#include "core/runtime/IExecutable.hpp"
#include "core/runtime/Module.hpp"

#include <sstream>
#include <string>

namespace sight::core::runtime::utils
{

/**
 * @brief   Defines a generic template executable factory class.
 */
template<typename E>
class GenericExecutableFactory : public ExecutableFactory
{
public:

    /**
     * @brief       Constructor.
     *
     * @param[in]   type    a string containing the type supported by the factory
     */
    GenericExecutableFactory(const std::string& type) :
        ExecutableFactory(type)
    {
    }

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment forbidden for this class.
     */
    void operator=(const GenericExecutableFactory&) noexcept = delete;

    /**
     * @brief   Destructor : does nothing.
     */
    ~GenericExecutableFactory() override
    = default;

    /**
     * @brief   Creates an executable object instance.
     *
     * @return  a pointer to an executable instance
     */
    [[nodiscard]] IExecutable* createExecutable() const override
    {
        IExecutable* result = nullptr;
        try
        {
            result = new E();
            result->setModule(this->getModule());
        }
        catch(std::exception& exception)
        {
            std::ostringstream buffer;
            buffer << "Unexpected exception caught while creating an core::runtime::IExecutable instance. ";
            buffer << exception.what();
            throw RuntimeException(buffer.str());
        }
        catch(...)
        {
            throw RuntimeException("Unexpected exception caught while creating an core::runtime::IExecutable instance.");
        }
        return result;
    }
};

} // namespace sight::core::runtime::utils
