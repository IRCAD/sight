/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/writer/DicomSegmentWriter.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/DicomDictionarySegment.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomSegmentWriter::DicomSegmentWriter() :
        m_segmentNumber(1)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSegmentWriter::~DicomSegmentWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSegmentWriter::writeSurfaceSegmentation(const unsigned int idx, ::fwData::StructureTraits::csptr infoSegement ) throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // get the 3D reconstruction
    ::fwData::Reconstruction::csptr   reconstruction  =
            ::fwData::Reconstruction::dynamicCast ( this->getConcreteObject()->getField( ::fwData::Acquisition::ID_RECONSTRUCTIONS )->children()[idx] );

    //*****     Identification of the segment       *****//
    const std::string & segmentLabel    = reconstruction->getStructureType();   //
    const std::string & segmentDescription    = reconstruction->getOrganName();   // Get the organ name of the reconstruction


    // Identify the segmentation from its name
    const DicomCodedAttribute * anatReg = DicomDictionarySegment::guessAnatRegionFromLabel( infoSegement->getAnatomicRegion());
    const DicomCodedAttribute * propCat = DicomDictionarySegment::guessPropCategoryFromLabel( infoSegement->getPropertyCategory());
    const DicomCodedAttribute * propTyp = DicomDictionarySegment::guessPropTypeFromLabel( infoSegement->getPropertyType());

    // WARNING : Not compliant with DICOM
