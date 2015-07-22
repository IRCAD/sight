/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/reader/tid/Fiducial.hpp"
#include "gdcmIO/reader/tid/Measurement.hpp"

#include "gdcmIO/reader/tid/MeasurementReport.hpp"

namespace gdcmIO
{
namespace reader
{
namespace tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                                     SPTR(::gdcm::Reader)reader,
                                     SPTR(::gdcmIO::container::DicomInstance)instance,
                                     ::fwData::Image::sptr image) :
    ::gdcmIO::reader::tid::TemplateID< ::fwData::Image >(dicomSeries, reader, instance, image)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
{
}

//------------------------------------------------------------------------------

void MeasurementReport::readSR(SPTR(::gdcmIO::container::sr::DicomSRNode)root)
{
    if(root->getCodedAttribute() ==
       ::gdcmIO::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report"))
    {
        BOOST_FOREACH(const SPTR(::gdcmIO::container::sr::DicomSRNode)& node, root->getCRefSubNodeContainer())
        {
            // Try to identify a fiducial node
            if(node->getCodedAttribute() == ::gdcmIO::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"))
            {
                BOOST_FOREACH(const SPTR(::gdcmIO::container::sr::DicomSRNode)& subNode,
                              node->getCRefSubNodeContainer())
                {
                    ::gdcmIO::reader::tid::Fiducial fiducial(m_dicomSeries, m_reader, m_instance, m_object);
                    fiducial.readNode(subNode);
                }
            }
            // Try to identify a measurement node
            else if(node->getCodedAttribute() ==
                    ::gdcmIO::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"))
            {
                BOOST_FOREACH(const SPTR(::gdcmIO::container::sr::DicomSRNode)& subNode,
                              node->getCRefSubNodeContainer())
                {
                    ::gdcmIO::reader::tid::Measurement measurement(m_dicomSeries, m_reader, m_instance, m_object);
                    measurement.readNode(subNode);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace reader
} // namespace gdcmIO
