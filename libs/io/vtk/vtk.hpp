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

#include "io/vtk/config.hpp"

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <vtkSmartPointer.h>

// forward declaration
class vtkPolyData;
class vtkImageData;
class vtkImageImport;
class vtkMatrix4x4;
class vtkLookupTable;

namespace sight::io::vtk
{

struct type_translator
{
    using to_vtk_map_t   = std::map<core::type, int>;
    using from_vtk_map_t = std::map<int, core::type>;

    IO_VTK_API static to_vtk_map_t::mapped_type translate(const to_vtk_map_t::key_type& _key);
    IO_VTK_API static from_vtk_map_t::mapped_type translate(const from_vtk_map_t::key_type& _key);

    static const to_vtk_map_t TO_VTK;
    static const from_vtk_map_t FROM_VTK;
};

/**
 * @brief Allocate a new vtkImageData* from a data::image::sptr.
 *
 * @param[in] _data data::image::sptr.
 * @param[out] _dst the vtk image to fill if provided
 *
 * if dst is provided the new image will be imported into the dst image else
 * allocate a new vtkImage from a data::image, vtkImage doesn't manage its buffer (the buffer is not copied)
 *
 */
IO_VTK_API void to_vtk_image(data::image::csptr _data, vtkImageData* _dst);

/*!
 * @brief Convert a vtkImageData* to a data::image::sptr.
 *
 * @param[in] _source vtkImageData*.
 * @param[out] _destination data::image::sptr.
 *
 * Throw an exception if the conversion fails
 */
IO_VTK_API void from_vtk_image(vtkImageData* _source, data::image::sptr _destination);

/*!
 * @brief Configure a vtkImageImport* from a data::image::sptr.
 *
 * @param[out] _p_image_import vtkImageImport*.
 * @param[in] _p_data_image data::image::sptr.
 */
IO_VTK_API void configure_vtk_image_import(vtkImageImport* _p_image_import, data::image::csptr _p_data_image);

/*!
 * @brief Convert a data::matrix4::sptr to a vtkMatrix4x4*.
 *
 * @param[in] _transfo_matrix data::matrix4::sptr.
 * @return vtkPolyData*.
 */
IO_VTK_API vtkSmartPointer<vtkMatrix4x4> to_vtk_matrix(data::matrix4::csptr _transfo_matrix);

/*!
 * @brief Convert a vtkMatrix4x4* to a data::matrix4::sptr.
 *
 * @param[in] _matrix vtkMatrix4x4*.
 * @param[out] _transfo_matrix data::matrix4::sptr.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
IO_VTK_API bool from_vtk_matrix(vtkMatrix4x4* _matrix, data::matrix4::sptr _transfo_matrix);

} // namespace sight::io::vtk
