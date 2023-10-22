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

#include "Reader.hpp"

#include "detail/ReaderImpl.hxx"

#include <algorithm>
#include <fstream>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP BGRI RGBI NOLINTNEXTLINE LIBJPEG OPENJPEG

namespace sight::io::bitmap
{

Reader::Reader() :
    m_pimpl(std::make_unique<detail::ReaderImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Reader::~Reader() = default;

//------------------------------------------------------------------------------

std::string Reader::extension() const
{
    try
    {
        const auto& [backend, extensions] = detail::guess_backend_or_extension(
            Backend::ANY,
            get_file().extension().string()
        );

        return extensions.front();
    }
    catch(...)
    {
        return extensions(Backend::LIBTIFF).front();
    }
}

//------------------------------------------------------------------------------

void Reader::read()
{
    read(Backend::ANY);
}

//------------------------------------------------------------------------------

void Reader::read(Backend _backend)
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
            std::none_of(
                extensions_to_use.cbegin(),
                extensions_to_use.cend(),
                [&](const auto& extension)
            {
                return current_extension.ends_with(extension);
            })
        );
    }

    // Open the input file
    std::ifstream input;
    input.open(file.string(), std::ios::in | std::ios::binary);

    read(input, backend_to_use);
}

//------------------------------------------------------------------------------

void Reader::read(std::istream& _istream, Backend _backend)
{
    m_pimpl->read(_istream, _backend);
}

} // namespace sight::io::bitmap
