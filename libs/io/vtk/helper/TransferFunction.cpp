/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/vtk/helper/TransferFunction.hpp"

#include <vtkLookupTable.h>

namespace sight::io::vtk
{

namespace helper
{

//------------------------------------------------------------------------------

void TransferFunction::toVtkLookupTable(
    data::TransferFunction::csptr tf,
    vtkSmartPointer<vtkLookupTable> lt,
    bool allowTransparency,
    unsigned int size
)
{
    // Configures basic parameters
    lt->SetNumberOfTableValues(size);
    lt->SetScaleToLinear();

    data::TransferFunction::TFValuePairType minMax = tf->getMinMaxTFValues();

    lt->SetTableRange(minMax.first, minMax.second);

    double delta = (minMax.second - minMax.first) / (double) (size - 1);
    data::TransferFunction::TFColor interpolatedColor;

    if(allowTransparency)
    {
        for(unsigned int k = 0 ; k < size ; ++k)
        {
            interpolatedColor = tf->getInterpolatedColor(k * delta + minMax.first);
            lt->SetTableValue(k, interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, interpolatedColor.a);
        }
    }
    else
    {
        for(unsigned int k = 0 ; k < size ; ++k)
        {
            interpolatedColor = tf->getInterpolatedColor(k * delta + minMax.first);
            lt->SetTableValue(k, interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, 1.0);
        }
    }

    lt->Modified();
}

//------------------------------------------------------------------------------

void TransferFunction::toBWVtkLookupTable(
    double rangeMin,
    double rangeMax,
    vtkSmartPointer<vtkLookupTable> lt,
    unsigned int size
)
{
    // Configures basic parameters
    lt->Allocate(static_cast<int>(size), static_cast<int>(size));
    lt->SetScaleToLinear();

    lt->SetRampToLinear();
    lt->SetTableRange(rangeMin, rangeMax);
    lt->SetAlphaRange(1.0, 1.0);
    lt->SetHueRange(0.0, 0.0);
    lt->SetSaturationRange(0.0, 0.0);
    lt->SetValueRange(0.0, 1.0);

    lt->Build();

    lt->Modified();
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace sight::io::vtk
