/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwRenderVTK/vtk/fwVtkAxesActor.hpp"

#include <vtkActor.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

vtkStandardNewMacro(fwVtkAxesActor);

//------------------------------------------------------------------------------

fwVtkAxesActor::fwVtkAxesActor() :
    vtkAxesActor()
{

}

//------------------------------------------------------------------------------

fwVtkAxesActor::~fwVtkAxesActor()
{

}

//------------------------------------------------------------------------------

double* fwVtkAxesActor::GetBounds()
{
    double bounds[6];
    int i;

    this->XAxisShaft->GetBounds(this->Bounds);

    this->YAxisShaft->GetBounds(bounds);

    for ( i = 0; i < 3; ++i )
    {
        this->Bounds[2*i] =
            (bounds[2*i] < this->Bounds[2*i]) ? (bounds[2*i]) : (this->Bounds[2*i]);

        this->Bounds[2*i+1] =
            (bounds[2*i+1] > this->Bounds[2*i+1]) ? (bounds[2*i+1]) : (this->Bounds[2*i+1]);
    }

    this->ZAxisShaft->GetBounds(bounds);
    for ( i = 0; i < 3; ++i )
    {
        this->Bounds[2*i] =
            (bounds[2*i] < this->Bounds[2*i]) ? (bounds[2*i]) : (this->Bounds[2*i]);

        this->Bounds[2*i+1] =
            (bounds[2*i+1] > this->Bounds[2*i+1]) ? (bounds[2*i+1]) : (this->Bounds[2*i+1]);
    }

    this->XAxisTip->GetBounds(bounds);
    for ( i = 0; i < 3; ++i )
    {
        this->Bounds[2*i] =
            (bounds[2*i] < this->Bounds[2*i]) ? (bounds[2*i]) : (this->Bounds[2*i]);

        this->Bounds[2*i+1] =
            (bounds[2*i+1] > this->Bounds[2*i+1]) ? (bounds[2*i+1]) : (this->Bounds[2*i+1]);
    }

    this->YAxisTip->GetBounds(bounds);
    for ( i = 0; i < 3; ++i )
    {
        this->Bounds[2*i] =
            (bounds[2*i] < this->Bounds[2*i]) ? (bounds[2*i]) : (this->Bounds[2*i]);

        this->Bounds[2*i+1] =
            (bounds[2*i+1] > this->Bounds[2*i+1]) ? (bounds[2*i+1]) : (this->Bounds[2*i+1]);
    }

    this->ZAxisTip->GetBounds(bounds);
    for ( i = 0; i < 3; ++i )
    {
        this->Bounds[2*i] =
            (bounds[2*i] < this->Bounds[2*i]) ? (bounds[2*i]) : (this->Bounds[2*i]);

        this->Bounds[2*i+1] =
            (bounds[2*i+1] > this->Bounds[2*i+1]) ? (bounds[2*i+1]) : (this->Bounds[2*i+1]);
    }

    double dbounds[6];
    (vtkPolyDataMapper::SafeDownCast(this->YAxisShaft->GetMapper()))->
    GetInput()->GetBounds( dbounds );

    for ( i = 0; i < 3; ++i )
    {
        this->Bounds[2*i] =
            (dbounds[2*i] < this->Bounds[2*i]) ? (dbounds[2*i]) : (this->Bounds[2*i]);

        this->Bounds[2*i+1] =
            (dbounds[2*i+1] > this->Bounds[2*i+1]) ? (dbounds[2*i+1]) : (this->Bounds[2*i+1]);
    }

    return this->Bounds;
}

//------------------------------------------------------------------------------

void fwVtkAxesActor::PrintSelf(std::ostream& os, vtkIndent indent)
{
    os << indent << "fwVtkPicker: \n";
    this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
