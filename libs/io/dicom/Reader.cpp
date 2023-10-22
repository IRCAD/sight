/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "Reader.hpp"

#include "core/jobs/job.hpp"

#include <core/macros.hpp>
#include <core/tools/compare.hpp>

#include <data/dicom/Sop.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

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

namespace sight::io::dicom
{

// We consider that 1e-3 is a good enough precision for our purpose.
// All frames that have a z position closer than 1e-3 will be considered as the same.
static constexpr double Z_EPSILON = 1e-3;

struct FiducialSetWithMetadata
{
    data::fiducials_series::FiducialSet fiducialSet;
    std::string contentDate;
    std::string contentTime;
    std::optional<std::int32_t> instanceNumber;
    std::string contentLabel;
    std::string contentDescription;
    std::string contentCreatorName;
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
    static const std::vector<gdcm::Tag> UNIQUE_TAGS {
        // These will allow to identify the series.
        gdcm::Keywords::SeriesInstanceUID::GetTag(),
        gdcm::Keywords::SliceThickness::GetTag(),
        gdcm::Keywords::AcquisitionNumber::GetTag(),
        gdcm::Keywords::Rows::GetTag(),
        gdcm::Keywords::Columns::GetTag(),
        gdcm::Keywords::TemporalPositionIdentifier::GetTag(),
        gdcm::Keywords::TemporalPositionIndex::GetTag()
    };

    static const std::vector<gdcm::Tag> REQUESTED_TAGS =
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
                UNIQUE_TAGS.begin(),
                UNIQUE_TAGS.end()
            );

