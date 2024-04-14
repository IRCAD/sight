/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/vtk/helper/transfer_function.hpp"

#include <vtkLookupTable.h>

namespace sight::io::vtk::helper
{

//------------------------------------------------------------------------------

void transfer_function::to_vtk_lookup_table(
    data::transfer_function::csptr _tf,
    vtkSmartPointer<vtkLookupTable> _lt,
    bool _allow_transparency,
    unsigned int _size
)
{
    // Configures basic parameters
    _lt->SetNumberOfTableValues(_size);
    _lt->SetScaleToLinear();

    data::transfer_function::min_max_t min_max = _tf->min_max();

    _lt->SetTableRange(min_max.first, min_max.second);

    double delta = (min_max.second - min_max.first) / (double) (_size - 1);
    data::transfer_function::color_t interpolated_color;

    if(_allow_transparency)
    {
        for(unsigned int k = 0 ; k < _size ; ++k)
        {
            interpolated_color = _tf->sample(k * delta + min_max.first);
            _lt->SetTableValue(
                k,
                interpolated_color.r,
                interpolated_color.g,
                interpolated_color.b,
                interpolated_color.a
            );
        }
    }
    else
    {
        for(unsigned int k = 0 ; k < _size ; ++k)
        {
            interpolated_color = _tf->sample(k * delta + min_max.first);
            _lt->SetTableValue(k, interpolated_color.r, interpolated_color.g, interpolated_color.b, 1.0);
        }
    }

    _lt->Modified();
}

//------------------------------------------------------------------------------

void transfer_function::to_bw_vtk_lookup_table(
    double _range_min,
    double _range_max,
    vtkSmartPointer<vtkLookupTable> _lt,
    unsigned int _size
)
{
    // Configures basic parameters
    _lt->Allocate(static_cast<int>(_size), static_cast<int>(_size));
    _lt->SetScaleToLinear();

    _lt->SetRampToLinear();
    _lt->SetTableRange(_range_min, _range_max);
    _lt->SetAlphaRange(1.0, 1.0);
    _lt->SetHueRange(0.0, 0.0);
    _lt->SetSaturationRange(0.0, 0.0);
    _lt->SetValueRange(0.0, 1.0);

    _lt->Build();

    _lt->Modified();
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk::helper
