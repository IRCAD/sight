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

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

#include <data/Array.hpp>
#include <data/location/SingleFile.hpp>

namespace fwDataIO
{
namespace writer
{

/**
 * @brief   Array Writer. Write file format .raw.gz
 *
 *
 * Ircad writer to write a data::Array on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is written in a file with zlib.
 */
class FWDATAIO_CLASS_API GzArrayWriter :  public GenericObjectWriter< data::Array >,
                                          public data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(GzArrayWriter, GenericObjectWriter< data::Array>,
                     ::fwDataIO::writer::factory::New< GzArrayWriter >)

    /// Constructor. Do nothing.
    FWDATAIO_API GzArrayWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~GzArrayWriter();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void write() override;

    /// Defines extension supported by this writer ".raw.gz"
    FWDATAIO_API virtual std::string extension() override;

};

} // namespace writer
} // namespace fwDataIO
