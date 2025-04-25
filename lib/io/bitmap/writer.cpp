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

#include "writer.hpp"

#include "detail/writer_impl.hxx"

#include <algorithm>
#include <fstream>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP BGRI RGBI NOLINTNEXTLINE LIBJPEG OPENJPEG

namespace sight::io::bitmap
{

writer::writer() :
    m_pimpl(std::make_unique<detail::writer_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
writer::~writer() = default;

//------------------------------------------------------------------------------

std::string writer::extension() const
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

void writer::write()
{
    write(backend::any, mode::fast);
}

//------------------------------------------------------------------------------

std::size_t writer::write(backend _backend, mode _mode)
{
    auto file = get_file();

    try
    {
        if(file.has_parent_path() && !std::filesystem::exists(file.parent_path()))
        {
            std::filesystem::create_directories(file.parent_path());
        }

        // Compute the right backend to use
        const auto& [backend_to_use, extensions_to_use] = detail::guess_backend_or_extension(_backend, extension());

        // If there is no extension
        if(!file.has_extension())
        {
            // Add the good one
            const auto& ext = extensions_to_use.front();
            file.replace_extension(ext);

            SIGHT_WARN("No extension given, using '" << ext << "'");
        }
        else
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

        // Open the output file
        std::ofstream output;
        output.open(file.string(), std::ios::out | std::ios::binary | std::ios::trunc);

        return write(output, backend_to_use, _mode);
    }
    catch(...)
    {
        // Cleanup the created file in case of problem
        std::filesystem::remove_all(file);
        throw;
    }
}

//------------------------------------------------------------------------------

std::size_t writer::write(std::ostream& _ostream, backend _backend, mode _mode)
{
    return m_pimpl->write(_ostream, _backend, _mode);
}

//------------------------------------------------------------------------------

std::size_t writer::write(std::uint8_t** _buffer, backend _backend, mode _mode)
{
    return m_pimpl->write(_buffer, _backend, _mode);
}

//------------------------------------------------------------------------------

std::size_t writer::write(std::uint8_t* _buffer, backend _backend, mode _mode)
{
    return m_pimpl->write(_buffer, _backend, _mode);
}

//------------------------------------------------------------------------------

std::size_t writer::write(std::vector<uint8_t>& _buffer, backend _backend, mode _mode)
{
    return m_pimpl->write(_buffer, _backend, _mode);
}

} // namespace sight::io::bitmap
