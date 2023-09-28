/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/itk/config.hpp"

#include <core/location/single_file.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/Image.hpp>

#include <io/__/writer/GenericObjectWriter.hpp>

namespace sight::io::itk
{

class IO_ITK_CLASS_API NiftiImageWriter : public writer::GenericObjectWriter<data::Image>,
                                          public core::location::single_file,
                                          public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        NiftiImageWriter,
        io::writer::GenericObjectWriter<data::Image>,
        io::writer::factory::make<NiftiImageWriter>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    IO_ITK_API ~NiftiImageWriter() override = default;

    IO_ITK_API void write() override;

    IO_ITK_API std::string extension() const override;
};

} // namespace sight::io::itk
