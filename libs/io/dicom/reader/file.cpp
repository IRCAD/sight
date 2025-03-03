/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "file.hpp"

#include "core/jobs/job.hpp"

#include <core/compare.hpp>

#include <data/dicom/sop.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/matrix4.hpp>
#include <data/model_series.hpp>

#include <geometry/__/vector.hpp>

#include <gdcmDirectory.h>
#include <gdcmImageApplyLookupTable.h>
#include <gdcmImageChangePhotometricInterpretation.h>
#include <gdcmImageChangePlanarConfiguration.h>
#include <gdcmImageChangeTransferSyntax.h>
#include <gdcmImageReader.h>
#include <gdcmRescaler.h>
#include <gdcmScanner.h>
#include <gdcmTagKeywords.h>
#include <gdcmTagToVR.h>
#include <gdcmUIDs.h>

#include <glm/ext/matrix_relational.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

// cspell: ignore orthogonalize
namespace sight::io::dicom::reader
{

// We consider that 1e-3 is a good enough precision for our purpose.
// All frames that have a z position closer than 1e-3 will be considered as the same.
static constexpr double Z_EPSILON = 1e-3;

struct fiducial_set_with_metadata
{
    data::fiducials_series::fiducial_set fiducial_set;
    std::string content_date;
    std::string content_time;
    std::optional<std::int32_t> instance_number;
    std::string content_label;
    std::string content_description;
    std::string content_creator_name;
};

//------------------------------------------------------------------------------

inline static data::series_set::sptr scan_gdcm_files(
    const gdcm::Directory::FilenamesType& _files,
    const std::set<data::dicom::sop::Keyword>& _filters = {})
{
    // Use GDCM scanner to scan all files
    gdcm::Scanner scanner;

    // Select tags to be scanned.
    // This may also be used to display informations about series, so the user can select one wisely.
    static const std::vector<gdcm::Tag> s_UNIQUE_TAGS {
        // These will allow to identify the series.
        gdcm::Keywords::SeriesInstanceUID::GetTag(),
        gdcm::Keywords::SliceThickness::GetTag(),
        gdcm::Keywords::AcquisitionNumber::GetTag(),
        gdcm::Keywords::Rows::GetTag(),
        gdcm::Keywords::Columns::GetTag(),
        gdcm::Keywords::TemporalPositionIdentifier::GetTag(),
        gdcm::Keywords::TemporalPositionIndex::GetTag()
    };

    static const std::vector<gdcm::Tag> s_REQUESTED_TAGS =
        [&]
        {
            std::vector<gdcm::Tag> tmp {
                // These will allow to sort files using Image Position (Patient)
                gdcm::Keywords::ImagePositionPatient::GetTag(),
                gdcm::Keywords::ImageOrientationPatient::GetTag(),
                gdcm::Keywords::InstanceNumber::GetTag(),
                gdcm::Keywords::AcquisitionTime::GetTag(),
                gdcm::Keywords::ContentTime::GetTag(),
                gdcm::Keywords::SliceLocation::GetTag(),
                // These will allow to display useful informations
                gdcm::Keywords::SOPClassUID::GetTag(),
                gdcm::Keywords::SpecificCharacterSet::GetTag(),
                gdcm::Keywords::PatientID::GetTag(),
                gdcm::Keywords::PatientName::GetTag(),
                gdcm::Keywords::PatientSex::GetTag(),
                gdcm::Keywords::PatientBirthDate::GetTag(),
                gdcm::Keywords::PatientAge::GetTag(),
                gdcm::Keywords::StudyInstanceUID::GetTag(),
                gdcm::Keywords::StudyDescription::GetTag(),
                gdcm::Keywords::StudyDate::GetTag(),
                gdcm::Keywords::StudyTime::GetTag(),
                gdcm::Keywords::Modality::GetTag(),
                gdcm::Keywords::SeriesNumber::GetTag(),
                gdcm::Keywords::SeriesDescription::GetTag(),
                gdcm::Keywords::SeriesDate::GetTag(),
                gdcm::Keywords::SeriesTime::GetTag(),
                gdcm::Keywords::BodyPartExamined::GetTag(),
                gdcm::Keywords::PatientPosition::GetTag(),
                gdcm::Keywords::ContrastBolusAgent::GetTag(),
                gdcm::Keywords::ContrastBolusStartTime::GetTag()
            };

            tmp.insert(
                tmp.end(),
                s_UNIQUE_TAGS.begin(),
                s_UNIQUE_TAGS.end()
            );

            return tmp;
        }();

    for(const auto& tag : s_REQUESTED_TAGS)
    {
        scanner.AddTag(tag);
    }

    // Scan all files
    const auto result = scanner.Scan(_files);

    SIGHT_THROW_IF(
        "There is no DICOM files among the scanned files.",
        !result
    );

    // This map will used to merge DICOM instance that belongs to the same series
    std::map<std::string, data::series::sptr> unique_series;

    // The final list of series, with some attributes filled to allow sorting
    auto series_set = std::make_shared<data::series_set>();

    // Convert to our own format
    for(const auto& file : _files)
    {
        if(const char* const key = file.c_str(); scanner.IsKey(key))
        {
            const auto& mapping = scanner.GetMapping(key);

            // filter, if needed
            if(!_filters.empty())
            {
                // Get the SOP Class UID
                const auto& found = mapping.find(gdcm::Keywords::SOPClassUID::GetTag());

                if(found == mapping.end())
                {
                    // No need to continue if we cannot find the SOP Class UID
                    continue;
                }

                // Convert the string to SOP Class UID keyword
                const auto sop_keyword = data::dicom::sop::keyword(found->second);

                if(sop_keyword == data::dicom::sop::Keyword::INVALID)
                {
                    // No need to continue if the SOP Class UID string is unknown for us
                    continue;
                }

                // Check if the SOP Class UID is in the filter
                if(!_filters.contains(sop_keyword))
                {
                    continue;
                }
            }

            // Build an unique series identifier
            const std::string& unique_series_identifier =
                [&]
                {
                    std::string identifier;

                    // No, SeriesInstanceUID is not *always* an unique identifier.
                    //
                    // (from GDCM)
                    // - (0x0020, 0x0011) Series Number
                    //   A scout scan prior to a CT volume scan can share the same
                    //   SeriesUID, but they will sometimes have a different Series Number
                    //
                    // - (0x0018, 0x0050) Slice Thickness
                    //   On some CT systems, scout scans and subsequence volume scans will
                    //   have the same SeriesUID and Series Number - YET the slice
                    //   thickness will differ from the scout slice and the volume slices.
                    //
                    // - (0x0028, 0x0010) Rows and (0x0028, 0x0011) Columns
                    //   If the 2D images in a sequence don't have the same number of rows/cols,
                    //   then it is difficult to reconstruct them into a 3D volume.
                    //
                    for(const auto& tag : s_UNIQUE_TAGS)
                    {
                        if(const auto& found = mapping.find(tag); found != mapping.end())
                        {
                            identifier.append(found->second);
                        }
                    }

                    return identifier;
                }();

            // Retrieve the associated series and associated DICOM files
            auto& series               = unique_series[unique_series_identifier];
            const std::size_t instance = series ? series->num_instances() : 0;

            // If the series is not found, we create it
            if(!series)
            {
                series = std::make_shared<data::series>();
            }

            for(const auto& tag : s_REQUESTED_TAGS)
            {
                if(const auto& found = mapping.find(tag); found != mapping.end())
                {
                    series->set_string_value(
                        tag.GetGroup(),
                        tag.GetElement(),
                        found->second,
                        instance
                    );
                }
            }

            // Add the file to the series
            series->set_file(file, instance);

            // Add the series to the set
            series_set->push_back(series);
        }
    }

    return series_set;
}

//------------------------------------------------------------------------------

inline static auto convert_gdcm_image(
    gdcm::Image& _gdcm_image,
    const std::string& _filename
)
{
    // Decompress the Pixel Data buffer if needed
    if(_gdcm_image.GetTransferSyntax().IsEncapsulated())
    {
        gdcm::ImageChangeTransferSyntax changer;
        changer.SetInput(_gdcm_image);
        changer.SetTransferSyntax(gdcm::TransferSyntax::ImplicitVRLittleEndian);

        SIGHT_THROW_IF(
            "Cannot change the transfer syntax of DICOM file '" << _filename << "'.",
            !changer.Change()
        );

        _gdcm_image = changer.GetOutput();
    }

    // We only support 0 as planar configuration (r1g1b1, r2g2b2, ... not r1r2..g1g2..b1b2)
    if(_gdcm_image.GetPlanarConfiguration() == 1)
    {
        gdcm::ImageChangePlanarConfiguration changer;
        changer.SetInput(_gdcm_image);
        changer.SetPlanarConfiguration(0);

        SIGHT_THROW_IF(
            "Cannot change the planar configuration of DICOM file '" << _filename << "'.",
            !changer.Change()
        );

        _gdcm_image = changer.GetOutput();
    }

    // For palette color image, we need to convert palette values to real RGB values
    if(const auto& photometric_interpretation = _gdcm_image.GetPhotometricInterpretation();
       photometric_interpretation == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // Apply lookup table
        gdcm::ImageApplyLookupTable applier;
        applier.SetInput(_gdcm_image);

        SIGHT_THROW_IF(
            "Cannot Apply the lookup table of DICOM file '" << _filename << "'.",
            !applier.Apply()
        );

        _gdcm_image = applier.GetOutput();
    }
    // Sight assumes that the minimum sample value is intended to be displayed as black after any VOI grayscale
    // transformations have been performed.
    else if(photometric_interpretation == gdcm::PhotometricInterpretation::MONOCHROME1)
    {
        gdcm::ImageChangePhotometricInterpretation changer;
        changer.SetInput(_gdcm_image);
        changer.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);

        SIGHT_THROW_IF(
            "Cannot change the photometric interpretation of DICOM file '" << _filename << "'.",
            !changer.Change()
        );

        _gdcm_image = changer.GetOutput();
    }

