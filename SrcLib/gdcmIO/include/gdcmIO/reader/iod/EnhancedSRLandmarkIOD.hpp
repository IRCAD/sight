/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IOD_ENHANCEDSRLANDMARKIOD_HPP__
#define __GDCMIO_READER_IOD_ENHANCEDSRLANDMARKIOD_HPP__

#include "gdcmIO/container/DicomLandmark.hpp"

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @class EnhancedSRLandmarkIOD
 * @brief EnhancedSRLandmarkIOD class used to read landmarks stored in Enhanced Structured Report IODs
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is based on TID 320 (taken from PS 3.16).
 */
class GDCMIO_CLASS_API EnhancedSRLandmarkIOD : public ::gdcmIO::reader::iod::InformationObjectDefinition
{
public :

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     */
    GDCMIO_API EnhancedSRLandmarkIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
            SPTR(::gdcmIO::container::DicomInstance) instance);

    /// Destructor
    GDCMIO_API ~EnhancedSRLandmarkIOD();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    GDCMIO_API void read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed);

private :

    /**
     * @brief Read landmark(s) from its data set.
     * It will read one landmark with a non contiguous content.
     *
     * @param[in] dataset Data set of landmark.
     * @param[in] continuity Continuity of content.
     */
    void readLandmark(::fwData::Image::sptr image, const ::gdcm::DataSet & dataset, const bool continuity = false)
    throw (::gdcmIO::exception::Failed);

    /// Store and transform into fwData
    ::boost::shared_ptr< ::gdcmIO::container::DicomLandmark > m_dicomLandmarks;
};

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_IOD_ENHANCEDSRLANDMARKIOD_HPP__ */
