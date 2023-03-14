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

#include "backend.hpp"

#include "io/bitmap/config.hpp"

#include <core/location/SingleFile.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <data/Image.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

#include <ostream>

// cspell:ignore nvjpeg LIBJPEG OPENJPEG

namespace sight::io::bitmap::detail
{

class WriterImpl;

}

namespace sight::io::bitmap
{

/**
 * @brief Fast generic bitmap writer. It supports JPEG2000, JPEG, PNG and TIFF format.
 *
 * If a NVIDIA GPU is available and the support has been compiled in, NVidia nvJPEG / nvJPEG2000 library is used to
 * achieve very fast encoding. Otherwise, libjpeg-turbo, openJPEG, libtiff or libPNG are used as fallback.
 * The performance should still be better than VTK or even OpenCV because of direct API calls and avoided unneeded
 * buffer copy.
 *
 * Some backends have a FAST and a BEST mode. FAST compress faster and BEST compress better, at the expanse of the
 * speed. For lossy codecs, the configuration will always provide the maximum possible quality, which may reduce
 * compression.
 *
 * For lossless mode, TIFF "BEST" currently provide the best mix between speed and compression ratio (350 FPS), while
 * "FAST" mode is the fastest backend, but does not compress very well data (450 FPS).
 *
 * When a GPU is available, nothing beats the speed of nvJPEG (>1300 FPS) while preserving "visually" lossless quality.
 * On the contrary, nvJPEG2000 is fully lossless and provides a very good compression ratio, but is, of course, slower
 * (still 60 fps guaranteed on nowadays computers).
 */
class IO_BITMAP_CLASS_API Writer final : public base::writer::GenericObjectWriter<data::Image>,
                                         public core::location::SingleFile,
                                         public core::tools::ProgressAdviser
{
public:

    enum class Mode : std::uint8_t
    {
        FAST = 1,
        BEST = 2
    };

    SIGHT_DECLARE_CLASS(
        Writer,
        base::writer::GenericObjectWriter<data::Image>,
        base::writer::factory::New<Writer>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default constructors and assignment operators
    Writer()                         = delete;
    Writer(const Writer&)            = delete;
    Writer(Writer&&)                 = delete;
    Writer& operator=(const Writer&) = delete;
    Writer& operator=(Writer&&)      = delete;

    /// Constructor/Destructor
    IO_BITMAP_API Writer(io::base::writer::IObjectWriter::Key key);
    IO_BITMAP_API ~Writer() override;

    /// Main writing method from GenericObjectWriter
    IO_BITMAP_API void write() override;

    /// Specialized writing method that allows to specify the backend and the mode (Fast or Best compression)
    /// @arg backend: the backend to use. Can be LIBJPEG, LIBTIFF, LIBPNG, OPENJPEG or, if available, NVJPEG and
    ///      NVJPEG2K. DEFAULT is LIBTIFF and ANY will guess using the file extension. "*J2K" variant are
    ///      JPEG2000 "stream", without normal meta-data and is only useful for DICOM
    /// @arg mode: The mode to use. Can be FAST or BEST. FAST emphasise speed and BEST emphasise file size
    IO_BITMAP_API void write(Backend backend, Mode mode = Mode::FAST);

    /// Specialized writing method that allows to write to a ostream
    /// @arg ostream: the stream to write to. It is up to the user to open it.
    /// @arg backend: the backend to use. Can be LIBJPEG, LIBTIFF, LIBPNG, OPENJPEG or, if available, NVJPEG and
    ///      NVJPEG2K. DEFAULT is LIBTIFF. "*_J2K" variant are
    ///      JPEG2000 "stream", without normal meta-data and is only useful for DICOM
    /// @arg mode: The mode to use. Can be FAST or BEST. FAST emphasise speed and BEST emphasise file size
    IO_BITMAP_API void write(
        std::ostream& ostream,
        Backend backend = Backend::LIBTIFF,
        Mode mode       = Mode::FAST
    );

    /// Return the extension to use, by default, or the one from file set by SingleFile::setFile(), if valid
    /// @return an extension as string
    [[nodiscard]] IO_BITMAP_API std::string extension() const override;

private:

    /// PImpl
    std::unique_ptr<detail::WriterImpl> m_pimpl;
};

} // namespace sight::io::bitmap