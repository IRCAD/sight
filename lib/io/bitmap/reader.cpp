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

#include "reader.hpp"

#include "detail/reader_impl.hxx"

#include <core/notification/observer.hpp>

#include <algorithm>
#include <fstream>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP BGRI RGBI NOLINTNEXTLINE LIBJPEG OPENJPEG

namespace sight::io::bitmap
{

reader::reader() :
    m_pimpl(std::make_unique<detail::reader_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
reader::~reader() = default;

//------------------------------------------------------------------------------

std::string reader::extension() const
{
    try
    {
        const auto& [backend, extensions] = detail::guess_backend_or_extension(
            backend::any,
            get_file().extension().string()
        );

        return extensions.front();
    }
    catch(...)
    {
        return extensions(backend::libtiff).front();
    }
}

//------------------------------------------------------------------------------

void reader::read(sight::core::notification::observer::sptr _progress)
{
    read(backend::any);
    _progress->done();
}

//------------------------------------------------------------------------------

void reader::read(backend _backend)
{
    auto file = get_file();

    if(file.has_parent_path() && !std::filesystem::exists(file.parent_path()))
    {
        std::filesystem::create_directories(file.parent_path());
    }

    // Compute the right backend to use
    const auto& [backend_to_use, extensions_to_use] = detail::guess_backend_or_extension(_backend, extension());

    // If there is an extension
    if(file.has_extension())
    {
        // Check that the extension is valid
        const auto& current_extension = file.extension().string();

        SIGHT_THROW_IF(
            "Unsupported image extension: '" << file.extension().string() << "'",
            std::ranges::none_of(
                extensions_to_use,
                [&](const auto& _extension)
            {
                return current_extension.ends_with(_extension);
            })
        );
    }

    // Open the input file
    std::ifstream input;
    input.open(file.string(), std::ios::in | std::ios::binary);

    read(input, backend_to_use);
}

//------------------------------------------------------------------------------

void reader::read(std::istream& _istream, backend _backend)
{
    m_pimpl->read(_istream, _backend);
}

//------------------------------------------------------------------------------

void reader::read(
    const std::uint8_t* const _input,
    std::size_t _input_size,
    backend _backend,
    std::uint8_t* const _output
)
{
    m_pimpl->read(_input, _input_size, _output, _backend);
}

} // namespace sight::io::bitmap
