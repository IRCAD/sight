/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_TID_MEASUREMENT_HPP__
#define __GDCMIO_READER_TID_MEASUREMENT_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/reader/tid/TemplateID.hpp"

namespace fwData
{
    class Point;
}

namespace gdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @class Measurement
 * @brief Measurement TID
 * @see TID 300
 */
class GDCMIO_CLASS_API Measurement : public ::gdcmIO::reader::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image FW4SPL data object
     */
    GDCMIO_API Measurement(SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~Measurement();

    /**
     * @brief Read a measurement node
     * @param[in] node Node that must be read
     */
    GDCMIO_API virtual void readNode(SPTR(::gdcmIO::container::sr::DicomSRNode) node);

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
} // namespace gdcmIO

#endif /* __GDCMIO_READER_TID_MEASUREMENT_HPP__ */
