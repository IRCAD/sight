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

#include "io/dicom/writer/ie/image.hpp"

#include "io/dicom/helper/dicom_data_tools.hpp"
#include "io/dicom/helper/dicom_data_writer.hxx"

#include <data/image.hpp>

#include <gdcmImageWriter.h>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

image::image(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::image::csptr& _image,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::ie::information_entity<data::image>(_writer, _instance, _image, _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

image::~image()
= default;

//------------------------------------------------------------------------------

void image::write_general_image_module()
{
    // Nothing to write
}

//------------------------------------------------------------------------------

void image::write_general_image_module_specific_tags(unsigned int _instance_number)
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Instance Number
    io::dicom::helper::dicom_data_writer::set_tag_value<int, 0x0020, 0x0013>(int(_instance_number), dataset);
}

//------------------------------------------------------------------------------

void image::write_image_plane_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Retrieve GDCM image
    SPTR(gdcm::ImageWriter) image_writer = std::static_pointer_cast<gdcm::ImageWriter>(m_writer);
    gdcm::Image& gdcm_image = image_writer->GetImage();

    // Pixel Spacing - Type 1
    // WARNING : some DICOM image have not any spacing (NOT SUPPORTED BY Sight), but stuff like "Pixel Aspect Ratio"
    const std::size_t dimension = m_object->num_dimensions();
    const auto& spacing         = m_object->spacing();
    for(unsigned int i = 0 ; i < dimension ; ++i)
    {
        gdcm_image.SetSpacing(i, spacing[i]);
    }

    // Slice Thickness - Type 2
    // NOTE: gdcm::ImageWriter may replace SliceThickness values according to image Origins
    io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x0050>(spacing[2], dataset);

    // image Orientation (Patient) - Type 1
    // This tag his handled by gdcm writer
}

//------------------------------------------------------------------------------

void image::write_image_plane_module_specific_tags(unsigned int _instance_number)
{
    // Retrieve GDCM image
    SPTR(gdcm::ImageWriter) image_writer = std::static_pointer_cast<gdcm::ImageWriter>(m_writer);
    gdcm::Image& gdcm_image = image_writer->GetImage();

    // image Position (Patient) - Type 1
    const auto& origin  = m_object->origin();
    const auto& spacing = m_object->spacing();
    gdcm_image.SetOrigin(0, origin[0]);
    gdcm_image.SetOrigin(1, origin[1]);
    gdcm_image.SetOrigin(2, origin[2] + spacing[2] * _instance_number);
}

//------------------------------------------------------------------------------

void image::write_image_pixel_module()
{
    // Retrieve GDCM image
    gdcm::ImageWriter* image_writer = std::static_pointer_cast<gdcm::ImageWriter>(m_writer).get();
    gdcm::Image& gdcm_image         = image_writer->GetImage();

    // image's photometric interpretation - Type 1
    gdcm::PhotometricInterpretation photo_inter =
        io::dicom::helper::dicom_data_tools::get_photometric_interpretation(m_object);
    gdcm_image.SetPhotometricInterpretation(photo_inter);

    // image's pixel type
    gdcm::PixelFormat pixel_format = io::dicom::helper::dicom_data_tools::get_pixel_type(m_object->type());
    gdcm_image.SetPixelFormat(pixel_format);

    //image's number of dimension
    auto dimension =
        static_cast<unsigned int>((m_instance->get_is_multi_files()) ? 2 : m_object->num_dimensions());
    gdcm_image.SetNumberOfDimensions(dimension);

    // image's dimension
    const data::image::size_t& size = m_object->size();
    for(unsigned int i = 0 ; i < dimension ; ++i)
    {
        gdcm_image.SetDimension(i, static_cast<unsigned int>(size[i]));
    }
}

//------------------------------------------------------------------------------

void image::write_image_pixel_module_specific_tags(unsigned int _instance_number)
{
    SIGHT_ASSERT("Wrong instance number.", m_instance->get_is_multi_files() || _instance_number == 0);

    // Retrieve GDCM image
    gdcm::ImageWriter* image_writer = std::static_pointer_cast<gdcm::ImageWriter>(m_writer).get();
    gdcm::Image& gdcm_image         = image_writer->GetImage();

    // Compute buffer size
    const data::image::size_t& size = m_object->size();
    std::size_t buffer_length       = size[0] * size[1] * gdcm_image.GetPixelFormat().GetPixelSize();
    buffer_length = (!m_instance->get_is_multi_files()) ? (buffer_length * size[2]) : buffer_length;

    // Retrieve image buffer
    const auto dump_lock     = m_object->dump_lock();
    const char* image_buffer = static_cast<const char*>(m_object->buffer());

    // Pixel Data - Type 1C
    gdcm::DataElement pixeldata(gdcm::Tag(0x7fe0, 0x0010));
    pixeldata.SetByteValue(image_buffer + _instance_number * buffer_length, static_cast<unsigned int>(buffer_length));
    gdcm_image.SetDataElement(pixeldata);
}

