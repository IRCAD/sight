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

#include <fwCore/base.hpp>

#include <vtkObjectFactory.h>
#include <vtkOutputWindow.h>

class vtkOutputWindowToSpyLog : public vtkOutputWindow
{
public:

    vtkTypeMacro(vtkOutputWindowToSpyLog, vtkObject);
    // Description:
    // Print ObjectFactor to stream.
    virtual void PrintSelf(ostream& os, vtkIndent indent) override;

    static vtkOutputWindowToSpyLog* New();

    virtual void DisplayText(const char*) override;

protected:
    vtkOutputWindowToSpyLog();
    virtual ~vtkOutputWindowToSpyLog();

private:
    vtkOutputWindowToSpyLog(const vtkOutputWindowToSpyLog&);  // Not implemented.
    void operator=(const vtkOutputWindowToSpyLog&);  // Not implemented.
};

//------------------------------------------------------------------------------

vtkStandardNewMacro(vtkOutputWindowToSpyLog);

//------------------------------------------------------------------------------

vtkOutputWindowToSpyLog::vtkOutputWindowToSpyLog()
{
}

//------------------------------------------------------------------------------

vtkOutputWindowToSpyLog::~vtkOutputWindowToSpyLog()
{
}

//------------------------------------------------------------------------------

void vtkOutputWindowToSpyLog::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "vtkOutputWindowToSpyLog" << std::endl;
}

//------------------------------------------------------------------------------

void vtkOutputWindowToSpyLog::DisplayText(const char* txt)
{
    OSLM_WARN("[VTK]: " << txt);
}

//------------------------------------------------------------------------------

class InitializeVtkOutputWindow
{

public:
    InitializeVtkOutputWindow()
    {
        auto output = vtkOutputWindowToSpyLog::New();
        vtkOutputWindow::SetInstance( output );
        output->Delete();
    }
};

static InitializeVtkOutputWindow _vtkOutputWindowInstantiator;
