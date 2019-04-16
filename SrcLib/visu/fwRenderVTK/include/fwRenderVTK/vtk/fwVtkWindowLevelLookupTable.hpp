/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwRenderVTK/config.hpp"

#include <vtkLookupTable.h>

namespace fwRenderVTK
{

namespace vtk
{

///Reinplementation of vtkWindowLevelLookupTable : add specific out-of-bounds colors
class FWRENDERVTK_API fwVtkWindowLevelLookupTable : public vtkLookupTable
{
public:
    static fwVtkWindowLevelLookupTable* New();
    vtkTypeMacro(fwVtkWindowLevelLookupTable, vtkLookupTable);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    // Description:
    // Set the window for the lookup table.  The window is the difference
    // between TableRange[0] and TableRange[1].
    void SetWindow(double window)
    {
        this->InverseVideo = window < 0;
        if(this->InverseVideo)
        {
            window = -window;
        }
        if (window < 1e-5)
        {
            window = 1e-5;
        }
        this->Window = window;
        this->SetTableRange(this->Level - this->Window/2.0,
                            this->Level + this->Window/2.0);
    }
    vtkGetMacro(Window, double);

    void BuildInvert();

    // Description:
    // Set the Level for the lookup table.  The level is the average of
    // TableRange[0] and TableRange[1].
    void SetLevel(double level)
    {
        this->Level = level;
        this->SetTableRange(this->Level - this->Window/2.0,
                            this->Level + this->Window/2.0);
    }
    vtkGetMacro(Level, double);

    // Description:
    // Set  on or off.  You can achieve the same effect by
    // switching the LeftClampValue and the RightClampValue.
    vtkSetMacro(Clamping, int);
    vtkGetMacro(Clamping, int);
    vtkBooleanMacro(Clamping, int);

    // Description:
    // Set the minimum table value.  All lookup table entries below the
    // start of the ramp will be set to this color.  After you change
    // this value, you must re-build the lookup table.
    vtkSetVector4Macro(LeftClampValue, double);
    vtkGetVector4Macro(LeftClampValue, double);

    // Description:
    // Set the maximum table value. All lookup table entries above the
    // end of the ramp will be set to this color.  After you change
    // this value, you must re-build the lookup table.
    vtkSetVector4Macro(RightClampValue, double);
    vtkGetVector4Macro(RightClampValue, double);

    unsigned char* GetCurrentPointer(const vtkIdType id);

    // Description:
    // map a set of scalars through the lookup table
    void MapScalarsThroughTable2(void* input, unsigned char* output,
                                 int inputDataType, int numberOfValues,
                                 int inputIncrement, int outputIncrement) override;

protected:
    fwVtkWindowLevelLookupTable(int sze = 256, int ext = 256);
    ~fwVtkWindowLevelLookupTable();

    double Window;
    double Level;
    int InverseVideo;

    int Clamping;
    double RightClampValue[4];
    double LeftClampValue[4];

    vtkUnsignedCharArray* InvertTable;
    vtkTimeStamp InvertTime;

private:
    fwVtkWindowLevelLookupTable(const fwVtkWindowLevelLookupTable&); // Not implemented.
    void operator=(const fwVtkWindowLevelLookupTable&); // Not implemented.
};

} // namespace vtk

} // namespace fwRenderVTK