            return tmp;
        }();

    for(const auto& tag : REQUESTED_TAGS)
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

            // Filter, if needed
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
                const auto& sop_keyword = data::dicom::sop::keyword(found->second);

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
                    for(const auto& tag : UNIQUE_TAGS)
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
            const std::size_t instance = series ? series->numInstances() : 0;

            // If the series is not found, we create it
            if(!series)
            {
                series = std::make_shared<data::series>();
            }

            for(const auto& tag : REQUESTED_TAGS)
            {
                if(const auto& found = mapping.find(tag); found != mapping.end())
                {
                    series->setStringValue(
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

inline static data::image::Size compute_size(const data::series& _source, const gdcm::Image& _gdcm_image)
{
    const auto gdcm_num_dimensions = _gdcm_image.GetNumberOfDimensions();
    const auto& gdcm_dimensions    = _gdcm_image.GetDimensions();

    return {
        gdcm_num_dimensions > 0 ? gdcm_dimensions[0] : 1,
        gdcm_num_dimensions > 1 ? gdcm_dimensions[1] : 1,
        gdcm_num_dimensions > 2 ? gdcm_dimensions[2] : _source.numInstances()
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

inline static data::image::PixelFormat compute_format(
    const gdcm::Image& _gdcm_image,
    const std::string& _filename
)
{
    const auto gdcm_photometric_interpretation = _gdcm_image.GetPhotometricInterpretation();

    if(gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // PALETTE_COLOR is always expended as RGB
        return data::image::PixelFormat::RGB;
    }

    const auto gdcm_sample_per_pixel = _gdcm_image.GetPixelFormat().GetSamplesPerPixel();

    if(gdcm_sample_per_pixel == 1)
    {
        // No need to check, no color space conversion...
        return data::image::PixelFormat::GRAY_SCALE;
    }

    if(gdcm_sample_per_pixel == 3
       && (gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL_422
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_ICT
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_RCT
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::RGB))
    {
        return data::image::PixelFormat::RGB;
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
    return data::image::PixelFormat::UNDEFINED;
}

//------------------------------------------------------------------------------

inline static std::optional<double> compute_frame_position(const data::series& _series, std::size_t _instance)
{
    auto position    = _series.getImagePositionPatient(_instance);
    auto orientation = _series.getImageOrientationPatient(_instance);

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

    if(_series.numInstances() < 2)
    {
        SIGHT_WARN(
            "The Z spacing cannot be computed, there is not enough instances."
        );

        return _series.getSliceThickness();
    }

    for(std::size_t instance = 0, end = _series.numInstances() ; instance < end ; ++instance)
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

        return _series.getSliceThickness();
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

        return _series.getSliceThickness();
    }

    return first_spacing;
}

//------------------------------------------------------------------------------

inline static data::image::Spacing compute_spacing(
    const data::series& _source,
    const gdcm::Image& _gdcm_image
)
{
    /// @note Z spacing may have to be recomputed using ImagePositionPatient...
    const double* const gdcm_spacing = _gdcm_image.GetSpacing();

    // Use absolute value since gdcm sometimes return negative spacing, which is odd.
    data::image::Spacing spacing {std::abs(gdcm_spacing[0]), std::abs(gdcm_spacing[1]), std::abs(gdcm_spacing[2])};

    // Z Spacing correction
    // Overwrite only if GDCM returned the default value (1.0), since GDCM usually knows to compute it right
    if(core::tools::is_equal(spacing[2], 1.0))
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
    const data::image::PixelFormat& format = compute_format(_gdcm_image, _filename);

    SIGHT_THROW_IF(
        "Cannot guess the target pixel format to use while reading DICOM file '" << _filename << "'.",
        type == core::type::NONE || format == data::image::PixelFormat::UNDEFINED
    );

    if(_job && _job->cancel_requested())
    {
        return nullptr;
    }

    // Resize to the correct size, and allocate memory
    image_series->resize(size, type, format);

    // Spacing.
    image_series->setSpacing(compute_spacing(_source, _gdcm_image));

    // Origin
    const double* const origin = _gdcm_image.GetOrigin();
    image_series->setOrigin({origin[0], origin[1], origin[2]});

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

//------------------------------------------------------------------------------

inline static std::vector<double> tune_directions(const double* const _gdcm_direction_cosines)
{
    glm::dvec3 glm_u {_gdcm_direction_cosines[0], _gdcm_direction_cosines[1], _gdcm_direction_cosines[2]};
    glm::dvec3 glm_v {_gdcm_direction_cosines[3], _gdcm_direction_cosines[4], _gdcm_direction_cosines[5]};

    // Make them Orthogonal
    // This code is also found in ITK and is mostly a bugfix when direction vectors are not orthogonal.
    glm::dvec3 glm_w = glm::normalize(glm::cross(glm_u, glm_v));
    glm_u = glm::normalize(glm::cross(glm_v, glm_w));
    glm_v = glm::cross(glm_w, glm_u);

    return {glm_u.x, glm_u.y, glm_u.z, glm_v.x, glm_v.y, glm_v.z};
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
               !core::tools::is_equal(gdcm_intercept, 0.0))
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
               !core::tools::is_equal(gdcm_slope, 1.0))
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
    const bool split = gdcm_image.GetNumberOfDimensions() >= 3 && _source.numInstances() > 1;
    if(!_splitted_series || split)
    {
        if(_job && _job->cancel_requested())
        {
            return nullptr;
        }

        const auto& image_series = new_image_series(_source, _job, gdcm_image, gdcm_rescaler, filename);

        // User may have canceled the job
        if(image_series)
        {
            // Add the dataset to allow access to all DICOM attributes (not only the ones we have converted)
            image_series->setDataSet(gdcm_dataset);

            // Also save the file path. It could be useful to keep a link to the original file.
            image_series->set_file(filename);

            if(!image_series->isMultiFrame())
            {
                // Make direction vectors orthogonal. Also found in ITK
                // This is for some strange DICOM files that have non orthogonal direction vectors.
                /// @note This is not done for multi-frame images, because frame may be independently oriented.
                const auto& tuned_directions = tune_directions(gdcm_image.GetDirectionCosines());
                image_series->setImageOrientationPatient(tuned_directions);
            }

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
    image_series->setDataSet(gdcm_dataset, _instance);

    // Also save the file path. It could be useful to keep a link to the original file.
    image_series->set_file(filename, _instance);

    // Get the output buffer (as char* since gdcm takes char* as input)
    // If the series will be splitted by instance, we keep 0 as instance number
    char* const instance_buffer = &image_series->at<char>(0, 0, split ? 0 : _instance, 0);
    SIGHT_ASSERT("Null buffer.", instance_buffer != nullptr);

    // Compute the size
    const std::size_t instance_buffer_size =
        split ? image_series->getSizeInBytes()
              : image_series->getSizeInBytes() / std::max(std::size_t(1), _source.numInstances());

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
    // readImageInstance() returns a series set, because the series can be splitted in rare cases, like US 4D Volume.
    std::unique_ptr<std::vector<char> > gdcm_instance_buffer;
    auto splitted_series = read_image_instance(_source, _job, gdcm_instance_buffer, 0);

    if(!splitted_series)
    {
        // Job have been canceled
        return nullptr;
    }

    // Read the other instances if necessary
    for(std::size_t instance = 1, end = _source.numInstances() ; instance < end ; ++instance)
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

inline static data::series_set::sptr read_model(const data::series& /*unused*/, const core::jobs::job::sptr& /*unused*/)
{
    data::series_set::sptr splitted_series;

    /// @todo Implement model series reading

    return splitted_series;
}

//------------------------------------------------------------------------------

inline static std::vector<FiducialSetWithMetadata> read_fiducial_sets(const data::series& _series)
{
    gdcm::Reader reader;
    reader.SetFileName(_series.get_file().string().c_str());
    reader.Read();
    auto fiducials_series = std::make_shared<data::fiducials_series>();
    fiducials_series->setDataSet(reader.GetFile().GetDataSet());
    std::vector<FiducialSetWithMetadata> res;
    std::ranges::transform(
        fiducials_series->getFiducialSets(),
        std::back_inserter(res),
        [fiducials_series](data::fiducials_series::FiducialSet _fs) -> FiducialSetWithMetadata
        {
            return {
                .fiducialSet        = _fs,
                .contentDate        = fiducials_series->getContentDate(),
                .contentTime        = fiducials_series->getContentTime(),
                .instanceNumber     = fiducials_series->getInstanceNumber(),
                .contentLabel       = fiducials_series->getContentLabel(),
                .contentDescription = fiducials_series->getContentDescription(),
                .contentCreatorName = fiducials_series->getContentCreatorName()
            };
        });
    return res;
}

/// Private reader implementation
class Reader::ReaderImpl
{
public:

    /// Delete default constructors and assignment operators
    ReaderImpl(const ReaderImpl&)            = delete;
    ReaderImpl(ReaderImpl&&)                 = delete;
    ReaderImpl& operator=(const ReaderImpl&) = delete;
    ReaderImpl& operator=(ReaderImpl&&)      = delete;

    /// Constructor
    inline explicit ReaderImpl(Reader* const _reader) noexcept :
        M_READER(_reader)
    {
    }

    /// Default destructor
    inline ~ReaderImpl() noexcept = default;

    /// Pointer to the public interface
    Reader* const M_READER;

    /// Returns a list of DICOM series by scanning files using get_files()
    /// The files are NOT sorted!
    /// @return data::series_set::sptr: A set of series, with their associated files
    /// @throw std::runtime_error if the root directory is not an existing folder
    /// @throw std::runtime_error if there is no dicom files are found
    [[nodiscard]] inline data::series_set::sptr scanFiles(const std::vector<std::filesystem::path>& _files) const
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
    [[nodiscard]] inline data::series_set::sptr sort() const
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
            if(series->numInstances() <= 1)
            {
                continue;
            }

            // Try many sort strategies
            // Try first by Image position

            if(sortInstancesByImagePosition(series)
               || sortInstancesByContentTime(series)
               || sortInstancesByInstanceNumber(series)
               || sortInstancesByFilename(series))
            {
                continue;
            }
        }

        return m_scanned;
    }

    //------------------------------------------------------------------------------

    inline static bool sortInstancesByImagePosition(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->numInstances() ; instance < end ; ++instance)
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

    inline static bool sortInstancesByContentTime(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->numInstances() ; instance < end ; ++instance)
        {
            const auto& value = _series->getContentTime(instance);

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

    inline static bool sortInstancesByInstanceNumber(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->numInstances() ; instance < end ; ++instance)
        {
            const auto& value = _series->getInstanceNumber(instance);

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

    inline static bool sortInstancesByFilename(const data::series::sptr& _series)
    {
        // Use a map to sort for us....
        std::map<std::filesystem::path, std::size_t> sorter;

        for(std::size_t instance = 0, end = _series->numInstances() ; instance < end ; ++instance)
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

    inline void read()
    {
        SIGHT_THROW_IF(
            "There is no DICOM file to read.",
            !m_sorted || m_sorted->empty()
        );

        // Instantiate or reuse the output series set
        if(const auto& object = std::dynamic_pointer_cast<data::series_set>(M_READER->m_object.lock()); object)
        {
            m_read = object;
            m_read->clear();
        }
        else
        {
            m_read = std::make_shared<data::series_set>();
            M_READER->set_object(m_read);
        }

        std::vector<FiducialSetWithMetadata> fiducial_sets;

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

            if(source->get_dicom_type() == data::series::dicom_t::IMAGE)
            {
                // Read an image series
                splitted_series = read_image(*source, m_job);
            }
            else if(source->get_dicom_type() == data::series::dicom_t::MODEL)
            {
                // Read a model series
                splitted_series = read_model(*source, m_job);
            }
            else if(source->get_dicom_type() == data::series::dicom_t::FIDUCIALS)
            {
                std::ranges::copy(read_fiducial_sets(*source), std::back_inserter(fiducial_sets));
            }
            else
            {
                SIGHT_THROW("Unsupported DICOM IOD '" << data::dicom::sop::get(source->getSOPKeyword()).m_name << "'.");
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
        for(const FiducialSetWithMetadata& fiducial_set : fiducial_sets)
        {
            for(const data::series::sptr& series : *m_read)
            {
                auto image_series = std::dynamic_pointer_cast<data::image_series>(series);
                auto model_series = std::dynamic_pointer_cast<data::model_series>(series);
                if(image_series == nullptr && model_series == nullptr)
                {
                    break;
                }

                bool fiducial_set_is_relevant = fiducial_set.fiducialSet.frameOfReferenceUID && series->getStringValue(
                    data::dicom::attribute::Keyword::FrameOfReferenceUID
                )
                                                == fiducial_set.fiducialSet.frameOfReferenceUID;
                if(!fiducial_set_is_relevant && fiducial_set.fiducialSet.referencedImageSequence)
                {
                    for(const data::fiducials_series::ReferencedImage& referenced_image :
                        *fiducial_set.fiducialSet.referencedImageSequence)
                    {
                        // TODO: Take ReferencedSegmentNumber into account for Segmentation IOD
                        if(referenced_image.referencedSOPClassUID
                           == data::dicom::sop::get(series->getSOPKeyword()).m_uid
                           && referenced_image.referencedSOPInstanceUID == series->getSOPInstanceUID())
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
                        fiducials_series      = image_series->getFiducials();
                        series_have_fiducials = image_series->hasFiducials();
                    }
                    else if(model_series != nullptr)
                    {
                        fiducials_series      = model_series->getFiducials();
                        series_have_fiducials = model_series->hasFiducials();
                    }

                    if(!series_have_fiducials)
                    {
                        // It is the first fiducial set to be appended to this fiducials series; set fiducials metadata
                        fiducials_series->setContentDate(fiducial_set.contentDate);
                        fiducials_series->setContentTime(fiducial_set.contentTime);
                        fiducials_series->setInstanceNumber(fiducial_set.instanceNumber);
                        fiducials_series->setContentLabel(fiducial_set.contentLabel);
                        fiducials_series->setContentDescription(fiducial_set.contentDescription);
                        fiducials_series->setContentCreatorName(fiducial_set.contentCreatorName);
                    }

                    fiducials_series->appendFiducialSet(fiducial_set.fiducialSet);
                }
            }
        }

        // Not needed anymore, free some memory
        clear();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline bool cancel_requested() const noexcept
    {
        return m_job && m_job->cancel_requested();
    }

    //------------------------------------------------------------------------------

    inline void progress(std::uint64_t _units) const
    {
        if(m_job)
        {
            m_job->done_work(_units);
        }
    }

    //------------------------------------------------------------------------------

    inline void clear()
    {
        m_scanned.reset();
        m_sorted.reset();
    }

    /// The default filter to select only some type (Image, Model, ...) of DICOM files.
    data::series::SopKeywords m_filters {};

    /// Contains the list of files to sort and read.
    /// Usually, it is filed by user after showing a selection dialog, but calling read() will fill it automatically.
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

Reader::Reader() :
    core::location::single_folder(),
    core::location::multiple_files(),
    m_pimpl(std::make_unique<ReaderImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Reader::~Reader() noexcept = default;

//------------------------------------------------------------------------------

data::series_set::sptr Reader::scan()
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
        std::transform(filenames.cbegin(), filenames.cend(), std::back_inserter(files), [](const auto& _v){return _v;});
    }

    if(m_pimpl->cancel_requested())
    {
        m_pimpl->clear();
        return nullptr;
    }

    const auto& scanned = m_pimpl->scanFiles(files);
    setScanned(scanned);

    m_pimpl->progress(20);

    return scanned;
}

//------------------------------------------------------------------------------

data::series_set::sptr Reader::sort()
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
    setSorted(sorted);

    m_pimpl->progress(30);

    return sorted;
}

//------------------------------------------------------------------------------

void Reader::read()
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

void Reader::setFilters(const data::series::SopKeywords& _filters)
{
    m_pimpl->m_filters = _filters;
}

//------------------------------------------------------------------------------

void Reader::setScanned(const data::series_set::sptr& _scanned)
{
    m_pimpl->m_scanned = _scanned;

    // The sorted files are no more relevant
    m_pimpl->m_sorted.reset();
}

//------------------------------------------------------------------------------

void Reader::setSorted(const data::series_set::sptr& _sorted)
{
    m_pimpl->m_sorted = _sorted;

    // No need to keep the scanned files, they are not used anymore
    m_pimpl->m_scanned.reset();
}

//------------------------------------------------------------------------------

core::jobs::base::sptr Reader::getJob() const
{
    return m_pimpl->m_job;
}

//------------------------------------------------------------------------------

void Reader::setJob(core::jobs::job::sptr _job)
{
    SIGHT_ASSERT("Some work have already be reported.", _job->get_done_work_units() == 0);
    m_pimpl->m_job = _job;
    m_pimpl->m_job->set_total_work_units(100);
    m_pimpl->m_job->done_work(10);
}

} // namespace sight::io::dicom
