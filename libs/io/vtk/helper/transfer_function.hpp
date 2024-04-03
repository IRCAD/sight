/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/io/vtk/config.hpp>

#include <data/transfer_function.hpp>

#include <vtkLogLookupTable.h>
#include <vtkSmartPointer.h>

namespace sight::io::vtk::helper
{

/**
 * @brief   Helper to manage vtk data from data::transfer_function.
 */
class SIGHT_IO_VTK_CLASS_API transfer_function
{
public:

    /*!
     * @brief Convert a data::transfer_function to a vtkLookupTable.
     *
     * @param[in] _tf data::transfer_function
     * @param[in] _allow_transparency Enable conversion of data::transfer_function alpha channel
     * @param[in] _size lookup table size
     * @param[out] _lt vtkLookupTable
     */
    SIGHT_IO_VTK_API static void to_vtk_lookup_table(
        data::transfer_function::csptr _tf,
        vtkSmartPointer<vtkLookupTable> _lt,
        bool _allow_transparency = false,
        unsigned int _size       = 256
    );

    /*!
     * @brief Convert the range of an image or a transfer_function to a black and white vtkLookupTable.
     *
     * @param[in] _range_min
     * @param[in] _range_max
     * @param[in] _size lookup table size
     * @param[out] _lt vtkLookupTable
     *
     */
    SIGHT_IO_VTK_API static void to_bw_vtk_lookup_table(
        double _range_min,
        double _range_max,
        vtkSmartPointer<vtkLookupTable> _lt,
        unsigned int _size = 256
    );
};

} // namespace sight::io::vtk::helper
