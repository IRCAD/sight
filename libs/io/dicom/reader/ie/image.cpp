/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "io/dicom/reader/ie/image.hpp"

#include "io/dicom/exception/failed.hpp"
#include "io/dicom/helper/dicom_data_reader.hxx"
#include "io/dicom/helper/dicom_data_tools.hpp"

#include <data/dicom/image.hpp>
#include <data/dicom_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <gdcmImageApplyLookupTable.h>
#include <gdcmImageHelper.h>
#include <gdcmImageReader.h>
#include <gdcmIPPSorter.h>
#include <gdcmPixelFormat.h>
#include <gdcmRescaler.h>
#include <gdcmUIDGenerator.h>

#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

image::image(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::image::sptr& _image,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::image>(_dicom_series, _reader, _instance, _image,
                                                           _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

image::~image()
= default;

//------------------------------------------------------------------------------

double get_instance_z_position(const core::memory::buffer_object::sptr& _buffer_obj)
{
    gdcm::ImageReader reader;
    const core::memory::buffer_manager::stream_info stream_info = _buffer_obj->get_stream_info();
    SPTR(std::istream) is = stream_info.stream;
    reader.SetStream(*is);

    if(!reader.Read())
    {
        return 0;
    }

    // Retrieve dataset
    const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

    // Check tags availability
    if(!dataset.FindDataElement(gdcm::Tag(0x0020, 0x0032)) || !dataset.FindDataElement(gdcm::Tag(0x0020, 0x0037)))
    {
        const std::string msg = "Unable to compute the spacing between slices of the series.";
        throw io::dicom::exception::failed(msg);
    }

    // Retrieve image position
    const gdcm::Image& gdcm_image   = reader.GetImage();
    const double* gdcm_origin       = gdcm_image.GetOrigin();
    const glm::dvec3 image_position = {gdcm_origin[0], gdcm_origin[1], gdcm_origin[2]};

    // Retrieve image orientation
    const double* direction_cosines      = gdcm_image.GetDirectionCosines();
    const glm::dvec3 image_orientation_u = {
        std::round(direction_cosines[0]),
        std::round(direction_cosines[1]),
        std::round(direction_cosines[2])
    };
    const glm::dvec3 image_orientation_v = {
        std::round(direction_cosines[3]),
        std::round(direction_cosines[4]),
        std::round(direction_cosines[5])
    };

    //Compute Z direction (cross product)
    const glm::dvec3 z_vector = glm::cross(image_orientation_u, image_orientation_v);

    //Compute dot product to get the index
    const double index = glm::dot(image_position, z_vector);

    return index;
}

//------------------------------------------------------------------------------

void image::read_image_plane_module()
{
    // Retrieve GDCM image
    SPTR(gdcm::ImageReader) image_reader = std::static_pointer_cast<gdcm::ImageReader>(m_reader);
    const gdcm::Image& gdcm_image = image_reader->GetImage();

    // image Position (Patient) - Type 1
    const double* gdcm_origin    = gdcm_image.GetOrigin();
    data::image::origin_t origin = {0., 0., 0.
    };
    if(gdcm_origin != nullptr)
    {
        std::copy(gdcm_origin, gdcm_origin + 3, origin.begin());
    }

    m_object->set_origin(origin);

    // Pixel Spacing - Type 1
    // image dimension
    const unsigned int dimension = gdcm_image.GetNumberOfDimensions();

    // image's spacing
    const double* gdcm_spacing     = gdcm_image.GetSpacing();
    data::image::spacing_t spacing = {1., 1., 1.
    };
    if(gdcm_spacing != nullptr)
    {
        std::copy(gdcm_spacing, gdcm_spacing + dimension, spacing.begin());
    }

    // Computing Z image spacing from two adjacent slices is preferable than reading Slice Thickness tag
    // See http://gdcm.sourceforge.net/wiki/index.php/Imager_Pixel_Spacing, Spacing along Z (third dimension)
    const data::dicom_series::dicom_container_t dicom_container = m_dicom_series->get_dicom_container();
    if(dicom_container.size() > 1)
    {
        auto first_item       = dicom_container.begin();
        const auto& last_item = dicom_container.rbegin();

        // Compute the spacing between slices of the 2 first slices.
        const double first_index  = get_instance_z_position(first_item->second);
        const double second_index = get_instance_z_position((++first_item)->second);
        const double last_index   = get_instance_z_position(last_item->second);
        spacing[2] = std::abs(second_index - first_index);

        // Check that the same spacing is used for all the instances
        const double epsilon         = 1e-2;
        const double total_z_spacing = std::abs(last_index - first_index);
        const double error_gap       = std::abs(spacing[2] * static_cast<double>(dicom_container.size() - 1))
                                       - total_z_spacing;
        if(error_gap > epsilon)
        {
            std::stringstream ss;
            ss << "Computed spacing between slices may not be correct. (Error gap : " << error_gap << ")";
            m_logger->warning(ss.str());
        }
    }
    else
    {
        // Retrieve dataset
        const gdcm::DataSet& dataset = image_reader->GetFile().GetDataSet();

        // Check tags availability
        if(dataset.FindDataElement(gdcm::Tag(0x0018, 0x0050)))
        {
            const std::string& slice_thickness =
                io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x0050>(dataset);
            spacing[2] = std::stod(slice_thickness);
        }
    }

    m_object->set_spacing(spacing);
}

//------------------------------------------------------------------------------

void image::read_voilut_module()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    //image's window center (double)
    std::string window_center = io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x1050>(dataset);
    std::vector<std::string> split_window_centers;
    if(!window_center.empty())
    {
        boost::split(split_window_centers, window_center, boost::is_any_of("\\"));
        std::vector<double> window_centers;
        std::ranges::transform(
            split_window_centers,
            std::back_inserter(window_centers),
            [](const auto& _e){return std::stod(_e);});

        m_object->set_window_center(window_centers);
    }

    //image's window width (double)
    std::string window_width = io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x1051>(dataset);
    std::vector<std::string> split_window_widths;
    if(!window_width.empty())
    {
        boost::split(split_window_widths, window_width, boost::is_any_of("\\"));
        std::vector<double> window_widths;
        std::ranges::transform(
            split_window_widths,
            std::back_inserter(window_widths),
            [](const auto& _e){return std::stod(_e);});

        m_object->set_window_width(window_widths);
    }
}

//------------------------------------------------------------------------------

std::vector<double> get_rescale_intercept_slope_value(gdcm::ImageReader* _image_reader)
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = _image_reader->GetFile().GetDataSet();

    // Retrieve rescale values
    std::vector<double> rescale = gdcm::ImageHelper::GetRescaleInterceptSlopeValue(_image_reader->GetFile());

    // Correct Rescale Intercept and Rescale Slope as GDCM may fail to retrieve them.
    if(dataset.FindDataElement(gdcm::Tag(0x0028, 0x1052)) && dataset.FindDataElement(gdcm::Tag(0x0028, 0x1053)))
    {
        rescale[0] = io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x1052, double>(dataset);
        rescale[1] = io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x1053, double>(dataset);
    }

    return rescale;
}

