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

#include "fwVtkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <boost/filesystem/path.hpp>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief   Read a MetaImage.
 *
 * Read a MetaImage using the VTK lib
 */
class MetaImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                        public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassMacro(MetaImageReader, ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                     ::fwDataIO::reader::factory::New< MetaImageReader >);

    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API MetaImageReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~MetaImageReader();

    //! @brief Reading operator.
    FWVTKIO_API void read() override;

    /// @return ".mhd"
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace fwVtkIO
