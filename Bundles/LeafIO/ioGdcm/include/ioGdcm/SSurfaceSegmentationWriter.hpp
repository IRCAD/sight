/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SSURFACESEGMENTATIONWRITER_HPP__
#define __IOGDCM_SSURFACESEGMENTATIONWRITER_HPP__

#include "ioGdcm/config.hpp"

#include <fwData/Object.hpp>
#include <fwMedData/Series.hpp>
#include <fwGdcmIO/writer/Series.hpp>

#include <io/IWriter.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/DicomSeries.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief Services to write in DICOM 3D format from a DicomSeries and ModelSeries.
 */
class IOGDCM_CLASS_API SSurfaceSegmentationWriter : public ::io::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SSurfaceSegmentationWriter)( ::io::IWriter) );

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
    IOGDCM_API ::io::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Write a Series to a folder in DICOM format
     * @param[in] filename DICOM file name
     * @param[in] dicom DicomSeries that must be writen
     * @param[in] model ModelSeries that must be writen
     */
    void saveSurfaceSegmentation( const ::boost::filesystem::path filename,
                                  SPTR(::fwMedData::DicomSeries) dicom,
                                  SPTR(::fwMedData::ModelSeries) model);
};

} // namespace ioGdcm

#endif // __IOGDCM_SSURFACESEGMENTATIONWRITER_HPP__
