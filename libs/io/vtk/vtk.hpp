/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Image.hpp>
#include <data/Matrix4.hpp>

#include <vtkSmartPointer.h>

// forward declaration
class vtkPolyData;
class vtkImageData;
class vtkImageImport;
class vtkMatrix4x4;
class vtkLookupTable;

namespace sight::io::vtk
{

struct TypeTranslator
{
    typedef std::map<core::Type, int> fwToolsToVtkMap;
    typedef std::map<int, core::Type> VtkTofwToolsMap;

    IO_VTK_API static fwToolsToVtkMap::mapped_type translate(const fwToolsToVtkMap::key_type& key);
    IO_VTK_API static VtkTofwToolsMap::mapped_type translate(const VtkTofwToolsMap::key_type& key);

    static const fwToolsToVtkMap s_toVtk;
    static const VtkTofwToolsMap s_fromVtk;
};

/*!
 * @brief Convert data::Image PixelType to the VTK data type of pixels.
 *
 * @param[in] image data::Image::sptr.
 * @return int : VTK data scalar type
 *
 */
IO_VTK_API int getVtkScalarType(data::Image::sptr image);

/**
 * @brief Allocate a new vtkImageData* from a data::Image::sptr.
 *
 * @param[in] _data data::Image::sptr.
 * @param[out] dst the vtk image to fill if provided
 *
 * if dst is provided the new image will be imported into the dst image else
 * allocate a new vtkImage from a data::Image, vtkImage doesn't manage its buffer (the buffer is not copied)
 *
 */
IO_VTK_API void toVTKImage(data::Image::csptr _data, vtkImageData* dst);

/*!
 * @brief Convert a vtkImageData* to a data::Image::sptr.
 *
 * @param[in] _source vtkImageData*.
 * @param[out] _destination data::Image::sptr.
 *
 * Throw an exception if the conversion fails
 */
IO_VTK_API void fromVTKImage(vtkImageData* _source, data::Image::sptr _destination);

/*!
 * @brief Configure a vtkImageImport* from a data::Image::sptr.
 *
 * @param[in] _pDataImage data::Image::sptr.
 * @param[out] _pImageImport vtkImageImport*.
 */
IO_VTK_API void configureVTKImageImport(vtkImageImport* _pImageImport, data::Image::csptr _pDataImage);

/*!
 * @brief Convert a data::Matrix4::sptr to a vtkMatrix4x4*.
 *
 * @param[in] _transfoMatrix data::Matrix4::sptr.
 * @return vtkPolyData*.
 */
IO_VTK_API vtkSmartPointer<vtkMatrix4x4> toVTKMatrix(data::Matrix4::csptr _transfoMatrix);

/*!
 * @brief Convert a vtkMatrix4x4* to a data::Matrix4::sptr.
 *
 * @param[in] _matrix vtkMatrix4x4*.
 * @param[out] _transfoMatrix data::Matrix4::sptr.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
IO_VTK_API bool fromVTKMatrix(vtkMatrix4x4* _matrix, data::Matrix4::sptr _transfoMatrix);

} // namespace sight::io::vtk
