/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/tools/ProgressAdviser.hpp>

#include <data/Image.hpp>
#include <data/location/SingleFile.hpp>

#include <io/base/reader/GenericObjectReader.hpp>

namespace sight::io::itk
{

class ImageReader : public io::base::reader::GenericObjectReader< data::Image >,
                    public data::location::enableSingleFile< io::base::reader::IObjectReader >,
                    public core::tools::ProgressAdviser
{

public:

    fwCoreClassMacro(ImageReader, io::base::reader::GenericObjectReader< data::Image>,
                     io::base::reader::factory::New< ImageReader >);
    fwCoreAllowSharedFromThis();

    IO_ITK_API ImageReader(io::base::reader::IObjectReader::Key key);

    IO_ITK_API ~ImageReader();

    IO_ITK_API void read() override;
};

} // namespace sight::io::itk
