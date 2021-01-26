/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <core/log/Logger.hpp>

#include <data/ImageSeries.hpp>
#include <data/location/SingleFile.hpp>
#include <data/ModelSeries.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

namespace fwDicomData
{
class DicomSeries;
}

namespace sight::core::jobs
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
    public ::fwDataIO::writer::GenericObjectWriter< data::ModelSeries >,
    public data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(SurfaceSegmentation, ::fwDataIO::writer::GenericObjectWriter< data::ModelSeries >,
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
    FWGDCMIO_API SPTR(core::log::Logger) getLogger() const;

    /// Getter for writer's job
    FWGDCMIO_API SPTR(core::jobs::IJob) getJob() const override;

private:

    /// Logger
    core::log::Logger::sptr m_logger;

    /// Writer job
    SPTR(core::jobs::Observer) m_writerJob;

};

} // namespace writer
} // namespace fwGdcmIO
