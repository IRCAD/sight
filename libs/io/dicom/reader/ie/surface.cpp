/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/container/dicom_surface.hpp"
#include "io/dicom/reader/ie/surface.hpp"

#include "io/dicom/helper/dicom_data_reader.hxx"
#include "io/dicom/helper/dicom_data_tools.hpp"

#include <data/color.hpp>
#include <data/reconstruction.hpp>

#include <io/__/reader/dictionary_reader.hpp>

#include <data/dicom_series.hpp>

#include <boost/algorithm/clamp.hpp>

// Removes RGB macro defined in windows.h
// to avoid conflicts in gdcmSurfaceHelper.h
#undef RGB
#include <gdcmSurfaceHelper.h>

#include <sstream>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

surface::surface(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::model_series::sptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::model_series>(_dicom_series, _reader, _instance, _series,
                                                                  _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

surface::~surface()
= default;

//------------------------------------------------------------------------------

bool surface::load_segmented_property_registry(const std::filesystem::path& _filepath)
{
    return m_segmented_property_registry.read_segmented_property_registry_file(_filepath, true, m_logger);
}

//------------------------------------------------------------------------------

void surface::read_surface_segmentation_and_surface_mesh_modules()
{
    // Retrieve Surface Reader
    SPTR(gdcm::SurfaceReader) surface_reader = std::static_pointer_cast<gdcm::SurfaceReader>(m_reader);

    // Retrieve dataset
    const gdcm::DataSet& dataset = surface_reader->GetFile().GetDataSet();

    // Segment Sequence (0x0062,0x0002) - Type 1
    const gdcm::DataElement& sequence_data_element = dataset.GetDataElement(gdcm::Tag(0x0062, 0x0002));
    const auto segment_sequence                    = sequence_data_element.GetValueAsSQ();

    // Segment container
    const auto& segment_container = surface_reader->GetSegments();

    // Retrieve reconstruction DB
    data::model_series::reconstruction_vector_t reconstruction_db = m_object->get_reconstruction_db();

    // Lambda used to display reading errors
    auto display_error = [&](const gdcm::SmartPointer<gdcm::Segment>& _segment,
                             const std::string& _error)
                         {
                             std::string segment_label = _segment->GetSegmentLabel();
                             segment_label = gdcm::LOComp::Trim(segment_label.c_str());
                             const std::string msg = "Error while reading the '" + segment_label + "' segment: "
                                                     + _error;

                             SIGHT_WARN_IF(msg, !m_logger);
                             if(m_logger)
                             {
                                 m_logger->critical(msg);
                             }
                         };

    //===================
    // Read segmentations
    //===================
    std::size_t item_index = 1;
    for(const auto& segment : segment_container)
    {
        // We only handle segment containing one surface
        if(segment->GetSurfaceCount() != 1 || segment->GetSurfaces().size() != 1)
        {
            display_error(segment, "Unsupported surface count.");
            continue;
        }

        // Create the reconstruction
        data::reconstruction::sptr reconstruction = std::make_shared<data::reconstruction>();

        // Retrieve the Segment Sequence Item
        const gdcm::Item& segment_item = segment_sequence->GetItem(item_index++);

        // Get the associated surface of the current segmentation
        const auto& surface_container                    = segment->GetSurfaces();
        const gdcm::SmartPointer<gdcm::Surface>& surface = surface_container[0];

        try
        {
            // Read the Surface Segmentation Module - PS 3.3 C.8.23.1
            read_surface_segmentation_module(reconstruction, segment, segment_item);

            // Read the Surface Mesh Module - PS 3.3 C.27.1
            read_surface_mesh_module(reconstruction, surface);
        }
        catch(const io::dicom::exception::failed& exception)
        {
            display_error(segment, exception.what());
            continue;
        }

        // Add the reconstruction into the ModelSeries
        reconstruction_db.push_back(reconstruction);
    }

    if(reconstruction_db.empty())
    {
        throw io::dicom::exception::failed("Unable to read the Surface Segmentation: no segments have been found.");
    }

    // Update the reconstruction DB
    m_object->set_reconstruction_db(reconstruction_db);
}

//------------------------------------------------------------------------------

std::string get_structure_type_from_segment_identification(
    const gdcm::SmartPointer<gdcm::Segment>& _segment,
    const io::dicom::helper::segmented_property_registry& _registry
)
{
    // Lambda used to format single entry
    const auto format = [&](const gdcm::SegmentHelper::BasicCodedEntry& _entry) -> std::string
                        {
                            if(!_entry.IsEmpty())
                            {
                                return "(" + gdcm::LOComp::Trim(_entry.CV.c_str())
                                       + ";" + gdcm::LOComp::Trim(_entry.CSD.c_str())
                                       + ";" + gdcm::LOComp::Trim(_entry.CM.c_str()) + ")";
                            }

                            return "";
                        };

    // Lambda used to format multiple entries
    const auto format_vector = [&](const std::vector<gdcm::SegmentHelper::BasicCodedEntry>& _entries) -> std::string
                               {
                                   std::string result;

                                   for(const auto& entry : _entries)
                                   {
                                       result += format(entry);
                                   }

                                   return result;
                               };

    // Retrieve Structure Type from segment identification
    return _registry.get_structure_type(
        format(_segment->GetPropertyType()),
        format(_segment->GetPropertyCategory()),
        format_vector(_segment->GetPropertyTypeModifiers()),
        format(_segment->GetAnatomicRegion()),
        format_vector(_segment->GetAnatomicRegionModifiers())
    );
}

//------------------------------------------------------------------------------

void surface::read_surface_segmentation_module(
    const data::reconstruction::sptr& _reconstruction,
    const gdcm::SmartPointer<gdcm::Segment>& _segment,
    const gdcm::Item& _segment_item
)
{
    // Retrieve segment's dataset
    const gdcm::DataSet& segment_dataset = _segment_item.GetNestedDataSet();

    // Organ Name - Segment Label (0x0062,0x0005) - Type 1
    std::string organ_name = _segment->GetSegmentLabel();
    organ_name = gdcm::LOComp::Trim(organ_name.c_str());
    _reconstruction->set_organ_name(organ_name);

    // Structure Type from Private Tag (0x5649,0x1000)
    const gdcm::Tag structure_type_tag(0x5649, 0x1000);
    if(segment_dataset.FindDataElement(structure_type_tag))
    {
        const auto structure_type =
            io::dicom::helper::dicom_data_reader::get_tag_value<0x5649, 0x1000>(segment_dataset);
        _reconstruction->set_structure_type(structure_type);
    }
    // Structure Type from Segment Identification
    else
    {
        const auto structure_type =
            get_structure_type_from_segment_identification(_segment, m_segmented_property_registry);
        if(structure_type.empty())
        {
            const std::string msg = "Unable to retrieve structure type from segment identification "
                                    "for segment '" + organ_name + "'.";

            SIGHT_WARN_IF(msg, !m_logger);
            if(m_logger)
            {
                m_logger->warning(msg);
            }
        }
    }

    // Computed Mask Volume (0x5649, 0x1001)
    const gdcm::Tag computed_mask_volume_tag(0x5649, 0x1001);
    if(segment_dataset.FindDataElement(computed_mask_volume_tag))
    {
        gdcm::Attribute<0x5649, 0x1001, gdcm::VR::OD, gdcm::VM::VM1> attribute {};
        attribute.SetFromDataSet(segment_dataset);
        const double volume = attribute.GetValue();
        _reconstruction->set_computed_mask_volume(volume);
    }
    else
    {
        _reconstruction->set_computed_mask_volume(data::reconstruction::NO_COMPUTED_MASK_VOLUME);
    }
}

//------------------------------------------------------------------------------

void surface::read_surface_mesh_module(
    const data::reconstruction::sptr& _reconstruction,
    const gdcm::SmartPointer<gdcm::Surface>& _surface
)
{
    // Create material
    data::material::sptr material = std::make_shared<data::material>();

    // Convert CIE Lab to RGBA
    const std::uint16_t* lab = _surface->GetRecommendedDisplayCIELabValue();
    gdcm::SurfaceHelper::ColorArray cie_lab(lab, lab + 3);
    std::vector<float> color_vector = gdcm::SurfaceHelper::RecommendedDisplayCIELabToRGB(cie_lab, 1);

    // Recommended Presentation Opacity
    color_vector.push_back(_surface->GetRecommendedPresentationOpacity());

    // Adapt color to material
    data::color::color_array_t rgba;
    boost::algorithm::clamp_range(color_vector.begin(), color_vector.end(), rgba.begin(), 0.F, 1.F);

    // Set reconstruction's visibility
    const double epsilon = 1e-3;
    _reconstruction->set_is_visible(rgba[3] > epsilon);

    // Set reconstruction's color
    data::color::sptr color = std::make_shared<data::color>();
    color->set_rgba(rgba);
    material->set_diffuse(color);

    // Recommended Presentation Type
    const auto representation_mode =
        io::dicom::helper::dicom_data_tools::convert_to_representation_mode(_surface->GetRecommendedPresentationType());
    material->set_representation_mode(representation_mode);

    // Manifold
    if(_surface->GetManifold() == gdcm::Surface::YES)
    {
        throw io::dicom::exception::failed("Manifold meshes are not supported by the selected reader.");
    }

    // Mesh Primitive
    gdcm::SmartPointer<gdcm::MeshPrimitive> mesh_primitive = _surface->GetMeshPrimitive();
    if(mesh_primitive->GetPrimitiveType() != gdcm::MeshPrimitive::TRIANGLE)
    {
        throw io::dicom::exception::failed("The primitive type is not supported by the selected reader.");
    }

    // Point Coordinates Data
    const gdcm::ByteValue* point_coordinates = _surface->GetPointCoordinatesData().GetByteValue();
    if((point_coordinates == nullptr) || (point_coordinates->GetPointer() == nullptr))
    {
        throw io::dicom::exception::failed("No point coordinates data found.");
    }

    // Compute number of coordinates
    const auto point_coordinates_size = point_coordinates->GetLength() / sizeof(float);
    if((point_coordinates_size / 3) != _surface->GetNumberOfSurfacePoints())
    {
        throw io::dicom::exception::failed("Point coordinates data are corrupted.");
    }

    // Surface Points Normals
    const gdcm::ByteValue* normal_coordinates = _surface->GetVectorCoordinateData().GetByteValue();
    if(!_surface->GetVectorCoordinateData().IsEmpty())
    {
        // Check that the surface contains normals
        if((normal_coordinates == nullptr) || (normal_coordinates->GetPointer() == nullptr))
        {
            throw io::dicom::exception::failed("No normal coordinates data found.");
        }

        // Compute number of normal coordinates
        const std::uint64_t normal_coordinate_size = normal_coordinates->GetLength() / sizeof(float);
        if((normal_coordinate_size / 3) != _surface->GetNumberOfVectors()
           || normal_coordinate_size != point_coordinates_size)
        {
            throw io::dicom::exception::failed("Normal coordinates data are corrupted.");
        }
    }

    // Triangle Point Index List
    const gdcm::ByteValue* point_indices = mesh_primitive->GetPrimitiveData().GetByteValue();
    if((point_indices == nullptr) || (point_indices->GetPointer() == nullptr))
    {
        throw io::dicom::exception::failed("No triangle point index list found.");
    }

    // Get number of primitives
    const std::uint64_t index_size = point_indices->GetLength() / sizeof(uint32_t);

    // Create a new Mesh
    io::dicom::container::dicom_surface surface_container(
        reinterpret_cast<const float*>(point_coordinates->GetPointer()),
        data::mesh::size_t(point_coordinates_size),
        reinterpret_cast<const uint32_t*>(point_indices->GetPointer()),
        data::mesh::size_t(index_size),
        _surface->GetVectorCoordinateData().IsEmpty() ? nullptr
                                                      :
        reinterpret_cast<
            const float*>(normal_coordinates->GetPointer()));

    // Set the reconstruction
    _reconstruction->set_material(material);
    _reconstruction->set_mesh(surface_container.convert_to_data());
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
