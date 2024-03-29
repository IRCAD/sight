/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "backend.hpp"

#include "io/bitmap/detail/common.hxx"

#ifdef SIGHT_ENABLE_NVJPEG
#include <cuda.h>
#include <cuda_runtime_api.h>
#endif

namespace sight::io::bitmap
{

//------------------------------------------------------------------------------

data::sequenced_set<std::string> extensions(backend _backend)
{
    switch(_backend)
    {
        case backend::nvjpeg:
        case backend::libjpeg:
            return {detail::JPG_EXT, detail::JPEG_EXT};

        case backend::nvjpeg2k_j2k:
        case backend::openjpeg_j2k:
            return {detail::J2K_EXT};

        case backend::nvjpeg2k:
        case backend::openjpeg:
            return {detail::JP2_EXT};

        case backend::libtiff:
            return {detail::TIFF_EXT, detail::TIF_EXT};

        case backend::libpng:
            return {detail::PNG_EXT};

        default:
            SIGHT_THROW("Unsupported image backend: '" << std::uint8_t(_backend) << "'");
    }
}

//------------------------------------------------------------------------------

#ifdef SIGHT_ENABLE_NVJPEG
static const bool NVJPEG_AVAILABLE =
    []
    {
        try
        {
            int count = 0;

            if(const auto result = cudaGetDeviceCount(&count); result != cudaSuccess)
            {
                SIGHT_ERROR("cudaGetDeviceCount failed: " << result);
                return false;
            }

            if(count == 0)
            {
                SIGHT_ERROR("No CUDA device available.");
                return false;
            }

            return true;
        }
        catch(const std::exception& e)
        {
            SIGHT_ERROR("Exception occurred while checking for CUDA: " << e.what());
        }
        catch(...)
        {
            SIGHT_ERROR("Unknown exception occurred while checking for CUDA");
        }

        return false;
    }();
#else
static const bool NVJPEG_AVAILABLE = false;
#endif

//------------------------------------------------------------------------------

bool nv_jpeg()
{
    return NVJPEG_AVAILABLE;
}

//------------------------------------------------------------------------------

bool nv_jpeg_2k()
{
#ifdef SIGHT_ENABLE_NVJPEG2K
    return nv_jpeg();
#else
    return false;
#endif
}

//------------------------------------------------------------------------------

std::pair<std::string, std::string> wildcard_filter(backend _backend)
{
    switch(_backend)
    {
        case backend::nvjpeg:
        case backend::libjpeg:
            return std::make_pair(detail::JPEG_LABEL, std::string("*") + detail::JPG_EXT + " *" + detail::JPEG_EXT);

        case backend::nvjpeg2k:
        case backend::openjpeg:
            return std::make_pair(detail::J2K_LABEL, std::string("*") + detail::JP2_EXT);

        case backend::nvjpeg2k_j2k:
        case backend::openjpeg_j2k:
            return std::make_pair(detail::J2K_LABEL, std::string("*") + detail::J2K_EXT);

        case backend::libtiff:
            return std::make_pair(detail::TIFF_LABEL, std::string("*") + detail::TIF_EXT + +" *" + detail::TIFF_EXT);

        case backend::libpng:
            return std::make_pair(detail::PNG_LABEL, std::string("*") + detail::PNG_EXT);

        default:
            SIGHT_THROW("Unsupported backend: " << uint8_t(_backend));
    }
}

#ifdef SIGHT_ENABLE_NVJPEG
/// Ensure the CUDA context and all associated memory are frees when the application exits.
/// This allows to proper memory leak detection using tools like valgrind and cuda-memcheck.
static const struct cuda_resetter
{
    ~cuda_resetter()
    {
        cudaDeviceReset();
    }
} CUDA_RESETTER;
#endif

} // namespace sight::io::bitmap
