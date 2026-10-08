/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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
#include "libpng_reader.hxx"
#include "libtiff_reader.hxx"
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
    explicit reader_impl(reader* const _reader) :
        m_reader(_reader)
    {
    }

    /// Default destructor
    ~reader_impl() noexcept = default;

    /// Main read function
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void read(std::istream& _istream, backend _backend)
    {
        // Get the image pointer
        auto image = m_reader->get_concrete_object();

        if(image == nullptr)
        {
            image = std::make_shared<data::image>();
            m_reader->set_object(image);
        }

        // Protect the image from dump
        const auto dump_lock = image->dump_lock();

        const auto& backend_impl = get_reader_backend(_backend);
        SIGHT_THROW_IF("No suitable backend found.", backend_impl == nullptr);

        backend_impl->read(
            *image,
            _istream,
            _backend == backend::nvjpeg2k_j2k
            ? flag::j2k_stream
            : _backend == backend::openjpeg_j2k
            ? flag::j2k_stream
            : flag::none
        );

        sight::data::helper::medical_image::check_image_slice_index(*image);
    }

    /// Main read function
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void read(
        const std::uint8_t* const _input,
        std::size_t _input_size,
        std::uint8_t* const _output,
        backend _backend
)
    {
        const auto& backend_impl = get_reader_backend(_backend);

        if(_output == nullptr)
        {
            // Get or create the image shared pointer
            auto image = m_reader->get_concrete_object();

            if(image == nullptr)
            {
                image = std::make_shared<data::image>();
                m_reader->set_object(image);
            }

            // Dump lock in case the image has been created here
            const auto dump_lock = image->dump_lock();

            // Read the image
            backend_impl->read(
                *image,
                _input,
                _input_size,
                nullptr,
                _backend == backend::nvjpeg2k_j2k
                ? flag::j2k_stream
                : _backend == backend::openjpeg_j2k
                ? flag::j2k_stream
                : flag::none
            );

            sight::data::helper::medical_image::check_image_slice_index(*image);
        }
        else
        {
            backend_impl->read(
                std::nullopt,
                _input,
                _input_size,
                _output,
                _backend == backend::nvjpeg2k_j2k
                ? flag::j2k_stream
                : _backend == backend::openjpeg_j2k
                ? flag::j2k_stream
                : flag::none
            );
        }
    }

private:

    //------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    const std::unique_ptr<reader_backend>& get_reader_backend(backend _backend)
    {
        switch(_backend)
        {
#ifdef SIGHT_ENABLE_NVJPEG
            case backend::nvjpeg:
                if(!nvjpeg())
                {
                    break;
                }

                if(!m_nvjpeg)
                {
                    m_nvjpeg = std::make_unique<nvjpeg_reader>();
                }
                return m_nvjpeg;
#endif
#ifdef SIGHT_ENABLE_NVJPEG2K
            case backend::nvjpeg2k:
            case backend::nvjpeg2k_j2k:
                if(!nvjpeg2k())
                {
                    break;
                }

                if(!m_nvjpeg2k)
                {
                    m_nvjpeg2k = std::make_unique<nvjpeg2k_reader>();
                }
                return m_nvjpeg2k;
#endif
            case backend::libjpeg: // NOLINT(bugprone-branch-clone)
                if(!m_libjpeg)
                {
                    m_libjpeg = std::make_unique<libjpeg_reader>();
                }

                return m_libjpeg;

            case backend::libtiff: // NOLINT(bugprone-branch-clone)
                if(!m_libtiff)
                {
                    m_libtiff = std::make_unique<libtiff_reader>();
                }

                return m_libtiff;

            case backend::libpng: // NOLINT(bugprone-branch-clone)
                if(!m_libpng)
                {
                    m_libpng = std::make_unique<libpng_reader>();
                }

                return m_libpng;

            case backend::openjpeg:     // NOLINT(bugprone-branch-clone)
            case backend::openjpeg_j2k: // NOLINT(bugprone-branch-clone)
                if(!m_openjpeg)
                {
                    m_openjpeg = std::make_unique<openjpeg_reader>();
                }

                return m_openjpeg;

            default:
                break;
        }

        SIGHT_THROW("No suitable backend found.");
    }

    /// Pointer to the public interface
    reader* const m_reader {};

#ifdef SIGHT_ENABLE_NVJPEG
    std::unique_ptr<reader_backend> m_nvjpeg;
#endif

#ifdef SIGHT_ENABLE_NVJPEG2K
    std::unique_ptr<reader_backend> m_nvjpeg2k;
#endif

    std::unique_ptr<reader_backend> m_libjpeg;
    std::unique_ptr<reader_backend> m_libtiff;
    std::unique_ptr<reader_backend> m_libpng;
    std::unique_ptr<reader_backend> m_openjpeg;
};

} // namespace sight::io::bitmap::detail
