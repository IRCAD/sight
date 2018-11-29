/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{
class ModelSeries;
}

namespace fwJobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Write a ::fwData::Reconstruction.
 *
 * Write a ::fwData::Reconstruction as .obj file using the VTK lib and the obj format.
 */
class ModelSeriesObjWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (ModelSeriesObjWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >),
        (()),
        ::fwDataIO::writer::factory::New< ModelSeriesObjWriter >
        );

    fwCoreAllowSharedFromThis();

    //! Constructor.
    FWVTKIO_API ModelSeriesObjWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! Destructor.
    FWVTKIO_API ~ModelSeriesObjWriter();

    //! Writing operator.
    FWVTKIO_API void write() override;

    /**
     * @return ".obj"
     */
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace fwVtkIO
