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
    inline explicit WriterImpl(Writer* const writer) :
        m_writer(writer)
    {
    }

    /// Default destructor
    inline ~WriterImpl() noexcept = default;

    /// Main write function
    inline void write(std::ostream& ostream, Backend backend, Writer::Mode mode)
    {
        // Get the image pointer
        const auto& image = m_writer->getConcreteObject();
        SIGHT_THROW_IF("Source image is null", image == nullptr);

        /// @todo Should we split volume in 2D slices ?
        const auto& sizes = image->getSize();
        SIGHT_THROW_IF("Unsupported image dimension", sizes[0] == 0 || sizes[1] == 0);

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

#ifdef SIGHT_ENABLE_NVJPEG2K
        if(nvJPEG2K() && backend == Backend::NVJPEG2K)
        {
            try
            {
                write<NvJPEG2KWriter>(m_nvJPEG2K, *image, ostream, mode);
            }
            catch(const std::exception& e)
            {
                // This happens when trying to encode uniform random data which cannot be compressed
                // This is obviously a bug in the encoder (reported and known by NVidia), although this should not
                // happen with real data in the real world. To be in the safe side, we fallback to another backend.
                SIGHT_ERROR("Failed to write image with nvJPEG2K: " << e.what() << ". Fallback to OpenJPEG.");
                write<OpenJPEGWriter>(m_openJPEG, *image, ostream, mode);
            }
        }
        else if(nvJPEG2K() && backend == Backend::NVJPEG2K_J2K)
        {
            try
            {
                write<NvJPEG2KWriter>(m_nvJPEG2K, *image, ostream, mode, Flag::J2K_STREAM);
            }
            catch(const std::exception& e)
            {
                // Same as above...
                SIGHT_ERROR("Failed to write image with nvJPEG2K: " << e.what() << ". Fallback to OpenJPEG.");
                write<OpenJPEGWriter>(m_openJPEG, *image, ostream, mode, Flag::J2K_STREAM);
            }
        }
        else
#endif
        if(backend == Backend::OPENJPEG)
        {
            write<OpenJPEGWriter>(m_openJPEG, *image, ostream, mode);
        }
        else if(backend == Backend::OPENJPEG_J2K)
        {
            write<OpenJPEGWriter>(
                m_openJPEG,
                *image,
                ostream,
                mode,
                Flag::J2K_STREAM
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(nvJPEG() && backend == Backend::NVJPEG)
        {
            write<NvJPEGWriter>(m_nvJPEG, *image, ostream, mode);
        }
        else
#endif
        if(backend == Backend::LIBJPEG)
        {
            write<LibJPEGWriter>(m_libJPEG, *image, ostream, mode);
        }
        else if(backend == Backend::LIBTIFF)
        {
            write<LibTIFFWriter>(m_libTIFF, *image, ostream, mode);
        }
        else if(backend == Backend::LIBPNG)
        {
            write<LibPNGWriter>(m_libPNG, *image, ostream, mode);
        }
        else
        {
            SIGHT_THROW("No suitable backend found.");
        }
    }

private:

    //------------------------------------------------------------------------------

    template<typename W>
    inline static void write(
        std::unique_ptr<W>& backend,
        const data::Image& image,
        std::ostream& ostream,
        Writer::Mode mode,
        Flag flag = Flag::NONE
)
    {
        if(backend == nullptr)
        {
            backend = std::make_unique<W>();
            SIGHT_THROW_IF("Failed to initialize" << backend->m_name << " backend.", !backend->m_valid);
        }

        backend->write(image, ostream, mode, flag);
    }

    /// Pointer to the public interface
    Writer* const m_writer;

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