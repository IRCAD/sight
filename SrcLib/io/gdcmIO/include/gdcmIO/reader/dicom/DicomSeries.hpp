/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMSERIES_HPP__
#define __GDCMIO_READER_DICOM_DICOMSERIES_HPP__

#include <boost/shared_ptr.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Equipment.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/reader/dicom/IDicomFiles.hxx"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class read a complete tree of a series from DICOM file.
 *
 * It will sort content of files, launch corresponding reading tool and read
 * series information.
 *
 * @class   DicomSeries
 */
class GDCMIO_CLASS_API DicomSeries : public IDicomFiles< ::fwMedData::SeriesDB >
{

public :

    typedef std::vector<std::string> VectFilesType;
    typedef std::map< std::string, VectFilesType > MapSeriesType;

    GDCMIO_API DicomSeries();

    GDCMIO_API ~DicomSeries();

    /**
     * @brief   Read one series from DICOM file.
     *
     * It will load then start image, SR and RT reading tools.
     * If any image was found, the reading is stopped.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw(::gdcmIO::exception::Failed);

private :

    MapSeriesType sortSeriesFiles(const VectFilesType& seriesFiles) const;

    /**
     * @brief   Set a series from a DICOM file.
     *
     *@note     setReader() have to be called before this method.
     *          In addition, the reader have to be a gdcm::ImageReader
     */
    void readSeries(::fwMedData::Series::sptr series);

    void readStudy(::fwMedData::Study::sptr study);
    void readPatient(::fwMedData::Patient::sptr patient);
    void readEquipment(::fwMedData::Equipment::sptr equipment);

    /**
     * @brief   Get file names which contain DICOM image.
     *
     * @param   a_map   Map with SOP Class UID in key and file names for value.
     *
     * @return  File names array
     */
    VectFilesType getImageFileNames(const MapSeriesType& a_map);

    /**
     * @brief   Get file names which contain DICOM SR document.
     *
     * @param   a_map   Map with SOP Class UID in key and file names for value.
     *
     * @return  File names array
     */
    VectFilesType getSRFileNames(const MapSeriesType& a_map);

    /**
     * @brief   Get file names which contain DICOM RT document.
     *
     * @param   a_map   Map with SOP Class UID in key and file names for value.
     *
     * @return  File names array
     */
    VectFilesType getRTFileNames(const MapSeriesType& a_map);

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /*__GDCMIO_READER_DICOM_DICOMSERIES_HPP__*/