    return _gdcm_image;
}

//------------------------------------------------------------------------------

inline static data::image::size_t compute_size(const data::series& _source, const gdcm::Image& _gdcm_image)
{
    const auto gdcm_num_dimensions = _gdcm_image.GetNumberOfDimensions();
    const auto& gdcm_dimensions    = _gdcm_image.GetDimensions();

    return {
        gdcm_num_dimensions > 0 ? gdcm_dimensions[0] : 1,
        gdcm_num_dimensions > 1 ? gdcm_dimensions[1] : 1,
        gdcm_num_dimensions > 2 ? gdcm_dimensions[2] : _source.num_instances()
    };
}

//------------------------------------------------------------------------------

inline static core::type compute_type(
    const gdcm::Image& _gdcm_image,
    const std::unique_ptr<gdcm::Rescaler>& _gdcm_rescaler
)
{
    const auto& gdcm_pixel_format = _gdcm_image.GetPixelFormat();

    if(gdcm_pixel_format == gdcm::PixelFormat::SINGLEBIT)
    {
        // In all cases, Sight reads single bit per pixel images as uint8
        return core::type::UINT8;
    }

    // Let the target type be guessed by GDCM
    const auto& gdcm_rescaled_pixel_type = _gdcm_rescaler
                                           ? _gdcm_rescaler->ComputeInterceptSlopePixelType()
                                           : gdcm_pixel_format.GetScalarType();

    // Dumb boilerplate code to convert the GDCM pixel type to a Sight pixel type
    switch(gdcm_rescaled_pixel_type)
    {
        case gdcm::PixelFormat::UINT8:
            return core::type::UINT8;

        case gdcm::PixelFormat::INT8:
            return core::type::INT8;

        case gdcm::PixelFormat::UINT16:
            return core::type::UINT16;

        case gdcm::PixelFormat::INT16:
            return core::type::INT16;

        case gdcm::PixelFormat::UINT32:
            return core::type::UINT32;

        case gdcm::PixelFormat::INT32:
            return core::type::INT32;

        case gdcm::PixelFormat::UINT64:
            return core::type::UINT64;

        case gdcm::PixelFormat::INT64:
            return core::type::INT64;

        case gdcm::PixelFormat::FLOAT32:
            return core::type::FLOAT;

        case gdcm::PixelFormat::FLOAT64:
            return core::type::DOUBLE;

        default:
            return core::type::NONE;
    }
}

//------------------------------------------------------------------------------

inline static enum data::image::pixel_format_t compute_format(
    const gdcm::Image& _gdcm_image,
    const std::string& _filename
)
{
    const auto gdcm_photometric_interpretation = _gdcm_image.GetPhotometricInterpretation();

    if(gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // PALETTE_COLOR is always expended as RGB
        return data::image::pixel_format_t::rgb;
    }

    const auto gdcm_sample_per_pixel = _gdcm_image.GetPixelFormat().GetSamplesPerPixel();

    if(gdcm_sample_per_pixel == 1)
    {
        // No need to check, no color space conversion...
        return data::image::pixel_format_t::gray_scale;
    }

    if(gdcm_sample_per_pixel == 3
       && (gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL_422
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_ICT
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_RCT
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::RGB))
    {
        return data::image::pixel_format_t::rgb;
    }

    SIGHT_THROW_IF(
        "Retired photometric interpretation used in DICOM file '" << _filename << "'.",
        gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::ARGB
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::CMYK
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::HSV
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_PARTIAL_420
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_PARTIAL_422
    );

    // Unsupported...
    return data::image::pixel_format_t::undefined;
}

//------------------------------------------------------------------------------

