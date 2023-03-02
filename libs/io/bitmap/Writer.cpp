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

#include "Writer.hpp"

#include "detail/WriterImpl.hxx"

#include <algorithm>
#include <fstream>

// cspell:ignore nvjpeg nvjpeg2k nppi bitstream LRCP BGRI RGBI NOLINTNEXTLINE LIBJPEG OPENJPEG

namespace sight::io::bitmap
{

Writer::Writer(base::writer::IObjectWriter::Key /*unused*/) :
    m_pimpl(std::make_unique<detail::WriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Writer::~Writer() = default;

//------------------------------------------------------------------------------

std::string Writer::extension() const
{
    try
    {
        const auto& [backend, extensions] = detail::guessBackendOrExtension(
            Backend::ANY,
            getFile().extension().string()
        );

        return extensions.front();
    }
    catch(...)
    {
        return extensions(Backend::LIBTIFF).front();
    }
}

//------------------------------------------------------------------------------

void Writer::write()
{
    write(Backend::ANY, Mode::FAST);
}

//------------------------------------------------------------------------------

void Writer::write(Backend backend, Mode mode)
{
    auto file = getFile();

    try
    {
        if(file.has_parent_path() && !std::filesystem::exists(file.parent_path()))
        {
            std::filesystem::create_directories(file.parent_path());
        }

        // Compute the right backend to use
        const auto& [backend_to_use, extensions_to_use] = detail::guessBackendOrExtension(backend, extension());

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

        write(output, backend_to_use, mode);
    }
    catch(...)
    {
        // Cleanup the created file in case of problem
        std::filesystem::remove_all(file);
        throw;
    }
}

//------------------------------------------------------------------------------

void Writer::write(std::ostream& ostream, Backend backend, Mode mode)
{
    m_pimpl->write(ostream, backend, mode);
}

} // namespace sight::io::bitmap
