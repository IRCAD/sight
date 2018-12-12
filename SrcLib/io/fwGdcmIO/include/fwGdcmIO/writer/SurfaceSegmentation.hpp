/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

namespace fwDicomData
{
class DicomSeries;
}

namespace fwJobs
{
class Aggregator;
class Observer;
}

namespace fwGdcmIO
{
namespace writer
{

/**
 * @brief This class handles DICOM Surface Segmentation files writing.
 */
class FWGDCMIO_CLASS_API SurfaceSegmentation :
    public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >,
    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SurfaceSegmentation)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >), (()),
        ::fwDataIO::writer::factory::New< SurfaceSegmentation >);

    /// Constructor
    FWGDCMIO_API SurfaceSegmentation(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor
    FWGDCMIO_API ~SurfaceSegmentation();

    /// Load and start appropriate writing tools.
    FWGDCMIO_API void write() override;

    /// Does nothing
    FWGDCMIO_API std::string extension() override;

    /// Returns logger
    FWGDCMIO_API SPTR(::fwLog::Logger) getLogger() const;

    /// Getter for writer's job
    FWGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    /// Logger
    ::fwLog::Logger::sptr m_logger;

    /// Writer job
    SPTR(::fwJobs::Observer) m_writerJob;

};

} // namespace writer
} // namespace fwGdcmIO
