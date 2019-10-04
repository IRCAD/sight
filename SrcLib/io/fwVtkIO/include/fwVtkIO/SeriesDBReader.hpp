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

#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <filesystem>

namespace fwJobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Read a SeriesDB.
 *
 * Read VTK Mesh or Image files using the VTK lib, convert to ModelSeries or ImageSeries and push to SeriesDB.
 */
class SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                       public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassMacro(SeriesDBReader, ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                     ::fwDataIO::reader::factory::New< SeriesDBReader >);
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~SeriesDBReader();

    //! @brief Reading operator.
    FWVTKIO_API void read() override;

    /// @return ".vtk"
    FWVTKIO_API std::string extension() override;

    //------------------------------------------------------------------------------

    void setLazyMode(bool lazyMode)
    {
        m_lazyMode = lazyMode;
    }

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

    bool m_lazyMode;
};

} // namespace fwVtkIO
