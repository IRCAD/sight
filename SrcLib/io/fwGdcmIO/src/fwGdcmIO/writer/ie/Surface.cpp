/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomSurface.hpp"
#include "fwGdcmIO/writer/ie/Surface.hpp"

#include "fwGdcmIO/helper/DicomCodedAttribute.hpp"
#include "fwGdcmIO/helper/DicomDataTools.hpp"
#include "fwGdcmIO/helper/DicomDataWriter.hxx"

#include <fwData/Reconstruction.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/dateAndTime.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <gdcmSurfaceHelper.h>
#include <gdcmSurfaceWriter.h>
#include <gdcmUIDGenerator.h>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Surface::Surface(const SPTR(::gdcm::Writer)& writer,
                 const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                 const SPTR(::fwGdcmIO::container::DicomInstance)& imageInstance,
                 const ::fwMedData::ModelSeries::sptr& series,
                 const ::fwLog::Logger::sptr& logger,
                 ProgressCallback progress,
                 CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::ModelSeries >(writer, instance, series,
                                                                          logger, progress, cancel),
    m_imageInstance(imageInstance)
{
    SLM_ASSERT("Image instance should not be null.", imageInstance);
}

//------------------------------------------------------------------------------

Surface::~Surface()
{
}

//------------------------------------------------------------------------------

bool Surface::loadSegmentedPropertyRegistry(const ::boost::filesystem::path& filepath)
{
    return m_segmentedPropertyRegistry.readSegmentedPropertyRegistryFile(filepath, true, m_logger);
}

//------------------------------------------------------------------------------

void Surface::writeSOPCommonModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0016 >(m_instance->getSOPClassUID(), dataset);
    SLM_TRACE("SOP Class UID : " + m_instance->getSOPClassUID());

    // SOP Instance UID
    ::gdcm::UIDGenerator uidGenerator;
    std::string sopInstanceUID = uidGenerator.Generate();
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0018 >(sopInstanceUID, dataset);
}

//------------------------------------------------------------------------------

void Surface::writeSurfaceSegmentationAndSurfaceMeshModules()
{
    // Retrieve Surface Writer
    SPTR(::gdcm::SurfaceWriter) surfaceWriter = std::static_pointer_cast< ::gdcm::SurfaceWriter >(m_writer);

    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    //=======================================================
    // Table C.8.23-1. Surface Segmentation Module Attributes
    //=======================================================
    {
        //======================================================
        //  Table 10-12. Content Identification Macro Attributes
        //======================================================
        {
            // Instance Number - Type 1 - Only one instance per series
            ::fwGdcmIO::helper::DicomDataWriter::setTagValue< int, 0x0020, 0x0013 >(1, dataset);

            // Content Label - Type 1
            ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0070, 0x0080 >("SURFACE", dataset);

            // Content Description - Type 2
            ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0070, 0x0081 >("Surface Segmentation", dataset);

            // Content Creator's Name - Type 2 - TODO: Set surface creator name ?
            ::fwGdcmIO::helper::DicomDataWriter::setEmptyTagValue< 0x0070, 0x0084 >(dataset);
        }

        // Content Date - Type 1
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0023 >(m_object->getDate(), dataset);

        // Content Time - Type 1
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0033 >(m_object->getTime(), dataset);

    }

    //=============================================
    // Table C.27-1. Surface Mesh Module Attributes
    //=============================================
    {
        // Number of Surfaces - Type 1
        auto nbSurfaces = static_cast<unsigned int>(m_object->getReconstructionDB().size());
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< unsigned int, 0x0066, 0x0001 >(nbSurfaces, dataset);
    }

    // Segment Sequence (0x0062,0x0002) - Type 1
    // Segments will be added to this sequence thanks to the gdcm SegmentWriter
    auto segmentSequence = ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0062, 0x0002 >(dataset);

    // Surface Sequence
    auto surfaceSequence = ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0066, 0x0002 >(dataset);

    //====================
    // Write segmentations
    //====================
    unsigned short segmentNumber = 1;
    const auto& reconstuctionDB  = m_object->getReconstructionDB();
    for(const auto& reconstruction : reconstuctionDB)
    {
        // Add segmentation to GDCM Surface Writer
        ::gdcm::SmartPointer< ::gdcm::Segment > segment = new ::gdcm::Segment();
        surfaceWriter->AddSegment(segment);

        // Create Segment Sequence item
        ::gdcm::Item segmentItem;
        segmentItem.SetVLToUndefined();
        // Surface Segmentation Module - SegmentSequence
        writeSegmentSequence(reconstruction, segmentItem, segment, segmentNumber);
        segmentSequence->AddItem(segmentItem);

        // Add a surface to the segmentation
        ::gdcm::SmartPointer< ::gdcm::Surface > surface = new ::gdcm::Surface();
        segment->AddSurface(surface);

        // Create Surface Sequence item
        ::gdcm::Item surfaceItem;
        surfaceItem.SetVLToUndefined();
        // Surface Mesh Module - Surface Sequence
        writeSurfaceSequence(reconstruction, surfaceItem, surface, segmentNumber);
        surfaceSequence->AddItem(surfaceItem);

        // Next segment
        ++segmentNumber;

        if(m_logger)
        {
            m_logger->information("Surface '" + reconstruction->getOrganName() + "' has been exported");
        }
    }

}

