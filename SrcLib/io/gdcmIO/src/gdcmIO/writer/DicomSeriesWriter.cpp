/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// gdcm
#include <gdcmUIDGenerator.h>

// fwTools
#include <fwTools/dateAndTime.hpp>

#include "gdcmIO/writer/DicomSeriesWriter.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomSeriesWriter::DicomSeriesWriter():
        m_seriesID(1)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSeriesWriter::~DicomSeriesWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::write(::gdcm::DataSet & a_gDs)
{
    SLM_TRACE_FUNC();

    ::fwData::Acquisition::csptr            series          = this->getConcreteObject();
    SLM_ASSERT("fwData::Acquisition not instanced", series);

    ::boost::shared_ptr< DicomInstance >    dicomInstance   = this->getDicomInstance();

    // Acquisition's date and time
    ::boost::posix_time::ptime              acqCreatDate    = series->getCreationDate();

    // Serie's date
    helper::GdcmData::setTagValue<0x0008,0x0022>( ::fwTools::getDate(acqCreatDate), a_gDs); // Type 3
    OSLM_TRACE("Serie's date : " << ::fwTools::getDate(acqCreatDate));

    // Serie's time
    helper::GdcmData::setTagValue<0x0008,0x0032>( ::fwTools::getTime(acqCreatDate), a_gDs); // Type 3
    OSLM_TRACE("Serie's time : " << ::fwTools::getTime(acqCreatDate));

    // Series's modality
    std::string & modality = dicomInstance->getRefModality();
    // NOTE : if modality is evil (like "Unknown"), it will be replaced by "CT" in DicomImageWriter
    if (modality.empty())
    {
        modality = "CT";    // dirty solution
    }
    helper::GdcmData::setTagValue<0x0008,0x0060>(modality, a_gDs); // Type 1
    OSLM_TRACE("Series's modality : " << modality);

    // Serie's instance UID
    std::string seriesUID = series->getUID();
    if (seriesUID.empty())
    {
        ::gdcm::UIDGenerator gUIDgen;
        seriesUID = gUIDgen.Generate();
    }
    helper::GdcmData::setTagValue<0x0020,0x000e>(seriesUID, a_gDs); // Type 1
    OSLM_TRACE("Serie's instance UID : " << seriesUID);

    // Serie's number
    helper::GdcmData::setTagValue<int,0x0020,0x0011>(m_seriesID, a_gDs); // Type 2
    OSLM_TRACE("Serie's ID : " << m_seriesID);

    //*****     Set the DicomInstance       *****//
    dicomInstance->setSeriesInstanceUID( seriesUID );   // Store series UID to be used and write later
}

}   // namespace writer

}   // namespace gdcmIO
