/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomInstance.hpp"
#include "io/dicom/exception/Failed.hpp"

#include <core/location/SingleFolder.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <data/ImageSeries.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

namespace sight::io::dicom
{

namespace writer
{

/**
 * @brief This class handles DICOM files writing.
 * It defines needs of data storage and delegates writing to appropriate tools.
 * It manages all related data of one patient.
 */
class IO_DICOM_CLASS_API Series : public io::base::writer::GenericObjectWriter<data::Series>,
                                  public core::location::SingleFolder,
                                  public core::tools::ProgressAdviser
{
public:

    SIGHT_DECLARE_CLASS(
        Series,
        io::base::writer::GenericObjectWriter<data::Series>,
        io::base::writer::factory::New<Series>
    );

    typedef enum
    {
        SPATIAL_FIDUCIALS   = 1, /*! Export Fiducials using SpatialFiducialsIOD */
        COMPREHENSIVE_SR    = 2, /*! Export Fiducials using ComprehensiveSRIOD */
        COMPREHENSIVE_3D_SR = 3 /*! Export Fiducials using Comprehensive3DSRIOD */
    } FiducialsExportMode;

    typedef std::map<std::string, SPTR(io::dicom::container::DicomInstance)> DicomInstanceMapType;

    /// Constructor
    IO_DICOM_API Series(io::base::writer::IObjectWriter::Key key);

    /// Destructor
    IO_DICOM_API ~Series();

    /**
     * @brief Load and start appropriate writing tools.
     * In addition, it creates an adapted folder tree where DICOM files will be stored.
     * It manages writing of the following general DICOM modules : patient, study and series
     * @note Currently, it handles image, SR and RT document writing.
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    IO_DICOM_API std::string extension() override;

    /// Get Fiducials Export Mode
    const FiducialsExportMode& getFiducialsExportMode() const
    {
        return m_fiducialsExportMode;
    }

    /// Set Fiducials Export Mode);
    void setFiducialsExportMode(const FiducialsExportMode& fiducialsExportMode)
    {
        m_fiducialsExportMode = fiducialsExportMode;
    }

private:

    /**
     * @brief Check if there is comment on acquisition.
     * @param[in] series ImageSeries that must be checked.
     */
    bool hasDocumentSR(const data::ImageSeries::csptr& series) const;

    /**
     * @brief Returns the image instance used to create the reconstruction
     * @note This function assume that one and only one image instance has been created prior calling this function.
     * The created image instance must be the one used to generate the reconstruction.
     */
    SPTR(io::dicom::container::DicomInstance) getImageInstance();

    /// Dicom Instance map used to keep information between series writing calls
    DicomInstanceMapType m_dicomInstanceMap;

    /// Fiducials Export Mode
    FiducialsExportMode m_fiducialsExportMode;
};

} // namespace writer

} // namespace sight::io::dicom
