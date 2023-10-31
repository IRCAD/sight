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
#include "io/bitmap/reader.hpp"

#include <data/helper/medical_image.hpp>

#ifdef SIGHT_ENABLE_NVJPEG
#include "nvjpeg_reader.hxx"
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
#include "nvjpeg2k_reader.hxx"
#endif

#include "libjpeg_reader.hxx"
#include "libtiff_reader.hxx"
#include "libpng_reader.hxx"
#include "openjpeg_reader.hxx"

// cspell:ignore nvjpeg LIBJPEG LIBTIFF LIBPNG

namespace sight::io::bitmap::detail
{

class reader_impl final
{
public:

    /// Delete default constructors and assignment operators
    reader_impl()                              = delete;
    reader_impl(const reader_impl&)            = delete;
    reader_impl(reader_impl&&)                 = delete;
    reader_impl& operator=(const reader_impl&) = delete;
    reader_impl& operator=(reader_impl&&)      = delete;

    /// Constructor
    inline explicit reader_impl(reader* const _reader) :
        m_reader(_reader)
    {
    }

    /// Default destructor
    inline ~reader_impl() noexcept = default;

    /// Main read function
    inline void read(std::istream& _istream, backend _backend)
    {
        // Get the image pointer
        auto image = m_reader->get_concrete_object();
        SIGHT_THROW_IF("Output image is null", image == nullptr);

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

#ifdef SIGHT_ENABLE_NVJPEG2K
        if(nv_jpeg_2k() && _backend == backend::nvjpeg2k)
        {
            read<nv_jpe_g2_k_reader>(m_nv_jpe_g2_k, *image, _istream);
        }
        else if(nv_jpeg_2k() && _backend == backend::nvjpeg2k_j2k)
        {
            read<nv_jpe_g2_k_reader>(
                m_nv_jpe_g2_k,
                *image,
                _istream,
                flag::j2_k_stream
            );
        }
        else
#endif
        if(_backend == backend::openjpeg)
        {
            read<open_jpeg_reader>(m_open_jpeg, *image, _istream);
        }
        else if(_backend == backend::openjpeg_j2_k)
        {
            read<open_jpeg_reader>(
                m_open_jpeg,
                *image,
                _istream,
                flag::j2_k_stream
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(nv_jpeg() && _backend == backend::nvjpeg)
        {
            read<nv_jpeg_reader>(m_nv_jpeg, *image, _istream);
        }
        else
#endif
        if(_backend == backend::libjpeg)
        {
            read<lib_jpeg_reader>(m_lib_jpeg, *image, _istream);
        }
        else if(_backend == backend::libtiff)
        {
            read<lib_tiff_reader>(m_lib_tiff, *image, _istream);
        }
        else if(_backend == backend::libpng)
        {
            read<lib_png_reader>(m_lib_png, *image, _istream);
        }
        else
        {
            SIGHT_THROW("No suitable backend found.");
        }

        sight::data::helper::medical_image::check_image_slice_index(image);
    }

private:

    //------------------------------------------------------------------------------

    template<typename W>
    inline static void read(
        std::unique_ptr<W>& _backend,
        data::image& _image,
        std::istream& _istream,
        flag _flag = flag::none
)
    {
        if(_backend == nullptr)
        {
            _backend = std::make_unique<W>();
            SIGHT_THROW_IF("Failed to initialize" << _backend->m_name << " backend.", !_backend->m_valid);
        }

        _backend->read(_image, _istream, _flag);
    }

    /// Pointer to the public interface
    reader* const m_reader;

#ifdef SIGHT_ENABLE_NVJPEG
    std::unique_ptr<nv_jpeg_reader> m_nv_jpeg;
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
    std::unique_ptr<nv_jpe_g2_k_reader> m_nv_jpe_g2_k;
#endif

    std::unique_ptr<lib_jpeg_reader> m_lib_jpeg;
    std::unique_ptr<lib_tiff_reader> m_lib_tiff;
    std::unique_ptr<lib_png_reader> m_lib_png;
    std::unique_ptr<open_jpeg_reader> m_open_jpeg;
};

} // namespace sight::io::bitmap::detail
