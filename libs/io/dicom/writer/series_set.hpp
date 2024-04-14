/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/writer/series.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::io::dicom
{

namespace writer
{

/**
 * @brief   This class manages patient writing, in DICOM file format.
 */
class SIGHT_IO_DICOM_CLASS_API series_set : public io::writer::generic_object_writer<data::series_set>,
                                            public core::location::single_folder,
                                            public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        series_set,
        io::writer::generic_object_writer<data::series_set>,
        io::writer::factory::make<series_set>
    );

    /// Constructor
    SIGHT_IO_DICOM_API series_set();

    /// Destructor
    SIGHT_IO_DICOM_API ~series_set() override = default;

    /**
     * @brief Manage writing tools to save every series.
     */
    SIGHT_IO_DICOM_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    SIGHT_IO_DICOM_API std::string extension() const override;

    /// Get Fiducials Export Mode
    const io::dicom::writer::series::fiducials_export_mode& get_fiducials_export_mode() const
    {
        return m_fiducials_export_mode;
    }

    /// Set Fiducials Export Mode
    void set_fiducials_export_mode(const io::dicom::writer::series::fiducials_export_mode& _fiducials_export_mode)
    {
        m_fiducials_export_mode = _fiducials_export_mode;
    }

protected:

    /// Fiducials Export Mode
    io::dicom::writer::series::fiducials_export_mode m_fiducials_export_mode;
};

} // namespace writer

} // namespace sight::io::dicom
