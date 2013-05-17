/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// fwComEd
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

// fwData
#include <fwData/PointList.hpp>

#include "gdcmIO/writer/module/DicomLandmark.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/container/DicomDictionarySR.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomLandmark::DicomLandmark()
{
    SLM_TRACE_FUNC();

    this->m_landmarks = ::boost::shared_ptr< ::gdcmIO::container::DicomLandmark >( new ::gdcmIO::container::DicomLandmark );
}

//------------------------------------------------------------------------------

DicomLandmark::~DicomLandmark()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomLandmark::write(::gdcm::DataSet & a_gDs) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::csptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not set", image);

    //*****       Set Landmarks     *****//
    try
    {
        // Try to change landmarks into a DICOM form
        m_landmarks->setFromData(image);
    }
    catch(::gdcmIO::exception::Failed &)
    {
        throw;
    }

    //*****       Write Landmarks     *****//

    // Get or create a content sequence in order to insert landmark
    const ::gdcm::Tag contentSQTag(0x0040,0xa730);  // Content Sequence
    if ( !a_gDs.FindDataElement(contentSQTag) )
    {// Create the content sequence
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSQ = new ::gdcm::SequenceOfItems();
        helper::GdcmData::setSQ<0x0040,0xa730>(gSQ, a_gDs);
    }
    // Get the content sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gContentSQ = a_gDs.GetDataElement(contentSQTag).GetValueAsSQ();

    // Write each landmark in a sequence of items
    ::fwData::PointList::sptr pl = image->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId );
    if(pl)
    {
        const unsigned int nbLandmark = pl->getPoints().size();
        for (unsigned int i = 0; i < nbLandmark; ++i)
        {
            this->writeLandmark(i, gContentSQ);
        }
    }
}

//------------------------------------------------------------------------------

void DicomLandmark::writeLandmark(const unsigned int                              idx,
                                        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > a_gSq)
{
    SLM_TRACE_FUNC();

    // Create an item for a landmark
    ::gdcm::Item        gItLandmark;
    gItLandmark.SetVLToUndefined();
    ::gdcm::DataSet &   gDsLandmark = gItLandmark.GetNestedDataSet();

    // Relationship Value
    helper::GdcmData::setTagValue<0x0040,0xa010>(::gdcmIO::container::DicomDictionarySR::getRelationshipString(
            ::gdcmIO::container::DicomDictionarySR::CONTAINS), gDsLandmark);   // Type 1

    // Type Value
    helper::GdcmData::setTagValue<0x0040,0xa040>(::gdcmIO::container::DicomDictionarySR::getTypeString(
            ::gdcmIO::container::DicomDictionarySR::TEXT), gDsLandmark);   //Type 1

    // Text Value
    helper::GdcmData::setTagValue<0x0040,0xa160>(m_landmarks->getLabels()[ idx ],    gDsLandmark);  // Type 1C

    // Concept Name Code Sequence
    helper::DicomTools::createCodeSequence<0x0040,0xa043>(::gdcmIO::container::DicomDictionarySR::getCodeValue(
            ::gdcmIO::container::DicomDictionarySR::LANDMARK),
            ::gdcmIO::container::DicomDictionarySR::getCodeScheme(
                    ::gdcmIO::container::DicomDictionarySR::LANDMARK),
            ::gdcmIO::container::DicomDictionarySR::getCodeMeaning(
                    ::gdcmIO::container::DicomDictionarySR::LANDMARK), gDsLandmark);        //Type 1C

    // Spatial COORDinates (SCOORD)
    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();
    ::gdcmIO::container::DicomSCoord & currentSCOORD = m_landmarks->getRefSCoords()[ idx ];
    const unsigned int numberOfReferencedFrame = m_landmarks->getCRefRefFrames()[ idx ];

    /*
    112171 Fiducial mark A location in image space, which
                         may or may not correspond to
                         an anatomical reference, which
                         is often used for registering data
                         sets.
    */
    currentSCOORD.setContext( ::gdcmIO::container::DicomCodedAttribute("112171", "DCM", "Fiducial mark") );

    if (dicomInstance->getIsMultiFrame())
    {
        helper::DicomSR::createSCOORD(m_landmarks->getSCoords()[ idx ],
                                      ::fwTools::getString( numberOfReferencedFrame ),
                                      dicomInstance->getSOPClassUID(),
                                      dicomInstance->getSOPInstanceUID(),
                                      gDsLandmark);
    }
    else
    {
        helper::DicomSR::createSCOORD(m_landmarks->getSCoords()[ idx ],
                                      dicomInstance->getSOPClassUID(),
                                      dicomInstance->getReferencedSOPInstanceUIDs()[ numberOfReferencedFrame -1 ], // -1 because frame number start at 1
                                      gDsLandmark);
    }

    // Add the landmark in to content sequence of root
    a_gSq->AddItem( gItLandmark );
}

} // namespace module
} // namespace writer
} // namespace gdcmIO
