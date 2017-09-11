/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomSurface.hpp"
#include "fwGdcmIO/writer/ie/Surface.hpp"

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/helper/DictionarySegment.hpp"

#include <fwData/Reconstruction.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/dateAndTime.hpp>

#include <gdcmSurfaceHelper.h>
#include <gdcmSurfaceWriter.h>
#include <gdcmUIDGenerator.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Surface::Surface(SPTR(::gdcm::Writer)writer,
                 SPTR(::fwGdcmIO::container::DicomInstance)instance,
                 SPTR(::fwGdcmIO::container::DicomInstance)imageInstance,
                 ::fwMedData::ModelSeries::sptr series) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::ModelSeries >(writer, instance, series),
    m_imageInstance(imageInstance)
{
    SLM_ASSERT("Image instance should not be null.", imageInstance);
    SLM_ASSERT("Image instance SOPInstanceUID container should not be empty.",
               !imageInstance->getSOPInstanceUIDContainer().empty());

    // Create dictionary
    m_structureDictionary = ::fwData::StructureTraitsDictionary::New();

    // Ready the dictionary
    ::fwDataIO::reader::DictionaryReader::sptr dictionaryReader = ::fwDataIO::reader::DictionaryReader::New();
    dictionaryReader->setObject(m_structureDictionary);
    dictionaryReader->setFile(RC_PREFIX "/fwDataIO_0-2/OrganDictionary.dic");
    try
    {
        dictionaryReader->read();
    }
    catch (const std::exception& e)
    {
        SLM_ERROR(e.what());
    }

}

//------------------------------------------------------------------------------

Surface::~Surface()
{
}

//------------------------------------------------------------------------------