//------------------------------------------------------------------------------

void image::read_image_pixel_module()
{
    // Retrieve GDCM image
    SPTR(gdcm::ImageReader) image_reader = std::static_pointer_cast<gdcm::ImageReader>(m_reader);
    const gdcm::Image& gdcm_image = image_reader->GetImage();

    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Retrieve image information before processing the rescaling
    gdcm::PixelFormat pixel_format = gdcm::ImageHelper::GetPixelFormatValue(image_reader->GetFile());

    // Retrieve rescale intercept/slope values
    std::vector<double> rescale = get_rescale_intercept_slope_value(image_reader.get());
    double rescale_intercept    = rescale[0];
    double rescale_slope        = rescale[1];

    const std::uint16_t samples_per_pixel    = pixel_format.GetSamplesPerPixel();
    const std::uint16_t bits_allocated       = pixel_format.GetBitsAllocated();
    const std::uint16_t bits_stored          = pixel_format.GetBitsStored();
    const std::uint16_t high_bit             = pixel_format.GetHighBit();
    const std::uint16_t pixel_representation = pixel_format.GetPixelRepresentation();

    // Compute final image type
    data::dicom::image image_helper(
        samples_per_pixel, bits_allocated, bits_stored, high_bit, pixel_representation, rescale_slope,
        rescale_intercept);
    core::type image_type                 = image_helper.find_image_type_from_min_max_values();
    gdcm::PixelFormat target_pixel_format = io::dicom::helper::dicom_data_tools::get_pixel_type(image_type);

    if(target_pixel_format == gdcm::PixelFormat::UNKNOWN)
    {
        throw io::dicom::exception::failed("Unsupported image pixel format.");
    }

    // Compute number of components
    const std::string photometric_interpretation =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x0004>(dataset);
    const std::string pixel_presentation =
        io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x9205>(dataset);

    // Retrieve image dimensions
    std::vector<unsigned int> dimensions = gdcm::ImageHelper::GetDimensionsValue(image_reader->GetFile());

    // Compute real image size (we assume every instance has the same number of
    // slices (1 for CT and MR, may be more for enhanced CT and MR)
    const std::uint64_t frame_buffer_size = gdcm_image.GetBufferLength();
    const std::uint64_t depth             = frame_buffer_size
                                            / (std::uint64_t(dimensions[0]) * dimensions[1] * (bits_allocated / 8));
    dimensions[2] = static_cast<unsigned int>(m_dicom_series->get_dicom_container().size() * depth);

    const std::uint64_t image_buffer_size =
        std::uint64_t(dimensions[0]) * dimensions[1] * dimensions[2] * (bits_allocated / 8);
    const std::uint64_t new_image_buffer_size =
        std::uint64_t(dimensions[0]) * dimensions[1] * dimensions[2] * (target_pixel_format.GetBitsAllocated() / 8);

    // Let's read the image buffer
    bool perform_rescale = (photometric_interpretation != "PALETTE COLOR" && pixel_presentation != "COLOR");
    char* image_buffer   = this->read_image_buffer(
        dimensions,
        image_type,
        bits_allocated,
        target_pixel_format.GetBitsAllocated(),
        perform_rescale
    );

    // Correct image buffer according to the image orientation
    if(!(m_cancel_requested_callback && m_cancel_requested_callback()) && m_enable_buffer_rotation)
    {
        image_buffer = this->correct_image_orientation(
            image_buffer,
            dimensions,
            (perform_rescale) ? target_pixel_format.GetBitsAllocated() : bits_allocated
        );
    }

    // Apply lookup table if required
    if(!(m_cancel_requested_callback && m_cancel_requested_callback())
       && (photometric_interpretation == "PALETTE COLOR" || pixel_presentation == "COLOR"))
    {
        try
        {
            // Create new buffer
            char* colored_buffer = nullptr;
            colored_buffer = new char [new_image_buffer_size * 3];

            // Apply lookup
            gdcm_image.GetLUT().Decode(colored_buffer, new_image_buffer_size * 3, image_buffer, image_buffer_size);

            // Swap buffers
            delete[] image_buffer;
            image_buffer = colored_buffer;
        }
        catch(...)
        {
            throw io::dicom::exception::failed("There is not enough memory available to open this image.");
        }
    }

    // TODO_FB: This should probably be finer-tuned, but we would need to add new pixel formats before
    enum sight::data::image::pixel_format_t format
    {
        sight::data::image::pixel_format_t::undefined
    };
    if(photometric_interpretation == "MONOCHROME2")
    {
        format = data::image::pixel_format_t::gray_scale;
    }
    else if(photometric_interpretation == "RGB" || photometric_interpretation == "YBR"
            || photometric_interpretation == "PALETTE COLOR" || pixel_presentation == "COLOR")
    {
        format = data::image::pixel_format_t::rgb;
    }
    else if(photometric_interpretation == "ARGB" || photometric_interpretation == "CMYK")
    {
        format = data::image::pixel_format_t::rgba;
    }
    else
    {
        const std::string msg = "The photometric interpretation \"" + photometric_interpretation
                                + "\" is not supported.";
        throw io::dicom::exception::failed(msg);
    }

    // Last, set image buffer
    m_object->set_buffer(
        image_buffer,
        true,
        image_type,
        {dimensions[0], dimensions[1], dimensions[2]},
        format,
        std::make_shared<core::memory::buffer_new_policy>()
    );

    if(sight::data::helper::medical_image::check_image_validity(m_object))
    {
        sight::data::helper::medical_image::check_image_slice_index(m_object);
    }
}

