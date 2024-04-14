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

// cspell:ignore NOLINTNEXTLINE

#include "io/vtk/vtk.hpp"

#include <core/runtime/profile.hpp>
#include <core/tools/os.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <geometry/data/mesh_functions.hpp>

#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkFileOutputWindow.h>
#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkImageImport.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataWriter.h>
#include <vtkSetGet.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkUnstructuredGrid.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <numeric>
#include <stdexcept>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

// Function to initialize VTK Log file, where VTK message will be printed in.
static bool init_vtk_log_file()
{
    SIGHT_INFO("Creating VTK Log file");

    // Get current date.
    const auto timenow =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Create VTK.log in user cache folder.
    // Find log dir
    const auto& current_profile = core::runtime::get_current_profile();
    const auto& profile_name    = current_profile ? current_profile->name() : std::string();
    const auto& vtk_log         = core::tools::os::get_user_cache_dir(profile_name) / "VTK.log";

    // TODO: Gather all .log file together in Session.
    vtkSmartPointer<vtkFileOutputWindow> outwin = vtkFileOutputWindow::New();

    // MSVC doesn't have std::filesystem::path::c_str()
    const auto& vtk_log_string = vtk_log.string();
    outwin->SetFileName(vtk_log_string.c_str());

    // Print header.
    std::string header = "# VTK LOG File " + std::string(ctime(&timenow));
    outwin->DisplayText(header.c_str());

    vtkFileOutputWindow::SetInstance(outwin);

    return true;
}

// static variable to call initVTKLogFile();
static bool s_enable_log = init_vtk_log_file();

// ------------------------------------------------------------------------------

type_translator::to_vtk_map_t::mapped_type type_translator::translate(
    const type_translator::to_vtk_map_t::key_type& _key
)
{
    auto it = TO_VTK.find(_key);
    SIGHT_THROW_IF("Unknown Type: " << _key, it == TO_VTK.end());
    return it->second;
}

// ------------------------------------------------------------------------------

type_translator::from_vtk_map_t::mapped_type type_translator::translate(
    const type_translator::from_vtk_map_t::key_type& _key
)
{
    auto it = FROM_VTK.find(_key);
    SIGHT_THROW_IF("Unknown Type: " << _key, it == FROM_VTK.end());
    return it->second;
}

const type_translator::to_vtk_map_t type_translator::TO_VTK = {
    // char and signed char are treated as the same type.
    // and plain char is used when writing an int8 image
    {core::type::INT8, VTK_CHAR},
    {core::type::UINT8, VTK_UNSIGNED_CHAR},
    {core::type::INT16, VTK_SHORT},
    {core::type::UINT16, VTK_UNSIGNED_SHORT},
    {core::type::INT32, VTK_INT},
    {core::type::UINT32, VTK_UNSIGNED_INT},
    {core::type::INT64, VTK_LONG_LONG},
    {core::type::UINT64, VTK_UNSIGNED_LONG_LONG},
    {core::type::FLOAT, VTK_FLOAT},
    {core::type::DOUBLE, VTK_DOUBLE}
};

const type_translator::from_vtk_map_t type_translator::FROM_VTK = {
    // char and signed char are treated as the same type.
    // and plain char is used when writing an int8 image
    {VTK_SIGNED_CHAR, core::type::INT8},
    {VTK_CHAR, core::type::INT8},
    {VTK_UNSIGNED_CHAR, core::type::UINT8},
    {VTK_SHORT, core::type::INT16},
    {VTK_UNSIGNED_SHORT, core::type::UINT16},
    {VTK_INT, core::type::INT32},
    {VTK_UNSIGNED_INT, core::type::UINT32},
    {VTK_FLOAT, core::type::FLOAT},
    {VTK_DOUBLE, core::type::DOUBLE},
    {VTK_LONG_LONG, core::type::INT64},
    {VTK_UNSIGNED_LONG_LONG, core::type::UINT64},

#if (INT_MAX < LONG_MAX)
    {
        VTK_LONG, core::type::INT64
    },
    {VTK_UNSIGNED_LONG, core::type::UINT64}
#else
    {
        VTK_LONG, core::type::INT32
    },
    {VTK_UNSIGNED_LONG, core::type::UINT32}
#endif
};

// -----------------------------------------------------------------------------

