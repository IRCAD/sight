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

#include "io/dicom/container/dicom_instance.hpp"
#include "io/dicom/exception/failed.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/image_series.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::io::dicom::writer
{

/**
 * @brief This class handles DICOM files writing.
 * It defines needs of data storage and delegates writing to appropriate tools.
 * It manages all related data of one patient.
 */
class SIGHT_IO_DICOM_CLASS_API series : public io::writer::generic_object_writer<data::series>,
                                        public core::location::single_folder,
                                        public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(series, io::writer::generic_object_writer<data::series>);

    //cspell: ignore ComprehensiveSRIOD Comprehensive3DSRIOD
    enum fiducials_export_mode
    {
        spatial_fiducials    = 1, /*! Export Fiducials using SpatialFiducialsIOD */
        comprehensive_sr     = 2, /*! Export Fiducials using ComprehensiveSRIOD */
        comprehensive_3_d_sr = 3  /*! Export Fiducials using Comprehensive3DSRIOD */
    };

    using dicom_instance_map_t = std::map<std::string, std::shared_ptr<io::dicom::container::dicom_instance> >;

    /// Destructor
    SIGHT_IO_DICOM_API ~series() override = default;

    /**
     * @brief Load and start appropriate writing tools.
     * In addition, it creates an adapted folder tree where DICOM files will be stored.
     * It manages writing of the following general DICOM modules : patient, study and series
     * @note Currently, it handles image, SR and RT document writing.
     * @throw io::dicom::exception::Failed
     */
    SIGHT_IO_DICOM_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    SIGHT_IO_DICOM_API std::string extension() const override;

    /// Get Fiducials Export Mode
    const fiducials_export_mode& get_fiducials_export_mode() const
    {
        return m_fiducials_export_mode;
    }

    /// Set Fiducials Export Mode);
    void set_fiducials_export_mode(const fiducials_export_mode& _fiducials_export_mode)
    {
        m_fiducials_export_mode = _fiducials_export_mode;
    }

private:

    /**
     * @brief Check if there is comment on acquisition.
     * @param[in] _series ImageSeries that must be checked.
     */
    static bool has_document_sr(const data::image_series::csptr& _series);

    /**
     * @brief Returns the image instance used to create the reconstruction
     * @note This function assume that one and only one image instance has been created prior calling this function.
     * The created image instance must be the one used to generate the reconstruction.
     */
    SPTR(io::dicom::container::dicom_instance) get_image_instance();

    /// Dicom Instance map used to keep information between series writing calls
    dicom_instance_map_t m_dicom_instance_map;

    /// Fiducials Export Mode
    fiducials_export_mode m_fiducials_export_mode {spatial_fiducials};
};

} // namespace sight::io::dicom::writer
