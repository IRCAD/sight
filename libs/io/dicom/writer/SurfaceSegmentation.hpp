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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomInstance.hpp"
#include "io/dicom/exception/Failed.hpp"

#include <core/location/SingleFile.hpp>
#include <core/log/Logger.hpp>

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

namespace fwDicomData
{

class DicomSeries;

}

namespace sight::core::jobs
{

class Aggregator;
class Observer;

}

namespace sight::io::dicom
{

namespace writer
{

/**
 * @brief This class handles DICOM Surface Segmentation files writing.
 */
class IO_DICOM_CLASS_API SurfaceSegmentation :
    public io::base::writer::GenericObjectWriter<data::ModelSeries>,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        SurfaceSegmentation,
        io::base::writer::GenericObjectWriter<data::ModelSeries>,
        io::base::writer::factory::New<SurfaceSegmentation>
    );

    /// Constructor
    IO_DICOM_API SurfaceSegmentation(io::base::writer::IObjectWriter::Key key);

    /// Destructor
    IO_DICOM_API ~SurfaceSegmentation();

    /// Load and start appropriate writing tools.
    IO_DICOM_API void write() override;

    /// Does nothing
    IO_DICOM_API std::string extension() override;

    /// Returns logger
    IO_DICOM_API SPTR(core::log::Logger) getLogger() const;

    /// Getter for writer's job
    IO_DICOM_API SPTR(core::jobs::IJob) getJob() const override;

private:

    /// Logger
    core::log::Logger::sptr m_logger;

    /// Writer job
    SPTR(core::jobs::Observer) m_writerJob;
};

} // namespace writer

} // namespace sight::io::dicom