void to_vtk_image(data::image::csptr _data, vtkImageData* _dst)
{
    vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();

    configure_vtk_image_import(importer, _data);

    importer->Update();

    _dst->ShallowCopy(importer->GetOutput());
}

// -----------------------------------------------------------------------------

template<typename IMAGETYPE>
void* new_buffer(std::size_t _size)
{
    IMAGETYPE* dest_buffer = nullptr;
    try
    {
        dest_buffer = new IMAGETYPE[_size];
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR(
            "No enough memory to allocate an image of type "
            << core::type::get<IMAGETYPE>().name()
            << " and of size " << _size << "." << std::endl
            << e.what()
        );
        throw;
    }
    return dest_buffer;
}

// -----------------------------------------------------------------------------

template<typename IMAGETYPE>
void from_rgb_buffer(void* _input, std::size_t _size, void*& _dest_buffer)
{
    if(_dest_buffer == nullptr)
    {
        _dest_buffer = newBuffer<IMAGETYPE>(_size);
    }

    auto* dest_buffer_typed = static_cast<IMAGETYPE*>(_dest_buffer);
    auto* input_typed       = static_cast<IMAGETYPE*>(_input);
    IMAGETYPE* final_ptr    = static_cast<IMAGETYPE*>(_dest_buffer) + _size;
    IMAGETYPE val_r;
    IMAGETYPE val_g;
    IMAGETYPE val_b;

    while(dest_buffer_typed < final_ptr)
    {
        val_r                  = static_cast<IMAGETYPE>(float((*(input_typed++)) * 0.30));
        val_g                  = static_cast<IMAGETYPE>(float((*(input_typed++)) * 0.59));
        val_b                  = static_cast<IMAGETYPE>(float((*(input_typed++)) * 0.11));
        (*dest_buffer_typed++) = val_r + val_g + val_b;
    }
}

// -----------------------------------------------------------------------------

template<typename IMAGETYPE>
void from_rgb_buffer_color(void* _input, std::size_t _size, void*& _dest_buffer)
{
    if(_dest_buffer == nullptr)
    {
        _dest_buffer = newBuffer<IMAGETYPE>(_size);
    }

    auto* dest_buffer_typed = static_cast<IMAGETYPE*>(_dest_buffer);
    auto* input_typed       = static_cast<IMAGETYPE*>(_input);
    IMAGETYPE* final_ptr    = static_cast<IMAGETYPE*>(_dest_buffer) + _size;

    while(dest_buffer_typed < final_ptr)
    {
        (*dest_buffer_typed++) = (*(input_typed++));
    }
}

// -----------------------------------------------------------------------------

void from_vtk_image(vtkImageData* _source, data::image::sptr _destination)
{
    SIGHT_ASSERT("vtkImageData source and/or data::image destination are not correct", _destination && _source);

    // ensure image size correct
//    source->UpdateInformation();
//    source->PropagateUpdateExtent();

    int dim = _source->GetDataDimension();
    data::image::size_t image_size;

    if(dim == 2)
    {
        image_size = {static_cast<std::size_t>(_source->GetDimensions()[0]),
                      static_cast<std::size_t>(_source->GetDimensions()[1]), 0
        };

        const data::image::spacing_t spacing = {_source->GetSpacing()[0], _source->GetSpacing()[1], 0.
        };
        _destination->set_spacing(spacing);

        const data::image::origin_t origin = {_source->GetOrigin()[0], _source->GetOrigin()[1], 0.
        };
        _destination->set_origin(origin);
    }
    else
    {
        image_size = {static_cast<std::size_t>(_source->GetDimensions()[0]),
                      static_cast<std::size_t>(_source->GetDimensions()[1]),
                      static_cast<std::size_t>(_source->GetDimensions()[2])
        };

        const data::image::spacing_t spacing =
        {_source->GetSpacing()[0], _source->GetSpacing()[1], _source->GetSpacing()[2]
        };
        _destination->set_spacing(spacing);

        const data::image::origin_t origin = {_source->GetOrigin()[0], _source->GetOrigin()[1], _source->GetOrigin()[2]
        };
        _destination->set_origin(origin);
    }

    const int nb_components = _source->GetNumberOfScalarComponents();
    const std::size_t size  = static_cast<std::size_t>(
        std::accumulate(
            _source->GetDimensions(),
            _source->GetDimensions() + static_cast<std::size_t>(dim),
            std::max(static_cast<std::size_t>(3), static_cast<std::size_t>(nb_components)),
            std::multiplies<>()
        )
    );
    const void* input = _source->GetScalarPointer();

    if(size != 0)
    {
        void* dest_buffer = nullptr;

        enum sight::data::image::pixel_format format = data::image::pixel_format::gray_scale;
        if(nb_components == 1)
        {
            format = data::image::pixel_format::gray_scale;
        }
        else if(nb_components == 2)
        {
            format = data::image::pixel_format::rg;
        }
        else if(nb_components == 3)
        {
            format = data::image::pixel_format::rgb;
        }
        else if(nb_components == 4)
        {
            format = data::image::pixel_format::rgba;
        }
        else
        {
            SIGHT_FATAL("Unhandled pixel format");
        }

        _destination->resize(image_size, type_translator::translate(_source->GetScalarType()), format);

        const auto dump_lock = _destination->dump_lock();

        dest_buffer = _destination->buffer();
        const std::size_t size_in_bytes = _destination->size_in_bytes();
        std::memcpy(dest_buffer, input, size_in_bytes);

        sight::data::helper::medical_image::check_image_slice_index(_destination);
    }
}

