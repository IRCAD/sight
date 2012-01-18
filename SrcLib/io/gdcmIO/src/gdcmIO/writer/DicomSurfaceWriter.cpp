/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmSurfaceHelper.h>

#include <fwData/Reconstruction.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/StructureTraits.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>

#include "gdcmIO/writer/DicomSurfaceWriter.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/writer/DicomEquipmentWriter.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomSurfaceWriter::DicomSurfaceWriter():
        m_surfaceNumber(0)
{
    SLM_TRACE_FUNC();

    this->m_surface = ::boost::shared_ptr<DicomSurface>( new DicomSurface );

    // Instantiate a specific GDCM writer
    this->setWriter( ::boost::shared_ptr< ::gdcm::SurfaceWriter >( new ::gdcm::SurfaceWriter ) );
}

//------------------------------------------------------------------------------

DicomSurfaceWriter::~DicomSurfaceWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSurfaceWriter::write() throw (::fwTools::Failed)
{
    ::fwData::Acquisition::csptr series = this->getConcreteObject();
    SLM_ASSERT("fwData::Acquisition not instanced", series);


    //*****     Get the dictionary *****//
    // Reading of the dictionary
    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    // Ready of the dictionary file.

    ::fwDataIO::reader::DictionaryReader::NewSptr dictionaryReader;
    dictionaryReader->setObject(structDico);
    std::string dictionaryPath("./share/fwDataIO_0-2/OrganDictionary.dic");
    dictionaryReader->setFile(dictionaryPath);
    try
    {
        dictionaryReader->read();
    }
    catch(const std::exception & e)
    {
        OSLM_ERROR(e.what());
    }

    //*****     Write surface segmentations     *****//
    unsigned int    skippedSegment  = 0;    // Number of segmentation not written
    const uint32_t  nbSegmentation  = series->getReconstructionSize();

    for (unsigned int i = 0; i < nbSegmentation; ++i)
    {
        try
        {
            // Get the info of the struture type
            ::fwData::Reconstruction::csptr   reconstruction  =
                    ::fwData::Reconstruction::dynamicCast ( series->getField( ::fwData::Acquisition::ID_RECONSTRUCTIONS )->children()[i] );
            const std::string & segmentLabel    = reconstruction->getStructureType();
            ::fwData::StructureTraitsDictionary::StructureTypeNameContainer segmentLabels = structDico->getStructureTypeNames();
            ::fwData::StructureTraitsDictionary::StructureTypeNameContainer::const_iterator itr = std::find(segmentLabels.begin(), segmentLabels.end(),segmentLabel);
            ::fwData::StructureTraits::sptr structure;
            ::fwData::StructureTraits::NewSptr emptyStructure;
            if(itr != segmentLabels.end())
            {
                structure = structDico->getStructure(segmentLabel);
            }
            else
            {
                structure = emptyStructure;
            }

            // Write segmentation
            DicomSegmentWriter::writeSurfaceSegmentation(i, structure);

            // Here, the segment is identified and its surface can be written.
            this->writeSurfaceMesh(i);
        }
        catch(::fwTools::Failed & e)
        {
            ++skippedSegment;
            OSLM_ERROR(e.what());
        }
    }

    if (skippedSegment == nbSegmentation)
    {
        throw ::fwTools::Failed("All 3D reconstructions have been rejected");
    }
    else if (skippedSegment > 0)
    {
        OSLM_WARN(skippedSegment<<" 3D reconstruction(s) have been rejected");
    }

    ::boost::shared_ptr< ::gdcm::SurfaceWriter >    gSurfaceWriter  = ::boost::static_pointer_cast< ::gdcm::SurfaceWriter >( this->getWriter() );
    ::gdcm::DataSet &                               gDsRoot         = this->getDataSet();

    // Number of Surfaces Tag(0x0066,0x0001)
    gSurfaceWriter->SetNumberOfSurfaces( nbSegmentation - skippedSegment ); // Type 1
    OSLM_TRACE("Number of Surfaces : " << nbSegmentation - skippedSegment);

    //*****     Write Enhanced General Equipement Module     *****//
    //See: PS 3.3 C.7.5.2
    {
        DicomEquipmentWriter equipmentWriter;
        equipmentWriter.writeEnhancedGeneralEquipement(gDsRoot);
    }

    //*****     Write Content Identification Module     *****//
    //See: PS 3.3 Table 10-12
    this->writeContentIdentification();

    //*****     Complete the file      *****//
    // Set the instance to surface segmentation storage
    ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();
    const char * SOPClassUID = ::gdcm::MediaStorage::GetMSString( ::gdcm::MediaStorage::SurfaceSegmentationStorage );
    std::string SOPClassUIDStr;
    if (SOPClassUID == 0)
        SOPClassUIDStr = "1.2.840.10008.5.1.4.1.1.66.5";
    else
        SOPClassUIDStr = SOPClassUID;
    dicomInstance->setSOPClassUID( SOPClassUIDStr );

    ::gdcm::UIDGenerator gUIDgen;
    gUIDgen.SetRoot( SOPClassUIDStr.c_str() );
    dicomInstance->setSOPInstanceUID( gUIDgen.Generate() );

    dicomInstance->setModality("SEG");

    //*****     Write the file      *****//
    DicomRefInstanceWriter< ::fwData::Acquisition >::write();
}

