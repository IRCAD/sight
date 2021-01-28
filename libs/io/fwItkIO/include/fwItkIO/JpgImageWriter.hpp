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

#include "fwItkIO/config.hpp"

#include <core/tools/ProgressAdviser.hpp>

#include <data/Image.hpp>
#include <data/location/Folder.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

#include <filesystem>

namespace fwItkIO
{

class JpgImageWriter :  public io::base::writer::GenericObjectWriter< data::Image >,
                        public data::location::enableFolder< io::base::writer::IObjectWriter >,
                        public core::tools::ProgressAdviser
{

public:

    fwCoreClassMacro(JpgImageWriter, io::base::writer::GenericObjectWriter< data::Image>,
                     io::base::writer::factory::New< JpgImageWriter >);
    fwCoreAllowSharedFromThis();

    FWITKIO_API JpgImageWriter(io::base::writer::IObjectWriter::Key key);

    FWITKIO_API ~JpgImageWriter();

    FWITKIO_API void write() override;

    FWITKIO_API std::string extension() override;
};

} // namespace fwItkIO
