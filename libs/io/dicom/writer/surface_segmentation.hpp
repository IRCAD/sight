/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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
#include "io/dicom/container/dicom_instance.hpp"
#include "io/dicom/exception/failed.hpp"

#include <core/location/single_file.hpp>
#include <core/log/logger.hpp>

#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::core::jobs
{

class aggregator;
class observer;

} // namespace sight::core::jobs

namespace sight::io::dicom::writer
{

/**
 * @brief This class handles DICOM Surface Segmentation files writing.
 */
class IO_DICOM_CLASS_API surface_segmentation :
    public io::writer::generic_object_writer<data::model_series>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(
        surface_segmentation,
        io::writer::generic_object_writer<data::model_series>,
        io::writer::factory::make<surface_segmentation>
    );

    /// Constructor
    IO_DICOM_API surface_segmentation();

    /// Destructor
    IO_DICOM_API ~surface_segmentation() override;

    /// Load and start appropriate writing tools.
    IO_DICOM_API void write() override;

    /// Does nothing
    IO_DICOM_API std::string extension() const override;

    /// Returns logger
    IO_DICOM_API SPTR(core::log::logger) get_logger() const;

    /// Getter for writer's job
    IO_DICOM_API SPTR(core::jobs::base) get_job() const override;

private:

    /// Logger
    core::log::logger::sptr m_logger;

    /// Writer job
    SPTR(core::jobs::observer) m_writer_job;
};

} // namespace sight::io::dicom::writer
