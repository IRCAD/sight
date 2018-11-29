/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWITKIO_JPGIMAGEWRITER_HPP__
#define __FWITKIO_JPGIMAGEWRITER_HPP__

#include "fwItkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include <boost/filesystem/path.hpp>

namespace fwItkIO
{

class JpgImageWriter :  public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                        public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                        public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((JpgImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< JpgImageWriter >
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API JpgImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    FWITKIO_API ~JpgImageWriter();

    FWITKIO_API void write() override;

    FWITKIO_API std::string extension() override;
};

} // namespace fwItkIO

#endif // __FWITKIO_JPGIMAGEWRITER_HPP__
