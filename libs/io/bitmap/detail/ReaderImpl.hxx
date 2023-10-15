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

#include <data/helper/MedicalImage.hpp>

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
    inline explicit ReaderImpl(Reader* const reader) :
        m_reader(reader)
    {
    }

    /// Default destructor
    inline ~ReaderImpl() noexcept = default;

    /// Main read function
    inline void read(std::istream& istream, Backend backend)
    {
        // Get the image pointer
        auto image = m_reader->getConcreteObject();
        SIGHT_THROW_IF("Output image is null", image == nullptr);

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

#ifdef SIGHT_ENABLE_NVJPEG2K
        if(nvJPEG2K() && backend == Backend::NVJPEG2K)
        {
            read<NvJPEG2KReader>(m_nvJPEG2K, *image, istream);
        }
        else if(nvJPEG2K() && backend == Backend::NVJPEG2K_J2K)
        {
            read<NvJPEG2KReader>(
                m_nvJPEG2K,
                *image,
                istream,
                Flag::J2K_STREAM
            );
        }
        else
#endif
        if(backend == Backend::OPENJPEG)
        {
            read<OpenJPEGReader>(m_openJPEG, *image, istream);
        }
        else if(backend == Backend::OPENJPEG_J2K)
        {
            read<OpenJPEGReader>(
                m_openJPEG,
                *image,
                istream,
                Flag::J2K_STREAM
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(nvJPEG() && backend == Backend::NVJPEG)
        {
            read<NvJPEGReader>(m_nvJPEG, *image, istream);
        }
        else
#endif
        if(backend == Backend::LIBJPEG)
        {
            read<LibJPEGReader>(m_libJPEG, *image, istream);
        }
        else if(backend == Backend::LIBTIFF)
        {
            read<LibTIFFReader>(m_libTIFF, *image, istream);
        }
        else if(backend == Backend::LIBPNG)
        {
            read<LibPNGReader>(m_libPNG, *image, istream);
        }
        else
        {
            SIGHT_THROW("No suitable backend found.");
        }

        sight::data::helper::MedicalImage::checkImageSliceIndex(image);
    }

private:

    //------------------------------------------------------------------------------

    template<typename W>
    inline static void read(
        std::unique_ptr<W>& backend,
        data::image& image,
        std::istream& istream,
        Flag flag = Flag::NONE
)
    {
        if(backend == nullptr)
        {
            backend = std::make_unique<W>();
            SIGHT_THROW_IF("Failed to initialize" << backend->m_name << " backend.", !backend->m_valid);
        }

        backend->read(image, istream, flag);
    }

    /// Pointer to the public interface
    Reader* const m_reader;

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
