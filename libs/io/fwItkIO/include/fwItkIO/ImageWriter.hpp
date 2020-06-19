/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwItkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwItkIO
{

class ImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                    public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassMacro(ImageWriter, ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image>,
                     ::fwDataIO::writer::factory::New< ImageWriter >);
    fwCoreAllowSharedFromThis();

    FWITKIO_API ImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    FWITKIO_API ~ImageWriter();

    FWITKIO_API void write() override;

    FWITKIO_API std::string extension() override;
};

} // namespace fwItkIO
