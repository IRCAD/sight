/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