void Surface::writeSurfaceSegmentationModule(unsigned short segmentationNumber)
{
    // Retrieve Surface Writer
    SPTR(::gdcm::SurfaceWriter) surfaceWriter = std::static_pointer_cast< ::gdcm::SurfaceWriter >(m_writer);

    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Retrieve reconstruction
    ::fwData::Reconstruction::sptr reconstruction = m_object->getReconstructionDB()[segmentationNumber];

    // Write Content Identification Macro
    this->writeContentIdentificationMacro(segmentationNumber);

    ::boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();

    // Content Date - Type 1 - FIXME: Keep series date ?
    std::string date = m_object->getDate().empty() ? ::fwTools::getDate(ptime) : m_object->getDate();
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0023 >(date, dataset);

    // Content Time - Type 1 - FIXME: Keep series time ?
    std::string time = m_object->getTime().empty() ? ::fwTools::getTime(ptime) : m_object->getTime();
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0033 >(time, dataset);

    // Identification of the segment
    const std::string& segmentDescription = reconstruction->getOrganName();

    // Get the info of the struture type and format it
    std::string segmentLabel = reconstruction->getStructureType();

    if(segmentLabel.empty())
    {
        SLM_WARN("Trying to retrieve structure type from organ name : \"" + segmentDescription + "\".");
        segmentLabel = segmentDescription;
    }

    // Format segment label
    ::boost::algorithm::trim(segmentLabel);
    if(segmentLabel.size() > 1)
    {
        segmentLabel = ::boost::algorithm::to_upper_copy(segmentLabel.substr(0,1)) + segmentLabel.substr(1);
    }

    ::fwData::StructureTraitsDictionary::StructureTypeNameContainer segmentLabelContainer =
        m_structureDictionary->getStructureTypeNames();

    ::fwData::StructureTraits::sptr structure;
    if (std::find(segmentLabelContainer.begin(), segmentLabelContainer.end(), segmentLabel) !=
        segmentLabelContainer.end())
    {
        structure = m_structureDictionary->getStructure(segmentLabel);
    }
    else
    {
        SLM_WARN("No structure traits found in dictionary for value \"" + segmentLabel + "\".");
        structure = ::fwData::StructureTraits::New();
    }

    // Identify the segmentation from its name
    const ::fwGdcmIO::container::DicomCodedAttribute* anatomicRegion =
        ::fwGdcmIO::helper::DictionarySegment::guessAnatRegionFromLabel(structure->getAnatomicRegion());

    const ::fwGdcmIO::container::DicomCodedAttribute* propertyCategory =
        ::fwGdcmIO::helper::DictionarySegment::guessPropCategoryFromLabel(structure->getPropertyCategory());

    const ::fwGdcmIO::container::DicomCodedAttribute* propertyType =
        ::fwGdcmIO::helper::DictionarySegment::guessPropTypeFromLabel(structure->getPropertyType());


    // Add segmentation to GDCM Surface Writer
    ::gdcm::SmartPointer< ::gdcm::Segment > segment = new ::gdcm::Segment();
    surfaceWriter->AddSegment(segment);

    // Create Segment Sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > surfaceSequence = new ::gdcm::SequenceOfItems();
    surfaceSequence->SetLengthToUndefined();

    // Create Surface item
    ::gdcm::Item surfaceItem;
    surfaceItem.SetVLToUndefined();
    ::gdcm::DataSet& segmentationDataset = surfaceItem.GetNestedDataSet();

    // Segment Number (0x0062,0x0004) - Type 1 (start at 1)
    unsigned short realSegmentationNumber = static_cast<unsigned short>(segmentationNumber + 1);
    segment->SetSegmentNumber(realSegmentationNumber);
    OSLM_TRACE("Segment Number : " << realSegmentationNumber);

    // Segment Label (0x0062,0x0005) - Type 1
    segment->SetSegmentLabel(segmentLabel.c_str());
    OSLM_TRACE("Segment Label : " << segmentLabel);

    // Segment Description (0x0062,0x0006) - Type 3
    segment->SetSegmentDescription(segmentDescription.c_str());
    OSLM_TRACE("Segment Description : " << segmentDescription);

    // Segment Algorithm Type (0x0062,0x0008) - Type 1
    segment->SetSegmentAlgorithmType(::gdcm::Segment::MANUAL);

    // Create Anatomic Region Sequence - See General Anatomy Macros PS 3.3 C.10.5
    if (anatomicRegion != 0)
    {
        // Anatomic Region Sequence (0x0008,0x2218) - Type 1
        ::gdcm::SegmentHelper::BasicCodedEntry anatomicRegionEntry(anatomicRegion->getCodeValue().c_str(),
                                                                   anatomicRegion->getCodingSchemeDesignator().c_str(),
                                                                   anatomicRegion->getCodeMeaning().c_str());
        segment->SetAnatomicRegion(anatomicRegionEntry);
    }

    // Create Segmented Property Category Code Sequence
    if (propertyCategory != 0)
    {
        // Segmented Property Category Code Sequence (0x0062,0x0003) - Type 1
        // See: PS.3.3 Table 8.8-1 and Context ID is 7150
        ::gdcm::SegmentHelper::BasicCodedEntry propertyCategoryEntry(propertyCategory->getCodeValue().c_str(),
                                                                     propertyCategory->getCodingSchemeDesignator().c_str(),
                                                                     propertyCategory->getCodeMeaning().c_str());
        segment->SetPropertyCategory(propertyCategoryEntry);
    }

    // Create Segmented Property Type Code Sequence
    if (propertyType != 0)
    {
        // Segmented Property Type Code Sequence (0x0062,0x000F) - Type 1
        // See: PS.3.3 Table 8.8-1 and PS 3.16 Context ID 7151
        ::gdcm::SegmentHelper::BasicCodedEntry propertyTypeEntry(propertyType->getCodeValue().c_str(),
                                                                 propertyType->getCodingSchemeDesignator().c_str(),
                                                                 propertyType->getCodeMeaning().c_str());
        segment->SetPropertyType(propertyTypeEntry);
    }

    // Surface Count (0x0066,0x002A) - Type 1
    // WARNING : Hard coded because currently, reconstruction just contains one surface.
    segment->SetSurfaceCount(1);

    // Create Referenced Surface Sequence
    // Each item of this sequence references a surface for the current segment.
    // A segment can have several surfaces (see tag(0x0066,0x002A), Surface Count).
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > referencedSurfaceSequence = new ::gdcm::SequenceOfItems();
    referencedSurfaceSequence->SetLengthToUndefined();
    {
        // Create its item
        ::gdcm::Item referencedSurfaceItem;
        referencedSurfaceItem.SetVLToUndefined();
        ::gdcm::DataSet& referencedSurfaceDataset = referencedSurfaceItem.GetNestedDataSet();

        // WARNING : index+1 == segmentNumber == surfaceNumber because currently, reconstruction just contains one surface.*
        // Referenced Surface Number - Type 1
        ::fwGdcmIO::helper::DicomData::setTagValue< uint16_t, 0x0066, 0x002C >(realSegmentationNumber,
                                                                               referencedSurfaceDataset);

        // Create Segment Surface Source Instance Sequence
        // Reference the image of the fwMedData::ImageSeries
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > surfaceSourceInstanceSequence = new ::gdcm::SequenceOfItems();
        surfaceSourceInstanceSequence->SetLengthToUndefined();

        // Include ‘Image SOP Instance Reference Macro’ Table C.10-3
        const std::string& referencedSOPClassUID                            = m_imageInstance->getSOPClassUID();
        const std::vector< std::string >& referencedSOPInstanceUIDContainer =
            m_imageInstance->getSOPInstanceUIDContainer();

        for(const std::string& sopInstanceUID: referencedSOPInstanceUIDContainer)
        {
            // Create one item
            ::gdcm::Item imageSOPItem;
            imageSOPItem.SetVLToUndefined();
            ::gdcm::DataSet& imageSOPDataset = imageSOPItem.GetNestedDataSet();

            // SOP Class UID - Type 1
            ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x1150 >(referencedSOPClassUID, imageSOPDataset);

            // SOP Instance UID - Type 1
            ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x1155 >(sopInstanceUID, imageSOPDataset);

            surfaceSourceInstanceSequence->AddItem(imageSOPItem);
        }

        // Segment Surface Source Instance Sequence - Type 1C
        ::fwGdcmIO::helper::DicomData::insertSQ< 0x0066, 0x002E >(surfaceSourceInstanceSequence,
                                                                  referencedSurfaceDataset);

        referencedSurfaceSequence->AddItem(referencedSurfaceItem);

        // Referenced Surface Sequence
        ::fwGdcmIO::helper::DicomData::insertSQ< 0x0066, 0x002B >(referencedSurfaceSequence, segmentationDataset);
    }
    surfaceSequence->AddItem(surfaceItem);

    // Segment Sequence
    ::fwGdcmIO::helper::DicomData::insertSQ< 0x0062, 0x0002 >(surfaceSequence, dataset);

}

