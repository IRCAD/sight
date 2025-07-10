/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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
#include "io/bitmap/writer.hpp"

#ifdef SIGHT_ENABLE_NVJPEG
#include "nv_jpeg_writer.hxx"
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
#include "nvjpeg2k_writer.hxx"
#endif

#include "libjpeg_writer.hxx"
#include "libtiff_writer.hxx"
#include "libpng_writer.hxx"
#include "openjpeg_writer.hxx"

// cspell:ignore nvjpeg LIBJPEG LIBTIFF LIBPNG

namespace sight::io::bitmap::detail
{

class writer_impl final
{
public:

    /// Delete default constructors and assignment operators
    writer_impl()                              = delete;
    writer_impl(const writer_impl&)            = delete;
    writer_impl(writer_impl&&)                 = delete;
    writer_impl& operator=(const writer_impl&) = delete;
    writer_impl& operator=(writer_impl&&)      = delete;

    /// Constructor
    inline explicit writer_impl(writer* const _writer) :
        m_writer(_writer)
    {
    }

    /// Default destructor
    inline ~writer_impl() noexcept = default;

    /// Main write function
    template<typename O>
    inline std::size_t write(O& _output, backend _backend, writer::mode _mode)
    {
        // Get the image pointer
        const auto& image = m_writer->get_concrete_object();
        SIGHT_THROW_IF("Source image is null", image == nullptr);

        /// @todo Should we split volume in 2D slices ?
        const auto& sizes = image->size();
        SIGHT_THROW_IF("Unsupported image dimension", sizes[0] == 0 || sizes[1] == 0);

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

#ifdef SIGHT_ENABLE_NVJPEG2K
        if(nv_jpeg_2k() && _backend == backend::nvjpeg2k)
        {
            try
            {
                return write<nv_jpe_g2_k_writer>(m_nv_jpe_g2_k, *image, _output, _mode);
            }
            catch(const std::exception& e)
            {
                // Check if the rerror is because of unsupported pixel type
                const auto& pixel_type = image->type();
                if(pixel_type != core::type::UINT8
                   && pixel_type != core::type::UINT16)
                {
                    throw;
                }

                // Check if the rerror is because of unsupported pixel format
                const auto& pixel_format = image->pixel_format();
                if(pixel_format != data::image::pixel_format_t::gray_scale
                   && pixel_format != data::image::pixel_format_t::rgb
                   && pixel_format != data::image::pixel_format_t::rgba
                   && pixel_format != data::image::pixel_format_t::bgr
                   && pixel_format != data::image::pixel_format_t::bgra)
                {
                    throw;
                }

                /// @todo Check new version of nvjpeg2k (>0.6).
                if((pixel_format == data::image::pixel_format_t::rgba
                    || pixel_format == data::image::pixel_format_t::bgra)
                   && pixel_type == core::type::UINT16)
                {
                    throw;
                }

                // This happens when trying to encode uniform random data which cannot be compressed
                // This is obviously a bug in the encoder (reported and known by NVidia), although this should not
                // happen with real data in the real world. To be in the safe side, we fallback to another backend.
                SIGHT_ERROR("Failed to write image with nv_jpeg_2k: " << e.what() << ". Fallback to OpenJPEG.");
                return write<open_jpeg_writer>(m_open_jpeg, *image, _output, _mode);
            }
        }
        else if(nv_jpeg_2k() && _backend == backend::nvjpeg2k_j2k)
        {
            try
            {
                return write<nv_jpe_g2_k_writer>(m_nv_jpe_g2_k, *image, _output, _mode, flag::j2_k_stream);
            }
            catch(const std::exception& e)
            {
                // Same as above...
                SIGHT_ERROR("Failed to write image with nv_jpeg_2k: " << e.what() << ". Fallback to OpenJPEG.");
                return write<open_jpeg_writer>(m_open_jpeg, *image, _output, _mode, flag::j2_k_stream);
            }
        }
        else
#endif
        if(_backend == backend::openjpeg)
        {
            return write<open_jpeg_writer>(m_open_jpeg, *image, _output, _mode);
        }
        else if(_backend == backend::openjpeg_j2k)
        {
            return write<open_jpeg_writer>(
                m_open_jpeg,
                *image,
                _output,
                _mode,
                flag::j2_k_stream
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(nv_jpeg() && _backend == backend::nvjpeg)
        {
            return write<nv_jpeg_writer>(m_nv_jpeg, *image, _output, _mode);
        }
        else
#endif
        if(_backend == backend::libjpeg)
        {
            return write<lib_jpeg_writer>(m_lib_jpeg, *image, _output, _mode);
        }
        else if(_backend == backend::libtiff)
        {
            return write<lib_tiff_writer>(m_lib_tiff, *image, _output, _mode);
        }
        else if(_backend == backend::libpng)
        {
            return write<lib_png_writer>(m_lib_png, *image, _output, _mode);
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
        writer::mode _mode,
        flag _flag = flag::none
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
    writer* const m_writer;

#ifdef SIGHT_ENABLE_NVJPEG
    std::unique_ptr<nv_jpeg_writer> m_nv_jpeg;
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
    std::unique_ptr<nv_jpe_g2_k_writer> m_nv_jpe_g2_k;
#endif

    std::unique_ptr<lib_jpeg_writer> m_lib_jpeg;
    std::unique_ptr<lib_tiff_writer> m_lib_tiff;
    std::unique_ptr<lib_png_writer> m_lib_png;
    std::unique_ptr<open_jpeg_writer> m_open_jpeg;
};

} // namespace sight::io::bitmap::detail
