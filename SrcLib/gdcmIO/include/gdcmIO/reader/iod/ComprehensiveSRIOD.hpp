/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IOD_COMPREHENSIVESRIOD_HPP__
#define __GDCMIO_READER_IOD_COMPREHENSIVESRIOD_HPP__

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @class ComprehensiveSRIOD
 * @brief ComprehensiveSRIOD class used to read landmarks and distances stored in Enhanced Structured Report IODs
 */
class GDCMIO_CLASS_API ComprehensiveSRIOD : public ::gdcmIO::reader::iod::InformationObjectDefinition
{

public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     */
    GDCMIO_API ComprehensiveSRIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
                                  SPTR(::gdcmIO::container::DicomInstance)instance);

    /// Destructor
    GDCMIO_API ~ComprehensiveSRIOD();

    //// Read DICOM file
    GDCMIO_API void read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed);


};

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_IOD_COMPREHENSIVESRIOD_HPP__ */
