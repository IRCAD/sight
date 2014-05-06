/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_TID_MEASUREMENTREPORT_HPP__
#define __GDCMIO_WRITER_TID_MEASUREMENTREPORT_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/writer/tid/TemplateID.hpp"

namespace gdcmIO
{
namespace writer
{
namespace tid
{

/**
 * @class MeasurementReport
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class GDCMIO_CLASS_API MeasurementReport : public ::gdcmIO::writer::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    GDCMIO_API MeasurementReport(SPTR(::gdcm::Writer) writer,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~MeasurementReport();

    /**
     * @brief Create Measurement Report template root node
     * @param[in] useSCoord3D True if we must write SCOORD3D, false if we must write SCOORD
     */
    GDCMIO_API virtual SPTR(::gdcmIO::container::sr::DicomSRNode) createRootNode(bool useSCoord3D = true);


};

} // namespace tid
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_TID_MEASUREMENTREPORT_HPP__ */