inline static std::optional<double> compute_frame_position(const data::series& _series, std::size_t _instance)
{
    auto position    = _series.get_image_position_patient(_instance);
    auto orientation = _series.get_image_orientation_patient(_instance);

    if(position.size() != 3 || orientation.size() != 6)
    {
        // Fallback to gdcm::ImageReader if the position is not available
        // This is of course slower...
        const auto& file = _series.get_file(_instance);

        if(file.empty() || !std::filesystem::exists(file) || std::filesystem::is_directory(file))
        {
            // Nothing to do here.
            return std::nullopt;
        }

        // Create the reader
        gdcm::ImageReader gdcm_image_reader;
        const auto& filename = file.string();
        gdcm_image_reader.SetFileName(filename.c_str());

        if(!gdcm_image_reader.Read())
        {
            return std::nullopt;
        }

        const auto& image               = gdcm_image_reader.GetImage();
        const double* const gdcm_origin = image.GetOrigin();
        position = {gdcm_origin[0], gdcm_origin[1], gdcm_origin[2]};

        const double* const gdcm_orientation = image.GetDirectionCosines();
        orientation = {
            gdcm_orientation[0], gdcm_orientation[1], gdcm_orientation[2],
            gdcm_orientation[3], gdcm_orientation[4], gdcm_orientation[5]
        };
    }

    // Compute w
    const glm::dvec3 glm_u = {orientation[0], orientation[1], orientation[2]};
    const glm::dvec3 glm_v = {orientation[3], orientation[4], orientation[5]};

    const auto glm_w = glm::cross(glm_u, glm_v);

    // Compute z position
    const glm::dvec3 glm_position = {position[0], position[1], position[2]};
    return glm::dot(glm_position, glm_w);
}

//------------------------------------------------------------------------------

inline static std::optional<double> compute_z_spacing(const data::series& _series)
{
    // Use a map to sort for us....
    std::map<std::int64_t, double> sorted_positions;

    if(_series.num_instances() < 2)
    {
        SIGHT_WARN(
            "The Z spacing cannot be computed, there is not enough instances."
        );

        return _series.get_slice_thickness();
    }

    for(std::size_t instance = 0, end = _series.num_instances() ; instance < end ; ++instance)
    {
        const auto& value = compute_frame_position(_series, instance);

        if(!value)
        {
            // No need to continue if we cannot compute the position for one frame
            return std::nullopt;
        }

        const double position = *value;

        // Simplify the z position, using the EPSILON precision
        const auto index = std::int64_t(position / Z_EPSILON);

        // Let the map sort the frames
        sorted_positions.insert_or_assign(index, position);
    }

    if(sorted_positions.size() < 2)
    {
        SIGHT_WARN(
            "The Z spacing cannot be computed, too much frame where dropped."
        );

        return _series.get_slice_thickness();
    }

    // cspell: ignore crbegin
    const double first_position  = sorted_positions.cbegin()->second;
    const double second_position = (++sorted_positions.cbegin())->second;
    const double last_position   = sorted_positions.crbegin()->second;

    const double first_spacing = std::abs(first_position - second_position);
    const double all_spacing   = std::abs(last_position - first_position);
    const double error         = std::abs(first_spacing * double(sorted_positions.size() - 1)) - all_spacing;

    if(error > Z_EPSILON)
    {
        SIGHT_WARN(
            "The Z spacing cannot be calculated, error ("
            << error
            << ") is bigger than current epsilon ("
            << Z_EPSILON
            << ")."
        );

        return _series.get_slice_thickness();
    }

    return first_spacing;
}

//------------------------------------------------------------------------------

inline static data::image::spacing_t compute_spacing(
    const data::series& _source,
    const gdcm::Image& _gdcm_image
)
{
    /// @note Z spacing may have to be recomputed using ImagePositionPatient...
    const double* const gdcm_spacing = _gdcm_image.GetSpacing();

    // Use absolute value since gdcm sometimes return negative spacing, which is odd.
    data::image::spacing_t spacing {std::abs(gdcm_spacing[0]), std::abs(gdcm_spacing[1]), std::abs(gdcm_spacing[2])};

    // Z Spacing correction
    // Overwrite only if GDCM returned the default value (1.0), since GDCM usually knows to compute it right
    if(core::is_equal(spacing[2], 1.0))
    {
        const auto& computed_spacing = compute_z_spacing(_source);

        if(computed_spacing)
        {
            spacing[2] = *computed_spacing;
        }
    }

    return spacing;
}

//------------------------------------------------------------------------------

inline static data::image_series::sptr new_image_series(
    const data::series& _source,
    const core::jobs::job::sptr& _job,
    const gdcm::Image& _gdcm_image,
    const std::unique_ptr<gdcm::Rescaler>& _gdcm_rescaler,
    const std::string& _filename
)
{
    // Create a new series and set the common dataset
    /// @note Window center / width is directly read from dataset, nothing to set here
    auto image_series = std::make_shared<data::image_series>();

    // Retrieve the image information
    // Target sizes (that's easy)
    const auto& size = compute_size(_source, _gdcm_image);

    // Target type, a bit more complicated
    const core::type& type = compute_type(_gdcm_image, _gdcm_rescaler);

    // Target PixelFormat, even more complicated
    const enum data::image::pixel_format_t& format = compute_format(_gdcm_image, _filename);

    SIGHT_THROW_IF(
        "Cannot guess the target pixel format to use while reading DICOM file '" << _filename << "'.",
        type == core::type::NONE || format == data::image::pixel_format_t::undefined
    );

    if(_job && _job->cancel_requested())
    {
        return nullptr;
    }

    // Resize to the correct size, and allocate memory
    image_series->resize(size, type, format);

    // Spacing.
    image_series->set_spacing(compute_spacing(_source, _gdcm_image));

    return image_series;
}

//------------------------------------------------------------------------------

template<typename T>
constexpr static void ybr_to_rgb(T* _buffer, std::size_t _size, std::uint16_t _stored_bits = 8)
{
    for(std::size_t i = 0 ; i < _size ; i += 3)
    {
        gdcm::ImageChangePhotometricInterpretation::YBR2RGB<T>(
            &_buffer[i],
            &_buffer[i],
            _stored_bits
        );
    }
}

//------------------------------------------------------------------------------

inline static const char* read_gdcm_buffer(
    const gdcm::Image& _gdcm_image,
    char* const _buffer,
    const std::string& _filename
)
{
    SIGHT_ASSERT("Null buffer.", _buffer != nullptr);

    SIGHT_THROW_IF(
        "Cannot read Pixel Data from DICOM file '" << _filename << "'.",
        _buffer == nullptr || !_gdcm_image.GetBuffer(_buffer)
    );

    return _buffer;
}

//------------------------------------------------------------------------------

