/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/spyLog.hpp"        // for SLM_TRACE_FUNC()

#include "gdcmIO/DicomInstance.hpp"

namespace gdcmIO
{

//------------------------------------------------------------------------------

DicomInstance::DicomInstance():
        m_SOPClassUID(""),
        m_SOPInstanceUID(""),
        m_studyInstanceUID(""),
        m_seriesInstanceUID(""),
        m_instanceNumber(""),
        m_modality(""),
        m_institutName(""),
        m_frameOfRefUID(""),
        m_referencedSOPInstanceUIDs(),
        m_isMultiFrame(false)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomInstance::DicomInstance( const DicomInstance & dicomInstance)
{
    SLM_TRACE_FUNC();
    m_SOPClassUID = dicomInstance.m_SOPClassUID;
    m_SOPInstanceUID = dicomInstance.m_SOPInstanceUID;
    m_studyInstanceUID = dicomInstance.m_studyInstanceUID;
    m_seriesInstanceUID = dicomInstance.m_seriesInstanceUID;
    m_instanceNumber = dicomInstance.m_instanceNumber;
    m_modality = dicomInstance.m_modality;
    m_institutName = dicomInstance.m_institutName;
    m_frameOfRefUID = dicomInstance.m_frameOfRefUID;
    m_referencedSOPInstanceUIDs = dicomInstance.m_referencedSOPInstanceUIDs;
    m_isMultiFrame = dicomInstance.m_isMultiFrame;
}

//------------------------------------------------------------------------------

DicomInstance::~DicomInstance()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomInstance::setSOPClassUID(const ::gdcm::MediaStorage::MSType a_MSType)
{
    this->m_SOPClassUID = ::gdcm::MediaStorage::GetMSString(a_MSType);
}

//------------------------------------------------------------------------------

void DicomInstance::addReferencedSOPInstanceUID(const std::string & a_UID)
{
    assert(!a_UID.empty());
    m_referencedSOPInstanceUIDs.push_back(a_UID);
}

}