//------------------------------------------------------------------------------

char* image::read_image_buffer(
    const std::vector<unsigned int>& _dimensions,
    const core::type _image_type,
    const std::uint16_t _bits_allocated,
    const std::uint16_t _new_bits_allocated,
    const bool _perform_rescale
)
{
    // Retrieve GDCM image
    SPTR(gdcm::ImageReader) image_reader = std::static_pointer_cast<gdcm::ImageReader>(m_reader);
    const gdcm::Image& gdcm_first_image = image_reader->GetImage();

    // Path container
    data::dicom_series::dicom_container_t dicom_container = m_dicom_series->get_dicom_container();

    // Raw buffer for all frames
    char* frame_buffer                        = nullptr;
    char* image_buffer                        = nullptr;
    const std::uint64_t frame_buffer_size     = gdcm_first_image.GetBufferLength();
    const std::uint64_t new_frame_buffer_size = frame_buffer_size * (_new_bits_allocated / _bits_allocated);
    const std::uint64_t image_buffer_size     = std::uint64_t(_dimensions.at(0)) * _dimensions.at(1) * _dimensions.at(2)
                                                * ((_perform_rescale ? _new_bits_allocated : _bits_allocated) / 8);

    // Allocate raw buffer
    try
    {
        frame_buffer = new char [frame_buffer_size];
        image_buffer = new char [image_buffer_size];
    }
    catch(...)
    {
        throw io::dicom::exception::failed("There is not enough memory available to open this image.");
    }

    // Read every frames
    unsigned int frame_number = 0;
    for(const auto& item : dicom_container)
    {
        // Read a frame
        gdcm::ImageReader frame_reader;
        const core::memory::buffer_object::sptr buffer_obj          = item.second;
        const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
        const std::string dicom_path                                = buffer_obj->get_stream_info().fs_file.string();
        SPTR(std::istream) is = stream_info.stream;
        frame_reader.SetStream(*is);

        if(frame_reader.Read())
        {
            const gdcm::Image& gdcm_image = frame_reader.GetImage();

            // Check frame buffer size
            if(frame_buffer_size != gdcm_image.GetBufferLength())
            {
                throw io::dicom::exception::failed("The frame buffer does not have the expected size : " + dicom_path);
            }

            // Get raw buffer and set it in the image buffer
            if(!gdcm_image.GetBuffer(frame_buffer))
            {
                throw io::dicom::exception::failed("Failed to get a frame buffer");
            }
        }
        else
        {
            std::stringstream ss;
            ss << "Reading error on frame : " << frame_number;
            throw io::dicom::exception::failed(ss.str());
        }

        // Rescale
        if(_perform_rescale)
        {
            // Retrieve rescale intercept/slope values
            std::vector<double> rescale = get_rescale_intercept_slope_value(&frame_reader);
            double rescale_intercept    = rescale[0];
            double rescale_slope        = rescale[1];

            // Retrieve image information before processing the rescaling
            gdcm::PixelFormat pixel_format            = gdcm::ImageHelper::GetPixelFormatValue(frame_reader.GetFile());
            gdcm::PixelFormat::ScalarType scalar_type = pixel_format.GetScalarType();
            gdcm::PixelFormat target_pixel_format     =
                io::dicom::helper::dicom_data_tools::get_pixel_type(_image_type);

            if(target_pixel_format == gdcm::PixelFormat::UNKNOWN)
            {
                throw io::dicom::exception::failed("Unsupported image pixel format.");
            }

            // Create rescaler
            gdcm::Rescaler rescaler;
            rescaler.SetIntercept(rescale_intercept);
            rescaler.SetSlope(rescale_slope);
            rescaler.SetPixelFormat(scalar_type);
            rescaler.SetTargetPixelType(target_pixel_format.GetScalarType());
            rescaler.SetUseTargetPixelType(true);

            // Rescale the image
            rescaler.Rescale(image_buffer + (frame_number * new_frame_buffer_size), frame_buffer, frame_buffer_size);
        }
        else
        {
            // Copy bytes
            memcpy(image_buffer + frame_number * frame_buffer_size, frame_buffer, frame_buffer_size);
        }

        // Reference SOP Instance UID in dicomInstance for SR reading
        const gdcm::DataSet& gdcm_dataset_root = frame_reader.GetFile().GetDataSet();
        const std::string sop_instance_uid     = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0018>(
            gdcm_dataset_root
        );
        if(!sop_instance_uid.empty())
        {
            m_instance->get_sop_instance_uid_container().push_back(sop_instance_uid);
        }
        else
        {
            m_logger->warning("A frame with an undefined SOP instance UID has been detected.");
        }

        // Next frame
        ++frame_number;

        auto progress =
            static_cast<unsigned int>(18 + (frame_number * 100 / static_cast<double>(dicom_container.size())) * 0.6);
        m_progress_callback(progress);

        if(m_cancel_requested_callback && m_cancel_requested_callback())
        {
            break;
        }
    }

    // Delete frame buffer
    delete[] frame_buffer;

    return image_buffer;
}

