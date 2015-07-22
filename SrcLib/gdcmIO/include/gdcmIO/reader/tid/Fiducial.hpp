/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_TID_FIDUCIAL_HPP__
#define __GDCMIO_READER_TID_FIDUCIAL_HPP__

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
 * @class Fiducial
 * @brief Fiducial TID
 * @see TID t1tt3 FIXME: Set the final TID
 */
class GDCMIO_CLASS_API Fiducial : public ::gdcmIO::reader::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image FW4SPL data object
     */
    GDCMIO_API Fiducial(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                        SPTR(::gdcm::Reader)reader,
                        SPTR(::gdcmIO::container::DicomInstance)instance,
                        ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~Fiducial();

    /**
     * @brief Read a fiducial node
     * @param[in] node Node that must be read
     */
    GDCMIO_API virtual void readNode(SPTR(::gdcmIO::container::sr::DicomSRNode) node);

protected:

    /**
     * @brief Add a landmark into the image
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @param label Landmark label
     */
    void addLandmark(float x, float y, float z, const std::string& label);


};

} // namespace tid
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_TID_FIDUCIAL_HPP__ */
