/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_TID_MEASUREMENT_HPP__
#define __GDCMIO_WRITER_TID_MEASUREMENT_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/writer/tid/TemplateID.hpp"

namespace fwData
{
    class PointList;
}

namespace gdcmIO
{
namespace writer
{
namespace tid
{

/**
 * @class Measurement
 * @brief Measurement TID
 * @see TID 300
 */
class GDCMIO_CLASS_API Measurement : public ::gdcmIO::writer::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    GDCMIO_API Measurement(SPTR(::gdcm::Writer) writer,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~Measurement();

    /**
     * @brief Create nodes according to the template
     * @param[in] parent Parent node
     * @param[in] useSCoord3D True if we must write SCOORD3D, false if we must write SCOORD
     */
    GDCMIO_API virtual void createNodes(SPTR(::gdcmIO::container::sr::DicomSRNode) parent, bool useSCoord3D = true);


protected:

    /**
     * @brief Create a measurement node
     * @param[in] parent Parent node
     * @param[in] pointList Pointlist containing distance points
     * @param[in] id ID of the ficudial
     * @param[in] useSCoord3D True if we must use 3D coordinates
     */
    void createMeasurement(SPTR(::gdcmIO::container::sr::DicomSRNode) parent,
            const SPTR(::fwData::PointList)& pointList, unsigned int id, bool useSCoord3D);


};

} // namespace tid
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_TID_MEASUREMENT_HPP__ */
