/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_TID_MEASUREMENTREPORT_HPP__
#define __FWGDCMIO_READER_TID_MEASUREMENTREPORT_HPP__

#include "fwGdcmIO/container/sr/DicomSRNode.hpp"
#include "fwGdcmIO/reader/tid/TemplateID.hpp"

#include <fwData/Image.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class FWGDCMIO_CLASS_API MeasurementReport : public ::fwGdcmIO::reader::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image FW4SPL data object
     * @param[in] logger Logger
     */
    FWGDCMIO_API MeasurementReport(SPTR(::fwMedData::DicomSeries)dicomSeries,
                                   SPTR(::gdcm::Reader)reader,
                                   SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                   ::fwData::Image::sptr image,
                                   ::fwLog::Logger::sptr logger);

    /// Destructor
    FWGDCMIO_API virtual ~MeasurementReport();

    /**
     * @brief Create Measurement Report template root node
     * @param[in] root Root node
     */
    FWGDCMIO_API virtual void readSR(SPTR(::fwGdcmIO::container::sr::DicomSRNode) root);


};

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_TID_MEASUREMENTREPORT_HPP__ */
