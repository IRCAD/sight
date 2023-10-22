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
#include "io/bitmap/Reader.hpp"

#include <data/helper/medical_image.hpp>

#ifdef SIGHT_ENABLE_NVJPEG
#include "NvJPEGReader.hxx"
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
#include "NvJPEG2KReader.hxx"
#endif

#include "LibJPEGReader.hxx"
#include "LibTIFFReader.hxx"
#include "LibPNGReader.hxx"
#include "OpenJPEGReader.hxx"

// cspell:ignore nvjpeg LIBJPEG LIBTIFF LIBPNG

namespace sight::io::bitmap::detail
{

class ReaderImpl final
{
public:

    /// Delete default constructors and assignment operators
    ReaderImpl()                             = delete;
    ReaderImpl(const ReaderImpl&)            = delete;
    ReaderImpl(ReaderImpl&&)                 = delete;
    ReaderImpl& operator=(const ReaderImpl&) = delete;
    ReaderImpl& operator=(ReaderImpl&&)      = delete;

    /// Constructor
    inline explicit ReaderImpl(Reader* const _reader) :
        M_READER(_reader)
    {
    }

    /// Default destructor
    inline ~ReaderImpl() noexcept = default;

    /// Main read function
    inline void read(std::istream& _istream, Backend _backend)
    {
        // Get the image pointer
        auto image = M_READER->getConcreteObject();
        SIGHT_THROW_IF("Output image is null", image == nullptr);

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

#ifdef SIGHT_ENABLE_NVJPEG2K
        if(nv_jpeg_2k() && _backend == Backend::NVJPEG2K)
        {
            read<NvJPEG2KReader>(m_nvJPEG2K, *image, _istream);
        }
        else if(nv_jpeg_2k() && _backend == Backend::NVJPEG2K_J2K)
        {
            read<NvJPEG2KReader>(
                m_nvJPEG2K,
                *image,
                _istream,
                Flag::J2K_STREAM
            );
        }
        else
#endif
        if(_backend == Backend::OPENJPEG)
        {
            read<OpenJPEGReader>(m_openJPEG, *image, _istream);
        }
        else if(_backend == Backend::OPENJPEG_J2K)
        {
            read<OpenJPEGReader>(
                m_openJPEG,
                *image,
                _istream,
                Flag::J2K_STREAM
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(nv_jpeg() && _backend == Backend::NVJPEG)
        {
            read<NvJPEGReader>(m_nvJPEG, *image, _istream);
        }
        else
#endif
        if(_backend == Backend::LIBJPEG)
        {
            read<LibJPEGReader>(m_libJPEG, *image, _istream);
        }
        else if(_backend == Backend::LIBTIFF)
        {
            read<LibTIFFReader>(m_libTIFF, *image, _istream);
        }
        else if(_backend == Backend::LIBPNG)
        {
            read<LibPNGReader>(m_libPNG, *image, _istream);
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
        Flag _flag = Flag::NONE
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
    Reader* const M_READER;

#ifdef SIGHT_ENABLE_NVJPEG
    std::unique_ptr<NvJPEGReader> m_nvJPEG;
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
    std::unique_ptr<NvJPEG2KReader> m_nvJPEG2K;
#endif

    std::unique_ptr<LibJPEGReader> m_libJPEG;
    std::unique_ptr<LibTIFFReader> m_libTIFF;
    std::unique_ptr<LibPNGReader> m_libPNG;
    std::unique_ptr<OpenJPEGReader> m_openJPEG;
};

} // namespace sight::io::bitmap::detail
