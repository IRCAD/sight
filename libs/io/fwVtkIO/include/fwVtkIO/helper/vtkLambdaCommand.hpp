/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWVTKIO_HELPER_VTKLAMBDACOMMAND_HPP__
#define __FWVTKIO_HELPER_VTKLAMBDACOMMAND_HPP__

#include "fwVtkIO/config.hpp"

#include <vtkCommand.h>

#include <functional>

namespace fwVtkIO
{

namespace helper
{

/**
 * @brief Allows use of vtkCommand with lambdas
 */
class FWVTKIO_CLASS_API vtkLambdaCommand : public vtkCommand
{
public:
    typedef std::function< void (vtkObject*, unsigned long, void*) > Callback;

    vtkTypeMacro(vtkLambdaCommand, vtkCommand)

    /**
     * @brief returns a new vtkLambdaCommand
     */
    static vtkLambdaCommand *New()
    {
        return new vtkLambdaCommand;
    }

    /**
     * @brief vtkCommand::Execute implementation
     */
    FWVTKIO_API virtual void Execute(vtkObject* caller, unsigned long eid, void* callData) override;

    /**
     * @brief Sets callback
     */
    virtual void SetCallback(Callback callback)
    {
        this->m_callback = callback;
    }

protected:

    vtkLambdaCommand()
    {
    }
    ~vtkLambdaCommand()
    {
    }

    Callback m_callback;
};

} // namespace helper
} // namespace fwVtkIO

#endif // __FWVTKIO_HELPER_VTKLAMBDACOMMAND_HPP__
