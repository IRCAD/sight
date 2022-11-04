/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <core/macros.hpp>
#include <core/tools/compare.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

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

//------------------------------------------------------------------------------

inline static data::SeriesSet::sptr scanGDCMFiles(
    const gdcm::Directory::FilenamesType& files,
    data::Series::DicomTypes filter = 0
)
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
    const auto result = scanner.Scan(files);

    SIGHT_THROW_IF(
        "There is no DICOM files among the scanned files.",
        !result
    );

    // This map will used to merge DICOM instance that belongs to the same series
    std::map<std::string, data::Series::sptr> unique_series;

    // The final list of series, with some attributes filled to allow sorting
    auto series_set = data::SeriesSet::New();

    // Convert to our own format
    for(const auto& file : files)
    {
        if(const char* const key = file.c_str(); scanner.IsKey(key))
        {
            const auto& mapping = scanner.GetMapping(key);

            // Filter, if needed
            if(filter != 0)
            {
                if(const auto& found = mapping.find(gdcm::Keywords::SOPClassUID::GetTag()); found != mapping.end())
                {
                    const auto type = static_cast<data::Series::DicomTypes>(data::Series::getDicomType(found->second));
                    if(type == 0 || (type & filter) != type)
                    {
                        continue;
                    }
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

            // If we cannot build an unique series identifier, we skip this file, it is certainly bogus.
            if(unique_series_identifier.empty())
            {
                continue;
            }

            // Retrieve the associated series and associated DICOM files
            auto& series               = unique_series[unique_series_identifier];
            const std::size_t instance = series ? series->numInstances() : 0;

            // If the series is not found, we create it
            if(!series)
            {
                series = data::Series::New();
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
            series->setFile(file, instance);

            // Add the series to the set
            series_set->push_back(series);
        }
    }

    return series_set;
}

//------------------------------------------------------------------------------

inline static auto convertGDCMImage(
    gdcm::Image& gdcm_image,
    const std::string& filename
)
{
    // Decompress the Pixel Data buffer if needed
    if(gdcm_image.GetTransferSyntax().IsEncapsulated())
    {
        gdcm::ImageChangeTransferSyntax changer;
        changer.SetInput(gdcm_image);
        changer.SetTransferSyntax(gdcm::TransferSyntax::ImplicitVRLittleEndian);

        SIGHT_THROW_IF(
            "Cannot change the transfer syntax of DICOM file '" << filename << "'.",
            !changer.Change()
        );

        gdcm_image = changer.GetOutput();
    }

    // We only support 0 as planar configuration (r1g1b1, r2g2b2, ... not r1r2..g1g2..b1b2)
    if(gdcm_image.GetPlanarConfiguration() == 1)
    {
        gdcm::ImageChangePlanarConfiguration changer;
        changer.SetInput(gdcm_image);
        changer.SetPlanarConfiguration(0);

        SIGHT_THROW_IF(
            "Cannot change the planar configuration of DICOM file '" << filename << "'.",
            !changer.Change()
        );

        gdcm_image = changer.GetOutput();
    }

    // For palette color image, we need to convert palette values to real RGB values
    if(const auto& photometric_interpretation = gdcm_image.GetPhotometricInterpretation();
       photometric_interpretation == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // Apply lookup table
        gdcm::ImageApplyLookupTable applier;
        applier.SetInput(gdcm_image);

        SIGHT_THROW_IF(
            "Cannot Apply the lookup table of DICOM file '" << filename << "'.",
            !applier.Apply()
        );

        gdcm_image = applier.GetOutput();
    }
    // Sight assumes that the minimum sample value is intended to be displayed as black after any VOI grayscale
    // transformations have been performed.
    else if(photometric_interpretation == gdcm::PhotometricInterpretation::MONOCHROME1)
    {
        gdcm::ImageChangePhotometricInterpretation changer;
        changer.SetInput(gdcm_image);
        changer.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);

        SIGHT_THROW_IF(
            "Cannot change the photometric interpretation of DICOM file '" << filename << "'.",
            !changer.Change()
        );

        gdcm_image = changer.GetOutput();
    }

    return gdcm_image;
}

//------------------------------------------------------------------------------

inline static data::Image::Size computeSize(const data::Series& source, const gdcm::Image& gdcm_image)
{
    const auto gdcm_num_dimensions = gdcm_image.GetNumberOfDimensions();
    const auto& gdcm_dimensions    = gdcm_image.GetDimensions();

    return {
        gdcm_num_dimensions > 0 ? gdcm_dimensions[0] : 1,
        gdcm_num_dimensions > 1 ? gdcm_dimensions[1] : 1,
        gdcm_num_dimensions > 2 ? gdcm_dimensions[2] : source.numInstances()
    };
}

//------------------------------------------------------------------------------

inline static core::Type computeType(
    const gdcm::Image& gdcm_image,
    const std::unique_ptr<gdcm::Rescaler>& gdcm_rescaler
)
{
    const auto& gdcm_pixel_format = gdcm_image.GetPixelFormat();

    if(gdcm_pixel_format == gdcm::PixelFormat::SINGLEBIT)
    {
        // In all cases, Sight reads single bit per pixel images as uint8
        return core::Type::UINT8;
    }

    // Let the target type be guessed by GDCM
    const auto& gdcm_rescaled_pixel_type = gdcm_rescaler
                                           ? gdcm_rescaler->ComputeInterceptSlopePixelType()
                                           : gdcm_pixel_format.GetScalarType();

    // Dumb boilerplate code to convert the GDCM pixel type to a Sight pixel type
    switch(gdcm_rescaled_pixel_type)
    {
        case gdcm::PixelFormat::UINT8:
            return core::Type::UINT8;

        case gdcm::PixelFormat::INT8:
            return core::Type::INT8;

        case gdcm::PixelFormat::UINT16:
            return core::Type::UINT16;

        case gdcm::PixelFormat::INT16:
            return core::Type::INT16;

        case gdcm::PixelFormat::UINT32:
            return core::Type::UINT32;

        case gdcm::PixelFormat::INT32:
            return core::Type::INT32;

        case gdcm::PixelFormat::UINT64:
            return core::Type::UINT64;

        case gdcm::PixelFormat::INT64:
            return core::Type::INT64;

        case gdcm::PixelFormat::FLOAT32:
            return core::Type::FLOAT;

        case gdcm::PixelFormat::FLOAT64:
            return core::Type::DOUBLE;

        default:
            return core::Type::NONE;
    }
}

//------------------------------------------------------------------------------

inline static data::Image::PixelFormat computeFormat(
    const gdcm::Image& gdcm_image,
    const std::string& filename
)
{
    const auto gdcm_photometric_interpretation = gdcm_image.GetPhotometricInterpretation();

    if(gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // PALETTE_COLOR is always expended as RGB
        return data::Image::PixelFormat::RGB;
    }

    const auto gdcm_sample_per_pixel = gdcm_image.GetPixelFormat().GetSamplesPerPixel();

    if(gdcm_sample_per_pixel == 1)
    {
        // No need to check, no color space conversion...
        return data::Image::PixelFormat::GRAY_SCALE;
    }

    if(gdcm_sample_per_pixel == 3
       && (gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL_422
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_ICT
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_RCT
           || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::RGB))
    {
        return data::Image::PixelFormat::RGB;
    }

    SIGHT_THROW_IF(
        "Retired photometric interpretation used in DICOM file '" << filename << "'.",
        gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::ARGB
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::CMYK
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::HSV
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_PARTIAL_420
        || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_PARTIAL_422
    );

    // Unsupported...
    return data::Image::PixelFormat::UNDEFINED;
}

//------------------------------------------------------------------------------

inline static std::optional<double> computeFramePosition(const data::Series& series, std::size_t instance)
{
    auto position    = series.getImagePositionPatient(instance);
    auto orientation = series.getImageOrientationPatient(instance);

    if(position.size() != 3 || orientation.size() != 6)
    {
        // Fallback to gdcm::ImageReader if the position is not available
        // This is of course slower...
        const auto& file = series.getFile(instance);

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

inline static std::optional<double> computeZSpacing(const data::Series& series)
{
    // Use a map to sort for us....
    std::map<std::int64_t, double> sorted_positions;

    if(series.numInstances() < 2)
    {
        SIGHT_WARN(
            "The Z spacing cannot be computed, there is not enough instances."
        );

        return series.getSliceThickness();
    }

    for(std::size_t instance = 0, end = series.numInstances() ; instance < end ; ++instance)
    {
        const auto& value = computeFramePosition(series, instance);

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

        return series.getSliceThickness();
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

        return series.getSliceThickness();
    }

    return first_spacing;
}

//------------------------------------------------------------------------------

inline static data::Image::Spacing computeSpacing(
    const data::Series& source,
    const gdcm::Image& gdcm_image
)
{
    /// @note Z spacing may have to be recomputed using ImagePositionPatient...
    const double* const gdcm_spacing = gdcm_image.GetSpacing();

    // Use absolute value since gdcm sometimes return negative spacing, which is odd.
    data::Image::Spacing spacing {std::abs(gdcm_spacing[0]), std::abs(gdcm_spacing[1]), std::abs(gdcm_spacing[2])};

    // Z Spacing correction
    // Overwrite only if GDCM returned the default value (1.0), since GDCM usually knows to compute it right
    if(core::tools::is_equal(spacing[2], 1.0))
    {
        const auto& computed_spacing = computeZSpacing(source);

        if(computed_spacing)
        {
            spacing[2] = *computed_spacing;
        }
    }

    return spacing;
}

//------------------------------------------------------------------------------

inline static data::ImageSeries::sptr newImageSeries(
    const data::Series& source,
    const core::jobs::Job::sptr& job,
    const gdcm::Image& gdcm_image,
    const std::unique_ptr<gdcm::Rescaler>& gdcm_rescaler,
    const std::string& filename
)
{
    // Create a new series and set the common dataset
    /// @note Window center / width is directly read from dataset, nothing to set here
    auto image_series = data::ImageSeries::New();

    // Retrieve the image information
    // Target sizes (that's easy)
    const auto& size = computeSize(source, gdcm_image);

    // Target type, a bit more complicated
    const core::Type& type = computeType(gdcm_image, gdcm_rescaler);

    // Target PixelFormat, even more complicated
    const data::Image::PixelFormat& format = computeFormat(gdcm_image, filename);

    SIGHT_THROW_IF(
        "Cannot guess the target pixel format to use while reading DICOM file '" << filename << "'.",
        type == core::Type::NONE || format == data::Image::PixelFormat::UNDEFINED
    );

    if(job && job->cancelRequested())
    {
        return nullptr;
    }

    // Resize to the correct size, and allocate memory
    image_series->resize(size, type, format);

    // Spacing.
    image_series->setSpacing(computeSpacing(source, gdcm_image));

    // Origin
    const double* const origin = gdcm_image.GetOrigin();
    image_series->setOrigin({origin[0], origin[1], origin[2]});

    return image_series;
}

//------------------------------------------------------------------------------

template<typename T>
constexpr static void ybrToRgb(T* buffer, std::size_t size, std::uint16_t stored_bits = 8)
{
    for(std::size_t i = 0 ; i < size ; i += 3)
    {
        gdcm::ImageChangePhotometricInterpretation::YBR2RGB<T>(
            &buffer[i],
            &buffer[i],
            stored_bits
        );
    }
}

//------------------------------------------------------------------------------

inline static const char* readGDCMBuffer(
    const gdcm::Image& gdcm_image,
    char* const buffer,
    const std::string& filename
)
{
    SIGHT_ASSERT("Null buffer.", buffer != nullptr);

    SIGHT_THROW_IF(
        "Cannot read Pixel Data from DICOM file '" << filename << "'.",
        buffer == nullptr || !gdcm_image.GetBuffer(buffer)
    );

    return buffer;
}

//------------------------------------------------------------------------------

inline static bool readBuffer(
    const core::jobs::Job::sptr& job,
    const gdcm::Image& gdcm_image,
    const std::unique_ptr<gdcm::Rescaler>& gdcm_rescaler,
    std::unique_ptr<std::vector<char> >& gdcm_instance_buffer,
    char* const instance_buffer,
    const std::size_t instance_buffer_size,
    const std::string& filename
)
{
    if(job && job->cancelRequested())
    {
        return false;
    }

    // Get the input buffer size
    const std::size_t gdcm_buffer_size = gdcm_image.GetBufferLength();
    SIGHT_THROW_IF(
        "Source buffer size cannot be read in DICOM file '" << filename << "'.",
        gdcm_buffer_size == 0
    );

    if(const auto& gdcm_pixel_format = gdcm_image.GetPixelFormat();
       gdcm_pixel_format == gdcm::PixelFormat::SINGLEBIT)
    {
        SIGHT_ASSERT("Instance Buffer size must large enough.", instance_buffer_size == gdcm_buffer_size * 8);

        if(job && job->cancelRequested())
        {
            return false;
        }

        // Read the buffer. Use the buffer from the image series object
        readGDCMBuffer(gdcm_image, instance_buffer, filename);

        if(job && job->cancelRequested())
        {
            return false;
        }

        // We need to convert bits to bytes...
        /// @note iterate from the end to the beginning, so we can use the same buffer
        auto* end_instance_buffer = reinterpret_cast<std::uint8_t*>(
            instance_buffer + instance_buffer_size - 1
        );

        for(auto i = std::streamsize(gdcm_buffer_size) ; --i >= 0 ; )
        {
            const auto byte = std::uint8_t(instance_buffer[i]);

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
    else if(gdcm_rescaler)
    {
        // Apply rescale slope / intercept
        SIGHT_ASSERT(
            "Instance Buffer size must large enough.",
            instance_buffer_size >= (
                gdcm_buffer_size
                * gdcm::PixelFormat(gdcm_rescaler->ComputeInterceptSlopePixelType()).GetPixelSize()
                / std::max(gdcm_pixel_format.GetPixelSize(), std::uint8_t(1))
            )
        );

        if(job && job->cancelRequested())
        {
            return false;
        }

        // Prepare the input buffer
        if(!gdcm_instance_buffer)
        {
            gdcm_instance_buffer = std::make_unique<std::vector<char> >(gdcm_buffer_size);
        }
        else if(gdcm_instance_buffer->size() < gdcm_buffer_size)
        {
            gdcm_instance_buffer->resize(gdcm_buffer_size);
        }

        // Read raw input buffer
        const char* const gdcm_buffer = readGDCMBuffer(gdcm_image, gdcm_instance_buffer->data(), filename);

        if(job && job->cancelRequested())
        {
            return false;
        }

        gdcm_rescaler->Rescale(instance_buffer, gdcm_buffer, gdcm_buffer_size);
    }
    else if(const auto gdcm_photometric_interpretation = gdcm_image.GetPhotometricInterpretation();
            gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL
            || gdcm_photometric_interpretation == gdcm::PhotometricInterpretation::YBR_FULL_422)
    {
        // Convert YBR to RGB
        SIGHT_THROW_IF(
            "Source buffer is incompatible with the Photometric Interpretation in DICOM file '" << filename << "'.",
            gdcm_pixel_format.GetSamplesPerPixel() != 3 && gdcm_buffer_size % 3 != 0
        );

        SIGHT_ASSERT(
            "Instance Buffer size must large enough.",
            instance_buffer_size >= gdcm_buffer_size
        );

        if(job && job->cancelRequested())
        {
            return false;
        }

        // Read the buffer. Use the buffer from the image series object
        readGDCMBuffer(gdcm_image, instance_buffer, filename);

        // 99% of the time, the pixel type is 8 bits per component, but it can be 16 bits too.
        switch(gdcm_pixel_format.GetBitsAllocated())
        {
            case 8:
            {
                ybrToRgb(
                    reinterpret_cast<std::uint8_t*>(instance_buffer),
                    gdcm_buffer_size,
                    gdcm_pixel_format.GetBitsStored()
                );
                break;
            }

            case 16:
            {
                ybrToRgb(
                    reinterpret_cast<std::uint16_t*>(instance_buffer),
                    gdcm_buffer_size,
                    gdcm_pixel_format.GetBitsStored()
                );
                break;
            }

            default:
                SIGHT_THROW("Unsupported allocated bits per pixel in DICOM file '" << filename << "'.");
        }
    }
    else
    {
        // Nothing to do other than copying the buffer
        readGDCMBuffer(gdcm_image, instance_buffer, filename);
    }

    return true;
}

//------------------------------------------------------------------------------

inline static std::vector<double> tuneDirections(const double* const gdcm_direction_cosines)
{
    glm::dvec3 glm_u {gdcm_direction_cosines[0], gdcm_direction_cosines[1], gdcm_direction_cosines[2]};
    glm::dvec3 glm_v {gdcm_direction_cosines[3], gdcm_direction_cosines[4], gdcm_direction_cosines[5]};

    // Make them Orthogonal
    // This code is also found in ITK and is mostly a bugfix when direction vectors are not orthogonal.
    glm::dvec3 glm_w = glm::normalize(glm::cross(glm_u, glm_v));
    glm_u = glm::normalize(glm::cross(glm_v, glm_w));
    glm_v = glm::cross(glm_w, glm_u);

    return {glm_u.x, glm_u.y, glm_u.z, glm_v.x, glm_v.y, glm_v.z};
}

//------------------------------------------------------------------------------

inline static data::SeriesSet::sptr readImageInstance(
    const data::Series& source,
    const core::jobs::Job::sptr& job,
    std::unique_ptr<std::vector<char> >& gdcm_instance_buffer,
    std::size_t instance                  = 0,
    data::SeriesSet::sptr splitted_series = nullptr
)
{
    if(job && job->cancelRequested())
    {
        return nullptr;
    }

    // Read the DICOM file using GDCM ImageReader
    gdcm::ImageReader gdcm_reader;
    const std::string& filename = source.getFile(instance).string();
    gdcm_reader.SetFileName(filename.c_str());

    SIGHT_INFO("Reading DICOM file '" << filename << "'.");
    SIGHT_THROW_IF("Cannot read DICOM file '" << filename << "'.", !gdcm_reader.Read());

    // Get the image and convert it to a suitable format
    const auto& gdcm_image = convertGDCMImage(gdcm_reader.GetImage(), filename);

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
                gdcm::Keywords::RescaleIntercept rescaleIntercept;
                rescaleIntercept.SetFromDataSet(gdcm_dataset);
                return std::make_pair(true, rescaleIntercept.GetValue());
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
                gdcm::Keywords::RescaleSlope rescaleSlope;
                rescaleSlope.SetFromDataSet(gdcm_dataset);
                return std::make_pair(true, rescaleSlope.GetValue());
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
    const bool split = gdcm_image.GetNumberOfDimensions() >= 3 && source.numInstances() > 1;
    if(!splitted_series || split)
    {
        if(job && job->cancelRequested())
        {
            return nullptr;
        }

        const auto& image_series = newImageSeries(source, job, gdcm_image, gdcm_rescaler, filename);

        // User may have canceled the job
        if(image_series)
        {
            image_series->setDataSet(gdcm_dataset);

            // Make direction vectors orthogonal. Also found in ITK
            // This is for some strange DICOM files that have non orthogonal direction vectors.
            const auto& tuned_directions = tuneDirections(gdcm_image.GetDirectionCosines());
            image_series->setImageOrientationPatient(tuned_directions);

            // Add the series to a new dataset
            if(!splitted_series)
            {
                splitted_series = data::SeriesSet::New();
            }

            splitted_series->push_back(image_series);
        }
    }

    if(job && job->cancelRequested())
    {
        return nullptr;
    }

    // Use the last series as current series
    auto image_series    = std::static_pointer_cast<data::ImageSeries>(splitted_series->back());
    const auto dump_lock = image_series->dump_lock();
    image_series->setDataSet(gdcm_dataset, instance);

    // Get the output buffer (as char* since gdcm takes char* as input)
    // If the series will be splitted by instance, we keep 0 as instance number
    char* const instance_buffer = &image_series->at<char>(0, 0, split ? 0 : instance, 0);
    SIGHT_ASSERT("Null buffer.", instance_buffer != nullptr);

    // Compute the size
    const std::size_t instance_buffer_size =
        split ? image_series->getSizeInBytes()
              : image_series->getSizeInBytes() / std::max(std::size_t(1), source.numInstances());

    // Read the image data and fill the image series
    if(!readBuffer(
           job,
           gdcm_image,
           gdcm_rescaler,
           gdcm_instance_buffer,
           instance_buffer,
           instance_buffer_size,
           filename
    ))
    {
        // Job have been canceled
        return nullptr;
    }

    return splitted_series;
}

//------------------------------------------------------------------------------

inline static data::SeriesSet::sptr readImage(const data::Series& source, const core::jobs::Job::sptr& job)
{
    if(job && job->cancelRequested())
    {
        return nullptr;
    }

    // Read first instance to get image information
    // readImageInstance() returns a series set, because the series can be splitted in rare cases, like US 4D Volume.
    std::unique_ptr<std::vector<char> > gdcm_instance_buffer;
    auto splitted_series = readImageInstance(source, job, gdcm_instance_buffer, 0);

    if(!splitted_series)
    {
        // Job have been canceled
        return nullptr;
    }

    // Read the other instances if necessary
    for(std::size_t instance = 1, end = source.numInstances() ; instance < end ; ++instance)
    {
        if(job && job->cancelRequested())
        {
            return nullptr;
        }

        readImageInstance(source, job, gdcm_instance_buffer, instance, splitted_series);
    }

    for(const auto& series : *splitted_series)
    {
        auto image_series = std::static_pointer_cast<data::ImageSeries>(series);

        if(data::helper::MedicalImage::checkImageValidity(image_series))
        {
            data::helper::MedicalImage::updateDefaultTransferFunction(*image_series);
            data::helper::MedicalImage::checkImageSliceIndex(image_series);
        }

        ///@todo check if we must rotate the buffer to match ImageOrientationPatient. Not sure it is a good idea...
    }

    return splitted_series;
}

//------------------------------------------------------------------------------

inline static data::SeriesSet::sptr readModel(const data::Series& /*unused*/, const core::jobs::Job::sptr& /*unused*/)
{
    data::SeriesSet::sptr splitted_series;

    /// @todo Implement model series reading

    return splitted_series;
}

/// Private SReader implementation
class Reader::ReaderImpl
{
public:

    /// Delete default constructors and assignment operators
    ReaderImpl(const ReaderImpl&)            = delete;
    ReaderImpl(ReaderImpl&&)                 = delete;
    ReaderImpl& operator=(const ReaderImpl&) = delete;
    ReaderImpl& operator=(ReaderImpl&&)      = delete;

    /// Constructor
    inline explicit ReaderImpl(Reader* const reader) noexcept :
        m_reader(reader)
    {
    }

    /// Default destructor
    inline ~ReaderImpl() noexcept = default;

    /// Pointer to the public interface
    Reader* const m_reader;

    /// Returns a list of DICOM series by scanning files using getFiles()
    /// The files are NOT sorted!
    /// @return data::SeriesSet::sptr: A set of series, with their associated files
    /// @throw std::runtime_error if the root directory is not an existing folder
    /// @throw std::runtime_error if there is no dicom files are found
    [[nodiscard]] inline data::SeriesSet::sptr scanFiles(const std::vector<std::filesystem::path>& files) const
    {
        // Convert std::vector<std::filesystem::path> to std::vector<std::string>
        gdcm::Directory::FilenamesType gdcm_files;

        for(const auto& file : files)
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

        return scanGDCMFiles(gdcm_files, m_filter);
    }

    /// Returns a list of DICOM series with associated files sorted
    /// @return data::SeriesSet::sptr: A set of series, with their associated files sorted
    /// @throw std::runtime_error if there is no scanned series
    [[nodiscard]] inline data::SeriesSet::sptr sort() const
    {
        SIGHT_THROW_IF(
            "There is no DICOM file to sort.",
            !m_scanned || m_scanned->empty()
        );

        for(const auto& series : *m_scanned)
        {
            if(cancelRequested())
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

    inline static bool sortInstancesByImagePosition(const data::Series::sptr& series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = series->numInstances() ; instance < end ; ++instance)
        {
            const auto& value = computeFramePosition(*series, instance);

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
            [](const auto& v){return v.second;});

        return series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    inline static bool sortInstancesByContentTime(const data::Series::sptr& series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = series->numInstances() ; instance < end ; ++instance)
        {
            const auto& value = series->getContentTime(instance);

            if(value.empty())
            {
                // No need to continue if we cannot get the instance number for one frame
                return false;
            }

            // Convert the time to a number
            // *INDENT-OFF*
            using namespace std::chrono_literals;
            std::chrono::hours hh = 0h;
            // *INDENT-ON*

            // Start to parse DICOM time
            ///@note Maybe it would be a good idea to export this function as DICOM TS are used elsewhere...
            if(value.length() >= 2)
            {
                try
                {
                    hh = std::chrono::hours(std::stoi(value.substr(0, 2)));
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

            std::chrono::minutes mm = 0min;

            if(value.length() >= 4)
            {
                try
                {
                    mm = std::chrono::minutes(std::stoi(value.substr(2, 4)));
                }
                catch(...)
                {
                    // If length >= 4, minutes shall be present
                    return false;
                }
            }

            std::chrono::seconds ss = 0s;

            if(value.length() >= 6)
            {
                try
                {
                    ss = std::chrono::seconds(std::stoi(value.substr(4, 6)));
                }
                catch(...)
                {
                    // If length >= 6, seconds shall be present
                    return false;
                }
            }

            std::chrono::microseconds ffffff = 0us;

            if(value.length() >= 8)
            {
                try
                {
                    // Do not forget '.' after the seconds
                    auto us = value.substr(7);

                    // Fill with trailing 0 to always have microseconds
                    us.resize(6, '0');

                    ffffff = std::chrono::microseconds(std::stoi(us));
                }
                catch(...)
                {
                    // If length >= 8, microseconds shall be present
                    return false;
                }
            }

            // Let the map sort the frames
            const std::int64_t index =
                std::chrono::duration_cast<std::chrono::microseconds>(hh + mm + ss + ffffff).count();
            sorter.insert_or_assign(index, instance);
        }

        // Finally, we can sort the frames in the series
        std::vector<std::size_t> sorted;
        sorted.reserve(sorter.size());

        std::transform(
            sorter.cbegin(),
            sorter.cend(),
            std::back_inserter(sorted),
            [](const auto& v){return v.second;});

        return series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    inline static bool sortInstancesByInstanceNumber(const data::Series::sptr& series)
    {
        // Use a map to sort for us....
        std::map<std::int64_t, std::size_t> sorter;

        for(std::size_t instance = 0, end = series->numInstances() ; instance < end ; ++instance)
        {
            const auto& value = series->getInstanceNumber(instance);

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
            [](const auto& v){return v.second;});

        return series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    inline static bool sortInstancesByFilename(const data::Series::sptr& series)
    {
        // Use a map to sort for us....
        std::map<std::filesystem::path, std::size_t> sorter;

        for(std::size_t instance = 0, end = series->numInstances() ; instance < end ; ++instance)
        {
            const auto& value = series->getFile(instance);

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
            [](const auto& v){return v.second;});

        return series->sort(sorted);
    }

    //------------------------------------------------------------------------------

    inline void read()
    {
        SIGHT_THROW_IF(
            "There is no DICOM file to read.",
            !m_sorted || m_sorted->empty()
        );

        // Instantiate or reuse the output series set
        if(const auto& object = data::SeriesSet::dynamicCast(m_reader->m_object.lock()); object)
        {
            m_read = object;
            m_read->clear();
        }
        else
        {
            m_read = data::SeriesSet::New();
            m_reader->setObject(m_read);
        }

        // Start reading selected series
        for(const auto& source : *m_sorted)
        {
            if(cancelRequested())
            {
                clear();
                return;
            }

            // Very unusual but we can have a series with several volumes, like in 4D Volume US
            // Therefore, we use a series set, so we can split the series if needed.
            data::SeriesSet::sptr splitted_series;

            if(source->getDicomType() == data::Series::DicomType::IMAGE)
            {
                // Read an image series
                splitted_series = readImage(*source, m_job);
            }
            else if(source->getDicomType() == data::Series::DicomType::MODEL)
            {
                // Read a model series
                splitted_series = readModel(*source, m_job);
            }
            else
            {
                SIGHT_THROW("Unsupported DICOM IOD '" << source->getSOPClassName() << "'.");
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

        // Not needed anymore, free some memory
        clear();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline bool cancelRequested() const noexcept
    {
        return m_job && m_job->cancelRequested();
    }

    //------------------------------------------------------------------------------

    inline void progress(std::uint64_t units) const
    {
        if(m_job)
        {
            m_job->doneWork(units);
        }
    }

    //------------------------------------------------------------------------------

    inline void clear()
    {
        m_scanned.reset();
        m_sorted.reset();
    }

    /// The default filter to select only some type (Image, Model, ...) of DICOM files.
    data::Series::DicomTypes m_filter {0};

    /// Contains the list of files to sort and read.
    /// Usually, it is filed by user after showing a selection dialog, but calling read() will fill it automatically.
    data::SeriesSet::sptr m_scanned;

    /// Contains the list of sorted files to read.
    /// Usually, it is filed when calling read(), but user can set it to bypass default sort algorithms.
    data::SeriesSet::sptr m_sorted;

    /// The final output
    /// This allows to keep a reference as GenericObjectReader / IObjectReader only keep a weak_ptr to the output.
    data::SeriesSet::sptr m_read;

    /// The default job. Allows to watch for cancellation and report progress.
    core::jobs::Job::sptr m_job;
};

Reader::Reader(io::base::reader::IObjectReader::Key /*unused*/) :
    core::location::SingleFolder(),
    core::location::MultipleFiles(),
    m_pimpl(std::make_unique<ReaderImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Reader::~Reader() noexcept = default;

//------------------------------------------------------------------------------

data::SeriesSet::sptr Reader::scan()
{
    auto files = getFiles();

    if(files.empty())
    {
        const auto& root = getFolder();

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
        std::transform(filenames.cbegin(), filenames.cend(), std::back_inserter(files), [](const auto& v){return v;});
    }

    if(m_pimpl->cancelRequested())
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

data::SeriesSet::sptr Reader::sort()
{
    if(!m_pimpl->m_scanned || m_pimpl->m_scanned->empty())
    {
        scan();
    }

    if(m_pimpl->cancelRequested())
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

    if(m_pimpl->cancelRequested())
    {
        m_pimpl->clear();
        return;
    }

    m_pimpl->read();

    m_pimpl->progress(100);
}

//------------------------------------------------------------------------------

void Reader::setFilter(data::Series::DicomTypes filter)
{
    m_pimpl->m_filter = filter;
}

//------------------------------------------------------------------------------

void Reader::setScanned(const data::SeriesSet::sptr& scanned)
{
    m_pimpl->m_scanned = scanned;

    // The sorted files are no more relevant
    m_pimpl->m_sorted.reset();
}

//------------------------------------------------------------------------------

void Reader::setSorted(const data::SeriesSet::sptr& sorted)
{
    m_pimpl->m_sorted = sorted;

    // No need to keep the scanned files, they are not used anymore
    m_pimpl->m_scanned.reset();
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr Reader::getJob() const
{
    return m_pimpl->m_job;
}

//------------------------------------------------------------------------------

void Reader::setJob(core::jobs::Job::sptr job)
{
    SIGHT_ASSERT("Some work have already be reported.", job->getDoneWorkUnits() == 0);
    m_pimpl->m_job = job;
    m_pimpl->m_job->setTotalWorkUnits(100);
    m_pimpl->m_job->doneWork(10);
}

} // namespace sight::io::dicom