// ------------------------------------------------------------------------------

void configure_vtk_image_import(vtkImageImport* _p_image_import, data::image::csptr _p_data_image)
{
    const auto dump_lock = _p_data_image->dump_lock();

    if(_p_data_image->num_dimensions() == 2)
    {
        _p_image_import->SetDataSpacing(
            _p_data_image->spacing()[0],
            _p_data_image->spacing()[1],
            0
        );

        _p_image_import->SetDataOrigin(
            _p_data_image->origin()[0],
            _p_data_image->origin()[1],
            0
        );

        _p_image_import->SetWholeExtent(
            0,
            static_cast<int>(_p_data_image->size()[0]) - 1,
            0,
            static_cast<int>(_p_data_image->size()[1]) - 1,
            0,
            0
        );
    }
    else
    {
        _p_image_import->SetDataSpacing(
            _p_data_image->spacing()[0],
            _p_data_image->spacing()[1],
            _p_data_image->spacing()[2]
        );

        _p_image_import->SetDataOrigin(
            _p_data_image->origin()[0],
            _p_data_image->origin()[1],
            _p_data_image->origin()[2]
        );

        _p_image_import->SetWholeExtent(
            0,
            static_cast<int>(_p_data_image->size()[0]) - 1,
            0,
            static_cast<int>(_p_data_image->size()[1]) - 1,
            0,
            static_cast<int>(_p_data_image->size()[2]) - 1
        );
    }

    _p_image_import->SetNumberOfScalarComponents(static_cast<int>(_p_data_image->num_components()));

    // copy WholeExtent to DataExtent
    _p_image_import->SetDataExtentToWholeExtent();

    // no copy, no buffer destruction/management
    // Remove const of the pointer.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    _p_image_import->SetImportVoidPointer(const_cast<void*>(_p_data_image->buffer()));

    // used to set correct pixeltype to VtkImage
    _p_image_import->SetDataScalarType(type_translator::translate(_p_data_image->type()));
}

// -----------------------------------------------------------------------------

vtkSmartPointer<vtkMatrix4x4> to_vtk_matrix(data::matrix4::csptr _transfo_matrix)
{
    auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for(std::uint8_t l = 0 ; l < 4 ; l++)
    {
        for(std::uint8_t c = 0 ; c < 4 ; c++)
        {
            matrix->SetElement(l, c, (*_transfo_matrix)(l, c));
        }
    }

    return matrix;
}

// -----------------------------------------------------------------------------

bool from_vtk_matrix(vtkMatrix4x4* _matrix, data::matrix4::sptr _transfo_matrix)
{
    bool res = true;
    for(std::uint8_t l = 0 ; l < 4 ; l++)
    {
        for(std::uint8_t c = 0 ; c < 4 ; c++)
        {
            (*_transfo_matrix)(l, c) = _matrix->GetElement(l, c);
        }
    }

    return res;
}

// -----------------------------------------------------------------------------

} // namespace sight::io::vtk