inline static bool read_buffer(
    const core::jobs::job::sptr& _job,
    const gdcm::Image& _gdcm_image,
    const std::unique_ptr<gdcm::Rescaler>& _gdcm_rescaler,
    std::unique_ptr<std::vector<char> >& _gdcm_instance_buffer,
    char* const _instance_buffer,
    const std::size_t _instance_buffer_size,
    const std::string& _filename
)
{
    if(_job && _job->cancel_requested())
    {
        return false;
    }

    // Get the input buffer size
    const std::size_t gdcm_buffer_size = _gdcm_image.GetBufferLength();
    SIGHT_THROW_IF(
        "Source buffer size cannot be read in DICOM file '" << _filename << "'.",
        gdcm_buffer_size == 0
    );

    if(const auto& gdcm_pixel_format = _gdcm_image.GetPixelFormat();
       gdcm_pixel_format == gdcm::PixelFormat::SINGLEBIT)
    {
        SIGHT_ASSERT("Instance Buffer size must large enough.", _instance_buffer_size == gdcm_buffer_size * 8);

        if(_job && _job->cancel_requested())
        {
            return false;
        }

        // Read the buffer. Use the buffer from the image series object
        read_gdcm_buffer(_gdcm_image, _instance_buffer, _filename);

        if(_job && _job->cancel_requested())
        {
            return false;
        }

        // We need to convert bits to bytes...
        /// @note iterate from the end to the beginning, so we can use the same buffer
        auto* end_instance_buffer = reinterpret_cast<std::uint8_t*>(
            _instance_buffer + _instance_buffer_size - 1
        );

        for(auto i = std::streamsize(gdcm_buffer_size) ; --i >= 0 ; )
        {
            const auto byte = std::uint8_t(_instance_buffer[i]);

            end_instance_buffer[0] = (byte & 0x01) != 0 ? 0xff : 0x00;
            end_instance_buffer[1] = (byte & 0x02) != 0 ? 0xff : 0x00;
            end_instance_buffer[2] = (byte & 0x04) != 0 ? 0xff : 0x00;
            end_instance_buffer[3] = (byte & 0x08) != 0 ? 0xff : 0x00;
            end_instance_buffer[4] = (byte & 0x10) != 0 ? 0xff : 0x00;
            end_instance_buffer[5] = (byte & 0x20) != 0 ? 0xff : 0x00;
            end_instance_buffer[6] = (byte & 0x40) != 0 ? 0xff : 0x00;
            end_instance_buffer[7] = (byte & 0x80) != 0 ? 0xff : 0x00;

            end_instance_buffer -= 8;
        }
    }
    else if(_gdcm_rescaler)
    {
        // Apply rescale slope / intercept
        SIGHT_ASSERT(
            "Instance Buffer size must large enough.",
            _instance_buffer_size >= (
                gdcm_buffer_size
                * gdcm::PixelFormat(_gdcm_rescaler->ComputeInterceptSlopePixelType()).GetPixelSize()
                / std::max(gdcm_pixel_format.GetPixelSize(), std::uint8_t(1))
            )
        );

        if(_job && _job->cancel_requested())
        {
            return false;
        }

        // Prepare the input buffer
        if(!_gdcm_instance_buffer)
        {
            _gdcm_instance_buffer = std::make_unique<std::vector<char> >(gdcm_buffer_size);
        }
        else if(_gdcm_instance_buffer->size() < gdcm_buffer_size)
        {
            _gdcm_instance_buffer->resize(gdcm_buffer_size);
        }

        // Read raw input buffer
        const char* const gdcm_buffer = read_gdcm_buffer(_gdcm_image, _gdcm_instance_buffer->data(), _filename);

        if(_job && _job->cancel_requested())
        {
            return false;
        }

        _gdcm_rescaler->Rescale(_instance_buffer, gdcm_buffer, gdcm_buffer_size);
    }
    else if(const auto gdcm_photometric_interpretation = _gdcm_image.GetPhotometricInterpretation();
            gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL
            || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL_422)
    {
        // Convert YBR to RGB
        SIGHT_THROW_IF(
            "Source buffer is incompatible with the Photometric Interpretation in DICOM file '" << _filename << "'.",
            gdcm_pixel_format.GetSamplesPerPixel() != 3 && gdcm_buffer_size % 3 != 0
        );

        SIGHT_ASSERT(
            "Instance Buffer size must large enough.",
            _instance_buffer_size >= gdcm_buffer_size
        );

        if(_job && _job->cancel_requested())
        {
            return false;
        }

        // Read the buffer. Use the buffer from the image series object
        read_gdcm_buffer(_gdcm_image, _instance_buffer, _filename);

        // 99% of the time, the pixel type is 8 bits per component, but it can be 16 bits too.
        switch(gdcm_pixel_format.GetBitsAllocated())
        {
            case 8:
            {
                ybr_to_rgb(
                    reinterpret_cast<std::uint8_t*>(_instance_buffer),
                    gdcm_buffer_size,
                    gdcm_pixel_format.GetBitsStored()
                );
                break;
            }

            case 16:
            {
                ybr_to_rgb(
                    reinterpret_cast<std::uint16_t*>(_instance_buffer),
                    gdcm_buffer_size,
                    gdcm_pixel_format.GetBitsStored()
                );
                break;
            }

            default:
                SIGHT_THROW("Unsupported allocated bits per pixel in DICOM file '" << _filename << "'.");
        }
    }
    else
    {
        // Nothing to do other than copying the buffer
        read_gdcm_buffer(_gdcm_image, _instance_buffer, _filename);
    }

    return true;
}

/// Make direction cosines orthogonal. Also found in ITK
/// This is for some strange DICOM files that have non orthogonal direction cosines.
/// @warning column major order
inline static std::vector<double> orthogonalize(const double* const _direction_cosines)
{
    glm::dvec3 u = {_direction_cosines[0], _direction_cosines[1], _direction_cosines[2]};
    glm::dvec3 v = {_direction_cosines[3], _direction_cosines[4], _direction_cosines[5]};
    glm::dvec3 w;

    if(geometry::orthogonalize(u, v, w))
    {
        SIGHT_WARN("Direction cosines are not orthogonal, they will be corrected, but the result must be checked.");
    }

    return {u[0], u[1], u[2], v[0], v[1], v[2], w[0], w[1], w[2]};
}