//------------------------------------------------------------------------------

void Surface::writeSurfaceMeshModule(unsigned short segmentationNumber)
{
    // Retrieve Surface Writer
    SPTR(::gdcm::SurfaceWriter) surfaceWriter = std::static_pointer_cast< ::gdcm::SurfaceWriter >(m_writer);

    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Get the last segment added in SegmentWriter::writeSurfaceSegmentation()
    ::gdcm::SmartPointer< ::gdcm::Segment > segment = surfaceWriter->GetSegments().back();

    // Add it a surface
    ::gdcm::SmartPointer< ::gdcm::Surface > surface = new ::gdcm::Surface();
    segment->AddSurface(surface);

    // Retrieve reconstruction
    ::fwData::Reconstruction::sptr reconstruction = m_object->getReconstructionDB()[segmentationNumber];

    // Retrieve material
    ::fwData::Material::csptr material = reconstruction->getMaterial();

    // Set DicomSurface data - NOTE: must be called before points and primitives writing
    SPTR(::fwGdcmIO::container::DicomSurface) surfaceContainer =
        std::make_shared< ::fwGdcmIO::container::DicomSurface >();
    surfaceContainer->setFromData(reconstruction);

    // Create Surface Sequence
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > surfaceSequence = new ::gdcm::SequenceOfItems();
    surfaceSequence->SetLengthToUndefined();
    {
        // Create one item for the surface
        ::gdcm::Item surfaceItem;
        surfaceItem.SetVLToUndefined();
        ::gdcm::DataSet& surfaceDataset = surfaceItem.GetNestedDataSet();

        // Surface Number (0x0066,0x0003) - Type 1
        surface->SetSurfaceNumber(segmentationNumber + 1); // (start at 1)
        OSLM_TRACE("Surface Number : " << surface->GetSurfaceNumber());

        // Surface Processing (0x0066,0x0009) - Type 2
        surface->SetSurfaceProcessing(false);
        OSLM_TRACE("Surface Processing : NO");

        // Get reconstruction's color
        std::array< float, 4 > rgba = material->diffuse()->getRGBA();
        std::vector< float > rgb(3);
        rgb[0] = rgba[0];
        rgb[1] = rgba[1];
        rgb[2] = rgba[2];

        // Recommended Display Grayscale Value (0x0062,0x000C) - Type 1
        const unsigned short grayscale = ::gdcm::SurfaceHelper::RGBToRecommendedDisplayGrayscale(rgb, 1.);
        surface->SetRecommendedDisplayGrayscaleValue(grayscale);
        OSLM_TRACE("Recommended Display Grayscale Value : " << grayscale);

        // Recommended Display CIELab Value (0x0062,0x000D) - Type 1
        const std::vector< unsigned short > CIELab = ::gdcm::SurfaceHelper::RGBToRecommendedDisplayCIELab(rgb, 1.);
        surface->SetRecommendedDisplayCIELabValue(CIELab);
        OSLM_TRACE("Recommended Display CIELab Value : " << CIELab[0] << " " << CIELab[1] << " " << CIELab[2]);

        // Recommended Presentation Opacity (0x0066,0x000C) - Type 1
        surface->SetRecommendedPresentationOpacity(rgba[3]);
        OSLM_TRACE("Recommended Presentation Opacity : " << rgba[3]);

        // Recommended Presentation Type (0x0066,0x000D)
        surface->SetRecommendedPresentationType(
            ::fwGdcmIO::helper::DicomData::convertToPresentationType(material->getRepresentationMode()));
        OSLM_TRACE( "Recommended Presentation Type : " <<
                    ::fwGdcmIO::helper::DicomData::convertToPresentationTypeString(material->getRepresentationMode()));

        // Finite Volume (0x0066,0x000E) - Type 1
        ::fwDataTools::helper::Mesh helperMesh(reconstruction->getMesh());
        surface->SetFiniteVolume(helperMesh.isClosed() ? (::gdcm::Surface::YES): (::gdcm::Surface::NO));

        // Manifold (0x0066,0x0010) - Type 1
        surface->SetManifold(::gdcm::Surface::NO);
        SLM_TRACE("Manifold : NO");

        // Create Surface Points Sequence
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > pointsSequence = new ::gdcm::SequenceOfItems();
        pointsSequence->SetLengthToUndefined();
        {
            // Create its item
            ::gdcm::Item pointsItem;
            pointsItem.SetVLToUndefined();

            // Number Of Points
            surface->SetNumberOfSurfacePoints(surfaceContainer->getPointCoordSize());
            OSLM_TRACE("Number Of Points : " << surfaceContainer->getPointCoordSize());

            // Point Coordinates Data
            ::gdcm::DataElement& pointCoordData = surface->GetPointCoordinatesData();
            const float* pointCoordinatesData = surfaceContainer->getPointCoordData().get();
            pointCoordData.SetByteValue((char*)(pointCoordinatesData),
                                        static_cast<unsigned int>(3 * surfaceContainer->getPointCoordSize() *
                                                                  sizeof(*pointCoordinatesData)));
            OSLM_TRACE("Point Coordinates Data buffer size : " << 3 * surfaceContainer->getPointCoordSize()
                       * sizeof(*pointCoordinatesData));

            pointsSequence->AddItem(pointsItem);

            // Surface Points Sequence
            ::fwGdcmIO::helper::DicomData::insertSQ< 0x0066, 0x0011 >(pointsSequence, surfaceDataset);
        }

        // Create Surface Points Normals Sequence (0066,0012) - Type 2
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > normalSequence = new ::gdcm::SequenceOfItems();
        normalSequence->SetLengthToUndefined();
        if (surfaceContainer->getNormalCoordSize() > 0)
        {
            // Create its item
            ::gdcm::Item normalsItem;
            normalsItem.SetVLToUndefined();

            // Number Of Points
            surface->SetNumberOfVectors(surfaceContainer->getNormalCoordSize());
            OSLM_TRACE("Number Of Normals : " << surfaceContainer->getNormalCoordSize());

            surface->SetVectorDimensionality(1);

            // Normal Coordinates Data
            ::gdcm::DataElement& normalCoordData = surface->GetVectorCoordinateData();
            const float* normalCoordinatesData = surfaceContainer->getNormalCoordData().get();
            normalCoordData.SetByteValue((char*)(normalCoordinatesData),
                                         static_cast<unsigned int>(3 * surfaceContainer->getNormalCoordSize() *
                                                                   sizeof(*normalCoordinatesData)));
            OSLM_TRACE("Point Coordinates Data buffer size : " << 3 * surfaceContainer->getNormalCoordSize() *
                       sizeof(*normalCoordinatesData));

            normalSequence->AddItem(normalsItem);
        }
        ::fwGdcmIO::helper::DicomData::insertSQ< 0x0066, 0x0012 >(normalSequence, surfaceDataset);

        // Create Surface Mesh Primitives Sequence - PS 3.3 C.27.4
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > primitivesSequence = new ::gdcm::SequenceOfItems();
        primitivesSequence->SetLengthToUndefined();

        {
            // Mesh primitive type (fixed to TRIANGLE by FXW4SPL)
            ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > primitive = surface->GetMeshPrimitive();
            primitive->SetPrimitiveType(::gdcm::MeshPrimitive::TRIANGLE);

            // Triangle Point Index List (0066,0023) - Type 2
            ::gdcm::DataElement& pointIndexData = primitive->GetPrimitiveData();
            pointIndexData.SetVL(sizeof(uint32_t));
            const uint32_t* pointIndexListData = surfaceContainer->getPointIndexList().get();
            unsigned long indexCount           = 3 * surfaceContainer->getPointIndexSize();
            pointIndexData.SetByteValue((char*)(pointIndexListData),
                                        static_cast<unsigned int>(indexCount * sizeof(uint32_t)));
            OSLM_TRACE("Point Index List buffer size : " << indexCount * sizeof(uint32_t));

        }

        // Surface Mesh Primitives Sequence
        ::fwGdcmIO::helper::DicomData::insertSQ< 0x0066, 0x0013 >(primitivesSequence, surfaceDataset);

        surfaceSequence->AddItem(surfaceItem);
    }

    // Surface Sequence
    ::fwGdcmIO::helper::DicomData::insertSQ< 0x0066, 0x0002 >(surfaceSequence, dataset);

    // Complete Segment Sequence
    // Create Algorithm Family Code Sequence
    // See: Supplement 132 Table 10-X
    {
        //See: PS.3.3 Table 8.8-1 and PS 3.16 Context ID 7162
        ::gdcm::SegmentHelper::BasicCodedEntry algoFamily;
        algoFamily.CV  = "1231009";
        algoFamily.CSD = "DCM";
        algoFamily.CM  = "Manual Processing";
        // Algorithm Family Code Sequence (0x0066,0x002F) - Type 1
        surface->SetAlgorithmFamily(algoFamily);
    }

    // Retrieve runtime profile
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    // Algorithm Version (0x0066,0x0031) - Type 1
    std::string algorithVersion = (profile) ? (profile->getName()+" "+profile->getVersion()) : "Unknown";
    surface->SetAlgorithmVersion(algorithVersion.c_str());

    // Algorithm Name (0x0066,0x0036) - Type 1
    std::string algorithName = (profile) ? (profile->getName()) : "Unknown";
    surface->SetAlgorithmName(algorithName.c_str());

}

//------------------------------------------------------------------------------

void Surface::writeSOPCommonModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0016 >(m_instance->getSOPClassUID(), dataset);
    SLM_TRACE("SOP Class UID : " + m_instance->getSOPClassUID());

    // SOP Instance UID
    ::gdcm::UIDGenerator uidGenerator;
    std::string sopInstanceUID = uidGenerator.Generate();
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0018 >(sopInstanceUID, dataset);
}

//------------------------------------------------------------------------------

void Surface::writeContentIdentificationMacro(unsigned int segmentationNumber)
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Instance Number - Type 1
    ::fwGdcmIO::helper::DicomData::setTagValue< int, 0x0020, 0x0013 >(segmentationNumber, dataset);

    // Content Label - Type 1
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0070, 0x0080 >("SURFACE", dataset);

    // Content Description - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0070, 0x0081 >("Surface Segmentation", dataset);

    // Content Creator's Name - Type 2 - TODO: Set surface creator name ?
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0070, 0x0084 >("Unknown^Unknown", dataset);
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