//------------------------------------------------------------------------------

void image::write_voilut_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    const auto& window_centers = m_object->window_center();
    const auto& window_widths  = m_object->window_width();
    if(!window_centers.empty() || !window_widths.empty())
    {
        // image's windows center
        io::dicom::helper::dicom_data_writer::set_tag_values<double, 0x0028, 0x1050>(
            window_centers.data(),
            window_centers.size(),
            dataset
        );

        // image's windows width
        io::dicom::helper::dicom_data_writer::set_tag_values<double, 0x0028, 0x1051>(
            window_widths.data(),
            window_widths.size(),
            dataset
        );
    }
}

//------------------------------------------------------------------------------

void image::write_sop_common_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0016>(m_instance->get_sop_class_uid(), dataset);
}

//------------------------------------------------------------------------------

void image::write_sop_common_module_specific_tags(unsigned int _instance_number)
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Instance UID
    const std::string sop_instance_uid = m_instance->get_sop_instance_uid_container()[_instance_number];
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0008, 0x0018>(sop_instance_uid, dataset);
}

//------------------------------------------------------------------------------

void image::write_ct_image_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // image Type - Type 1 - FIXME: Fake Value
    const gdcm::String<92, 16> image_type = "ORIGINAL\\PRIMARY\\AXIAL";
    io::dicom::helper::dicom_data_writer::set_tag_values<gdcm::String<92, 16>, 0x0008, 0x0008>(
        &image_type,
        1,
        dataset
    );

    // Acquisition Number - Type 2 - FIXME: Fake Value
    unsigned int acquisition_number = 1;
    io::dicom::helper::dicom_data_writer::set_tag_value<unsigned int, 0x0020, 0x0012>(acquisition_number, dataset);

    // KVP - Type 2 - FIXME: Fake Value
    io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x0060>(1, dataset);

    // Rescale Type - Type 1C - FIXME: Fake Value
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0028, 0x1054>("HU", dataset);
}

//------------------------------------------------------------------------------

void image::write_mr_image_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // image Type - Type 1 - FIXME: Fake Value
    const gdcm::String<92, 16> image_type = "ORIGINAL\\PRIMARY\\MPR";
    io::dicom::helper::dicom_data_writer::set_tag_values<gdcm::String<92, 16>, 0x0008, 0x0008>(
        &image_type,
        1,
        dataset
    );

    // Scanning Sequence - Type 1 - FIXME: Fake Value
    const gdcm::String<92, 16> scanning_sequence = "SE";
    io::dicom::helper::dicom_data_writer::set_tag_values<gdcm::String<92, 16>, 0x0018, 0x0020>(
        &scanning_sequence,
        1,
        dataset
    );

    // Sequence Variant - Type 1 - FIXME: Fake Value
    const gdcm::String<92, 16> sequence_variant = "NONE";
    io::dicom::helper::dicom_data_writer::set_tag_values<gdcm::String<92, 16>, 0x0018, 0x0021>(
        &sequence_variant,
        1,
        dataset
    );

    // Scan Options - Type 2 - FIXME: Fake Value
    const gdcm::String<92, 16> scan_option = "";
    io::dicom::helper::dicom_data_writer::set_tag_values<gdcm::String<92, 16>, 0x0018, 0x0022>(
        &scan_option,
        1,
        dataset
    );

    // MR Acquisition Type - Type 2 - FIXME: Fake Value
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0018, 0x0023>("3D", dataset);

    // Repetition Time - Type 2C - FIXME: Fake Value
    io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x0080>(0, dataset);

    // Echo Time - Type 2 - FIXME: Fake Value
    io::dicom::helper::dicom_data_writer::set_tag_value<double, 0x0018, 0x0081>(1, dataset);

    // Echo Train Length - Type 2 - FIXME: Fake Value
    io::dicom::helper::dicom_data_writer::set_tag_value<unsigned int, 0x0018, 0x0091>(1, dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