/// Decode image orientation / position
/// GDCM doesn't handle Enhanced US Volume (which is rather a complicated case)
/// see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.8.24.2.html
inline static data::matrix4 compute_image_transform(
    const gdcm::Image& _gdcm_image,
    data::image_series::sptr _image_series
)
{
    data::matrix4 transform;

    switch(_image_series->get_ultrasound_acquisition_geometry())
    {
        case data::dicom::ultrasound_acquisition_geometry_t::apex:
        {
            // Search for Volume to Transducer Mapping Matrix
            if(const auto& transducer_mapping = _image_series->get_volume_to_transducer_mapping_matrix();
               transducer_mapping)
            {
                return transducer_mapping->values();
            }

            break;
        }

        case data::dicom::ultrasound_acquisition_geometry_t::patient:
        {
            // Make direction cosines orthogonal. Also found in ITK
            // This is for some strange DICOM files that have non orthogonal direction cosines.

            // Tune the shared orientation
            if(const auto& orientation = _image_series->get_image_orientation_patient(std::nullopt);
               orientation.size() == 6)
            {
                auto orthogonal_directions = orthogonalize(orientation.data());

                // Store the orientation to use it later if there is no Volume to Table Mapping Matrix
                // Convert to row major order
                transform.set_orientation(
                    {
                        orthogonal_directions[0], orthogonal_directions[3], orthogonal_directions[6],
                        orthogonal_directions[1], orthogonal_directions[4], orthogonal_directions[7],
                        orthogonal_directions[2], orthogonal_directions[5], orthogonal_directions[8]
                    });

                // Having more than 6 elements makes GDCM assert
                orthogonal_directions.resize(6);

                _image_series->set_image_orientation_patient(
                    orthogonal_directions,
                    std::nullopt
                );
            }

            // Tune the per-frame orientation
            for(std::size_t frame = 0, end_index = _image_series->num_frames() ;
                frame < end_index ; ++frame)
            {
                // For each frame, make the direction cosines orthogonal, if needed
                if(const auto& orientation = _image_series->get_image_orientation_patient(frame);
                   orientation.size() == 6)
                {
                    auto orthogonal_directions = orthogonalize(orientation.data());

                    if(frame == 0)
                    {
                        // Store the orientation to use it later if there is no Volume to Table Mapping Matrix
                        transform.set_orientation(
                            {
                                orthogonal_directions[0], orthogonal_directions[3], orthogonal_directions[6],
                                orthogonal_directions[1], orthogonal_directions[4], orthogonal_directions[7],
                                orthogonal_directions[2], orthogonal_directions[5], orthogonal_directions[8]
                            });
                    }

                    // Having more than 6 elements makes GDCM assert
                    orthogonal_directions.resize(6);

                    _image_series->set_image_orientation_patient(
                        orthogonal_directions,
                        frame
                    );
                }
            }

            if(_image_series->get_patient_frame_of_reference_source()
               == data::dicom::patient_frame_of_reference_source_t::table)
            {
                // Search for Volume to Table Mapping Matrix
                if(const auto& table_mapping = _image_series->get_volume_to_table_mapping_matrix(); table_mapping)
                {
                    return table_mapping->values();
                }
            }

            // If there is no Volume to Table Mapping Matrix, use the first frame
            if(const auto& frame_position = _image_series->get_image_position_patient(0);
               frame_position.size() == 3)
            {
                transform.set_position(frame_position);
            }
            else if(const auto& shared_position = _image_series->get_image_position_patient(std::nullopt);
                    shared_position.size() == 3)
            {
                transform.set_position(shared_position);
            }

            // If the transform has been modified, return it
            if(!transform.is_identity())
            {
                return transform.values();
            }

            break;
        }

        default:
            break;
    }

    // Last resort: GDCM Direction cosines + origin
    const auto& orthogonal_directions = orthogonalize(_gdcm_image.GetDirectionCosines());
    const auto* const origin          = _gdcm_image.GetOrigin();

    return data::matrix4(
        {
            orthogonal_directions[0], orthogonal_directions[3], orthogonal_directions[6], origin[0],
            orthogonal_directions[1], orthogonal_directions[4], orthogonal_directions[7], origin[1],
            orthogonal_directions[2], orthogonal_directions[5], orthogonal_directions[8], origin[2],
            0., 0., 0., 1.
        });
}

//------------------------------------------------------------------------------

inline static data::series_set::sptr read_image_instance(
    const data::series& _source,
    const core::jobs::job::sptr& _job,
    std::unique_ptr<std::vector<char> >& _gdcm_instance_buffer,
    std::size_t _instance                   = 0,
    data::series_set::sptr _splitted_series = nullptr
)
{
    if(_job && _job->cancel_requested())
    {
        return nullptr;
    }

    // Read the DICOM file using GDCM ImageReader
    gdcm::ImageReader gdcm_reader;
    const std::string& filename = _source.get_file(_instance).string();
    gdcm_reader.SetFileName(filename.c_str());

    SIGHT_INFO("Reading DICOM file '" << filename << "'.");
    SIGHT_THROW_IF("Cannot read DICOM file '" << filename << "'.", !gdcm_reader.Read());

    // Get the image and convert it to a suitable format
    const auto& gdcm_image = convert_gdcm_image(gdcm_reader.GetImage(), filename);

    // Get the dataset and the input pixel format
    const auto& gdcm_dataset = gdcm_reader.GetFile().GetDataSet();

    // GDCM you are disappointing. gdcm::Image::GetIntercept() and gdcm::Image::GetSlope() doesn't always work.
    const auto& [use_intercept, fixed_intercept] =
        [&]
        {
            if(const double gdcm_intercept = gdcm_image.GetIntercept();
               !core::is_equal(gdcm_intercept, 0.0))
            {
                return std::make_pair(true, gdcm_intercept);
            }

            if(gdcm_dataset.FindDataElement(gdcm::Keywords::RescaleIntercept::GetTag()))
            {
                gdcm::Keywords::RescaleIntercept rescale_intercept;
                rescale_intercept.SetFromDataSet(gdcm_dataset);
                return std::make_pair(true, rescale_intercept.GetValue());
            }

            return std::make_pair(false, 0.0);
        }();

    const auto& [use_slope, fixed_slope] =
        [&]
        {
            if(const double gdcm_slope = gdcm_image.GetSlope();
               !core::is_equal(gdcm_slope, 1.0))
            {
                return std::make_pair(true, gdcm_slope);
            }

            if(gdcm_dataset.FindDataElement(gdcm::Keywords::RescaleSlope::GetTag()))
            {
                gdcm::Keywords::RescaleSlope rescale_slope;
                rescale_slope.SetFromDataSet(gdcm_dataset);
                return std::make_pair(true, rescale_slope.GetValue());
            }

            return std::make_pair(false, 1.0);
        }();

    // Initialize the rescaler if there is a Rescale Intercept / Rescale Slope
    // The Rescale Intercept / Rescale Slope can be specific to each instance !
    std::unique_ptr<gdcm::Rescaler> gdcm_rescaler;
    if(use_intercept || use_slope)
    {
        gdcm_rescaler = std::make_unique<gdcm::Rescaler>();
        gdcm_rescaler->SetIntercept(fixed_intercept);
        gdcm_rescaler->SetSlope(fixed_slope);
        gdcm_rescaler->SetPixelFormat(gdcm_image.GetPixelFormat());
    }

    // Create the ImageSeries, if needed, get or compute needed image information
    // Special case here: if the current image is a volume, and we have more than one instance, we have no other
    // choice than splitting the series.
    const bool split = gdcm_image.GetNumberOfDimensions() >= 3 && _source.num_instances() > 1;
    if(!_splitted_series || split)
    {
        if(_job && _job->cancel_requested())
        {
            return nullptr;
        }

        // User may have canceled the job
        if(const auto& image_series = new_image_series(_source, _job, gdcm_image, gdcm_rescaler, filename);
           image_series)
        {
            // Add the dataset to allow access to all DICOM attributes (not only the ones we have converted)
            image_series->set_data_set(gdcm_dataset);

            // Also save the file path. It could be useful to keep a link to the original file.
            image_series->set_file(filename);

            const auto& transform = compute_image_transform(gdcm_image, image_series);
            image_series->data::image::set_origin(transform.position());
            image_series->data::image::set_orientation(transform.orientation());

            ///@todo remove that once we remove field 'direction' from image_series
            data::helper::medical_image::set_direction(
                *image_series,
                std::make_shared<data::matrix4>(transform.values())
            );

            // Add the series to a new dataset
            if(!_splitted_series)
            {
                _splitted_series = std::make_shared<data::series_set>();
            }

            _splitted_series->push_back(image_series);
        }
    }

    if(_job && _job->cancel_requested())
    {
        return nullptr;
    }

    // Use the last series as current series
    auto image_series    = std::static_pointer_cast<data::image_series>(_splitted_series->back());
    const auto dump_lock = image_series->dump_lock();

    // Add the dataset to allow access to all DICOM attributes (not only the ones we have converted)
    image_series->set_data_set(gdcm_dataset, _instance);

    // Also save the file path. It could be useful to keep a link to the original file.
    image_series->set_file(filename, _instance);

    // Get the output buffer (as char* since gdcm takes char* as input)
    // If the series will be splitted by instance, we keep 0 as instance number
    char* const instance_buffer = &image_series->at<char>(0, 0, split ? 0 : _instance, 0);
    SIGHT_ASSERT("Null buffer.", instance_buffer != nullptr);

    // Compute the size
    const std::size_t instance_buffer_size =
        split ? image_series->size_in_bytes()
              : image_series->size_in_bytes() / std::max(std::size_t(1), _source.num_instances());

    // Read the image data and fill the image series
    if(!read_buffer(
           _job,
           gdcm_image,
           gdcm_rescaler,
           _gdcm_instance_buffer,
           instance_buffer,
           instance_buffer_size,
           filename
    ))
    {
        // Job have been canceled
        return nullptr;
    }

    return _splitted_series;
}

