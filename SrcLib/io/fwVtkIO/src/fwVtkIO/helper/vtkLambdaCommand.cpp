/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"


namespace fwVtkIO
{

namespace helper
{
void vtkLambdaCommand::Execute(vtkObject *caller, unsigned long event, void *callData)
{
    if (m_callback)
    {
        m_callback(caller, event, callData);
    }
}


} // namespace helper
} // namespace fwVtkIO

