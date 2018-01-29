/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioGdcm/config.hpp"

#include <fwData/Object.hpp>

#include <fwGdcmIO/writer/Series.hpp>

#include <fwIO/IWriter.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>

#include <boost/filesystem/path.hpp>

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief Services to write in DICOM 3D format from a DicomSeries and ModelSeries.
 */
class IOGDCM_CLASS_API SSurfaceSegmentationWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SSurfaceSegmentationWriter)( ::fwIO::IWriter) );

    /**
     * @brief Constructor
     */
    IOGDCM_API SSurfaceSegmentationWriter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SSurfaceSegmentationWriter() noexcept;

    /// Propose a directory selection where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM() override;

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() override;

    /// Does nothing
    IOGDCM_API virtual void stopping() override;

    /// Does nothing
    IOGDCM_API virtual void configuring() override;

    /// Write the ImageSeries in DICOM format.
    IOGDCM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Write a Series to a folder in DICOM format
     * @param[in] filename DICOM file name
     * @param[in] dicom DicomSeries that must be writen
     * @param[in] model ModelSeries that must be writen
     */
    void saveSurfaceSegmentation( const ::boost::filesystem::path filename,
                                  const ::fwMedData::DicomSeries::sptr& dicom,
                                  const ::fwMedData::ModelSeries::sptr& model);
};

} // namespace ioGdcm
