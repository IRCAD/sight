/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
