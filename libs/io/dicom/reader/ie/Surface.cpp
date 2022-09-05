/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/container/DicomSurface.hpp"
#include "io/dicom/reader/ie/Surface.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/Color.hpp>
#include <data/Reconstruction.hpp>

#include <io/base/reader/DictionaryReader.hpp>

#include <data/DicomSeries.hpp>

#include <boost/algorithm/clamp.hpp>

// Removes RGB macro defined in windows.h
// to avoid conflicts in gdcmSurfaceHelper.h
#undef RGB
#include <gdcmSurfaceHelper.h>

#include <sstream>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

Surface::Surface(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::ModelSeries::sptr& series,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::ModelSeries>(dicomSeries, reader, instance, series,
                                                                logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Surface::~Surface()
= default;

//------------------------------------------------------------------------------

bool Surface::loadSegmentedPropertyRegistry(const std::filesystem::path& filepath)
{
    return m_segmentedPropertyRegistry.readSegmentedPropertyRegistryFile(filepath, true, m_logger);
}

//------------------------------------------------------------------------------

void Surface::readSurfaceSegmentationAndSurfaceMeshModules()
{
    // Retrieve Surface Reader
    SPTR(gdcm::SurfaceReader) surfaceReader = std::static_pointer_cast<gdcm::SurfaceReader>(m_reader);

    // Retrieve dataset
    const gdcm::DataSet& dataset = surfaceReader->GetFile().GetDataSet();

    // Segment Sequence (0x0062,0x0002) - Type 1
    const gdcm::DataElement& sequenceDataElement = dataset.GetDataElement(gdcm::Tag(0x0062, 0x0002));
    const auto segmentSequence                   = sequenceDataElement.GetValueAsSQ();

    // Segment container
    const auto& segmentContainer = surfaceReader->GetSegments();

    // Retrieve reconstruction DB
    data::ModelSeries::ReconstructionVectorType reconstructionDB = m_object->getReconstructionDB();

    // Lambda used to display reading errors
    auto displayError = [&](const gdcm::SmartPointer<gdcm::Segment>& segment,
                            const std::string& error)
                        {
                            std::string segmentLabel = segment->GetSegmentLabel();
                            segmentLabel = gdcm::LOComp::Trim(segmentLabel.c_str());
                            const std::string msg =
                                std::string("Error while reading the '").append(segmentLabel).append("' segment: ").
                                append(error);

                            SIGHT_WARN_IF(msg, !m_logger);
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
        if(segment->GetSurfaceCount() != 1 || segment->GetSurfaces().size() != 1)
        {
            displayError(segment, "Unsupported surface count.");
            continue;
        }

        // Create the reconstruction
        data::Reconstruction::sptr reconstruction = data::Reconstruction::New();

        // Retrieve the Segment Sequence Item
        const gdcm::Item& segmentItem = segmentSequence->GetItem(itemIndex++);

        // Get the associated surface of the current segmentation
        const auto& surfaceContainer                     = segment->GetSurfaces();
        const gdcm::SmartPointer<gdcm::Surface>& surface = surfaceContainer[0];

        try
        {
            // Read the Surface Segmentation Module - PS 3.3 C.8.23.1
            readSurfaceSegmentationModule(reconstruction, segment, segmentItem);

            // Read the Surface Mesh Module - PS 3.3 C.27.1
            readSurfaceMeshModule(reconstruction, surface);
        }
        catch(const io::dicom::exception::Failed& exception)
        {
            displayError(segment, exception.what());
            continue;
        }

        // Add the reconstruction into the ModelSeries
        reconstructionDB.push_back(reconstruction);
    }

    if(reconstructionDB.empty())
    {
        throw io::dicom::exception::Failed("Unable to read the Surface Segmentation: no segments have been found.");
    }

    // Update the reconstruction DB
    m_object->setReconstructionDB(reconstructionDB);
}

//------------------------------------------------------------------------------

std::string getStructureTypeFromSegmentIdentification(
    const gdcm::SmartPointer<gdcm::Segment>& segment,
    const io::dicom::helper::SegmentedPropertyRegistry& registry
)
{
    // Lambda used to format single entry
    const auto format = [&](const gdcm::SegmentHelper::BasicCodedEntry& entry) -> std::string
                        {
                            if(!entry.IsEmpty())
                            {
                                return "(" + gdcm::LOComp::Trim(entry.CV.c_str())
                                       + ";" + gdcm::LOComp::Trim(entry.CSD.c_str())
                                       + ";" + gdcm::LOComp::Trim(entry.CM.c_str()) + ")";
                            }

                            return "";
                        };

    // Lambda used to format multiple entries
    const auto formatVector = [&](const std::vector<gdcm::SegmentHelper::BasicCodedEntry>& entries) -> std::string
                              {
                                  std::string result;

                                  for(const auto& entry : entries)
                                  {
                                      result += format(entry);
                                  }

                                  return result;
                              };

    // Retrieve Structure Type from segment identification
    return registry.getStructureType(
        format(segment->GetPropertyType()),
        format(segment->GetPropertyCategory()),
        formatVector(segment->GetPropertyTypeModifiers()),
        format(segment->GetAnatomicRegion()),
        formatVector(segment->GetAnatomicRegionModifiers())
    );
}

//------------------------------------------------------------------------------

void Surface::readSurfaceSegmentationModule(
    const data::Reconstruction::sptr& reconstruction,
    const gdcm::SmartPointer<gdcm::Segment>& segment,
    const gdcm::Item& segmentItem
)
{
    // Retrieve segment's dataset
    const gdcm::DataSet& segmentDataset = segmentItem.GetNestedDataSet();

    // Organ Name - Segment Label (0x0062,0x0005) - Type 1
    std::string organName = segment->GetSegmentLabel();
    organName = gdcm::LOComp::Trim(organName.c_str());
    reconstruction->setOrganName(organName);

    // Structure Type from Private Tag (0x5649,0x1000)
    const gdcm::Tag structureTypeTag(0x5649, 0x1000);
    auto privateCreator = gdcm::LOComp::Trim(segmentDataset.GetPrivateCreator(structureTypeTag).c_str());
    if(segmentDataset.FindDataElement(structureTypeTag))
    {
        const auto structureType = io::dicom::helper::DicomDataReader::getTagValue<0x5649, 0x1000>(segmentDataset);
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

            SIGHT_WARN_IF(msg, !m_logger);
            if(m_logger)
            {
                m_logger->warning(msg);
            }
        }
    }

    // Computed Mask Volume (0x5649, 0x1001)
    const gdcm::Tag computedMaskVolumeTag(0x5649, 0x1001);
    privateCreator = gdcm::LOComp::Trim(segmentDataset.GetPrivateCreator(computedMaskVolumeTag).c_str());
    if(segmentDataset.FindDataElement(computedMaskVolumeTag))
    {
        gdcm::Attribute<0x5649, 0x1001, gdcm::VR::OD, gdcm::VM::VM1> attribute {};
        attribute.SetFromDataSet(segmentDataset);
        const double volume = attribute.GetValue();
        reconstruction->setComputedMaskVolume(volume);
    }
    else
    {
        reconstruction->setComputedMaskVolume(data::Reconstruction::s_NO_COMPUTED_MASK_VOLUME);
    }
}

//------------------------------------------------------------------------------

void Surface::readSurfaceMeshModule(
    const data::Reconstruction::sptr& reconstruction,
    const gdcm::SmartPointer<gdcm::Surface>& surface
)
{
    // Create material
    data::Material::sptr material = data::Material::New();

    // Convert CIE Lab to RGBA
    const std::uint16_t* lab = surface->GetRecommendedDisplayCIELabValue();
    gdcm::SurfaceHelper::ColorArray CIELab(lab, lab + 3);
    std::vector<float> colorVector = gdcm::SurfaceHelper::RecommendedDisplayCIELabToRGB(CIELab, 1);

    // Recommended Presentation Opacity
    colorVector.push_back(surface->GetRecommendedPresentationOpacity());

    // Adapt color to material
    data::Color::ColorArray rgba;
    boost::algorithm::clamp_range(colorVector.begin(), colorVector.end(), rgba.begin(), 0.F, 1.F);

    // Set reconstruction's visibility
    const double epsilon = 1e-3;
    reconstruction->setIsVisible(rgba[3] > epsilon);

    // Set reconstruction's color
    data::Color::sptr color = data::Color::New();
    color->setRGBA(rgba);
    material->setDiffuse(color);

    // Recommended Presentation Type
    const auto representationMode =
        io::dicom::helper::DicomDataTools::convertToRepresentationMode(surface->GetRecommendedPresentationType());
    material->setRepresentationMode(representationMode);

    // Manifold
    if(surface->GetManifold() == gdcm::Surface::YES)
    {
        throw io::dicom::exception::Failed("Manifold meshes are not supported by the selected reader.");
    }

    // Mesh Primitive
    gdcm::SmartPointer<gdcm::MeshPrimitive> meshPrimitive = surface->GetMeshPrimitive();
    if(meshPrimitive->GetPrimitiveType() != gdcm::MeshPrimitive::TRIANGLE)
    {
        throw io::dicom::exception::Failed("The primitive type is not supported by the selected reader.");
    }

    // Point Coordinates Data
    const gdcm::ByteValue* pointCoordinates = surface->GetPointCoordinatesData().GetByteValue();
    if((pointCoordinates == nullptr) || (pointCoordinates->GetPointer() == nullptr))
    {
        throw io::dicom::exception::Failed("No point coordinates data found.");
    }

    // Compute number of coordinates
    const auto pointCoordinatesSize = pointCoordinates->GetLength() / sizeof(float);
    if((pointCoordinatesSize / 3) != surface->GetNumberOfSurfacePoints())
    {
        throw io::dicom::exception::Failed("Point coordinates data are corrupted.");
    }

    // Surface Points Normals
    const gdcm::ByteValue* normalCoordinates = surface->GetVectorCoordinateData().GetByteValue();
    const char* normalCoordinatesPointer     = nullptr;
    if(!surface->GetVectorCoordinateData().IsEmpty())
    {
        // Check that the surface contains normals
        if((normalCoordinates == nullptr) || (normalCoordinates->GetPointer() == nullptr))
        {
            throw io::dicom::exception::Failed("No normal coordinates data found.");
        }

        normalCoordinatesPointer = normalCoordinates->GetPointer();

        // Compute number of normal coordinates
        const std::uint64_t normalCoordinateSize = normalCoordinates->GetLength() / sizeof(float);
        if((normalCoordinateSize / 3) != surface->GetNumberOfVectors() || normalCoordinateSize != pointCoordinatesSize)
        {
            throw io::dicom::exception::Failed("Normal coordinates data are corrupted.");
        }
    }

    // Triangle Point Index List
    const gdcm::ByteValue* pointIndices = meshPrimitive->GetPrimitiveData().GetByteValue();
    if((pointIndices == nullptr) || (pointIndices->GetPointer() == nullptr))
    {
        throw io::dicom::exception::Failed("No triangle point index list found.");
    }

    // Get number of primitives
    const std::uint64_t indexSize = pointIndices->GetLength() / sizeof(uint32_t);

    // Create a new Mesh
    io::dicom::container::DicomSurface surfaceContainer(reinterpret_cast<const float*>(pointCoordinates->GetPointer()),
                                                        data::Mesh::size_t(pointCoordinatesSize),
                                                        reinterpret_cast<const uint32_t*>(pointIndices->GetPointer()),
                                                        data::Mesh::size_t(indexSize),
                                                        reinterpret_cast<const float*>(normalCoordinatesPointer));

    // Set the reconstruction
    reconstruction->setMaterial(material);
    reconstruction->setMesh(surfaceContainer.convertToData());
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
