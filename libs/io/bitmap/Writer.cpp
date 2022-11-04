/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "Writer.hpp"

#include "detail/WriterImpl.hxx"

#include <algorithm>
#include <fstream>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP BGRI RGBI NOLINTNEXTLINE LIBJPEG OPENJPEG

namespace sight::io::bitmap
{

static constexpr auto JPEG_EXT {".jpeg"};
static constexpr auto JPG_EXT {".jpg"};
static constexpr auto TIF_EXT {".tif"};
static constexpr auto TIFF_EXT {".tiff"};
static constexpr auto PNG_EXT {".png"};
static constexpr auto JP2_EXT {".jp2"};
static constexpr auto J2K_EXT {".j2k"};

static constexpr auto JPEG_LABEL {"JPEG image"};
static constexpr auto TIFF_LABEL {"TIFF image"};
static constexpr auto PNG_LABEL {"PNG image"};
static constexpr auto J2K_LABEL {"JPEG2000 image"};

//------------------------------------------------------------------------------

/// Returns the backend associated to an extension
/// @arg extension: the selected extension
/// @return backend suitable for the given extension
inline static Writer::Backend extensionToBackend(const std::string& extension)
{
    if(extension.ends_with(JPEG_EXT) || extension.ends_with(JPG_EXT))
    {
        if(Writer::nvJPEG())
        {
            return Writer::Backend::NVJPEG;
        }

        return Writer::Backend::LIBJPEG;
    }

    if(extension.ends_with(J2K_EXT))
    {
        if(Writer::nvJPEG2K())
        {
            return Writer::Backend::NVJPEG2K_J2K;
        }

        return Writer::Backend::OPENJPEG_J2K;
    }

    if(extension.ends_with(JP2_EXT))
    {
        if(Writer::nvJPEG2K())
        {
            return Writer::Backend::NVJPEG2K;
        }

        return Writer::Backend::OPENJPEG;
    }

    if(extension.ends_with(TIFF_EXT) || extension.ends_with(TIF_EXT))
    {
        return Writer::Backend::LIBTIFF;
    }

    if(extension.ends_with(PNG_EXT))
    {
        return Writer::Backend::LIBPNG;
    }

    SIGHT_THROW("Unsupported extension: " << extension);
}

//------------------------------------------------------------------------------

inline static std::pair<Writer::Backend, data::sequenced_set<std::string> > guessBackendOrExtension(
    Writer::Backend backend,
    std::string ext
)
{
    // If no backend is given, only rely on extension
    if(backend == Writer::Backend::ANY)
    {
        return {extensionToBackend(ext), {ext}};
    }

    // Enforce the extension to match the backend
    return {backend, Writer::extensions(backend)};
}

Writer::Writer(base::writer::IObjectWriter::Key /*unused*/) :
    m_pimpl(std::make_unique<detail::WriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Writer::~Writer() = default;

//------------------------------------------------------------------------------

std::string Writer::extension() const
{
    try
    {
        const auto& [backend, extensions] = guessBackendOrExtension(Backend::ANY, getFile().extension().string());
        return extensions.front();
    }
    catch(...)
    {
        return extensions(Backend::DEFAULT).front();
    }
}

//------------------------------------------------------------------------------

data::sequenced_set<std::string> Writer::extensions(Backend backend)
{
    switch(backend)
    {
        case Backend::NVJPEG:
        case Backend::LIBJPEG:
            return {JPG_EXT, JPEG_EXT};

        case Backend::NVJPEG2K_J2K:
        case Backend::OPENJPEG_J2K:
            return {J2K_EXT};

        case Backend::NVJPEG2K:
        case Backend::OPENJPEG:
            return {JP2_EXT};

        case Backend::LIBTIFF:
            return {TIFF_EXT, TIF_EXT};

        case Backend::LIBPNG:
            return {PNG_EXT};

        default:
            SIGHT_THROW("Unsupported image backend: '" << std::uint8_t(backend) << "'");
    }
}

//------------------------------------------------------------------------------

void Writer::write()
{
    write(Backend::ANY, Mode::DEFAULT);
}

//------------------------------------------------------------------------------

void Writer::write(Backend backend, Mode mode)
{
    auto file = getFile();

    try
    {
        if(file.has_parent_path() && !std::filesystem::exists(file.parent_path()))
        {
            std::filesystem::create_directories(file.parent_path());
        }

        // Compute the right backend to use
        const auto& [backend_to_use, extensions_to_use] = guessBackendOrExtension(backend, extension());

        // If there is no extension
        if(!file.has_extension())
        {
            // Add the good one
            const auto& ext = extensions_to_use.front();
            file.replace_extension(ext);

            SIGHT_WARN("No extension given, using '" << ext << "'");
        }
        else
        {
            // Check that the extension is valid
            const auto& current_extension = file.extension().string();

            SIGHT_THROW_IF(
                "Unsupported image extension: '" << file.extension().string() << "'",
                std::none_of(
                    extensions_to_use.cbegin(),
                    extensions_to_use.cend(),
                    [&](const auto& extension)
                {
                    return current_extension.ends_with(extension);
                })
            );
        }

        // Open the output file
        std::ofstream output;
        output.open(file.string(), std::ios::out | std::ios::binary | std::ios::trunc);

        write(output, backend_to_use, mode);
    }
    catch(...)
    {
        // Cleanup the created file in case of problem
        std::filesystem::remove_all(file);
        throw;
    }
}

//------------------------------------------------------------------------------

void Writer::write(std::ostream& ostream, Backend backend, Mode mode)
{
    m_pimpl->write(ostream, backend, mode);
}

//------------------------------------------------------------------------------

bool Writer::nvJPEG()
{
    // This will returns true if SIGHT_ENABLE_NVJPEG CMake option is set, which means CUDA libraries has
    // been found during configure
    static const bool available =
        []
        {
#ifdef SIGHT_ENABLE_NVJPEG
            try
            {
                if(const auto result = cuInit(0); result != CUDA_SUCCESS)
                {
                    SIGHT_ERROR("cuInit failed: " << result);
                    return false;
                }

                int count = 0;

                if(const auto result = cuDeviceGetCount(&count); result != CUDA_SUCCESS)
                {
                    SIGHT_ERROR("cuDeviceGetCount failed: " << result);
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
#endif
            return false;
        }();

    return available;
}

//------------------------------------------------------------------------------

bool Writer::nvJPEG2K()
{
    // This will returns true if SIGHT_ENABLE_NVJPEG2K CMake option is set, which means CUDA AND nvJPEG200 libraries has
    // been found during configure
    static const bool available =
        []
        {
#ifdef SIGHT_ENABLE_NVJPEG2K
            return Writer::nvJPEG();
#else
            return false;
#endif
        }();

    return available;
}

//------------------------------------------------------------------------------

std::pair<std::string, std::string> Writer::wildcardFilter(Backend backend)
{
    switch(backend)
    {
        case Backend::NVJPEG:
        case Backend::LIBJPEG:
            return std::make_pair(JPEG_LABEL, std::string("*") + JPG_EXT + " *" + JPEG_EXT);

        case Backend::NVJPEG2K:
        case Backend::OPENJPEG:
            return std::make_pair(J2K_LABEL, std::string("*") + JP2_EXT);

        case Backend::NVJPEG2K_J2K:
        case Backend::OPENJPEG_J2K:
            return std::make_pair(J2K_LABEL, std::string("*") + J2K_EXT);

        case Backend::LIBTIFF:
            return std::make_pair(TIFF_LABEL, std::string("*") + TIF_EXT + +" *" + TIFF_EXT);

        case Backend::LIBPNG:
            return std::make_pair(PNG_LABEL, std::string("*") + PNG_EXT);

        default:
            SIGHT_THROW("Unsupported backend: " << uint8_t(backend));
    }
}

} // namespace sight::io::bitmap
