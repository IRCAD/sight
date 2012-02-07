/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMREFINSTANCEWRITER_HXX_
#define _GDCMIO_DICOMREFINSTANCEWRITER_HXX_

#include "gdcmIO/writer/DicomInstanceWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class define a writer for instance which depend on an other.
 *
 * The other instance will be referenced by the current one.
 *
 * @class   DicomRefInstanceWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
template < class DATATYPE >
class GDCMIO_CLASS_API DicomRefInstanceWriter : public DicomInstanceWriter< DATATYPE >
{
public:

    GDCMIO_API DicomRefInstanceWriter() :
        m_SOPClassUIDRef("0"),
        m_SOPInstanceUIDRef("0")
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomRefInstanceWriter()
    {
        SLM_TRACE_FUNC();
    }

    /**
     * @brief   Write referenced instance attributes and launch DicomInstanceWriter::write().
     */
    GDCMIO_API void write();

    /**
     * @brief   It references the current instance UIDs and set DicomInstance.
     */
    GDCMIO_API void setDicomInstance(::boost::shared_ptr< DicomInstance > a_dicomInstance);

    GDCMIO_API fwGettersSettersDocMacro(SOPClassUIDRef, SOPClassUIDRef, std::string, The referenced SOP Class UID);

    GDCMIO_API fwGettersSettersDocMacro(SOPInstanceUIDRef, SOPInstanceUIDRef, std::string, The referenced SOP Instance UID);

private:

    /**
     * @brief   Write Common Instance Reference Module.
     *
     * @see PS 3.3 C.12.2
     */
    void writeCommonInstanceRef();

    std::string m_SOPClassUIDRef;       ///< Referenced SOP Class UID.
    std::string m_SOPInstanceUIDRef;    ///< Referenced SOP Instance UID.
};


//------------------------------------------------------------------------------

template < class DATATYPE >
void DicomRefInstanceWriter< DATATYPE >::write()
{
    SLM_TRACE_FUNC();

    // Write required attributes for a referenced instance
    this->writeCommonInstanceRef();

    // Write the instance
    DicomInstanceWriter< DATATYPE >::write();
}


//------------------------------------------------------------------------------

template < class DATATYPE >
void DicomRefInstanceWriter< DATATYPE >::setDicomInstance(::boost::shared_ptr< DicomInstance > a_dicomInstance)
{
    SLM_TRACE_FUNC();

    //*****     Set the referenced instance     *****//
    // WARN : Before set the new instance!
    this->m_SOPClassUIDRef    = a_dicomInstance->getSOPClassUID();
    this->m_SOPInstanceUIDRef = a_dicomInstance->getSOPInstanceUID();

    //*****     Set the instance     *****//
    DicomInstanceWriter< DATATYPE >::setDicomInstance( a_dicomInstance );
}

//------------------------------------------------------------------------------

template < class DATATYPE >
void DicomRefInstanceWriter< DATATYPE >::writeCommonInstanceRef()
{
    SLM_TRACE_FUNC();

    ::gdcm::DataSet &                       gDsRoot         = this->getWriter()->GetFile().GetDataSet();
    ::boost::shared_ptr< DicomInstance >    dicomInstance   = this->getDicomInstance();

    //*** Create Referenced Series Sequence ***//
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefSeries = new ::gdcm::SequenceOfItems();
    gSqRefSeries->SetLengthToUndefined();
    {
        // Create its item
        ::gdcm::Item        gItRefSeries;
        gItRefSeries.SetVLToUndefined();
        ::gdcm::DataSet &   gDsRefSeries = gItRefSeries.GetNestedDataSet();

        //*** Create Referenced Instance Sequence ***//
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefInstance = new ::gdcm::SequenceOfItems();
        gSqRefInstance->SetLengthToUndefined();
        {
            const std::string &                 refSOPClassUID      = this->getCRefSOPClassUIDRef();
            const std::vector< std::string > &  refSOPInstanceUIDs  = dicomInstance->getCRefReferencedSOPInstanceUIDs();
            if (dicomInstance->getIsMultiFrame())
            {
                // Create its item
                ::gdcm::Item        gItRefInstance;
                gItRefInstance.SetVLToUndefined();
                ::gdcm::DataSet &   gDsRefInstance = gItRefInstance.GetNestedDataSet();

                // Referenced SOP Class UID
                helper::GdcmData::setTagValue<0x0008,0x1150>( refSOPClassUID,     gDsRefInstance);
//                    OSLM_TRACE("Referenced SOP Class UID : " << m_SOPClassUIDRef);

                // Referenced SOP Instance UID
                helper::GdcmData::setTagValue<0x0008,0x1155>( m_SOPInstanceUIDRef,  gDsRefInstance);
//                    OSLM_TRACE("Referenced SOP Instance UID : "<< m_SOPInstanceUIDRef);

                // No Referenced Frame Number because we suppose all frames are referenced

                gSqRefInstance->AddItem(gItRefInstance);
            }
            else
            {
                // Limit references to the bounding box of each surface?
                std::vector< std::string >::const_iterator  it      = refSOPInstanceUIDs.begin();
                std::vector< std::string >::const_iterator  itEnd   = refSOPInstanceUIDs.end();
                for(; it != itEnd; it++)
                {
                    // Create one item
                    ::gdcm::Item        gItRefInstance;
                    gItRefInstance.SetVLToUndefined();
                    ::gdcm::DataSet &   gDsRefInstance = gItRefInstance.GetNestedDataSet();

                    // Referenced SOP Class UID
                    helper::GdcmData::setTagValue<0x0008,0x1150>( refSOPClassUID, gDsRefInstance);
//                        OSLM_TRACE("Referenced SOP Class UID : " << m_SOPClassUIDRef);

                    // Referenced SOP Instance UID
                    helper::GdcmData::setTagValue<0x0008,0x1155>( *it,              gDsRefInstance);
//                        OSLM_TRACE("Referenced SOP Instance UID : "<< *it);

                    gSqRefInstance->AddItem( gItRefInstance );
                }
            }

            helper::GdcmData::insertSQ<0x0008,0x114A>(gSqRefInstance, gDsRefSeries); // Referenced Instance Sequence
        }

        // Series Instance UID
        helper::GdcmData::setTagValue<0x0020,0x000E>(dicomInstance->getSeriesInstanceUID(), gDsRefSeries);
        OSLM_TRACE("Referenced Series Instance UID : " << dicomInstance->getSeriesInstanceUID());

        // Study Instance UID
        helper::GdcmData::setTagValue<0x0020,0x000D>(dicomInstance->getStudyInstanceUID(), gDsRefSeries);
        OSLM_TRACE("Referenced Study Instance UID : " << dicomInstance->getStudyInstanceUID());

        //TODO: if multi-studies : refer to PS 3.3 C.12-8 (Tag(0088,1200))

        gSqRefSeries->AddItem(gItRefSeries);

        helper::GdcmData::insertSQ<0x0008,0x1115>(gSqRefSeries, gDsRoot); // Referenced Series Sequence
    }
}

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMREFINSTANCEWRITER_HXX_ */