//------------------------------------------------------------------------------

void DicomSurfaceWriter::setSurfaceNumber(const unsigned short a_num)
{
    m_surfaceNumber = a_num;
    DicomSegmentWriter::setSegmentNumber(a_num);
}

//------------------------------------------------------------------------------

void DicomSurfaceWriter::writeSurfaceMesh(const unsigned int a_idx)
{
    SLM_TRACE_FUNC();

    ::boost::shared_ptr< ::gdcm::SurfaceWriter >    gSurfaceWriter  = ::boost::static_pointer_cast< ::gdcm::SurfaceWriter >( this->getWriter() );
    ::gdcm::DataSet &               gDsRoot         = this->getDataSet();

    // Get the last segment added in SegmentWriter::writeSurfaceSegmentation()
    ::gdcm::SmartPointer< ::gdcm::Segment >         segment         = gSurfaceWriter->GetSegments().back();

    // Add to it a surface
    ::gdcm::SmartPointer< ::gdcm::Surface >         surface         = new ::gdcm::Surface;
    segment->AddSurface(surface);

    ::fwData::Reconstruction::csptr reconstruction = fwData::Reconstruction::dynamicCast (
                                                     this->getConcreteObject()->getField( fwData::Acquisition::ID_RECONSTRUCTIONS )->children()[a_idx]
                                                                                         );
    ::fwData::Material::csptr       material       = reconstruction->getMaterial();

    // Set DicomSurface data
    m_surface->setFromData(reconstruction);    // NOTE: must be called before points and primitives writing

    //*** Create Surface Sequence ***//
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqSurf = new ::gdcm::SequenceOfItems();
    gSqSurf->SetLengthToUndefined();
    {
        // Create one item for the surface
        ::gdcm::Item        gItSurf;
        gItSurf.SetVLToUndefined();
        ::gdcm::DataSet &   gDsSurf = gItSurf.GetNestedDataSet();

        // Surface Number (0x0066,0x0003)
        surface->SetSurfaceNumber( a_idx+1 );         // (start to 1)
        OSLM_TRACE("Surface Number : " << a_idx+1);

        // Surface Processing (0x0066,0x0009)
        surface->SetSurfaceProcessing( false );
        OSLM_TRACE("Surface Processing : NO");

        // Get reconstruction's color
        ::boost::array<float,4> rgba        = material->ambient()->getRGBA();

        // Recommended Display Grayscale Value (0x0062,0x000C)
        std::vector<float>      rgb(3);
        rgb[0] = rgba[0]; rgb[1] = rgba[1]; rgb[2] = rgba[2];
        const unsigned short    grayscale   = ::gdcm::SurfaceHelper::RGBToRecommendedDisplayGrayscale(rgb, 1.);
        surface->SetRecommendedDisplayGrayscaleValue( grayscale );
        OSLM_TRACE("Recommended Display Grayscale Value : " << grayscale);

        // Recommended Display CIELab Value (0x0062,0x000D)
        const std::vector<unsigned short>   CIELab      = ::gdcm::SurfaceHelper::RGBToRecommendedDisplayCIELab(rgb, 1.);
        surface->SetRecommendedDisplayCIELabValue( CIELab );
        OSLM_TRACE("Recommended Display CIELab Value : " << CIELab[0] << " " << CIELab[1] << " " << CIELab[2]);

        // Recommended Presentation Opacity (0x0066,0x000C)
        surface->SetRecommendedPresentationOpacity( rgba[3] );
        OSLM_TRACE("Recommended Presentation Opacity : " << rgba[3]);

        // Recommended Presentation Type (0x0066,0x000D)
        surface->SetRecommendedPresentationType( helper::GdcmData::convertToPresentationType(material->getRepresentationMode()) );
        OSLM_TRACE("Recommended Presentation Type : " << helper::DicomTools::convertToPresentationTypeString(material->getRepresentationMode()) );

//        helper::GdcmData::setTagValue<0x0066,0x000e>( (reconstruction->getIsClosed()?"YES":"NO"),   gDsSurf);   // Finite Volume
        bool _bIsClosed;
        if (::boost::logic::indeterminate(reconstruction->getCRefIsClosed()))
        {
            _bIsClosed = fwData::Reconstruction::isClosed(reconstruction);
        }
        else
        {
            _bIsClosed = reconstruction->getCRefIsClosed();
        }
        surface->SetFiniteVolume( (_bIsClosed ? ::gdcm::Surface::YES : ::gdcm::Surface::NO) );
        OSLM_TRACE("Finite Volume : " << (reconstruction->getIsClosed()?"YES":"NO"));

//        helper::GdcmData::setTagValue<0x0066,0x0010>("NO",                                          gDsSurf);   // Manifold
        surface->SetManifold( ::gdcm::Surface::NO );
        OSLM_TRACE("Manifold : NO");

        //*** Create Surface Points Sequence ***//
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqPoints = new ::gdcm::SequenceOfItems();
        gSqPoints->SetLengthToUndefined();
        {
            // Create its item
            ::gdcm::Item gItPoints;
            gItPoints.SetVLToUndefined();

            // Number Of Points
            surface->SetNumberOfSurfacePoints( m_surface->getPointCoordSize() );
            OSLM_TRACE("Number Of Points : " << m_surface->getPointCoordSize());

            // Point Coordinates Data
            ::gdcm::DataElement &   pointCoordData          = surface->GetPointCoordinatesData();
            const float *           pointCoordinatesData    = m_surface->getPointCoordData().get();
            pointCoordData.SetByteValue( (char*) pointCoordinatesData, 3 * m_surface->getPointCoordSize() * sizeof(*pointCoordinatesData) );
            OSLM_TRACE("Point Coordinates Data buffer size : " << 3 * m_surface->getPointCoordSize() * sizeof(*pointCoordinatesData));

            gSqPoints->AddItem(gItPoints);

            helper::GdcmData::insertSQ<0x0066,0x0011>(gSqPoints, gDsSurf); // Surface Points Sequence
        }

        //*** Create Surface Normals Sequence ***//
        if (m_surface->getNormalCoordSize() > 0)
        {
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqNormals = new ::gdcm::SequenceOfItems();
            gSqNormals->SetLengthToUndefined();
            {
                // Create its item
                ::gdcm::Item gItNormals;
                gItNormals.SetVLToUndefined();

                // Number Of Points
                surface->SetNumberOfVectors( m_surface->getNormalCoordSize() );
                OSLM_TRACE("Number Of Normals : " << m_surface->getNormalCoordSize());

                surface->SetVectorDimensionality(1);

                // Normal Coordinates Data
                ::gdcm::DataElement &   normalCoordData          = surface->GetVectorCoordinateData();
                const float *           normalCoordinatesData    = m_surface->getNormalCoordData().get();
                normalCoordData.SetByteValue( (char*) normalCoordinatesData, 3 * m_surface->getNormalCoordSize() * sizeof(*normalCoordinatesData) );
                OSLM_TRACE("Point Coordinates Data buffer size : " << 3 * m_surface->getNormalCoordSize() * sizeof(*normalCoordinatesData));

                gSqNormals->AddItem(gItNormals);

                helper::GdcmData::insertSQ<0x0066,0x0012>(gSqNormals, gDsSurf); // Surface Normal Sequence
            }
        }

        //*** Create Surface Mesh Primitives Sequence ***//
        //SEE: Supp 132 Table C.X-4
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqPrimitives = new ::gdcm::SequenceOfItems();
        gSqPrimitives->SetLengthToUndefined();
        {
            // Create its item
            ::gdcm::Item gItPrimitives;
            gItPrimitives.SetVLToUndefined();

            // Mesh primitive type (fixed to TRIANGLE by FXW4SPL)
            ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > primitive = surface->GetMeshPrimitive();
            primitive->SetPrimitiveType( ::gdcm::MeshPrimitive::TRIANGLE );

            // Triangle Point Index List
            ::gdcm::DataElement &   pointIndexData      = primitive->GetPrimitiveData();
            pointIndexData.SetVL( sizeof (uint32_t) );

            const uint32_t *        pointIndexListData  = m_surface->getPointIndexList().get();
            pointIndexData.SetByteValue( (char*) pointIndexListData, 3 * m_surface->getPointIndexSize() * sizeof(uint32_t) );
            OSLM_TRACE("Point Index List buffer size : " << 3 * m_surface->getPointIndexSize() * sizeof(uint32_t));

            gSqPrimitives->AddItem(gItPrimitives);

            helper::GdcmData::insertSQ<0x0066,0x0013>(gSqPrimitives, gDsSurf); // Surface Mesh Primitives Sequence
        }

        gSqSurf->AddItem(gItSurf);
    }

    helper::GdcmData::insertSQ<0x0066,0x0002>(gSqSurf, gDsRoot); // Surface Sequence


    //** Complete Segment Sequence **//

    //*** Create Algorithm Family Code Sequence ***//
    //See: Supp 132 Table 10-X
    {
        //see: PS.3.3 Table 8.8-1 and PS 3.16 Context ID 7162
        ::gdcm::SegmentHelper::BasicCodedEntry gAlgoFamily;
//        if ( reconstruction->getIsAutomatic() )
//        {
//            gAlgoFamily.CV = "1231011"; gAlgoFamily.CSD = "DCM"; gAlgoFamily.CM = "Deformable Models";   // FIXME : check
//        }
//        else
        {
            gAlgoFamily.CV = "1231009"; gAlgoFamily.CSD = "DCM"; gAlgoFamily.CM = "Manual Processing";
        }
        // Algorithm Family Code Sequence (0x0066,0x002F)
        surface->SetAlgorithmFamily( gAlgoFamily ); // Type 1
    }

    // Algorithm Version (0x0066,0x0031)
    surface->SetAlgorithmVersion("0.5");// Type 1 //FIXME

    // Algorithm Name (0x0066,0x0036)
    surface->SetAlgorithmName("VRMed"); // Type 1 //FIXME
}

//------------------------------------------------------------------------------

void DicomSurfaceWriter::writeContentIdentification()
{
    // Instance Number
    this->getDicomInstance()->setInstanceNumber("1");   // Instance number is fixed at 1 because there is one file
                                                        // with all segmentations for each series.

    ::gdcm::DataSet & gDsRoot = this->getDataSet();

    // Content Label
    helper::GdcmData::setTagValue<0x0070,0x0080>("SURFACE",                 gDsRoot);

    // Content Description
    helper::GdcmData::setTagValue<0x0070,0x0081>("Surface Segmentation",    gDsRoot);   // Type 2

    // Content Creator’s Name
    helper::GdcmData::setTagValue<0x0070,0x0084>("VRMed",                   gDsRoot);   // Type 2 (eg : VRMed, FW4SPL, ...)
}
}   // namespace writer

}   // namespace gdcmIO
