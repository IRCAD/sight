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

#pragma once

#include "common.hxx"
#include "io/bitmap/Writer.hpp"

#ifdef SIGHT_ENABLE_NVJPEG
#include "NvJPEGWriter.hxx"
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
#include "NvJPEG2KWriter.hxx"
#endif

#include "LibJPEGWriter.hxx"
#include "LibTIFFWriter.hxx"
#include "LibPNGWriter.hxx"
#include "OpenJPEGWriter.hxx"

// cspell:ignore nvjpeg LIBJPEG LIBTIFF LIBPNG

namespace sight::io::bitmap::detail
{

class WriterImpl final
{
public:

    /// Delete default constructors and assignment operators
    WriterImpl()                             = delete;
    WriterImpl(const WriterImpl&)            = delete;
    WriterImpl(WriterImpl&&)                 = delete;
    WriterImpl& operator=(const WriterImpl&) = delete;
    WriterImpl& operator=(WriterImpl&&)      = delete;

    /// Constructor
    inline explicit WriterImpl(Writer* const _writer) :
        M_WRITER(_writer)
    {
    }

    /// Default destructor
    inline ~WriterImpl() noexcept = default;

    /// Main write function
    template<typename O>
    inline std::size_t write(O& _output, Backend _backend, Writer::Mode _mode)
    {
        // Get the image pointer
        const auto& image = M_WRITER->getConcreteObject();
        SIGHT_THROW_IF("Source image is null", image == nullptr);

        /// @todo Should we split volume in 2D slices ?
        const auto& sizes = image->size();
        SIGHT_THROW_IF("Unsupported image dimension", sizes[0] == 0 || sizes[1] == 0);

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

#ifdef SIGHT_ENABLE_NVJPEG2K
        if(nv_jpeg_2k() && _backend == Backend::NVJPEG2K)
        {
            try
            {
                return write<NvJPEG2KWriter>(m_nvJPEG2K, *image, _output, _mode);
            }
            catch(const std::exception& e)
            {
                // Check if the rerror is because of unsupported pixel type
                const auto& pixel_type = image->getType();
                if(pixel_type != core::type::UINT8
                   && pixel_type != core::type::UINT16)
                {
                    throw;
                }

                // Check if the rerror is because of unsupported pixel format
                const auto& pixel_format = image->getPixelFormat();
                if(pixel_format != data::image::PixelFormat::GRAY_SCALE
                   && pixel_format != data::image::PixelFormat::RGB
                   && pixel_format != data::image::PixelFormat::RGBA
                   && pixel_format != data::image::PixelFormat::BGR
                   && pixel_format != data::image::PixelFormat::BGRA)
                {
                    throw;
                }

                /// @todo Check new version of nvjpeg2k (>0.6).
                if((pixel_format == data::image::PixelFormat::RGBA || pixel_format == data::image::PixelFormat::BGRA)
                   && pixel_type == core::type::UINT16)
                {
                    throw;
                }

                // This happens when trying to encode uniform random data which cannot be compressed
                // This is obviously a bug in the encoder (reported and known by NVidia), although this should not
                // happen with real data in the real world. To be in the safe side, we fallback to another backend.
                SIGHT_ERROR("Failed to write image with nv_jpeg_2k: " << e.what() << ". Fallback to OpenJPEG.");
                return write<OpenJPEGWriter>(m_openJPEG, *image, _output, _mode);
            }
        }
        else if(nv_jpeg_2k() && _backend == Backend::NVJPEG2K_J2K)
        {
            try
            {
                return write<NvJPEG2KWriter>(m_nvJPEG2K, *image, _output, _mode, Flag::J2K_STREAM);
            }
            catch(const std::exception& e)
            {
                // Same as above...
                SIGHT_ERROR("Failed to write image with nv_jpeg_2k: " << e.what() << ". Fallback to OpenJPEG.");
                return write<OpenJPEGWriter>(m_openJPEG, *image, _output, _mode, Flag::J2K_STREAM);
            }
        }
        else
#endif
        if(_backend == Backend::OPENJPEG)
        {
            return write<OpenJPEGWriter>(m_openJPEG, *image, _output, _mode);
        }
        else if(_backend == Backend::OPENJPEG_J2K)
        {
            return write<OpenJPEGWriter>(
                m_openJPEG,
                *image,
                _output,
                _mode,
                Flag::J2K_STREAM
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(nv_jpeg() && _backend == Backend::NVJPEG)
        {
            return write<NvJPEGWriter>(m_nvJPEG, *image, _output, _mode);
        }
        else
#endif
        if(_backend == Backend::LIBJPEG)
        {
            return write<LibJPEGWriter>(m_libJPEG, *image, _output, _mode);
        }
        else if(_backend == Backend::LIBTIFF)
        {
            return write<LibTIFFWriter>(m_libTIFF, *image, _output, _mode);
        }
        else if(_backend == Backend::LIBPNG)
        {
            return write<LibPNGWriter>(m_libPNG, *image, _output, _mode);
        }
        else
        {
            SIGHT_THROW("No suitable backend found.");
        }
    }

private:

    //------------------------------------------------------------------------------

    template<typename W, typename O>
    inline static std::size_t write(
        std::unique_ptr<W>& _backend,
        const data::image& _image,
        O& _output,
        Writer::Mode _mode,
        Flag _flag = Flag::NONE
)
    {
        if(_backend == nullptr)
        {
            _backend = std::make_unique<W>();
            SIGHT_THROW_IF("Failed to initialize" << _backend->m_name << " backend.", !_backend->m_valid);
        }

        return _backend->write(_image, _output, _mode, _flag);
    }

    /// Pointer to the public interface
    Writer* const M_WRITER;

#ifdef SIGHT_ENABLE_NVJPEG
    std::unique_ptr<NvJPEGWriter> m_nvJPEG;
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
    std::unique_ptr<NvJPEG2KWriter> m_nvJPEG2K;
#endif

    std::unique_ptr<LibJPEGWriter> m_libJPEG;
    std::unique_ptr<LibTIFFWriter> m_libTIFF;
    std::unique_ptr<LibPNGWriter> m_libPNG;
    std::unique_ptr<OpenJPEGWriter> m_openJPEG;
};

} // namespace sight::io::bitmap::detail
