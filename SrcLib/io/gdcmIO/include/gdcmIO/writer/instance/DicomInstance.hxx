/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_INSTANCE_DICOMINSTANCEWRITER_HXX_
#define __GDCMIO_WRITER_INSTANCE_DICOMINSTANCEWRITER_HXX_

#include <gdcmAttribute.h>
#include <gdcmUIDGenerator.h>

#include "gdcmIO/writer/DicomFile.hxx"
#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

/**
 * @brief   This class defines a writer of a DICOM instance storage.
 *
 * It is a base for all DICOM writers.
 * It writes common needed attributes for all DICOM storage.
 *
 * @class   DicomInstance.
 */
template < class DATATYPE >
class GDCMIO_CLASS_API DicomInstance : public ::gdcmIO::writer::DicomFile< DATATYPE >
{

public:

    GDCMIO_API DicomInstance()
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomInstance()
    {
        SLM_TRACE_FUNC();
    }


    GDCMIO_API ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > getDicomInstance();

    GDCMIO_API virtual void setDicomInstance( ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > a_dicomInstance);

protected :

    /**
     * @brief   Write some ::gdcmIO::DicomInstance members.
     *
     * This method write SOP Common Module, modality and instance number.
     *
     * @note    setDicomInstance() have to be called before this method.
     *
     * @see     PS 3.3 C.12.1
     */
    virtual void write();

private :

    /**
     * @brief   Write SOP Common Module.
     *
     * @see     PS 3.3 C.12.1
     */
    void writeSOPCommon();


    ::boost::weak_ptr< ::gdcmIO::container::DicomInstance >  m_dicomInstance;    ///< Store data on current instance and image instance.
};

//------------------------------------------------------------------------------

template < class DATATYPE >
void DicomInstance< DATATYPE >::write()
{
    SLM_TRACE_FUNC();

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();

    // Get the data set
    ::gdcm::DataSet &  gDsRoot = this->getWriter()->GetFile().GetDataSet();

    // Remove previous attributes if necessary to insert new ones
    if (gDsRoot.FindDataElement( ::gdcm::Attribute<0x0008,0x0060>::GetTag() ))
    {
        gDsRoot.Remove( ::gdcm::Attribute<0x0008,0x0060>::GetTag() );
        OSLM_TRACE("Modality was removed");
    }
    if (gDsRoot.FindDataElement( ::gdcm::Attribute<0x0020,0x0013>::GetTag() ))
    {
        gDsRoot.Remove( ::gdcm::Attribute<0x0020,0x0013>::GetTag() );
        OSLM_TRACE("Instance number was removed");
    }

    // Modality
    helper::GdcmData::setTagValue<0x0008,0x0060>( dicomInstance->getModality(), gDsRoot);
    OSLM_TRACE("Modality : " << dicomInstance->getModality());

    // Instance Number
    helper::GdcmData::setTagValue<int,0x0020,0x0013>( atoi(dicomInstance->getInstanceNumber().c_str()), gDsRoot);
    OSLM_TRACE("Instance number : " << dicomInstance->getInstanceNumber());

    //*****     Handled SOP common module    *****//
    // Insert SOP UIDs (can generate them)
    this->writeSOPCommon();

    //*****     Write the file    *****//
    DicomFile< DATATYPE >::write();
}

//------------------------------------------------------------------------------

template < class DATATYPE >
void DicomInstance< DATATYPE >::writeSOPCommon()
{
    SLM_TRACE_FUNC();

    // Get the data set
    ::gdcm::DataSet & gDsRoot = this->getWriter()->GetFile().GetDataSet();
    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance  = this->getDicomInstance();

    // SOP Class UID must not be empty ( Type 1 )
    if (dicomInstance->getSOPClassUID().empty())
    {
        ::gdcm::UIDGenerator gUIDgen;
        dicomInstance->setSOPClassUID( gUIDgen.GetRoot() );  // Set to GDCM one (eurk!)
    }

    // SOP Class UID
    ::gdcm::Attribute<0x0008,0x0016> gSOPClassUIDAt;
    gSOPClassUIDAt.SetValue(dicomInstance->getSOPClassUID());
    gDsRoot.Replace(gSOPClassUIDAt.GetAsDataElement());
    OSLM_TRACE("SOP Class UID : " << dicomInstance->getSOPClassUID());

    // SOP Instance UID must not be empty ( Type 1 )
    if (dicomInstance->getSOPInstanceUID().empty())
    {
        ::gdcm::UIDGenerator gUIDgen;
        dicomInstance->setSOPClassUID( gUIDgen.Generate() );
    }

    // SOP Study Instance UID (already set by gdcm ? not for SR)
    ::gdcm::Attribute<0x0008,0x0018> gSOPInstanceUIDAt;
    gSOPInstanceUIDAt.SetValue(dicomInstance->getSOPInstanceUID());
    gDsRoot.Replace(gSOPInstanceUIDAt.GetAsDataElement());
}

//------------------------------------------------------------------------------

template < class DATATYPE >
::boost::shared_ptr< ::gdcmIO::container::DicomInstance > DicomInstance< DATATYPE >::getDicomInstance()
{
    SLM_ASSERT("DicomInstance has expired.", !m_dicomInstance.expired() );
    return m_dicomInstance.lock();
}

//------------------------------------------------------------------------------

template < class DATATYPE >
void DicomInstance< DATATYPE >::setDicomInstance( ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > a_dicomInstance )
{
    SLM_ASSERT("DicomInstance not instanced.", a_dicomInstance );
    m_dicomInstance = a_dicomInstance;
}

} // namespace instance
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_INSTANCE_DICOMINSTANCEWRITER_HXX_ */
