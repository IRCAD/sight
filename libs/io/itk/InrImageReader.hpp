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

#include <core/location/single_file.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/Image.hpp>

#include <io/__/reader/GenericObjectReader.hpp>

namespace sight::io::itk
{

class IO_ITK_CLASS_API InrImageReader : public reader::GenericObjectReader<data::Image>,
                                        public core::location::single_file,
                                        public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        InrImageReader,
        io::reader::GenericObjectReader<data::Image>,
        io::reader::factory::make<InrImageReader>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    IO_ITK_API ~InrImageReader() override = default;

    IO_ITK_API void read() override;
};

} // namespace sight::io::itk
