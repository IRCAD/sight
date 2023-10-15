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

#include <data/helper/MedicalImage.hpp>
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
static bool initVTKLogFile()
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
static bool s_enableLog = initVTKLogFile();

// ------------------------------------------------------------------------------

TypeTranslator::fwToolsToVtkMap::mapped_type TypeTranslator::translate(
    const TypeTranslator::fwToolsToVtkMap::key_type& key
)
{
    auto it = s_toVtk.find(key);
    SIGHT_THROW_IF("Unknown Type: " << key, it == s_toVtk.end());
    return it->second;
}

// ------------------------------------------------------------------------------

TypeTranslator::VtkTofwToolsMap::mapped_type TypeTranslator::translate(
    const TypeTranslator::VtkTofwToolsMap::key_type& key
)
{
    auto it = s_fromVtk.find(key);
    SIGHT_THROW_IF("Unknown Type: " << key, it == s_fromVtk.end());
    return it->second;
}

const TypeTranslator::fwToolsToVtkMap TypeTranslator::s_toVtk = {
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

const TypeTranslator::VtkTofwToolsMap TypeTranslator::s_fromVtk = {
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

void toVTKImage(data::image::csptr data, vtkImageData* dst)
{
    vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();

    configureVTKImageImport(importer, data);

    importer->Update();

    dst->ShallowCopy(importer->GetOutput());
}

// -----------------------------------------------------------------------------

template<typename IMAGETYPE>
void* newBuffer(std::size_t size)
{
    IMAGETYPE* destBuffer = nullptr;
    try
    {
        destBuffer = new IMAGETYPE[size];
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR(
            "No enough memory to allocate an image of type "
            << core::type::get<IMAGETYPE>().name()
            << " and of size " << size << "." << std::endl
            << e.what()
        );
        throw;
    }
    return destBuffer;
}

// -----------------------------------------------------------------------------

template<typename IMAGETYPE>
void fromRGBBuffer(void* input, std::size_t size, void*& destBuffer)
{
    if(destBuffer == nullptr)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    auto* destBufferTyped = static_cast<IMAGETYPE*>(destBuffer);
    auto* inputTyped      = static_cast<IMAGETYPE*>(input);
    IMAGETYPE* finalPtr   = static_cast<IMAGETYPE*>(destBuffer) + size;
    IMAGETYPE valR;
    IMAGETYPE valG;
    IMAGETYPE valB;

    while(destBufferTyped < finalPtr)
    {
        valR                 = static_cast<IMAGETYPE>(float((*(inputTyped++)) * 0.30));
        valG                 = static_cast<IMAGETYPE>(float((*(inputTyped++)) * 0.59));
        valB                 = static_cast<IMAGETYPE>(float((*(inputTyped++)) * 0.11));
        (*destBufferTyped++) = valR + valG + valB;
    }
}

// -----------------------------------------------------------------------------

template<typename IMAGETYPE>
void fromRGBBufferColor(void* input, std::size_t size, void*& destBuffer)
{
    if(destBuffer == nullptr)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    auto* destBufferTyped = static_cast<IMAGETYPE*>(destBuffer);
    auto* inputTyped      = static_cast<IMAGETYPE*>(input);
    IMAGETYPE* finalPtr   = static_cast<IMAGETYPE*>(destBuffer) + size;

    while(destBufferTyped < finalPtr)
    {
        (*destBufferTyped++) = (*(inputTyped++));
    }
}

// -----------------------------------------------------------------------------

void fromVTKImage(vtkImageData* source, data::image::sptr destination)
{
    SIGHT_ASSERT("vtkImageData source and/or data::image destination are not correct", destination && source);

    // ensure image size correct
//    source->UpdateInformation();
//    source->PropagateUpdateExtent();

    int dim = source->GetDataDimension();
    data::image::Size imageSize;

    if(dim == 2)
    {
        imageSize = {static_cast<std::size_t>(source->GetDimensions()[0]),
                     static_cast<std::size_t>(source->GetDimensions()[1]), 0
        };

        const data::image::Spacing spacing = {source->GetSpacing()[0], source->GetSpacing()[1], 0.
        };
        destination->setSpacing(spacing);

        const data::image::Origin origin = {source->GetOrigin()[0], source->GetOrigin()[1], 0.
        };
        destination->setOrigin(origin);
    }
    else
    {
        imageSize = {static_cast<std::size_t>(source->GetDimensions()[0]),
                     static_cast<std::size_t>(source->GetDimensions()[1]),
                     static_cast<std::size_t>(source->GetDimensions()[2])
        };

        const data::image::Spacing spacing =
        {source->GetSpacing()[0], source->GetSpacing()[1], source->GetSpacing()[2]
        };
        destination->setSpacing(spacing);

        const data::image::Origin origin = {source->GetOrigin()[0], source->GetOrigin()[1], source->GetOrigin()[2]
        };
        destination->setOrigin(origin);
    }

    const int nbComponents = source->GetNumberOfScalarComponents();
    const std::size_t size = static_cast<std::size_t>(
        std::accumulate(
            source->GetDimensions(),
            source->GetDimensions() + static_cast<std::size_t>(dim),
            std::max(static_cast<std::size_t>(3), static_cast<std::size_t>(nbComponents)),
            std::multiplies<>()
        )
    );
    const void* input = source->GetScalarPointer();

    if(size != 0)
    {
        void* destBuffer = nullptr;

        sight::data::image::PixelFormat format = data::image::PixelFormat::GRAY_SCALE;
        if(nbComponents == 1)
        {
            format = data::image::PixelFormat::GRAY_SCALE;
        }
        else if(nbComponents == 2)
        {
            format = data::image::PixelFormat::RG;
        }
        else if(nbComponents == 3)
        {
            format = data::image::PixelFormat::RGB;
        }
        else if(nbComponents == 4)
        {
            format = data::image::PixelFormat::RGBA;
        }
        else
        {
            SIGHT_FATAL("Unhandled pixel format");
        }

        destination->resize(imageSize, TypeTranslator::translate(source->GetScalarType()), format);

        const auto dumpLock = destination->dump_lock();

        destBuffer = destination->buffer();
        const std::size_t sizeInBytes = destination->getSizeInBytes();
        std::memcpy(destBuffer, input, sizeInBytes);

        sight::data::helper::MedicalImage::checkImageSliceIndex(destination);
    }
}

// ------------------------------------------------------------------------------

void configureVTKImageImport(vtkImageImport* _pImageImport, data::image::csptr _pDataImage)
{
    const auto dumpLock = _pDataImage->dump_lock();

    if(_pDataImage->numDimensions() == 2)
    {
        _pImageImport->SetDataSpacing(
            _pDataImage->getSpacing()[0],
            _pDataImage->getSpacing()[1],
            0
        );

        _pImageImport->SetDataOrigin(
            _pDataImage->getOrigin()[0],
            _pDataImage->getOrigin()[1],
            0
        );

        _pImageImport->SetWholeExtent(
            0,
            static_cast<int>(_pDataImage->size()[0]) - 1,
            0,
            static_cast<int>(_pDataImage->size()[1]) - 1,
            0,
            0
        );
    }
    else
    {
        _pImageImport->SetDataSpacing(
            _pDataImage->getSpacing()[0],
            _pDataImage->getSpacing()[1],
            _pDataImage->getSpacing()[2]
        );

        _pImageImport->SetDataOrigin(
            _pDataImage->getOrigin()[0],
            _pDataImage->getOrigin()[1],
            _pDataImage->getOrigin()[2]
        );

        _pImageImport->SetWholeExtent(
            0,
            static_cast<int>(_pDataImage->size()[0]) - 1,
            0,
            static_cast<int>(_pDataImage->size()[1]) - 1,
            0,
            static_cast<int>(_pDataImage->size()[2]) - 1
        );
    }

    _pImageImport->SetNumberOfScalarComponents(static_cast<int>(_pDataImage->numComponents()));

    // copy WholeExtent to DataExtent
    _pImageImport->SetDataExtentToWholeExtent();

    // no copy, no buffer destruction/management
    // Remove const of the pointer.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    _pImageImport->SetImportVoidPointer(const_cast<void*>(_pDataImage->buffer()));

    // used to set correct pixeltype to VtkImage
    _pImageImport->SetDataScalarType(TypeTranslator::translate(_pDataImage->getType()));
}

// -----------------------------------------------------------------------------

vtkSmartPointer<vtkMatrix4x4> toVTKMatrix(data::matrix4::csptr _transfoMatrix)
{
    auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for(std::uint8_t l = 0 ; l < 4 ; l++)
    {
        for(std::uint8_t c = 0 ; c < 4 ; c++)
        {
            matrix->SetElement(l, c, (*_transfoMatrix)(l, c));
        }
    }

    return matrix;
}

// -----------------------------------------------------------------------------

bool fromVTKMatrix(vtkMatrix4x4* _matrix, data::matrix4::sptr _transfoMatrix)
{
    bool res = true;
    for(std::uint8_t l = 0 ; l < 4 ; l++)
    {
        for(std::uint8_t c = 0 ; c < 4 ; c++)
        {
            (*_transfoMatrix)(l, c) = _matrix->GetElement(l, c);
        }
    }

    return res;
}

// -----------------------------------------------------------------------------

} // namespace sight::io::vtk
