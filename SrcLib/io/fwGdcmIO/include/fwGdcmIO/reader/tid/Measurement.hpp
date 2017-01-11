/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_TID_MEASUREMENT_HPP__
#define __FWGDCMIO_READER_TID_MEASUREMENT_HPP__

#include "fwGdcmIO/container/sr/DicomSRNode.hpp"
#include "fwGdcmIO/reader/tid/TemplateID.hpp"

#include <fwData/Image.hpp>

namespace fwData
{
class Point;
}

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @brief Measurement TID
 * @see TID 300
 */
class FWGDCMIO_CLASS_API Measurement : public ::fwGdcmIO::reader::tid::TemplateID< ::fwData::Image >
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
    FWGDCMIO_API Measurement(SPTR(::fwMedData::DicomSeries)dicomSeries,
                             SPTR(::gdcm::Reader)reader,
                             SPTR(::fwGdcmIO::container::DicomInstance)instance,
                             ::fwData::Image::sptr image,
                             ::fwLog::Logger::sptr logger);

    /// Destructor
    FWGDCMIO_API virtual ~Measurement();

    /**
     * @brief Read a measurement node
     * @param[in] node Node that must be read
     */
    FWGDCMIO_API virtual void readNode(SPTR(::fwGdcmIO::container::sr::DicomSRNode) node);

protected:

    /**
     * @brief Add a distance into the image
     * @param point1 First point
     * @param point2 Second point
     */
    void addDistance(SPTR(::fwData::Point) point1, SPTR(::fwData::Point) point2);


};

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_TID_MEASUREMENT_HPP__ */