//------------------------------------------------------------------------------

inline static data::series_set::sptr read_image(const data::series& _source, const core::jobs::job::sptr& _job)
{
    if(_job && _job->cancel_requested())
    {
        return nullptr;
    }

    // Read first instance to get image information
    // readImageInstance() returns a series set, because the series can be splitted in rare cases,
    // like US 4D Volume.
    std::unique_ptr<std::vector<char> > gdcm_instance_buffer;
    auto splitted_series = read_image_instance(_source, _job, gdcm_instance_buffer, 0);

    if(!splitted_series)
    {
        // Job have been canceled
        return nullptr;
    }

    // Read the other instances if necessary
    for(std::size_t instance = 1, end = _source.num_instances() ; instance < end ; ++instance)
    {
        if(_job && _job->cancel_requested())
        {
            return nullptr;
        }

        read_image_instance(_source, _job, gdcm_instance_buffer, instance, splitted_series);
    }

    for(const auto& series : *splitted_series)
    {
        auto image_series = std::static_pointer_cast<data::image_series>(series);

        if(data::helper::medical_image::check_image_validity(image_series))
        {
            data::helper::medical_image::check_image_slice_index(image_series);
        }

        ///@todo check if we must rotate the buffer to match ImageOrientationPatient. Not sure it is a good idea...
    }

    return splitted_series;
}

//------------------------------------------------------------------------------

inline static data::series_set::sptr read_model(
    const data::series& /*unused*/,
    const core::jobs::job::sptr& /*unused*/
)
{
    data::series_set::sptr splitted_series;

    /// @todo Implement model series reading

    return splitted_series;
}

//------------------------------------------------------------------------------

inline static std::vector<fiducial_set_with_metadata> read_fiducial_sets(const data::series& _series)
{
    gdcm::Reader reader;
    reader.SetFileName(_series.get_file().string().c_str());
    reader.Read();
    auto fiducials_series = std::make_shared<data::fiducials_series>();
    fiducials_series->set_data_set(reader.GetFile().GetDataSet());
    std::vector<fiducial_set_with_metadata> res;
    std::ranges::transform(
        fiducials_series->get_fiducial_sets(),
        std::back_inserter(res),
        [fiducials_series](data::fiducials_series::fiducial_set _fs) -> fiducial_set_with_metadata
        {
            return {
                .fiducial_set         = _fs,
                .content_date         = fiducials_series->get_content_date(),
                .content_time         = fiducials_series->get_content_time(),
                .instance_number      = fiducials_series->get_instance_number(),
                .content_label        = fiducials_series->get_content_label(),
                .content_description  = fiducials_series->get_content_description(),
                .content_creator_name = fiducials_series->get_content_creator_name()
            };
        });
    return res;
}

/// Private reader implementation
class file::reader_impl
{
public:

    /// Delete default constructors and assignment operators
    reader_impl(const reader_impl&)            = delete;
    reader_impl(reader_impl&&)                 = delete;
    reader_impl& operator=(const reader_impl&) = delete;
    reader_impl& operator=(reader_impl&&)      = delete;

    /// Constructor
    explicit reader_impl(reader::file* const _reader) noexcept :
        m_reader(_reader)
    {
    }

    /// Default destructor
    ~reader_impl() noexcept = default;

    /// Pointer to the public interface
    reader::file* const m_reader;

    /// Returns a list of DICOM series by scanning files using get_files()
    /// The files are NOT sorted!
    /// @return data::series_set::sptr: A set of series, with their associated files
    /// @throw std::runtime_error if the root directory is not an existing folder
    /// @throw std::runtime_error if there is no dicom files are found
    [[nodiscard]] data::series_set::sptr scan_files(const std::vector<std::filesystem::path>& _files) const
    {
        // Convert std::vector<std::filesystem::path> to std::vector<std::string>
        gdcm::Directory::FilenamesType gdcm_files;

        for(const auto& file : _files)
        {
            if(std::filesystem::exists(file) && !std::filesystem::is_directory(file))
            {
                gdcm_files.push_back(file.string());
            }
        }

        SIGHT_THROW_IF(
            "There is no DICOM file to scan.",
            gdcm_files.empty()
        );

        return scan_gdcm_files(gdcm_files, m_filters);
    }