//    if ( anatReg == 0 || propCat == 0 || propTyp == 0)
//        throw ::fwTools::Failed("Unidentified segment : " + segmentLabel);

    // Add a segment in GDCM writer
    ::boost::shared_ptr< ::gdcm::SurfaceWriter >    gSurfaceWriter  = ::boost::static_pointer_cast< ::gdcm::SurfaceWriter >( this->getWriter() );
    ::gdcm::SmartPointer< ::gdcm::Segment >         segment         = new ::gdcm::Segment;
    gSurfaceWriter->AddSegment(segment);

    ::gdcm::DataSet &                               gDsRoot         = this->getDataSet();

    //*****     Create Segment Sequence      ***//
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqSurfSeg      = new ::gdcm::SequenceOfItems();
    gSqSurfSeg->SetLengthToUndefined();
    {
        // Create its item
        ::gdcm::Item        gItSurfSeg;
        gItSurfSeg.SetVLToUndefined();
        ::gdcm::DataSet &   gDsSurfSeg = gItSurfSeg.GetNestedDataSet();

        // Segment Number (0x0062,0x0004)
        segment->SetSegmentNumber( idx+1 );                 // Type 1 (start to 1)
        OSLM_TRACE("Segment Number : " << idx+1);

        // Segment Label (0x0062,0x0005)
        segment->SetSegmentLabel( segmentLabel.c_str() );   // Type 1
        OSLM_TRACE("Segment Label : " << segmentLabel);

        // Segment Label (0x0062,0x0006)
        segment->SetSegmentDescription(segmentDescription.c_str() );   // Type 1
        OSLM_TRACE("Segment Description : " << segmentDescription);

        // Segment Algorithm Type (0x0062,0x0006)
        segment->SetSegmentAlgorithmType( (reconstruction->getIsAutomatic() ? ::gdcm::Segment::AUTOMATIC : ::gdcm::Segment::MANUAL) );  // Type 1
        OSLM_TRACE("Segment Algorithm Type : " << (reconstruction->getIsAutomatic()?"AUTOMATIC":"MANUAL"));

        //*** Create Anatomic Region Sequence ***//
        if (anatReg != 0)
        {
            // Anatomic Region Sequence (0x0008,0x2218)
            ::gdcm::SegmentHelper::BasicCodedEntry gAnatReg(anatReg->CV.c_str(),
                                                            anatReg->CSD.c_str(),
                                                            anatReg->CM.c_str());
            segment->SetAnatomicRegion( gAnatReg );     // Type 1
        }

        //*** Create Segmented Property Category Code Sequence ***//
        if (propCat != 0)
        {
            // Segmented Property Category Code Sequence (0x0062,0x0003)
            //see: PS.3.3 Table 8.8-1 and Context ID is 7150
            ::gdcm::SegmentHelper::BasicCodedEntry gPropCat(propCat->CV.c_str(),
                                                            propCat->CSD.c_str(),
                                                            propCat->CM.c_str());
            segment->SetPropertyCategory( gPropCat );   // Type 1
        }

        //*** Create Segmented Property Type Code Sequence ***//
        if (propTyp != 0)
        {
            // Segmented Property Type Code Sequence (0x0062,0x000F)
            //see: PS.3.3 Table 8.8-1 and PS 3.16 Context ID 7151
            ::gdcm::SegmentHelper::BasicCodedEntry gPropTyp(propTyp->CV.c_str(),
                                                            propTyp->CSD.c_str(),
                                                            propTyp->CM.c_str());
            segment->SetPropertyType( gPropTyp );       // Type 1
        }

        // Surface Count (0x0066,0x002A)
        // WARNING : hard coded because currently, reconstruction just contains one surface.
        segment->SetSurfaceCount( 1 ); // Type 1

        //*** Create Referenced Surface Sequence ***//
        // Each item of this sequence references a surface for the current segment.
        // A segment can have several surfaces (see tag(0x0066,0x002A), Surface Count).
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefSurf = new ::gdcm::SequenceOfItems();
        gSqRefSurf->SetLengthToUndefined();
        {
            // Create its item
            ::gdcm::Item        gItRefSurf;
            gItRefSurf.SetVLToUndefined();
            ::gdcm::DataSet &   gDsRefSurf = gItRefSurf.GetNestedDataSet();

            // WARNING : idx+1 == segmentNumber == surfaceNumber because currently, reconstruction just contains one surface.*
            // Referenced Surface Number
            helper::GdcmData::setTagValue<uint16_t,0x0066,0x002C>(idx+1, gDsRefSurf);   // Type 1

            //*** Create Segment Surface Generation Algorithm Identification Sequence ***//
            // Move to DicomSurfaceWriter::writeSurfaceMesh()

            //*** Create Segment Surface Source Instance Sequence ***//
            // Reference the image of the fwData::Acquisition
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqSrcInstance = new ::gdcm::SequenceOfItems();
            gSqSrcInstance->SetLengthToUndefined();
            {
                // include ‘Image SOP Instance Reference Macro’ Table C.10-3

                ::boost::shared_ptr< DicomInstance > dicomInstance      = this->getDicomInstance();

                const std::string &                 refSOPClassUID      = this->getCRefSOPClassUIDRef();
                const std::vector< std::string > &  refSOPInstanceUIDs  = dicomInstance->getCRefReferencedSOPInstanceUIDs();
                if (dicomInstance->getIsMultiFrame())
                {
                    ::gdcm::Item        gItImageSOP;
                    gItImageSOP.SetVLToUndefined();
                    ::gdcm::DataSet &   gDsImageSOP = gItImageSOP.GetNestedDataSet();

                    // SOP Class UID
                    helper::GdcmData::setTagValue<0x0008,0x1150>( refSOPClassUID,           gDsImageSOP); // Type 1
//                    OSLM_TRACE("Referenced SOP Class UID : " << dicomInstance->getSOPClassUID());

                    // SOP Instance UID
                    helper::GdcmData::setTagValue<0x0008,0x1155>( refSOPInstanceUIDs[0],    gDsImageSOP); // Type 1
//                    OSLM_TRACE("Referenced SOP Instance UID : "<< refSOPInstanceUIDs[0]);

                    // No Referenced Frame Number because we suppose all frames are referenced

                    gSqSrcInstance->AddItem( gItImageSOP );
                }
                else
                {
                    // Limit references to the bounding box of each surface?
                    std::vector< std::string >::const_iterator  it      = refSOPInstanceUIDs.begin();
                    std::vector< std::string >::const_iterator  itEnd   = refSOPInstanceUIDs.end();
                    for(; it != itEnd; it++)
                    {
                        // Create one item
                        ::gdcm::Item        gItImageSOP;
                        gItImageSOP.SetVLToUndefined();
                        ::gdcm::DataSet &   gDsImageSOP = gItImageSOP.GetNestedDataSet();

                        // SOP Class UID
                        helper::GdcmData::setTagValue<0x0008,0x1150>( refSOPClassUID,   gDsImageSOP); // Type 1
//                        OSLM_TRACE("Referenced SOP Class UID : " << dicomInstance->getSOPClassUID());

                        // SOP Instance UID
                        helper::GdcmData::setTagValue<0x0008,0x1155>( *it,              gDsImageSOP); // Type 1
//                        OSLM_TRACE("Referenced SOP Instance UID : "<< *it);

                        gSqSrcInstance->AddItem( gItImageSOP );
                    }
                }
                // Segment Surface Source Instance Sequence
                helper::GdcmData::insertSQ<0x0066,0x002E>(gSqSrcInstance, gDsRefSurf); // Type 1C
            }
            gSqRefSurf->AddItem(gItRefSurf);

            // Referenced Surface Sequence
            helper::GdcmData::insertSQ<0x0066,0x002B>(gSqRefSurf, gDsSurfSeg);
        }
        gSqSurfSeg->AddItem(gItSurfSeg);
    }
    // Segment Sequence
    helper::GdcmData::insertSQ<0x0062,0x0002>(gSqSurfSeg, gDsRoot);
}

//------------------------------------------------------------------------------

void DicomSegmentWriter::setSegmentNumber(const unsigned short a_num)
{
    this->m_segmentNumber = a_num;
}

}   // namespace writer

}   // namespace gdcmIO
