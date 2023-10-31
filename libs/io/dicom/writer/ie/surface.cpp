/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINT

#include "io/dicom/container/dicom_surface.hpp"
#include "io/dicom/writer/ie/surface.hpp"

#include "io/dicom/helper/dicom_coded_attribute.hpp"
#include "io/dicom/helper/dicom_data_tools.hpp"
#include "io/dicom/helper/dicom_data_writer.hxx"

#include <core/runtime/path.hpp>
#include <core/tools/date_and_time.hpp>

#include <data/reconstruction.hpp>
#include <data/series.hpp>
#include <data/types.hpp>

#include <geometry/data/mesh.hpp>

#include <io/__/reader/dictionary_reader.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <gdcmSurfaceHelper.h>
#include <gdcmSurfaceWriter.h>
#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

surface::surface(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    SPTR(io::dicom::container::dicom_instance)_image_instance,
    const data::model_series::csptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::ie::information_entity<data::model_series>(_writer, _instance, _series,
                                                                  _logger, std::move(_progress), std::move(_cancel)),
    m_image_instance(std::move(_image_instance))
{
    SIGHT_ASSERT("Image instance should not be null.", _image_instance);
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

void surface::write_sop_common_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0016>(m_instance->get_sop_class_uid(), dataset);

    // SOP Instance UID
    gdcm::UIDGenerator uid_generator;
    std::string sop_instance_uid = uid_generator.Generate();
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0018>(sop_instance_uid, dataset);
}

//------------------------------------------------------------------------------

void surface::write_surface_segmentation_and_surface_mesh_modules()
{
    // Retrieve Surface Writer
    SPTR(gdcm::SurfaceWriter) surface_writer = std::static_pointer_cast<gdcm::SurfaceWriter>(m_writer);

    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    //=======================================================
    // Table C.8.23-1. Surface Segmentation Module Attributes
    //=======================================================
    {
        //======================================================
        //  Table 10-12. Content Identification Macro Attributes
        //======================================================
        {
            // Instance Number - Type 1 - Only one instance per series
            io::dicom::helper::dicom_data_writer::set_tag_value<int, 0x0020, 0x0013>(1, dataset);

            // Content Label - Type 1
            io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0080>("SURFACE", dataset);

            // Content Description - Type 2
            io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0081>("Surface Segmentation", dataset);

            // Content Creator's Name - Type 2 - TODO: Set surface creator name ?
            io::dicom::helper::dicom_data_writer::set_empty_tag_value<0x0070, 0x0084>(dataset);
        }

        // Content Date - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0023>(m_object->get_series_date(), dataset);

        // Content Time - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0033>(m_object->get_series_time(), dataset);
    }

    //=============================================
    // Table C.27-1. Surface Mesh Module Attributes
    //=============================================
    {
        // Number of Surfaces - Type 1
        auto nb_surfaces = static_cast<unsigned int>(m_object->get_reconstruction_db().size());
        io::dicom::helper::dicom_data_writer::set_tag_value<unsigned int, 0x0066, 0x0001>(nb_surfaces, dataset);
    }

    // Segment Sequence (0x0062,0x0002) - Type 1
    // Segments will be added to this sequence thanks to the gdcm SegmentWriter
    auto segment_sequence = io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0062, 0x0002>(
        dataset
    );

    // Surface Sequence
    auto surface_sequence = io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0066, 0x0002>(
        dataset
    );

    //====================
    // Write segmentations
    //====================
    std::uint16_t segment_number  = 1;
    const auto& reconstruction_db = m_object->get_reconstruction_db();
    for(const auto& reconstruction : reconstruction_db)
    {
        // Add segmentation to GDCM Surface Writer
        gdcm::SmartPointer<gdcm::Segment> segment = new gdcm::Segment();
        surface_writer->AddSegment(segment);

        // Create Segment Sequence item
        gdcm::Item segment_item;
        segment_item.SetVLToUndefined();
        // Surface Segmentation Module - SegmentSequence
        write_segment_sequence(reconstruction, segment_item, segment, segment_number);
        segment_sequence->AddItem(segment_item);

        // Add a surface to the segmentation
        gdcm::SmartPointer<gdcm::Surface> surface = new gdcm::Surface();
        segment->AddSurface(surface);

        // Create Surface Sequence item
        gdcm::Item surface_item;
        surface_item.SetVLToUndefined();
        // Surface Mesh Module - Surface Sequence
        write_surface_sequence(reconstruction, surface_item, surface, segment_number);
        surface_sequence->AddItem(surface_item);

        // Next segment
        ++segment_number;

        if(m_logger)
        {
            m_logger->information("Surface '" + reconstruction->get_organ_name() + "' has been exported");
        }
    }
}

//------------------------------------------------------------------------------

void write_segment_identification(
    const std::string& _structure_type,
    const gdcm::SmartPointer<gdcm::Segment>& _segment,
    const io::dicom::helper::segmented_property_registry& _registry,
    core::log::logger::sptr _logger
)
{
    // Check that the structure name has an entry in the registry
    if(!_registry.has_entry(_structure_type))
    {
        const std::string msg = "No match in segmented property registry for structure '" + _structure_type + "'";
        SIGHT_WARN_IF(msg, !_logger);
        if(_logger)
        {
            _logger->warning(msg);
        }
    }

    // Note: No error or warning is emitted from this function regarding
    // multiplicity of coded attributes as checks are already performed in
    // registry.
    const auto entry = _registry.get_entry(_structure_type);

    //======================================================
    // Table 10-7. General Anatomy Optional Macro Attributes
    //======================================================
    {
        // Anatomic Region Sequence (0x0008,0x2218) - Type 3
        const std::string& anatomic_region = entry[3];
        if(!anatomic_region.empty())
        {
            const auto coded_attributes =
                io::dicom::helper::dicom_coded_attribute::convert_entry_to_gdcm_coded_attribute(anatomic_region);
            if(!coded_attributes.empty())
            {
                // Only a single Item shall be included in this Sequence
                _segment->SetAnatomicRegion(coded_attributes[0]);
            }
        }

        // Anatomic Region Modifier Sequence (0x0008,0x2220) - Type 3
        const std::string& anatomic_region_modifier = entry[4];
        if(!anatomic_region_modifier.empty())
        {
            const auto coded_attributes =
                io::dicom::helper::dicom_coded_attribute::convert_entry_to_gdcm_coded_attribute(anatomic_region_modifier);
            if(!coded_attributes.empty())
            {
                // One or more Items are permitted in this Sequence.
                _segment->SetAnatomicRegionModifiers(coded_attributes);
            }
        }
    }

    // Segmented Property Category Code Sequence (0x0062,0x0003) - Type 1
    {
        const std::string& property_category = entry[1];
        const auto coded_attributes          =
            io::dicom::helper::dicom_coded_attribute::convert_entry_to_gdcm_coded_attribute(property_category);
        if(!coded_attributes.empty())
        {
            // Only a single Item shall be included in this Sequence
            _segment->SetPropertyCategory(coded_attributes[0]);
        }
    }

    // Segmented Property Type Code Sequence (0x0062,0x000F) - Type 1
    {
        const std::string& property_type = entry[0];
        const auto coded_attributes      =
            io::dicom::helper::dicom_coded_attribute::convert_entry_to_gdcm_coded_attribute(property_type);
        if(!coded_attributes.empty())
        {
            // Only a single Item shall be included in this Sequence
            _segment->SetPropertyType(coded_attributes[0]);
        }
    }

    // Segmented Property Type Modifier Code Sequence (0x0062,0x0011) - Type 3
    {
        const std::string& property_type_modifier = entry[2];
        if(!property_type_modifier.empty())
        {
            const auto coded_attributes =
                io::dicom::helper::dicom_coded_attribute::convert_entry_to_gdcm_coded_attribute(property_type_modifier);
            if(!coded_attributes.empty())
            {
                // One or more Items are permitted in this Sequence.
                _segment->SetPropertyTypeModifiers(coded_attributes);
            }
        }
    }
}

//------------------------------------------------------------------------------

void write_private_tags(
    const data::reconstruction::csptr& _reconstruction,
    gdcm::DataSet& _dataset
)
{
    // Private group
    const auto reserved_group    = 0x5649;
    const auto reserving_element = 0x0010; // Reserve group (gggg,0x1000-0x10FF)

    // Reserve group
    {
        const auto* const private_creator = "Sight";
        gdcm::Attribute<reserved_group, reserving_element, gdcm::VR::LO, gdcm::VM::VM1> attribute;
        attribute.SetValue(private_creator);
        _dataset.Insert(attribute.GetAsDataElement());
    }

    // Structure type
    {
        const auto structure_type = _reconstruction->get_structure_type();
        gdcm::Attribute<reserved_group, 0x1000, gdcm::VR::LO, gdcm::VM::VM1> attribute;
        attribute.SetValue(structure_type);
        _dataset.Insert(attribute.GetAsDataElement());
    }

    // Computed mask volume
    const double volume = _reconstruction->get_computed_mask_volume();
    if(volume > 0)
    {
        gdcm::Attribute<reserved_group, 0x1001, gdcm::VR::OD, gdcm::VM::VM1> attribute {};
        attribute.SetValue(volume);
        _dataset.Insert(attribute.GetAsDataElement());
    }
}

//------------------------------------------------------------------------------

void surface::write_segment_sequence(
    const data::reconstruction::csptr& _reconstruction,
    gdcm::Item& _segment_item,
    const gdcm::SmartPointer<gdcm::Segment>& _segment,
    std::uint16_t _segment_number
)
{
    // Retrieve segment dataset
    gdcm::DataSet& segment_item_dataset = _segment_item.GetNestedDataSet();

    //=======================================================
    // Table C.8.20-4. 'Segment Description Macro Attributes'
    //=======================================================
    {
        // Segment Number (0x0062,0x0004) - Type 1 (start at 1)
        _segment->SetSegmentNumber(_segment_number);

        // Segment Label (0x0062,0x0005) - Type 1
        std::string segment_label = _reconstruction->get_organ_name();
        _segment->SetSegmentLabel(segment_label.c_str());

        // Segment Algorithm Type (0x0062,0x0008) - Type 1
        _segment->SetSegmentAlgorithmType(gdcm::Segment::SEMIAUTOMATIC);

        // Anatomic Region Sequence (0x0008,0x2218) - Type 2
        // Anatomic Region Modifier Sequence (0x0008,0x2220) - Type 3
        // Segmented Property Category Code Sequence (0x0062,0x0003) - Type 1
        // Segmented Property Type Code Sequence (0x0062,0x000F) - Type 1
        // Segmented Property Type Code Sequence (0x0062,0x0011) - Type 3
        const std::string structure_type = _reconstruction->get_structure_type();
        write_segment_identification(structure_type, _segment, m_segmented_property_registry, m_logger);
    }

    // Surface Count (0x0066,0x002A) - Type 1 - Reconstruction just contains one surface.
    _segment->SetSurfaceCount(1);

    // Referenced Surface Sequence - Type 1
    auto referenced_surface_sequence =
        io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0066, 0x002B>(segment_item_dataset);

    // Referenced Surface Sequence Item
    gdcm::Item ref_surface_seq_item;
    ref_surface_seq_item.SetVLToUndefined();
    // TODO

    gdcm::DataSet& ref_surface_seq_item_dataset = ref_surface_seq_item.GetNestedDataSet();
    {
        // Referenced Surface Number - Type 1
        io::dicom::helper::dicom_data_writer::set_tag_value<unsigned int, 0x0066, 0x002C>(
            _segment_number,
            ref_surface_seq_item_dataset
        );

        //=======================================================
        // Table 10-19. Algorithm Identification Macro Attributes
        //=======================================================
        // This section is handled in the surface section because of the GDCM soup

        // Segment Surface Source Instance Sequence - Type 2
        auto surface_source_instance_sequence =
            io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0066, 0x002E>(
                ref_surface_seq_item_dataset
            );

        //==========================================================
        // Table 10-3. Image SOP Instance Reference Macro Attributes
        //==========================================================
        {
            const std::vector<std::string>& referenced_sop_instance_uid_container =
                m_image_instance->get_sop_instance_uid_container();
            const std::string& referenced_sop_class_uid = m_image_instance->get_sop_class_uid();

            //=====================================================
            // Table 10-11. SOP Instance Reference Macro Attributes
            //=====================================================
            for(const std::string& sop_instance_uid : referenced_sop_instance_uid_container)
            {
                // Create the item
                gdcm::Item image_sop_item;
                image_sop_item.SetVLToUndefined();
                gdcm::DataSet& image_sop_dataset = image_sop_item.GetNestedDataSet();

                // Referenced SOP Class UID - Type 1
                io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1150>(
                    referenced_sop_class_uid,
                    image_sop_dataset
                );

                // Referenced SOP Instance UID - Type 1
                io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x1155>(
                    sop_instance_uid,
                    image_sop_dataset
                );

                surface_source_instance_sequence->AddItem(image_sop_item);
            }

            if(m_logger && referenced_sop_instance_uid_container.empty())
            {
                m_logger->warning("No DICOM instances have been added as 'Referenced SOP Instance UID'");
            }
        }
    }
    referenced_surface_sequence->AddItem(ref_surface_seq_item);

    // Private Tags
    write_private_tags(_reconstruction, segment_item_dataset);
}

//------------------------------------------------------------------------------

void surface::write_surface_sequence(
    const data::reconstruction::csptr& _reconstruction,
    gdcm::Item& _surface_item,
    const gdcm::SmartPointer<gdcm::Surface>& _surface,
    std::uint16_t _segment_number
)
{
    // Retrieve surface dataset
    gdcm::DataSet& surface_item_dataset = _surface_item.GetNestedDataSet();

    // Retrieve material
    data::material::csptr material = _reconstruction->get_material();

    // Set DicomSurface data - NOTE: must be called before points and primitives writing
    io::dicom::container::dicom_surface surface_container(_reconstruction);

    // Surface Number (0x0066,0x0003) - Type 1
    _surface->SetSurfaceNumber(_segment_number);

    // Surface Processing (0x0066,0x0009) - NO - Type 2
    _surface->SetSurfaceProcessing(false);

    // Get reconstruction's color
    const auto rgba = material->diffuse()->rgba();
    std::vector<float> rgb {rgba[0], rgba[1], rgba[2]};

    // Recommended Display Grayscale Value (0x0062,0x000C) - Type 1
    const auto grayscale = gdcm::SurfaceHelper::RGBToRecommendedDisplayGrayscale(rgb, 1.);
    _surface->SetRecommendedDisplayGrayscaleValue(grayscale);

    // Recommended Display CIELab Value (0x0062,0x000D) - Type 1
    const auto cie_lab = gdcm::SurfaceHelper::RGBToRecommendedDisplayCIELab(rgb, 1.);
    _surface->SetRecommendedDisplayCIELabValue(cie_lab);

    // Recommended Presentation Opacity (0x0066,0x000C) - Type 1
    _surface->SetRecommendedPresentationOpacity(rgba[3]);

    // Recommended Presentation Type (0x0066,0x000D) - Type 1
    _surface->SetRecommendedPresentationType(
        io::dicom::helper::dicom_data_tools::convert_to_presentation_type(material->get_representation_mode())
    );

    // Finite Volume (0x0066,0x000E) - Type 1
    _surface->SetFiniteVolume(
        geometry::data::mesh::is_closed(_reconstruction->get_mesh()) ? (gdcm::Surface::YES)
                                                                     : (gdcm::Surface::NO)
    );

    // Manifold (0x0066,0x0010) - Type 1
    _surface->SetManifold(gdcm::Surface::NO);

    // Surface Points Sequence (0x0066,0x0011) - Type 1
    auto points_sequence =
        io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0066, 0x0011>(surface_item_dataset);

    // Create one item
    gdcm::Item points_item;
    points_item.SetVLToUndefined();
    using ulong = unsigned long; // NOLINT(google-runtime-int)
    //======================================
    // Table C.27-2. Points Macro Attributes
    //======================================
    {
        // Number Of Surface Points (0x0066,0x0015) - Type 1
        _surface->SetNumberOfSurfacePoints(ulong(surface_container.get_point_buffer_size() / 3));

        // Point Coordinates Data (0x0066,0x0016) - Type 1
        gdcm::DataElement& point_coord_data = _surface->GetPointCoordinatesData();
        point_coord_data.SetByteValue(
            reinterpret_cast<const char*>(surface_container.get_point_buffer().data()),
            static_cast<uint32_t>(surface_container.get_point_buffer_size())
            * static_cast<uint32_t>(sizeof(float))
        );
    }
    points_sequence->AddItem(points_item);

    // Surface Points Normals Sequence (0x0066,0x0012) - Type 2
    auto normals_sequence =
        io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0066, 0x0012>(surface_item_dataset);

    //=======================================
    // Table C.27-3. Vectors Macro Attributes
    //=======================================
    if(surface_container.get_normal_buffer_size() > 0)
    {
        // Create one item
        gdcm::Item normals_item;
        normals_item.SetVLToUndefined();

        // Number Of Vectors (0x0066, 0x001e) - Type 1
        _surface->SetNumberOfVectors(ulong(surface_container.get_normal_buffer_size() / 3));

        // Vector Dimensionality (0x0066, 0x001f) - Type 1
        _surface->SetVectorDimensionality(1);

        // Vector Coordinate Data (0x0066,0x0021) - Type 1
        gdcm::DataElement& normal_coord_data = _surface->GetVectorCoordinateData();
        normal_coord_data.SetByteValue(
            reinterpret_cast<const char*>(surface_container.get_normal_buffer().data()),
            static_cast<uint32_t>(surface_container.get_normal_buffer_size())
            * static_cast<uint32_t>(sizeof(float))
        );

        normals_sequence->AddItem(normals_item);
    }

    // Surface Mesh Primitives Sequence (0x0066,0x0013) - Type 1
    auto primitives_sequence =
        io::dicom::helper::dicom_data_writer::create_and_set_sequence_tag_value<0x0066, 0x0013>(surface_item_dataset);

    //=======================================================
    // Table C.27-4. Surface Mesh Primitives Macro Attributes
    //=======================================================
    {
        // Mesh primitive type used by surface writer (fixed to TRIANGLE by Sight)
        gdcm::SmartPointer<gdcm::MeshPrimitive> primitive = _surface->GetMeshPrimitive();
        primitive->SetPrimitiveType(gdcm::MeshPrimitive::TRIANGLE);

        // Long Triangle Point Index List (0x0066,0x0041) - Type 2
        gdcm::DataElement& point_index_data = primitive->GetPrimitiveData();
        point_index_data.SetVL(sizeof(uint32_t));
        point_index_data.SetByteValue(
            reinterpret_cast<const char*>(surface_container.get_cell_buffer().data()),
            static_cast<uint32_t>(surface_container.get_cell_buffer_size())
            * static_cast<uint32_t>(sizeof(uint32_t))
        );
    }

    //=======================================================
    // Table 10-19. Algorithm Identification Macro Attributes
    //=======================================================
    {
        // Algorithm Family Code Sequence - Type 1
        _surface->SetAlgorithmFamily(gdcm::SegmentHelper::BasicCodedEntry("123109", "DCM", "Manual Processing"));

        // Algorithm Name (0x0066,0x0036) - Type 1
        _surface->SetAlgorithmName("Sight");

        // Algorithm Version (0x0066,0x0031) - Type 1
        _surface->SetAlgorithmVersion("1");
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