//------------------------------------------------------------------------------

void writeSegmentIdentification(const std::string& structureType,
                                const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                                const ::fwGdcmIO::helper::SegmentedPropertyRegistry& registry,
                                ::fwLog::Logger::sptr logger)
{
    // Check that the structure name has an entry in the registry
    if(!registry.hasEntry(structureType))
    {
        const std::string msg = "No match in segmented property registry for structure '" + structureType + "'";
        SLM_WARN_IF(msg, !logger);
        if(logger)
        {
            logger->warning(msg);
        }
    }

    // Note: No error or warning is emitted from this function regarding
    // multiplicity of coded attributes as checks are already performed in
    // registry.
    const auto entry = registry.getEntry(structureType);

    //======================================================
    // Table 10-7. General Anatomy Optional Macro Attributes
    //======================================================
    {
        // Anatomic Region Sequence (0x0008,0x2218) - Type 3
        const std::string& anatomicRegion = entry[3];
        if(!anatomicRegion.empty())
        {
            const auto codedAttributes =
                ::fwGdcmIO::helper::DicomCodedAttribute::convertEntryToGDCMCodedAttribute(anatomicRegion);
            if(!codedAttributes.empty())
            {
                // Only a single Item shall be included in this Sequence
                segment->SetAnatomicRegion(codedAttributes[0]);
            }
        }

        // Anatomic Region Modifier Sequence (0x0008,0x2220) - Type 3
        const std::string& anatomicRegionModifier = entry[4];
        if(!anatomicRegionModifier.empty())
        {
            const auto codedAttributes =
                ::fwGdcmIO::helper::DicomCodedAttribute::convertEntryToGDCMCodedAttribute(anatomicRegionModifier);
            if(!codedAttributes.empty())
            {
                // One or more Items are permitted in this Sequence.
                segment->SetAnatomicRegionModifiers(codedAttributes);
            }
        }
    }

    // Segmented Property Category Code Sequence (0x0062,0x0003) - Type 1
    {
        const std::string& propertyCategory = entry[1];
        const auto codedAttributes          =
            ::fwGdcmIO::helper::DicomCodedAttribute::convertEntryToGDCMCodedAttribute(propertyCategory);
        if(!codedAttributes.empty())
        {
            // Only a single Item shall be included in this Sequence
            segment->SetPropertyCategory(codedAttributes[0]);
        }
    }

    // Segmented Property Type Code Sequence (0x0062,0x000F) - Type 1
    {
        const std::string& propertyType = entry[0];
        const auto codedAttributes      =
            ::fwGdcmIO::helper::DicomCodedAttribute::convertEntryToGDCMCodedAttribute(propertyType);
        if(!codedAttributes.empty())
        {
            // Only a single Item shall be included in this Sequence
            segment->SetPropertyType(codedAttributes[0]);
        }

    }

    // Segmented Property Type Modifier Code Sequence (0x0062,0x0011) - Type 3
    {
        const std::string& propertyTypeModifier = entry[2];
        if(!propertyTypeModifier.empty())
        {
            const auto codedAttributes =
                ::fwGdcmIO::helper::DicomCodedAttribute::convertEntryToGDCMCodedAttribute(propertyTypeModifier);
            if(!codedAttributes.empty())
            {
                // One or more Items are permitted in this Sequence.
                segment->SetPropertyTypeModifiers(codedAttributes);
            }
        }
    }

}

//------------------------------------------------------------------------------

void writePrivateTags(const ::fwData::Reconstruction::csptr& reconstruction,
                      ::gdcm::DataSet& dataset)
{
    // Private group
    const auto reservedGroup    = 0x5649;
    const auto reservingElement = 0x0010; // Reserve group (gggg,0x1000-0x10FF)
    const auto privateCreator   = "FW4SPL";

    // Reserve group
    {
        ::gdcm::Attribute< reservedGroup, reservingElement, ::gdcm::VR::LO, ::gdcm::VM::VM1 > attribute;
        attribute.SetValue(privateCreator);
        dataset.Insert(attribute.GetAsDataElement());
    }

    // Structure type
    {
        const auto structureType = reconstruction->getStructureType();
        ::gdcm::Attribute< reservedGroup, 0x1000, ::gdcm::VR::LO, ::gdcm::VM::VM1 > attribute;
        attribute.SetValue(structureType);
        dataset.Insert(attribute.GetAsDataElement());
    }

    // Computed mask volume
    const double volume = reconstruction->getComputedMaskVolume();
    if(volume > 0)
    {
        ::gdcm::Attribute< reservedGroup, 0x1001, ::gdcm::VR::OD, ::gdcm::VM::VM1 > attribute;
        attribute.SetValue(volume);
        dataset.Insert(attribute.GetAsDataElement());
    }

}

//------------------------------------------------------------------------------

void Surface::writeSegmentSequence(const ::fwData::Reconstruction::csptr& reconstruction,
                                   ::gdcm::Item& segmentItem,
                                   const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                                   unsigned short segmentNumber)
{
    // Retrieve segment dataset
    ::gdcm::DataSet& segmentItemDataset = segmentItem.GetNestedDataSet();

    //=======================================================
    // Table C.8.20-4. 'Segment Description Macro Attributes'
    //=======================================================
    {
        // Segment Number (0x0062,0x0004) - Type 1 (start at 1)
        segment->SetSegmentNumber(segmentNumber);

        // Segment Label (0x0062,0x0005) - Type 1
        std::string segmentLabel = reconstruction->getOrganName();
        segment->SetSegmentLabel(segmentLabel.c_str());

        // Segment Algorithm Type (0x0062,0x0008) - Type 1
        segment->SetSegmentAlgorithmType(::gdcm::Segment::SEMIAUTOMATIC);

        // Anatomic Region Sequence (0x0008,0x2218) - Type 2
        // Anatomic Region Modifier Sequence (0x0008,0x2220) - Type 3
        // Segmented Property Category Code Sequence (0x0062,0x0003) - Type 1
        // Segmented Property Type Code Sequence (0x0062,0x000F) - Type 1
        // Segmented Property Type Code Sequence (0x0062,0x0011) - Type 3
        const std::string structureType = reconstruction->getStructureType();
        writeSegmentIdentification(structureType, segment, m_segmentedPropertyRegistry, m_logger);
    }

    // Surface Count (0x0066,0x002A) - Type 1 - Reconstruction just contains one surface.
    segment->SetSurfaceCount(1);

    // Referenced Surface Sequence - Type 1
    auto referencedSurfaceSequence =
        ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0066, 0x002B >(segmentItemDataset);

    // Referenced Surface Sequence Item
    ::gdcm::Item refSurfaceSeqItem;
    refSurfaceSeqItem.SetVLToUndefined();
    ::gdcm::DataSet& refSurfaceSeqItemDataset = refSurfaceSeqItem.GetNestedDataSet();
    {
        // Referenced Surface Number - Type 1
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< unsigned int, 0x0066, 0x002C >(segmentNumber,
                                                                                         refSurfaceSeqItemDataset);

        //=======================================================
        // Table 10-19. Algorithm Identification Macro Attributes
        //=======================================================
        // This section is handled in the surface section because of the GDCM soup

        // Segment Surface Source Instance Sequence - Type 2
        auto surfaceSourceInstanceSequence =
            ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0066, 0x002E >(
                refSurfaceSeqItemDataset);

        //==========================================================
        // Table 10-3. Image SOP Instance Reference Macro Attributes
        //==========================================================
        {
            const std::vector< std::string >& referencedSOPInstanceUIDContainer =
                m_imageInstance->getSOPInstanceUIDContainer();
            const std::string& referencedSOPClassUID = m_imageInstance->getSOPClassUID();

            //=====================================================
            // Table 10-11. SOP Instance Reference Macro Attributes
            //=====================================================
            for(const std::string& sopInstanceUID : referencedSOPInstanceUIDContainer)
            {
                // Create the item
                ::gdcm::Item imageSOPItem;
                imageSOPItem.SetVLToUndefined();
                ::gdcm::DataSet& imageSOPDataset = imageSOPItem.GetNestedDataSet();

                // Referenced SOP Class UID - Type 1
                ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1150 >(referencedSOPClassUID,
                                                                                   imageSOPDataset);

                // Referenced SOP Instance UID - Type 1
                ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1155 >(sopInstanceUID,
                                                                                   imageSOPDataset);

                surfaceSourceInstanceSequence->AddItem(imageSOPItem);
            }

            if(m_logger && referencedSOPInstanceUIDContainer.empty())
            {
                m_logger->warning("No DICOM instances have been added as 'Referenced SOP Instance UID'");
            }
        }
    }
    referencedSurfaceSequence->AddItem(refSurfaceSeqItem);

    // Private Tags
    writePrivateTags(reconstruction, segmentItemDataset);

}

//------------------------------------------------------------------------------

void Surface::writeSurfaceSequence(const ::fwData::Reconstruction::csptr& reconstruction,
                                   ::gdcm::Item& surfaceItem,
                                   const ::gdcm::SmartPointer< ::gdcm::Surface >& surface,
                                   unsigned short segmentNumber)
{
    // Retrieve surface dataset
    ::gdcm::DataSet& surfaceItemDataset = surfaceItem.GetNestedDataSet();

    // Retrieve material
    ::fwData::Material::csptr material = reconstruction->getMaterial();

    // Set DicomSurface data - NOTE: must be called before points and primitives writing
    ::fwGdcmIO::container::DicomSurface surfaceContainer(reconstruction);

    // Surface Number (0x0066,0x0003) - Type 1
    surface->SetSurfaceNumber(segmentNumber);

    // Surface Processing (0x0066,0x0009) - NO - Type 2
    surface->SetSurfaceProcessing(false);

    // Get reconstruction's color
    const auto rgba = material->diffuse()->getRGBA();
    std::vector< float > rgb { rgba[0], rgba[1], rgba[2] };

    // Recommended Display Grayscale Value (0x0062,0x000C) - Type 1
    const auto grayscale = ::gdcm::SurfaceHelper::RGBToRecommendedDisplayGrayscale(rgb, 1.);
    surface->SetRecommendedDisplayGrayscaleValue(grayscale);

    // Recommended Display CIELab Value (0x0062,0x000D) - Type 1
    const auto CIELab = ::gdcm::SurfaceHelper::RGBToRecommendedDisplayCIELab(rgb, 1.);
    surface->SetRecommendedDisplayCIELabValue(CIELab);

    // Recommended Presentation Opacity (0x0066,0x000C) - Type 1
    surface->SetRecommendedPresentationOpacity(rgba[3]);

    // Recommended Presentation Type (0x0066,0x000D) - Type 1
    surface->SetRecommendedPresentationType(
        ::fwGdcmIO::helper::DicomDataTools::convertToPresentationType(material->getRepresentationMode()));

    // Finite Volume (0x0066,0x000E) - Type 1
    ::fwDataTools::helper::Mesh helperMesh(reconstruction->getMesh());
    surface->SetFiniteVolume(helperMesh.isClosed() ? (::gdcm::Surface::YES): (::gdcm::Surface::NO));

    // Manifold (0x0066,0x0010) - Type 1
    surface->SetManifold(::gdcm::Surface::NO);

    // Surface Points Sequence (0x0066,0x0011) - Type 1
    auto pointsSequence =
        ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0066, 0x0011 >(surfaceItemDataset);

    // Create one item
    ::gdcm::Item pointsItem;
    pointsItem.SetVLToUndefined();

    //======================================
    // Table C.27-2. Points Macro Attributes
    //======================================
    {
        // Number Of Surface Points (0x0066,0x0015) - Type 1
        surface->SetNumberOfSurfacePoints(surfaceContainer.getPointBufferSize() / 3);

        // Point Coordinates Data (0x0066,0x0016) - Type 1
        ::gdcm::DataElement& pointCoordData = surface->GetPointCoordinatesData();
        pointCoordData.SetByteValue(reinterpret_cast<const char*>(surfaceContainer.getPointBuffer().data()),
                                    static_cast<uint32_t>(surfaceContainer.getPointBufferSize()) *
                                    static_cast<uint32_t>(sizeof(float)));
    }
    pointsSequence->AddItem(pointsItem);

    // Surface Points Normals Sequence (0x0066,0x0012) - Type 2
    auto normalsSequence =
        ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0066, 0x0012 >(surfaceItemDataset);

    //=======================================
    // Table C.27-3. Vectors Macro Attributes
    //=======================================
    if (surfaceContainer.getNormalBufferSize() > 0)
    {
        // Create one item
        ::gdcm::Item normalsItem;
        normalsItem.SetVLToUndefined();

        // Number Of Vectors (0x0066, 0x001e) - Type 1
        surface->SetNumberOfVectors(surfaceContainer.getNormalBufferSize() / 3);

        // Vector Dimensionality (0x0066, 0x001f) - Type 1
        surface->SetVectorDimensionality(1);

        // Vector Coordinate Data (0x0066,0x0021) - Type 1
        ::gdcm::DataElement& normalCoordData = surface->GetVectorCoordinateData();
        normalCoordData.SetByteValue(reinterpret_cast<const char*>(surfaceContainer.getNormalBuffer().data()),
                                     static_cast<uint32_t>(surfaceContainer.getNormalBufferSize()) *
                                     static_cast<uint32_t>(sizeof(float)));

        normalsSequence->AddItem(normalsItem);
    }

    // Surface Mesh Primitives Sequence (0x0066,0x0013) - Type 1
    auto primitivesSequence =
        ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0066, 0x0013 >(surfaceItemDataset);

    //=======================================================
    // Table C.27-4. Surface Mesh Primitives Macro Attributes
    //=======================================================
    {
        // Mesh primitive type used by surface writer (fixed to TRIANGLE by FW4SPL)
        ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > primitive = surface->GetMeshPrimitive();
        primitive->SetPrimitiveType(::gdcm::MeshPrimitive::TRIANGLE);

        // Long Triangle Point Index List (0x0066,0x0041) - Type 2
        ::gdcm::DataElement& pointIndexData = primitive->GetPrimitiveData();
        pointIndexData.SetVL(sizeof(uint32_t));
        pointIndexData.SetByteValue(reinterpret_cast<const char*>(surfaceContainer.getCellBuffer().data()),
                                    static_cast<uint32_t>(surfaceContainer.getCellBufferSize()) *
                                    static_cast<uint32_t>(sizeof(uint32_t)));
    }

    //=======================================================
    // Table 10-19. Algorithm Identification Macro Attributes
    //=======================================================
    {
        // Algorithm Family Code Sequence - Type 1
        surface->SetAlgorithmFamily(::gdcm::SegmentHelper::BasicCodedEntry("123109", "DCM", "Manual Processing"));

        // Algorithm Name (0x0066,0x0036) - Type 1
        surface->SetAlgorithmName("FW4SPL");

        // Algorithm Version (0x0066,0x0031) - Type 1
        surface->SetAlgorithmVersion("1");
    }
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
