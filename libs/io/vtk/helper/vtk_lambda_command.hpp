/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE NOLINT

#pragma once

#include <sight/io/vtk/config.hpp>

#include <vtkCommand.h>

#include <functional>

namespace sight::io::vtk::helper
{

/**
 * @brief Allows use of vtkCommand with lambdas
 */
class SIGHT_IO_VTK_CLASS_API vtk_lambda_command : public vtkCommand
{
public:

    using callback = std::function<void (vtkObject*, unsigned long, void*)>; // NOLINT(google-runtime-int)

    vtkTypeMacro(vtk_lambda_command, vtkCommand)

    /**
     * @brief returns a new vtk_lambda_command
     */
    static vtk_lambda_command* New()
    {
        return new vtk_lambda_command;
    }

    /**
     * @brief vtkCommand::Execute implementation
     */
    // NOLINTNEXTLINE(google-runtime-int)
    SIGHT_IO_VTK_API void Execute(vtkObject* _caller, unsigned long _eid, void* _call_data) override;

    /**
     * @brief Sets callback
     */
    virtual void set_callback(callback _callback)
    {
        this->m_callback = _callback;
    }

protected:

    vtk_lambda_command()
    = default;

    ~vtk_lambda_command() override
    = default;

    callback m_callback;
};

} // namespace sight::io::vtk::helper
