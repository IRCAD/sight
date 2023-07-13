/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/location/SingleFile.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <data/Image.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

namespace sight::io::itk
{

class IO_ITK_CLASS_API InrImageWriter : public base::writer::GenericObjectWriter<data::Image>,
                                        public core::location::SingleFile,
                                        public core::tools::ProgressAdviser
{
public:

    SIGHT_DECLARE_CLASS(
        InrImageWriter,
        io::base::writer::GenericObjectWriter<data::Image>,
        io::base::writer::factory::New<InrImageWriter>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    IO_ITK_API InrImageWriter(io::base::writer::IObjectWriter::Key key);

    IO_ITK_API ~InrImageWriter() override;

    IO_ITK_API void write() override;

    IO_ITK_API std::string extension() const override;
};

} // namespace sight::io::itk