//------------------------------------------------------------------------------

char* image::correct_image_orientation(
    char* _buffer,
    std::vector<unsigned int>& _dimensions,
    std::uint16_t _bits_allocated
)
{
    char* result = _buffer;

    // Retrieve GDCM image
    SPTR(gdcm::ImageReader) image_reader = std::static_pointer_cast<gdcm::ImageReader>(m_reader);
    const gdcm::Image& gdcm_image = image_reader->GetImage();

    // Retrieve image orientation
    const double* direction_cosines = gdcm_image.GetDirectionCosines();

    // Compute U vector
    glm::dvec3 image_orientation_u = {
        std::round(direction_cosines[0]),
        std::round(direction_cosines[1]),
        std::round(direction_cosines[2])
    };

    // Try to find the closest axe
    if((std::fabs(image_orientation_u[0]) + std::fabs(image_orientation_u[1]) + std::fabs(image_orientation_u[2])) > 1)
    {
        if(std::fabs(direction_cosines[0]) < std::fabs(direction_cosines[1])
           || std::fabs(direction_cosines[0]) < std::fabs(direction_cosines[2]))
        {
            image_orientation_u[0] = 0.;
        }

        if(std::fabs(direction_cosines[1]) < std::fabs(direction_cosines[0])
           || std::fabs(direction_cosines[1]) < std::fabs(direction_cosines[2]))
        {
            image_orientation_u[1] = 0.;
        }

        if(std::fabs(direction_cosines[2]) < std::fabs(direction_cosines[0])
           || std::fabs(direction_cosines[2]) < std::fabs(direction_cosines[1]))
        {
            image_orientation_u[2] = 0.;
        }

        m_logger->warning(
            "Unable to determine clearly the orientation of the image. "
            "The software may display the image in the wrong direction."
        );
    }

    // Compute V vector
    glm::dvec3 image_orientation_v = {
        std::round(direction_cosines[3]),
        std::round(direction_cosines[4]),
        std::round(direction_cosines[5])
    };

    // Try to find the closest axe
    if((std::fabs(image_orientation_v[0]) + std::fabs(image_orientation_v[1]) + std::fabs(image_orientation_v[2])) > 1)
    {
        if(std::fabs(direction_cosines[3]) < std::fabs(direction_cosines[4])
           || std::fabs(direction_cosines[3]) < std::fabs(direction_cosines[5]))
        {
            image_orientation_v[0] = 0.;
        }

        if(std::fabs(direction_cosines[4]) < std::fabs(direction_cosines[3])
           || std::fabs(direction_cosines[4]) < std::fabs(direction_cosines[5]))
        {
            image_orientation_v[1] = 0.;
        }

        if(std::fabs(direction_cosines[5]) < std::fabs(direction_cosines[3])
           || std::fabs(direction_cosines[5]) < std::fabs(direction_cosines[4]))
        {
            image_orientation_v[2] = 0.;
        }

        m_logger->warning(
            "Unable to determine clearly the orientation of the image. "
            "The software may display the image in the wrong direction."
        );
    }

    // Compute W vector
    const glm::dvec3 image_orientation_w = glm::cross(image_orientation_u, image_orientation_v);

    // Create orientation matrix
    glm::dmat4 matrix;
    matrix[0][0] = image_orientation_u[0];
    matrix[0][1] = image_orientation_u[1];
    matrix[0][2] = image_orientation_u[2];
    matrix[0][3] = 0;
    matrix[1][0] = image_orientation_v[0];
    matrix[1][1] = image_orientation_v[1];
    matrix[1][2] = image_orientation_v[2];
    matrix[1][3] = 0;
    matrix[2][0] = image_orientation_w[0];
    matrix[2][1] = image_orientation_w[1];
    matrix[2][2] = image_orientation_w[2];
    matrix[2][3] = 0;
    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;

    // Compute inverse matrix in order to rotate the buffer
    const glm::dmat4 inverse_matrix = glm::inverse(matrix);
    const auto identity_matrix      = glm::identity<glm::dmat4>();

    // Check whether the image must be rotated or not
    if(inverse_matrix != identity_matrix)
    {
        // Compute new image size
        glm::dvec4 size_vector {_dimensions.at(0), _dimensions.at(1), _dimensions.at(2), 0.};
        glm::dvec4 new_size_vector = size_vector * inverse_matrix;
        const auto new_size_x      = static_cast<std::uint16_t>(std::fabs(new_size_vector[0]));
        const auto new_size_y      = static_cast<std::uint16_t>(std::fabs(new_size_vector[1]));
        const auto new_size_z      = static_cast<std::uint16_t>(std::fabs(new_size_vector[2]));
        new_size_vector.x = new_size_x;
        new_size_vector.y = new_size_y;
        new_size_vector.z = new_size_z;

        // Compute old size from the new absolute size in order to retrieve pixel flips
        glm::dvec4 old_size_vector = new_size_vector * matrix;

        // Create new buffer to store rotated image
        const std::uint64_t size = std::uint64_t(_dimensions.at(0)) * _dimensions.at(1) * _dimensions.at(2)
                                   * (_bits_allocated / 8);
        char* new_buffer = new char [size];

        // Rotate image
        std::uint16_t x     = 0;
        std::uint16_t y     = 0;
        std::uint16_t z     = 0;
        std::uint16_t old_x = 0;
        std::uint16_t old_y = 0;
        std::uint16_t old_z = 0;
        for(z = 0 ; z < new_size_z && !(m_cancel_requested_callback && m_cancel_requested_callback()) ; ++z)
        {
            for(y = 0 ; y < new_size_y ; ++y)
            {
                for(x = 0 ; x < new_size_x ; ++x)
                {
                    // Create new position
                    glm::dvec4 new_position {x, y, z, 0.};

                    // Compute old position
                    glm::dvec4 old_position = new_position * matrix;
                    old_x = (old_size_vector[0] > 0) ? static_cast<std::uint16_t>(old_position[0])
                                                     : static_cast<std::uint16_t>((_dimensions.at(0) - 1)
                                                                                  + old_position[0]);
                    old_y = (old_size_vector[1] > 0) ? static_cast<std::uint16_t>(old_position[1])
                                                     : static_cast<std::uint16_t>((_dimensions.at(1) - 1)
                                                                                  + old_position[1]);
                    old_z = (old_size_vector[2] > 0) ? static_cast<std::uint16_t>(old_position[2])
                                                     : static_cast<std::uint16_t>((_dimensions.at(2) - 1)
                                                                                  + old_position[2]);

                    // Compute indices
                    unsigned int position_index = (x + (y * new_size_x) + z * (new_size_x * new_size_y))
                                                  * (_bits_allocated / 8);
                    unsigned int old_position_index =
                        (old_x + (old_y * _dimensions.at(0)) + old_z * (_dimensions.at(0) * _dimensions.at(1)))
                        * (_bits_allocated / 8);

                    // Copy bytes
                    memcpy(&new_buffer[position_index], &_buffer[old_position_index], (_bits_allocated / 8));
                }
            }

            auto progress = static_cast<unsigned int>(78 + (z * 100. / new_size_z) * 0.2);
            m_progress_callback(progress);
        }

        result = new_buffer;
        delete[] _buffer;

        _dimensions = {new_size_x, new_size_y, new_size_z};

        // Update image spacing
        const data::image::spacing_t spacing = m_object->spacing();
        glm::dvec4 spacing_vector {spacing[0], spacing[1], spacing[2], 0.};
        glm::dvec4 new_spacing_vector = spacing_vector * inverse_matrix;
        data::image::spacing_t new_spacing {std::fabs(new_spacing_vector[0]), std::fabs(new_spacing_vector[1]),
                                            std::fabs(
                                                new_spacing_vector[2]
                                            )
        };
        m_object->set_spacing(new_spacing);

        // Update image origin
        const data::image::origin_t origin = m_object->origin();
        glm::dvec4 origin_vector {origin[0], origin[1], origin[2], 0.};
        glm::dvec4 new_origin_vector = origin_vector * inverse_matrix;
        data::image::origin_t new_origin;
        new_origin[0] = new_origin_vector[0];
        new_origin[1] = new_origin_vector[1];
        new_origin[2] = new_origin_vector[2];
        m_object->set_origin(new_origin);

        m_logger->warning(
            "image buffer has been rotated in order to match patient orientation: "
            "image origin could be wrong."
        );
    }

    return result;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
