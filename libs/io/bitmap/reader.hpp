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

#include <sight/io/bitmap/config.hpp>

#include "backend.hpp"

#include <core/location/single_file.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/container.hpp>
#include <data/image.hpp>

#include <io/__/reader/generic_object_reader.hpp>

#include <ostream>

// cspell:ignore nvjpeg LIBJPEG OPENJPEG

namespace sight::io::bitmap::detail
{

class reader_impl;

}

namespace sight::io::bitmap
{

/**
 * @brief Fast generic bitmap reader. It supports JPEG2000, JPEG, PNG and TIFF format.
 *
 * If a NVIDIA GPU is available and the support has been compiled in, NVidia nvJPEG / nvJPEG2000 library is used to
 * achieve very fast decoding. Otherwise, libjpeg-turbo, openJPEG, libtiff or libPNG are used as fallback.
 * The performance should still be better than VTK or even OpenCV because of direct API calls and avoided unneeded
 * buffer copy.
 */
class SIGHT_IO_BITMAP_CLASS_API reader final : public io::reader::generic_object_reader<data::image>,
                                               public core::location::single_file,
                                               public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        reader,
        io::reader::generic_object_reader<data::image>,
        io::reader::factory::make<reader>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default constructors and assignment operators
    reader(const reader&)            = delete;
    reader(reader&&)                 = delete;
    reader& operator=(const reader&) = delete;
    reader& operator=(reader&&)      = delete;

    /// Constructor/Destructor
    SIGHT_IO_BITMAP_API reader();
    SIGHT_IO_BITMAP_API ~reader() override;

    /// Main writing method from generic_object_reader
    SIGHT_IO_BITMAP_API void read() override;

    /// Specialized writing method that allows to specify the backend and the mode (Fast or Best compression)
    /// @arg backend: the backend to use. Can be LIBJPEG, LIBTIFF, LIBPNG, OPENJPEG or, if available, NVJPEG and
    ///      NVJPEG2K. DEFAULT is LIBTIFF and ANY will guess using the file extension. "*J2K" variant are
    ///      JPEG2000 "stream", without normal meta-data and is only useful for DICOM
    /// @arg mode: The mode to use. Can be FAST or BEST. FAST emphasise speed and BEST emphasise file size
    SIGHT_IO_BITMAP_API void read(backend _backend);

    /// Specialized writing method that allows to write to a ostream
    /// @arg ostream: the stream to write to. It is up to the user to open it.
    /// @arg backend: the backend to use. Can be LIBJPEG, LIBTIFF, LIBPNG, OPENJPEG or, if available, NVJPEG and
    ///      NVJPEG2K. DEFAULT is LIBTIFF. "*_J2K" variant are
    ///      JPEG2000 "stream", without normal meta-data and is only useful for DICOM
    /// @arg mode: The mode to use. Can be FAST or BEST. FAST emphasise speed and BEST emphasise file size
    SIGHT_IO_BITMAP_API void read(
        std::istream& _istream,
        backend _backend = backend::libtiff
    );

    /// Return the extension to use, by default, or the one from file set by single_file::set_file(), if valid
    /// @return an extension as string
    [[nodiscard]] SIGHT_IO_BITMAP_API std::string extension() const override;

private:

    /// PImpl
    std::unique_ptr<detail::reader_impl> m_pimpl;
};

} // namespace sight::io::bitmap