    /// Returns a list of DICOM series with associated files sorted
    /// @return data::series_set::sptr: A set of series, with their associated files sorted
    /// @throw std::runtime_error if there is no scanned series
    [[nodiscard]] data::series_set::sptr sort() const
    {
        SIGHT_THROW_IF(
            "There is no DICOM file to sort.",
            !m_scanned || m_scanned->empty()
        );

        for(const auto& series : *m_scanned)
        {
            if(cancel_requested())
            {
                return nullptr;
            }

            // No need to sort if there is only one file
            if(series->num_instances() <= 1)
            {
                continue;
            }

            // Try many sort strategies
            // Try first by Image position

            if(sort_instances_by_image_position(series)
               || sort_instances_by_content_time(series)
               || sort_instances_by_instance_number(series)
               || sort_instances_by_filename(series))
            {
                continue;
            }
        }

        return m_scanned;
    }

    //------------------------------------------------------------------------------

    static bool sort_instances_by_image_position(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->num_instances() ; instance < end ; ++instance)
        {
            const auto& value = compute_frame_position(*_series, instance);

            if(!value)
            {
                // No need to continue if we cannot compute the position for one frame
                return false;
            }

            // Simplify the z position, using the EPSILON precision
            const auto index = std::int64_t(*value / Z_EPSILON);

            // Let the map sort the frames
            sorter.insert_or_assign(index, instance);
        }

        // Finally, we can sort the frames in the series
        std::vector<std::size_t> sorted;
        sorted.reserve(sorter.size());

        std::transform(
            sorter.cbegin(),
            sorter.cend(),
            std::back_inserter(sorted),
            [](const auto& _v){return _v.second;});

        return _series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    static bool sort_instances_by_content_time(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->num_instances() ; instance < end ; ++instance)
        {
            const auto& value = _series->get_content_time(instance);

            if(value.empty())
            {
                // No need to continue if we cannot get the instance number for one frame
                return false;
            }

            // Start to parse DICOM time
            std::chrono::hours hours {};
            if(value.length() >= 2)
            {
                try
                {
                    hours = std::chrono::hours(std::stoi(value.substr(0, 2)));
                }
                catch(...)
                {
                    return false;
                }
            }
            else
            {
                // Hours should always be there
                return false;
            }

            std::chrono::minutes minutes {};
            if(value.length() >= 4)
            {
                try
                {
                    minutes = std::chrono::minutes(std::stoi(value.substr(2, 2)));
                }
                catch(...)
                {
                    // If length >= 4, minutes shall be present
                    return false;
                }
            }

            std::chrono::seconds seconds {};
            if(value.length() >= 6)
            {
                try
                {
                    seconds = std::chrono::seconds(std::stoi(value.substr(4, 2)));
                }
                catch(...)
                {
                    // If length >= 6, seconds shall be present
                    return false;
                }
            }

            std::chrono::microseconds microseconds {};
            if(value.length() >= 8)
            {
                try
                {
                    // Do not forget '.' after the seconds
                    auto us = value.substr(7);

                    // Fill with trailing 0 to always have microseconds
                    us.resize(6, '0');

                    microseconds = std::chrono::microseconds(std::stoi(us));
                }
                catch(...)
                {
                    // If length >= 8, microseconds shall be present
                    return false;
                }
            }

            // Let the map sort the frames
            const std::int64_t index = std::chrono::duration_cast<std::chrono::microseconds>(
                hours + minutes + seconds + microseconds
            ).count();

            sorter.insert_or_assign(index, instance);
        }

        // Finally, we can sort the frames in the series
        std::vector<std::size_t> sorted;
        sorted.reserve(sorter.size());

        std::transform(
            sorter.cbegin(),
            sorter.cend(),
            std::back_inserter(sorted),
            [](const auto& _v){return _v.second;});

        return _series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    static bool sort_instances_by_instance_number(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->num_instances() ; instance < end ; ++instance)
        {
            const auto& value = _series->get_instance_number(instance);

            if(!value)
            {
                // No need to continue if we cannot get the instance number for one frame
                return false;
            }

            // Let the map sort the frames
            sorter.insert_or_assign(*value, instance);
        }

        // Finally, we can sort the frames in the series
        std::vector<std::size_t> sorted;
        sorted.reserve(sorter.size());

        std::transform(
            sorter.cbegin(),
            sorter.cend(),
            std::back_inserter(sorted),
            [](const auto& _v){return _v.second;});

        return _series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    static bool sort_instances_by_filename(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::filesystem::path, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->num_instances() ; instance < end ; ++instance)
        {
            const auto& value = _series->get_file(instance);

            if(value.empty())
            {
                // No need to continue if we cannot get the path for one frame
                return false;
            }

            // Let the map sort the frames
            sorter.insert_or_assign(value, instance);
        }

        // Finally, we can sort the frames in the series
        std::vector<std::size_t> sorted;
        sorted.reserve(sorter.size());

        std::transform(
            sorter.cbegin(),
            sorter.cend(),
            std::back_inserter(sorted),
            [](const auto& _v){return _v.second;});

