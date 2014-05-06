/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_TID_MEASUREMENTREPORT_HPP__
#define __GDCMIO_READER_TID_MEASUREMENTREPORT_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/reader/tid/TemplateID.hpp"

namespace gdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @class MeasurementReport
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class GDCMIO_CLASS_API MeasurementReport : public ::gdcmIO::reader::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image FW4SPL data object
     */
    GDCMIO_API MeasurementReport(SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~MeasurementReport();

    /**
     * @brief Create Measurement Report template root node
     * @param[in] root Root node
     */
    GDCMIO_API virtual void readSR(SPTR(::gdcmIO::container::sr::DicomSRNode) root);


};

} // namespace tid
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_TID_MEASUREMENTREPORT_HPP__ */
