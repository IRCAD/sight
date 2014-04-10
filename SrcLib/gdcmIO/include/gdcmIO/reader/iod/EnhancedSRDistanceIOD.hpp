/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IOD_ENHANCEDSRDISTANCEIOD_HPP__
#define __GDCMIO_READER_IOD_ENHANCEDSRDISTANCEIOD_HPP__

#include "gdcmIO/container/DicomDistance.hpp"

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @class EnhancedSRDistanceIOD
 * @brief EnhancedSRLandmarkIOD class used to read distances stored in Enhanced Structured Report IODs
 * It searches and read a specific template from the root of the document SR.
 * Currently, the template is TID 1400 (taken from PS 3.16).
 */
class GDCMIO_CLASS_API EnhancedSRDistanceIOD : public ::gdcmIO::reader::iod::InformationObjectDefinition
{

public :

    /**
    * @brief Constructor
    * @param[in] dicomSeries DicomSeries
    * @param[in] instance Instance
    */
   GDCMIO_API EnhancedSRDistanceIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
           SPTR(::gdcmIO::container::DicomInstance) instance);

   /// Destructor
   GDCMIO_API ~EnhancedSRDistanceIOD();

   /**
    * @brief Read a DICOM File
    * @param[in,out] series Series that must be enriched
    */
   GDCMIO_API void read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed);

private :

    /**
     * @brief Read distance(s) from its data set.
     * It will read only one distance with a non contiguous content.
     * @param[in] dataset Data set of distance.
     * @param[in] continuity Continuity of content.
     */
    void readDistance(::fwData::Image::sptr image, const ::gdcm::DataSet & dataset, const bool continuity = false)
    throw (::gdcmIO::exception::Failed);

    /**
     * @brief Check consistency of an SCOORD.
     * It verifies graphic type and data of the SCOORD are complied with a distance.
     * It will throw an exception with invalid content.
     * @param[in] scoord SCOORD to check.
     */
    void checkSCOORDContent(const ::gdcmIO::container::DicomSCoord& scoord) throw (::gdcmIO::exception::Failed);

    /// Store and transform into fwData
    ::boost::shared_ptr< ::gdcmIO::container::DicomDistance > m_dicomDistances;

};

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_IOD_ENHANCEDSRDISTANCEIOD_HPP__ */
