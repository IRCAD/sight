/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/tid/Fiducial.hpp"
#include "fwGdcmIO/reader/tid/Measurement.hpp"
#include "fwGdcmIO/reader/tid/MeasurementReport.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(SPTR(::fwMedData::DicomSeries)dicomSeries,
                                     SPTR(::gdcm::Reader)reader,
                                     SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                     ::fwData::Image::sptr image,
                                     ::fwLog::Logger::sptr logger) :
    ::fwGdcmIO::reader::tid::TemplateID< ::fwData::Image >(dicomSeries, reader, instance, image, logger)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
{
}

//------------------------------------------------------------------------------

void MeasurementReport::readSR(SPTR(::fwGdcmIO::container::sr::DicomSRNode)root)
{
    if(root->getCodedAttribute() ==
       ::fwGdcmIO::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report"))
    {
        for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& node: root->getSubNodeContainer())
        {
            // Try to identify a fiducial node
            if(node->getCodedAttribute() == ::fwGdcmIO::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"))
            {
                for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& subNode: node->getSubNodeContainer())
                {
                    ::fwGdcmIO::reader::tid::Fiducial fiducial(m_dicomSeries, m_reader, m_instance, m_object, m_logger);
                    fiducial.readNode(subNode);
                }
            }
            // Try to identify a measurement node
            else if(node->getCodedAttribute() ==
                    ::fwGdcmIO::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"))
            {
                for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& subNode: node->getSubNodeContainer())
                {
                    ::fwGdcmIO::reader::tid::Measurement measurement(m_dicomSeries, m_reader, m_instance, m_object,
                                                                     m_logger);
                    measurement.readNode(subNode);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO
