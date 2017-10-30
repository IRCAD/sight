/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/container/DicomSurface.hpp"
#include "fwGdcmIO/reader/ie/Surface.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/helper/DicomDataTools.hpp"

#include <fwData/Color.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <gdcmSurfaceHelper.h>

#include <sstream>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Surface::Surface(const ::fwMedData::DicomSeries::sptr& dicomSeries,
                 const SPTR(::gdcm::Reader)& reader,
                 const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                 const ::fwMedData::ModelSeries::sptr& series,
                 const ::fwLog::Logger::sptr& logger,
                 ProgressCallback progress,
                 CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::ModelSeries >(dicomSeries, reader, instance, series,
                                                                          logger, progress, cancel)
{
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

void Surface::readSurfaceSegmentationAndSurfaceMeshModules()
{
    // Retrieve Surface Reader
    SPTR(::gdcm::SurfaceReader) surfaceReader = std::static_pointer_cast< ::gdcm::SurfaceReader >(m_reader);

    // Retrieve dataset
    const ::gdcm::DataSet& dataset = surfaceReader->GetFile().GetDataSet();

    // Segment Sequence (0x0062,0x0002) - Type 1
    const ::gdcm::DataElement& sequenceDataElement = dataset.GetDataElement(::gdcm::Tag(0x0062, 0x0002));
    const auto segmentSequence                     = sequenceDataElement.GetValueAsSQ();

    // Segment container
    const auto& segmentContainer = surfaceReader->GetSegments();

    // Retrieve reconstruction DB
    ::fwMedData::ModelSeries::ReconstructionVectorType reconstructionDB = m_object->getReconstructionDB();

    // Lambda used to display reading errors
    auto displayError = [&](const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                            const std::string& error)
                        {
                            std::string segmentLabel = segment->GetSegmentLabel();
                            segmentLabel = ::gdcm::LOComp::Trim(segmentLabel.c_str());
                            const std::string msg = "Error while reading the '" + segmentLabel + "' segment: " + error;

                            SLM_WARN_IF(msg, !m_logger);
                            if(m_logger)
                            {
                                m_logger->critical(msg);
                            }
                        };

    //===================
    // Read segmentations
    //===================
    std::size_t itemIndex = 1;
    for(const auto& segment : segmentContainer)
    {
        // We only handle segment containing one surface
        if (segment->GetSurfaceCount() != 1 || segment->GetSurfaces().size() != 1)
        {
            displayError(segment, "Unsupported surface count.");
            continue;
        }

        // Create the reconstruction
        ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

        // Retrieve the Segment Sequence Item
        const ::gdcm::Item& segmentItem = segmentSequence->GetItem(itemIndex++);

        // Get the associated surface of the current segmentation
        const auto& surfaceContainer                           = segment->GetSurfaces();
        const ::gdcm::SmartPointer< ::gdcm::Surface >& surface = surfaceContainer[0];

        try
        {
            // Read the Surface Segmentation Module - PS 3.3 C.8.23.1
            readSurfaceSegmentationModule(reconstruction, segment, segmentItem);

            // Read the Surface Mesh Module - PS 3.3 C.27.1
            readSurfaceMeshModule(reconstruction, surface);
        }
        catch(const ::fwGdcmIO::exception::Failed& exception)
        {
            displayError(segment, exception.what());
            continue;
        }

        // Add the reconstruction into the ModelSeries
        reconstructionDB.push_back(reconstruction);
    }

    if(reconstructionDB.empty())
    {
        throw ::fwGdcmIO::exception::Failed("Unable to read the Surface Segmentation: no segments have been found.");
    }

    // Update the reconstruction DB
    m_object->setReconstructionDB(reconstructionDB);

}

//------------------------------------------------------------------------------

std::string getStructureTypeFromSegmentIdentification(const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                                                      const ::fwGdcmIO::helper::SegmentedPropertyRegistry& registry)
{
    // Lambda used to format single entry
    const auto format = [&](const ::gdcm::SegmentHelper::BasicCodedEntry& entry) -> std::string
                        {
                            if(!entry.IsEmpty())
                            {
                                return "(" + ::gdcm::LOComp::Trim(entry.CV.c_str()) +
                                       ";" + ::gdcm::LOComp::Trim(entry.CSD.c_str()) +
                                       ";" + ::gdcm::LOComp::Trim(entry.CM.c_str()) + ")";
                            }

                            return "";
                        };

    // Lambda used to format multiple entries
    const auto formatVector = [&](const std::vector< ::gdcm::SegmentHelper::BasicCodedEntry >& entries) -> std::string
                              {
                                  std::string result;

                                  for(const auto& entry : entries)
                                  {
                                      result += format(entry);
                                  }

                                  return result;
                              };

    // Retrieve Structure Type from segment identification
    return registry.getStructureType(format(segment->GetPropertyType()),
                                     format(segment->GetPropertyCategory()),
                                     formatVector(segment->GetPropertyTypeModifiers()),
                                     format(segment->GetAnatomicRegion()),
                                     formatVector(segment->GetAnatomicRegionModifiers()));

}

//------------------------------------------------------------------------------

void Surface::readSurfaceSegmentationModule(const ::fwData::Reconstruction::sptr& reconstruction,
                                            const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                                            const ::gdcm::Item& segmentItem)
{
    // Retrieve segment's dataset
    const ::gdcm::DataSet& segmentDataset = segmentItem.GetNestedDataSet();

    // Organ Name - Segment Label (0x0062,0x0005) - Type 1
    std::string organName = segment->GetSegmentLabel();
    organName = ::gdcm::LOComp::Trim(organName.c_str());
    reconstruction->setOrganName(organName);

    // Structure Type from Private Tag (0x5649,0x1000)
    const ::gdcm::Tag structureTypeTag(0x5649, 0x1000);
    auto privateCreator = ::gdcm::LOComp::Trim(segmentDataset.GetPrivateCreator(structureTypeTag).c_str());
    if(segmentDataset.FindDataElement(structureTypeTag))
    {
        const auto structureType = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x5649, 0x1000 >(segmentDataset);
        reconstruction->setStructureType(structureType);
    }
    // Structure Type from Segment Identification
    else
    {
        const auto structureType = getStructureTypeFromSegmentIdentification(segment, m_segmentedPropertyRegistry);
        if(structureType.empty())
        {
            const std::string msg = "Unable to retrieve structure type from segment identification "
                                    "for segment '" + organName + "'.";

            SLM_WARN_IF(msg, !m_logger);
            if(m_logger)
            {
                m_logger->warning(msg);
            }
        }
    }

    // Computed Mask Volume (0x5649, 0x1001)
    const ::gdcm::Tag computedMaskVolumeTag(0x5649, 0x1001);
    privateCreator = ::gdcm::LOComp::Trim(segmentDataset.GetPrivateCreator(computedMaskVolumeTag).c_str());
    if(segmentDataset.FindDataElement(computedMaskVolumeTag))
    {
        ::gdcm::Attribute< 0x5649, 0x1001, ::gdcm::VR::OD, ::gdcm::VM::VM1 > attribute;
        attribute.SetFromDataSet(segmentDataset);
        const double volume = attribute.GetValue();
        reconstruction->setComputedMaskVolume(volume);
    }
    else
    {
        reconstruction->setComputedMaskVolume(::fwData::Reconstruction::s_NO_COMPUTED_MASK_VOLUME);
    }

}

//------------------------------------------------------------------------------

void Surface::readSurfaceMeshModule(const ::fwData::Reconstruction::sptr& reconstruction,
                                    const ::gdcm::SmartPointer< ::gdcm::Surface >& surface)
{
    // Create material
    ::fwData::Material::sptr material = fwData::Material::New();

    // Convert CIE Lab to RGBA
    const unsigned short* lab = surface->GetRecommendedDisplayCIELabValue();
    ::gdcm::SurfaceHelper::ColorArray CIELab(lab, lab + sizeof(lab) / sizeof(unsigned short));
    std::vector<float> colorVector = ::gdcm::SurfaceHelper::RecommendedDisplayCIELabToRGB(CIELab, 1);

    // Recommended Presentation Opacity
    colorVector.push_back(surface->GetRecommendedPresentationOpacity());

    // Adapt color to material
    ::fwData::Color::ColorArray rgba;
    ::boost::algorithm::clamp_range(colorVector.begin(), colorVector.end(), rgba.begin(), 0.f, 1.f);

    // Set reconstruction's visibility
    const double epsilon = 1e-3;
    reconstruction->setIsVisible(rgba[3] > epsilon);

    // Set reconstruction's color
    ::fwData::Color::sptr color = ::fwData::Color::New();
    color->setRGBA( rgba );
    material->setDiffuse(color);
    OSLM_TRACE("RGBA color : " << rgba[0]<<" "<< rgba[1]<<" "<< rgba[2]<<" "<< rgba[3]);

    // Recommended Presentation Type
    const auto representationMode =
        ::fwGdcmIO::helper::DicomDataTools::convertToRepresentationMode(surface->GetRecommendedPresentationType());
    material->setRepresentationMode(representationMode);

    // Manifold
    if (surface->GetManifold() == ::gdcm::Surface::YES)
    {
        throw ::fwGdcmIO::exception::Failed("Manifold meshes are not supported by the selected reader.");
    }

    // Mesh Primitive
    ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > meshPrimitive = surface->GetMeshPrimitive();
    if (meshPrimitive->GetPrimitiveType() != ::gdcm::MeshPrimitive::TRIANGLE)
    {
        throw ::fwGdcmIO::exception::Failed("The primitive type is not supported by the selected reader.");
    }

    // Point Coordinates Data
    const ::gdcm::ByteValue* pointCoordinates = surface->GetPointCoordinatesData().GetByteValue();
    if(!pointCoordinates || !pointCoordinates->GetPointer())
    {
        throw ::fwGdcmIO::exception::Failed("No point coordinates data found.");
    }

    // Compute number of coordinates
    const auto pointCoordinatesSize = pointCoordinates->GetLength() / sizeof(float);
    if((pointCoordinatesSize / 3) != surface->GetNumberOfSurfacePoints())
    {
        throw ::fwGdcmIO::exception::Failed("Point coordinates data are corrupted.");
    }

    // Surface Points Normals
    const ::gdcm::ByteValue* normalCoordinates = surface->GetVectorCoordinateData().GetByteValue();
    const char* normalCoordinatesPointer       = nullptr;
    if (!surface->GetVectorCoordinateData().IsEmpty())
    {
        // Check that the surface contains normals
        if(!normalCoordinates || !normalCoordinates->GetPointer())
        {
            throw ::fwGdcmIO::exception::Failed("No normal coordinates data found.");
        }

        normalCoordinatesPointer = normalCoordinates->GetPointer();

        // Compute number of normal coordinates
        const unsigned long normalCoordinateSize = normalCoordinates->GetLength() / sizeof(float);
        if ((normalCoordinateSize / 3) != surface->GetNumberOfVectors() || normalCoordinateSize != pointCoordinatesSize)
        {
            throw ::fwGdcmIO::exception::Failed("Normal coordinates data are corrupted.");
        }
    }

    // Triangle Point Index List
    const ::gdcm::ByteValue* pointIndices = meshPrimitive->GetPrimitiveData().GetByteValue();
    if (!pointIndices || !pointIndices->GetPointer())
    {
        throw ::fwGdcmIO::exception::Failed("No triangle point index list found.");
    }

    // Get number of primitives
    const unsigned long indexSize = pointIndices->GetLength() / sizeof(uint32_t);

    // Create a new Mesh
    ::fwGdcmIO::container::DicomSurface surfaceContainer(reinterpret_cast<const float*>(pointCoordinates->GetPointer()),
                                                         pointCoordinatesSize,
                                                         reinterpret_cast<const uint32_t*>(pointIndices->GetPointer()),
                                                         indexSize,
                                                         reinterpret_cast<const float*>(normalCoordinatesPointer));

    // Set the reconstruction
    reconstruction->setMaterial( material );
    reconstruction->setMesh( surfaceContainer.convertToData() );

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
