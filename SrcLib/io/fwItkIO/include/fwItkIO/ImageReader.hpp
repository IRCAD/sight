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

#ifndef __FWITKIO_IMAGEREADER_HPP__
#define __FWITKIO_IMAGEREADER_HPP__

#include "fwItkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwItkIO
{

class ImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                    public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ImageReader)(::fwDataIO::reader::GenericObjectReader< ::fwData::Image>),
                                           (()),
                                           ::fwDataIO::reader::factory::New< ImageReader >
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API ImageReader(::fwDataIO::reader::IObjectReader::Key key);

    FWITKIO_API ~ImageReader();

    FWITKIO_API void read() override;
};

} // namespace fwItkIO

#endif // __FWITKIO_IMAGEREADER_HPP__
