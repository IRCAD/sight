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

#include "sight/modules/io/vtk/config.hpp"

#include <core/runtime/plugin.hpp>

namespace sight::module::io::vtk
{

class plugin : public core::runtime::plugin
{
public:

    //! @brief Destructor.
    ~plugin() noexcept override;

    /**
     * @brief Start method.
     *
     * This method is used by runtime in order to initialize the module.
     *
     * @exception core::runtime::RuntimeException
     */
    SIGHT_MODULE_IO_VTK_API void start() override;

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the module.
     */
    SIGHT_MODULE_IO_VTK_API void stop() noexcept override;
};

} // namespace sight::module::io::vtk