        return _series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    void read()
    {
        SIGHT_THROW_IF(
            "There is no DICOM file to read.",
            !m_sorted || m_sorted->empty()
        );

        // Instantiate or reuse the output series set
        if(const auto& object = std::dynamic_pointer_cast<data::series_set>(m_reader->m_object.lock()); object)
        {
            m_read = object;
            m_read->clear();
        }
        else
        {
            m_read = std::make_shared<data::series_set>();
            m_reader->set_object(m_read);
        }

        std::vector<fiducial_set_with_metadata> fiducial_sets;

        // Start reading selected series
        for(const auto& source : *m_sorted)
        {
            if(cancel_requested())
            {
                clear();
                return;
            }

            // Very unusual but we can have a series with several volumes, like in 4D Volume US
            // Therefore, we use a series set, so we can split the series if needed.
            data::series_set::sptr splitted_series;

            if(source->get_dicom_type() == data::series::dicom_t::image)
            {
                // Read an image series
                splitted_series = read_image(*source, m_job);
            }
            else if(source->get_dicom_type() == data::series::dicom_t::model)
            {
                // Read a model series
                splitted_series = read_model(*source, m_job);
            }
            else if(source->get_dicom_type() == data::series::dicom_t::fiducials)
            {
                std::ranges::copy(read_fiducial_sets(*source), std::back_inserter(fiducial_sets));
            }
            else
            {
                SIGHT_THROW(
                    "Unsupported DICOM IOD '" << data::dicom::sop::get(source->get_sop_keyword()).m_name
                    << "'."
                );
            }

            // Add the read series to the set
            if(splitted_series)
            {
                std::copy(
                    splitted_series->cbegin(),
                    splitted_series->cend(),
                    std::back_inserter(*m_read)
                );
            }
        }

        // Associate the fiducials to their images/models
        for(const fiducial_set_with_metadata& fiducial_set : fiducial_sets)
        {
            for(const data::series::sptr& series : *m_read)
            {
                auto image_series = std::dynamic_pointer_cast<data::image_series>(series);
                auto model_series = std::dynamic_pointer_cast<data::model_series>(series);
                if(image_series == nullptr && model_series == nullptr)
                {
                    break;
                }

                bool fiducial_set_is_relevant = fiducial_set.fiducial_set.frame_of_reference_uid
                                                && series->get_string_value(
                    data::dicom::attribute::Keyword::FrameOfReferenceUID
                                                )
                                                == fiducial_set.fiducial_set.frame_of_reference_uid;
                if(!fiducial_set_is_relevant && fiducial_set.fiducial_set.referenced_image_sequence)
                {
                    for(const data::fiducials_series::referenced_image& referenced_image :
                        *fiducial_set.fiducial_set.referenced_image_sequence)
                    {
                        // TODO: Take ReferencedSegmentNumber into account for Segmentation IOD
                        if(referenced_image.referenced_sop_class_uid
                           == data::dicom::sop::get(series->get_sop_keyword()).m_uid
                           && referenced_image.referenced_sop_instance_uid == series->get_sop_instance_uid())
                        {
                            fiducial_set_is_relevant = true;
                            break;
                        }
                    }
                }

                if(fiducial_set_is_relevant)
                {
                    data::fiducials_series::sptr fiducials_series;
                    bool series_have_fiducials = false;
                    if(image_series != nullptr)
                    {
                        fiducials_series      = image_series->get_fiducials();
                        series_have_fiducials = !fiducials_series->get_fiducial_sets().empty();
                    }
                    else if(model_series != nullptr)
                    {
                        fiducials_series      = model_series->get_fiducials();
                        series_have_fiducials = !fiducials_series->get_fiducial_sets().empty();
                    }

                    if(!series_have_fiducials)
                    {
                        // It is the first fiducial set to be appended to this fiducials series;
                        // set fiducials metadata
                        fiducials_series->set_content_date(fiducial_set.content_date);
                        fiducials_series->set_content_time(fiducial_set.content_time);
                        fiducials_series->set_instance_number(fiducial_set.instance_number);
                        fiducials_series->set_content_label(fiducial_set.content_label);
                        fiducials_series->set_content_description(fiducial_set.content_description);
                        fiducials_series->set_content_creator_name(fiducial_set.content_creator_name);
                    }

                    fiducials_series->append_fiducial_set(fiducial_set.fiducial_set);
                }
            }
        }

        // Not needed anymore, free some memory
        clear();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool cancel_requested() const noexcept
    {
        return m_job && m_job->cancel_requested();
    }

    //------------------------------------------------------------------------------

    void progress(std::uint64_t _units) const
    {
        if(m_job)
        {
            m_job->done_work(_units);
        }
    }

    //------------------------------------------------------------------------------

    void clear()
    {
        m_scanned.reset();
        m_sorted.reset();
    }

    /// The default filter to select only some type (Image, Model, ...) of DICOM files.
    data::series::SopKeywords m_filters;

    /// Contains the list of files to sort and read.
    /// Usually, it is filed by user after showing a selection dialog,
    /// but calling read() will fill it automatically.
    data::series_set::sptr m_scanned;

    /// Contains the list of sorted files to read.
    /// Usually, it is filed when calling read(), but user can set it to bypass default sort algorithms.
    data::series_set::sptr m_sorted;

    /// The final output
    /// This allows to keep a reference as generic_object_reader / object_reader only keep a weak_ptr to the output.
    data::series_set::sptr m_read;

    /// The default job. Allows to watch for cancellation and report progress.
    core::jobs::job::sptr m_job;
};

file::file() :
    core::location::single_folder(),
    core::location::multiple_files(),
    m_pimpl(std::make_unique<reader_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
file::~file() noexcept = default;

//------------------------------------------------------------------------------

data::series_set::sptr file::scan()
{
    auto files = get_files();

    if(files.empty())
    {
        const auto& root = get_folder();

        SIGHT_THROW_IF(
            "The DICOM root directory is not set.",
            root.empty()
        );

        SIGHT_THROW_IF(
            "The directory '" << root << "' is not an existing folder.",
            !std::filesystem::exists(root) || !std::filesystem::is_directory(root)
        );

        // List recursively all files in the folder
        gdcm::Directory gdcm_directory;
        const auto file_count = gdcm_directory.Load(root.string(), true);

        SIGHT_THROW_IF(
            "The folder '" << root << "' does not contain any files.",
            file_count == 0
        );

        // We need to transform std::vector<std::string> to std::vector<std::filesystem::path>
        const auto& filenames = gdcm_directory.GetFilenames();
        std::transform(
            filenames.cbegin(),
            filenames.cend(),
            std::back_inserter(files),
            [](const auto& _v)
            {
                return _v;
            });
    }

    if(m_pimpl->cancel_requested())
    {
        m_pimpl->clear();
        return nullptr;
    }

    const auto& scanned = m_pimpl->scan_files(files);
    set_scanned(scanned);

    m_pimpl->progress(20);

    return scanned;
}

//------------------------------------------------------------------------------

data::series_set::sptr file::sort()
{
    if(!m_pimpl->m_scanned || m_pimpl->m_scanned->empty())
    {
        scan();
    }

    if(m_pimpl->cancel_requested())
    {
        m_pimpl->clear();
        return nullptr;
    }

    const auto& sorted = m_pimpl->sort();
    set_sorted(sorted);

    m_pimpl->progress(30);

    return sorted;
}

//------------------------------------------------------------------------------

void file::read()
{
    if(!m_pimpl->m_sorted || m_pimpl->m_sorted->empty())
    {
        sort();
    }

    if(m_pimpl->cancel_requested())
    {
        m_pimpl->clear();
        return;
    }

    m_pimpl->read();

    m_pimpl->progress(100);
}

//------------------------------------------------------------------------------

void file::set_filters(const data::series::SopKeywords& _filters)
{
    m_pimpl->m_filters = _filters;
}

//------------------------------------------------------------------------------

void file::set_scanned(const data::series_set::sptr& _scanned)
{
    m_pimpl->m_scanned = _scanned;

    // The sorted files are no more relevant
    m_pimpl->m_sorted.reset();
}

//------------------------------------------------------------------------------

void file::set_sorted(const data::series_set::sptr& _sorted)
{
    m_pimpl->m_sorted = _sorted;

    // No need to keep the scanned files, they are not used anymore
    m_pimpl->m_scanned.reset();
}

//------------------------------------------------------------------------------

core::jobs::base::sptr file::get_job() const
{
    return m_pimpl->m_job;
}

//------------------------------------------------------------------------------

void file::set_job(core::jobs::job::sptr _job)
{
    SIGHT_ASSERT("Some work have already be reported.", _job->get_done_work_units() == 0);
    m_pimpl->m_job = _job;
    m_pimpl->m_job->set_total_work_units(100);
}

} // namespace sight::io::dicom::reader
