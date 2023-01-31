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

#pragma once

#include "types.hpp"

#ifdef SIGHT_ENABLE_NVJPEG
#include "NvJPEG.hxx"
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
#include "NvJPEG2K.hxx"
#endif

#include "LibJPEG.hxx"
#include "LibTIFF.hxx"
#include "LibPNG.hxx"
#include "OpenJPEG.hxx"

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
    inline void write(std::ostream& ostream, Writer::Backend backend, Writer::Mode mode)
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
        if(Writer::nvJPEG2K() && backend == Writer::Backend::NVJPEG2K)
        {
            write<NvJPEG2K>(m_nvJPEG2K, *image, ostream, static_cast<ExtendedMode>(mode));
        }
        else if(Writer::nvJPEG2K() && backend == Writer::Backend::NVJPEG2K_J2K)
        {
            write<NvJPEG2K>(
                m_nvJPEG2K,
                *image,
                ostream,
                mode == Writer::Mode::FAST ? ExtendedMode::J2K_FAST : ExtendedMode::J2K_BEST
            );
        }
        else
#endif
        if(backend == Writer::Backend::OPENJPEG)
        {
            write<OpenJPEG>(m_openJPEG, *image, ostream, static_cast<ExtendedMode>(mode));
        }
        else if(backend == Writer::Backend::OPENJPEG_J2K)
        {
            write<OpenJPEG>(
                m_openJPEG,
                *image,
                ostream,
                mode == Writer::Mode::FAST ? ExtendedMode::J2K_FAST : ExtendedMode::J2K_BEST
            );
        }
        else

#ifdef SIGHT_ENABLE_NVJPEG
        if(Writer::nvJPEG() && backend == Writer::Backend::NVJPEG)
        {
            write<NvJPEG>(m_nvJPEG, *image, ostream, static_cast<ExtendedMode>(mode));
        }
        else
#endif
        if(backend == Writer::Backend::LIBJPEG)
        {
            write<LibJPEG>(m_libJPEG, *image, ostream, static_cast<ExtendedMode>(mode));
        }
        else if(backend == Writer::Backend::LIBTIFF)
        {
            write<LibTIFF>(m_libTIFF, *image, ostream, static_cast<ExtendedMode>(mode));
        }
        else if(backend == Writer::Backend::LIBPNG)
        {
            write<LibPNG>(m_libPNG, *image, ostream, static_cast<ExtendedMode>(mode));
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
        ExtendedMode mode
)
    {
        if(backend == nullptr)
        {
            backend = std::make_unique<W>();
            SIGHT_THROW_IF("Failed to initialize" << backend->m_name << " backend.", !backend->m_valid);
        }

        backend->write(image, ostream, mode);
    }

    /// Pointer to the public interface
    Writer* const m_writer;

#ifdef SIGHT_ENABLE_NVJPEG
    std::unique_ptr<NvJPEG> m_nvJPEG;
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
    std::unique_ptr<NvJPEG2K> m_nvJPEG2K;
#endif

    std::unique_ptr<LibJPEG> m_libJPEG;
    std::unique_ptr<LibTIFF> m_libTIFF;
    std::unique_ptr<LibPNG> m_libPNG;
    std::unique_ptr<OpenJPEG> m_openJPEG;
};

} // namespace sight::io::bitmap::detail
