/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
#define __GDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__

#include <gdcmReader.h>

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/exception/Failed.hpp"

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @class InformationObjectDefinition
 * @brief InformationObjectDefinition base class used to read DICOM IODs
 */
class GDCMIO_CLASS_API InformationObjectDefinition
{

public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     */
    GDCMIO_API InformationObjectDefinition(::fwDicomData::DicomSeries::sptr dicomSeries,
                                           SPTR(::gdcmIO::container::DicomInstance)instance);

    /// Destructor
    GDCMIO_API virtual ~InformationObjectDefinition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    GDCMIO_API virtual void read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed) = 0;

protected:

    /// DICOM Instance
    SPTR(::gdcmIO::container::DicomInstance) m_instance;

    /// Dicom Series
    ::fwDicomData::DicomSeries::sptr m_dicomSeries;

};

} // namespace iod
} // namespace reader
} // namespace gdcmIO

#endif // __GDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
