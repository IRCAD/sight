/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        vtkOutputWindow::SetInstance( vtkOutputWindowToSpyLog::New() );
    }
};

static InitializeVtkOutputWindow _vtkOutputWindowInstantiator;
