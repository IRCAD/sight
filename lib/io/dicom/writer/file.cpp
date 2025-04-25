/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "data/model_series.hpp"

#include "io/dicom/codec/nv_jpeg2k.hpp"

#include <core/macros.hpp>

#include <data/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>

#include <io/bitmap/writer.hpp>

#include <gdcmImageChangeTransferSyntax.h>
#include <gdcmImageWriter.h>

#include <filesystem>
#include <iomanip>
#include <sstream>

// cspell:ignore JPEGLS NEARLOSSLESS

namespace sight::io::dicom::writer
{

/// Decode image orientation / position
/// GDCM doesn't handle Enhanced US Volume (which is rather a complicated case)
/// see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.8.24.2.html
inline static void compute_transform(
    data::matrix4& _matrix,
    const data::image_series& _image_series
)
{
    // Use the image metadata as a lower priority
    _matrix.set_position(_image_series.origin());

    // Try also the magical field "direction"
    ///@todo remove this when we get rid of the direction "field" from medical image
    if(const auto& direction_matrix = data::helper::medical_image::get_direction(_image_series); direction_matrix)
    {
        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            for(std::size_t j = 0 ; j < 3 ; ++j)
            {
                _matrix(i, j) = (*direction_matrix)(i, j);
            }
        }
    }

    // But DICOM metadata should have the highest priority !
    if(_image_series.get_ultrasound_acquisition_geometry() == data::dicom::ultrasound_acquisition_geometry_t::apex)
    {
        // Try first the shared group
        if(const auto& shared_orientation = _image_series.get_image_orientation_volume(std::nullopt);
           shared_orientation.size() == 6)
        {
            _matrix.set_orientation(data::image_series::from_dicom_orientation(shared_orientation));
        }

        if(const auto& shared_position = _image_series.get_image_position_volume(std::nullopt);
           shared_position.size() == 3)
        {
            _matrix.set_position(shared_position);
        }

        // Then the first frame from the per_frame group
        if(const auto& frame_orientation = _image_series.get_image_orientation_volume(0);
           frame_orientation.size() == 6)
        {
            _matrix.set_orientation(data::image_series::from_dicom_orientation(frame_orientation));
        }

        if(const auto& frame_position = _image_series.get_image_position_volume(0);
           frame_position.size() == 3)
        {
            _matrix.set_position(frame_position);
        }
    }
    else // data::dicom::ultrasound_acquisition_geometry_t::patient or unknown
    {
        // Try the shared group
        if(const auto& shared_orientation = _image_series.get_image_orientation_patient(std::nullopt);
           shared_orientation.size() == 6)
        {
            _matrix.set_orientation(data::image_series::from_dicom_orientation(shared_orientation));
        }

        if(const auto& shared_position = _image_series.get_image_position_patient(std::nullopt);
           shared_position.size() == 3)
        {
            _matrix.set_position(shared_position);
        }
    }
}

//------------------------------------------------------------------------------

inline static void write_enhanced_us_volume(
    const data::image_series& _image_series,
    data::series& _series_copy,
    const std::string& _filepath,
    writer::file::transfer_syntax _transfer_syntax,
    [[maybe_unused]] bool _force_cpu
)
{
    data::matrix4 transform;
    compute_transform(transform, _image_series);

    if(_series_copy.get_ultrasound_acquisition_geometry()
       == data::dicom::ultrasound_acquisition_geometry_t::unknown)
    {
        SIGHT_INFO(
            "The Ultrasound Acquisition Geometry is unknown. "
            "The default 'PATIENT' will be used."
        );

        _series_copy.set_ultrasound_acquisition_geometry(data::dicom::ultrasound_acquisition_geometry_t::patient);
    }

    if(_series_copy.get_ultrasound_acquisition_geometry()
       == data::dicom::ultrasound_acquisition_geometry_t::patient
       && _series_copy.get_patient_frame_of_reference_source()
       == data::dicom::patient_frame_of_reference_source_t::unknown)
    {
        SIGHT_INFO(
            "The Patient Frame of Reference Source is unknown but Ultrasound Acquisition Geometry is `PATIENT`. "
            "The default 'TABLE' will be used."
        );

        _series_copy.set_patient_frame_of_reference_source(
            data::dicom::patient_frame_of_reference_source_t::table
        );
    }

    if(_series_copy.get_ultrasound_acquisition_geometry()
       == data::dicom::ultrasound_acquisition_geometry_t::apex
       && !_series_copy.get_volume_to_transducer_mapping_matrix())
    {
        SIGHT_INFO(
            "The Volume to Transducer mapping is not present but Ultrasound Acquisition Geometry is `APEX`. "
            "The mapping matrix will be set."
        );

        _series_copy.set_volume_to_transducer_mapping_matrix(transform);
    }

    if(_series_copy.get_patient_frame_of_reference_source()
       == data::dicom::patient_frame_of_reference_source_t::table
       && !_series_copy.get_volume_to_table_mapping_matrix())
    {
        SIGHT_INFO(
            "The Volume to Table mapping is not present but Patient Frame of Reference Source is 'TABLE'. "
            "The mapping matrix will be set."
        );

        _series_copy.set_volume_to_table_mapping_matrix(transform);
    }

    if(_series_copy.get_dimension_organization_type()
       == data::dicom::dimension_organization_t::unknown)
    {
        ///@note TILED_SPARSE is not really documented in enhanced US volume, but before the volume is reconstructed
        ///      We need to tells that the slices may not be contiguous
        SIGHT_INFO(
            "The Dimension Organization Type is unknown. "
            "The default 'TILED_SPARSE' will be used."
        );

        _series_copy.set_dimension_organization_type(
            data::dicom::dimension_organization_t::tiled_sparse
        );
    }

    // Now that the defaults are set, we can set the image position / orientation if missing and needed
    if(_series_copy.get_ultrasound_acquisition_geometry()
       == data::dicom::ultrasound_acquisition_geometry_t::apex)
    {
        // Orientation
        if(_series_copy.get_image_orientation_volume(std::nullopt).size() != 6
           && _series_copy.get_image_orientation_volume(0).size() != 6)
        {
            // Orientation can be shared for all frames
            // It won't overwrite the per-frame orientation
            _series_copy.set_image_orientation_volume(
                data::image_series::to_dicom_orientation(transform.orientation()),
                std::nullopt
            );
        }

        // Position
        if(_series_copy.get_image_position_volume(std::nullopt).size() != 3
           && _series_copy.get_image_position_volume(0).size() != 3)
        {
            // Do it for each frames
            // Use the sight image origin for the first frame
            auto frame_position = transform.position<std::vector<double> >();

            // Get z spacing
            const auto z_spacing = _image_series.spacing()[2];

            // We need to compute the frame position from image origin and z spacing
            for(std::size_t frame = 0, end_index = _series_copy.num_frames() ;
                frame < end_index ; ++frame)
            {
                SIGHT_WARN_IF(
                    "ImagePositionVolume for frame " + std::to_string(frame) + " will be overwritten",
                    _series_copy.get_image_position_volume(frame).size() == 3
                );

                _series_copy.set_image_position_volume(frame_position, frame);
                frame_position[2] += z_spacing;
            }
        }
    }
    else
    {
        // Orientation
        if(_series_copy.get_image_orientation_patient(std::nullopt).size() != 6
           && _series_copy.get_image_orientation_patient(0).size() != 6)
        {
            // Orientation can be shared for all frames
            // It won't overwrite the per-frame orientation
            _series_copy.set_image_orientation_patient(
                data::image_series::to_dicom_orientation(transform.orientation()),
                std::nullopt
            );
        }

        // Position
        if(_series_copy.get_image_position_patient(0).size() != 3)
        {
            // Do it for each frames
            // Use the sight image origin for the first frame
            auto frame_position = transform.position<std::vector<double> >();

            // Get z spacing
            const auto z_spacing = _image_series.spacing()[2];

            // We need to compute the frame position from image origin and z spacing
            for(std::size_t frame = 0, end_index = std::max(std::size_t(1), _image_series.size()[2]) ;
                frame < end_index ; ++frame)
            {
                SIGHT_WARN_IF(
                    "ImagePositionPatient for frame " + std::to_string(frame) + " will be overwritten",
                    _series_copy.get_image_position_patient(frame).size() == 3
                );

                _series_copy.set_image_position_patient(frame_position, frame);
                frame_position[2] += z_spacing;
            }
        }
    }

    // Create the writer
    gdcm::ImageWriter writer;
    writer.SetFileName(_filepath.c_str());
    writer.GetFile().SetDataSet(_series_copy.get_data_set());

    auto& gdcm_image = writer.GetImage();

    // Set the dimension to 3. This magically allows to write multi-frame images.
    // Or at least, they are saved and we can read them back.
    gdcm_image.SetNumberOfDimensions(3);

    // Set the image dimensions
    const auto& image_sizes = _image_series.size();
    const std::array<std::uint32_t, 3> dimensions {
        std::uint32_t(image_sizes[0]),
        std::uint32_t(image_sizes[1]),
        std::uint32_t(image_sizes[2])
    };

    gdcm_image.SetDimensions(dimensions.data());

    // This is not enough for Enhanced US Volume
    // We still do it, just in case it will be managed correctly in the future
    const auto& gdcm_direction = transform.orientation();
    gdcm_image.SetDirectionCosines(gdcm_direction.data());
    const auto& gdcm_origin = transform.position();
    gdcm_image.SetOrigin(gdcm_origin.data());

    // Pixel Format
    switch(_image_series.type())
    {
        case core::type::INT8:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT8);
            break;

        case core::type::UINT8:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT8);
            break;

        case core::type::INT16:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT16);
            break;

        case core::type::UINT16:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT16);
            break;

        case core::type::INT32:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT32);
            break;

        case core::type::UINT32:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT32);
            break;

        case core::type::INT64:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT64);
            break;

        case core::type::UINT64:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT64);
            break;

        case core::type::FLOAT:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::FLOAT32);
            break;

        case core::type::DOUBLE:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::FLOAT64);
            break;

        default:
            SIGHT_THROW("Unsupported type: '" << _image_series.type().name() << "'");
    }

    // Photometric Interpretation
    switch(_image_series.pixel_format())
    {
        case data::image::pixel_format_t::gray_scale:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(1);
            break;

        case data::image::pixel_format_t::rgb:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::RGB);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(3);
            break;

        case data::image::pixel_format_t::rgba:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::RGB);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(4);
            break;

        default:
            SIGHT_THROW("Unsupported pixel format: '" << _image_series.pixel_format() << "'");
    }

    // Planar Configuration is always 0 (R1G1B1 R2G2B2 ...)
    gdcm_image.SetPlanarConfiguration(0);

    // Spacing
    gdcm_image.SetSpacing(_image_series.spacing().data());

    // Dump the image data to GDCM
    /// @note This seems to be sub-optimal as we are going to recompress the data in JPEG2000
    /// @todo Find a way to avoid this double copying of possibly huge image data
    const auto image_locked  = _image_series.dump_lock();
    const auto size_in_bytes = _image_series.size_in_bytes();

    SIGHT_THROW_IF("Size in Bytes is greater than 4GB", _image_series.size_in_bytes() > 0xFFFFFFFF);

    using pixel_data_t = data::dicom::attribute::Attribute<data::dicom::attribute::Keyword::PixelData>;
    gdcm::DataElement pixeldata(gdcm::Tag(pixel_data_t::s_group, pixel_data_t::s_element));

    pixeldata.SetByteValue(
        reinterpret_cast<const char*>(_image_series.buffer()),
        std::uint32_t(size_in_bytes)
    );

    gdcm_image.SetDataElement(pixeldata);

    std::unique_ptr<codec::nv_jpeg2_k> nvjpeg2k_codec;
    gdcm::ImageChangeTransferSyntax transfer_syntax_changer;

    switch(_transfer_syntax)
    {
        case writer::file::transfer_syntax::raw:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);
            break;

        case writer::file::transfer_syntax::rle:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::RLELossless);
            break;

        case writer::file::transfer_syntax::jpeg:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGBaselineProcess1);
            break;

        case writer::file::transfer_syntax::jpeg_lossless:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGLosslessProcess14_1);
            break;

        case writer::file::transfer_syntax::jpeg_ls_nearlossless:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGLSNearLossless);
            break;

        case writer::file::transfer_syntax::jpeg_ls_lossless:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGLSLossless);
            break;

        case writer::file::transfer_syntax::jpeg_2000:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEG2000);
            break;

        default:
        {
            if(_image_series.type().size() <= 2)
            {
                // Default is JPEG2000 lossless for 16 bits or less since jpeg2000 doesn't support more than 16 bits
                transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEG2000Lossless);

#ifdef SIGHT_ENABLE_NVJPEG2K
                if(!_force_cpu)
                {
                    SIGHT_THROW_IF(
                        "nvJPEG2000 is not available, but the support has been compiled in. "
                        "Check your nvJPEG2000 library installation",
                        !io::bitmap::nv_jpeg_2k()
                    );

                    nvjpeg2k_codec = std::make_unique<codec::nv_jpeg2_k>();
                    transfer_syntax_changer.SetUserCodec(nvjpeg2k_codec.get());

                    SIGHT_INFO("nvJPEG2000 will be used for JPEG2000 compression.");
                }
                else
                {
                    SIGHT_WARN(
                        "nvJPEG2000 support is available, but OpenJPEG has been forced for JPEG2000 compression."
                    );
                }
#else
                SIGHT_INFO("OpenJPEG will be used for JPEG2000 compression.");
#endif
            }
            else
            {
                // For more thant 16 bits, we use RLE
                transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::RLELossless);

                SIGHT_WARN(
                    "The image has more than 16 bits per component. "
                    "RLE compression will be used instead of JPEG2000."
                );
            }

            break;
        }
    }

    // Final check for JPEG compression
    switch(transfer_syntax_changer.GetTransferSyntax())
    {
        case gdcm::TransferSyntax::JPEGBaselineProcess1:
        case gdcm::TransferSyntax::JPEGExtendedProcess2_4:
        case gdcm::TransferSyntax::JPEGExtendedProcess3_5:
        case gdcm::TransferSyntax::JPEGSpectralSelectionProcess6_8:
        case gdcm::TransferSyntax::JPEGFullProgressionProcess10_12:
        case gdcm::TransferSyntax::JPEGLosslessProcess14:
        case gdcm::TransferSyntax::JPEGLosslessProcess14_1:
        case gdcm::TransferSyntax::JPEGLSLossless:
        case gdcm::TransferSyntax::JPEGLSNearLossless:
        case gdcm::TransferSyntax::JPEG2000Lossless:
        case gdcm::TransferSyntax::JPEG2000:
        case gdcm::TransferSyntax::JPEG2000Part2Lossless:
        case gdcm::TransferSyntax::JPEG2000Part2:
        {
            SIGHT_THROW_IF(
                "JPEG compression doesn't support more than 16 nits per component.",
                _image_series.type().size() > 2
            );

            break;
        }

        default:
            break;
    }

    transfer_syntax_changer.SetInput(gdcm_image);
    transfer_syntax_changer.Change();

    // This is hackish, but the only way to get the correct Photometric Interpretation without having to deep copying
    // the whole image. Anyway, this should have been done in gdcm::ImageChangeTransferSyntax.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& changed_gdcm_image = const_cast<gdcm::Image&>(transfer_syntax_changer.GetOutput());

    // Correct the Photometric Interpretation (This avoid a warning when GDCM decodes back the image)
    if(_image_series.pixel_format() != data::image::pixel_format_t::gray_scale)
    {
        switch(_transfer_syntax)
        {
            case writer::file::transfer_syntax::jpeg:
            case writer::file::transfer_syntax::jpeg_ls_nearlossless:
                changed_gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::YBR_FULL_422);
                break;

            case writer::file::transfer_syntax::jpeg_2000:
                changed_gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::YBR_ICT);
                break;

            case writer::file::transfer_syntax::jpeg_2000_lossless:
            case writer::file::transfer_syntax::sop_default:
                changed_gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::YBR_RCT);
                break;

            default:
                // Nothing to do
                break;
        }
    }

    writer.SetImage(changed_gdcm_image);

    // Finally write the file
    SIGHT_THROW_IF(
        "Failed to write the DICOM file '" + _filepath + "'.",
        !writer.Write()
    );
}

//------------------------------------------------------------------------------

static void write_spatial_fiducials(
    const data::fiducials_series& _fiducials_series,
    const std::filesystem::path& _image_path
)
{
    gdcm::Writer writer;
    std::filesystem::path folder = _image_path.parent_path();
    std::string filename         = _image_path.stem().string() + "_fiducials.dcm";

    writer.SetFileName((folder / filename).string().c_str());
    writer.GetFile().SetDataSet(_fiducials_series.get_data_set());
    // Dummy value else GDCM complains. TransferSyntax shouldn't be required since Fiducials aren't an image.
    writer.GetFile().GetHeader().SetDataSetTransferSyntax(gdcm::TransferSyntax::JPEG2000Lossless);
    writer.Write();
}

/// Private Writer implementation
class file::writer_impl
{
public:

    /// Delete default constructors and assignment operators
    writer_impl(const writer_impl&)            = delete;
    writer_impl(writer_impl&&)                 = delete;
    writer_impl& operator=(const writer_impl&) = delete;
    writer_impl& operator=(writer_impl&&)      = delete;

    /// Constructor
    explicit writer_impl(file* const _writer) noexcept :
        m_writer(_writer)
    {
    }

    /// Default destructor
    ~writer_impl() noexcept = default;

    /// Pointer to the public interface
    file* const m_writer;

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

    /// The default job. Allows to watch for cancellation and report progress.
    core::jobs::job::sptr m_job;

    /// True to disable GPU codec
    bool m_force_cpu {false};

    /// The overriden transfer syntax
    transfer_syntax m_transfer_syntax {transfer_syntax::sop_default};
};

file::file() :
    core::location::single_folder(),
    m_pimpl(std::make_unique<writer_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
file::~file() noexcept = default;

//------------------------------------------------------------------------------

void file::write()
{
    // Get the destination folder
    auto folder = get_folder();

    if(folder.empty() && get_file().has_parent_path())
    {
        folder = get_file().parent_path();
    }

    if(!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(folder);
    }

    SIGHT_THROW_IF("The folder '" + folder.string() + "' is not a directory.", !std::filesystem::is_directory(folder));

    // Retrieve the object to write
    const auto& series_set = get_concrete_object();

    // Compute progress for one step
    const size_t progress_step = (100 - 20) / series_set->size();

    // Compute the base name
    const auto& file         = get_file();
    const bool prepend_index = file.empty() || series_set->size() > 1;
    const auto& basename     =
        [&]
        {
            if(file.empty())
            {
                return extension();
            }

            if(prepend_index)
            {
                return "-" + file.filename().string();
            }

            return file.filename().string();
        }();

    // For each series..
    for(std::size_t index = 0 ; const auto& series : *series_set)
    {
        if(m_pimpl->cancel_requested())
        {
            break;
        }

        const auto& filepath =
            [&]
            {
                if(prepend_index)
                {
                    std::stringstream ss;
                    ss << std::setw(3) << std::setfill('0') << index;
                    return folder / (ss.str() + basename);
                }

                return folder / basename;
            }();

        if(const auto& sop_keyword = series->get_sop_keyword();
           sop_keyword == data::dicom::sop::Keyword::EnhancedUSVolumeStorage)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);

            SIGHT_THROW_IF(
                "The series '" + series->get_series_instance_uid() + "' is not an image series.",
                !image_series
            );

            // Shallow copy the series (but not the pixel data !)
            // This will allow to modify the GDCM DICOM context using series API to workaround some GDCM bugs or
            // unimplemented features like the image origin for Enhanced US Volume
            auto series_copy = std::make_shared<data::series>();
            series_copy->shallow_copy(series);

            write_enhanced_us_volume(
                *image_series,
                *series_copy,
                filepath.string(),
                m_pimpl->m_transfer_syntax,
                m_pimpl->m_force_cpu
            );
        }
        else
        {
            try
            {
                SIGHT_THROW(
                    "SOP Class '" << data::dicom::sop::get(sop_keyword).m_name << "' is not supported."
                );
            }
            catch(const std::exception&)
            {
                SIGHT_THROW("SOP Class ID '" << int(sop_keyword) << "' is unknown.");
            }
        }

        if(const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series))
        {
            if(auto fiducials = image_series->get_fiducials(); !fiducials->get_fiducial_sets().empty())
            {
                // Make sure to include this field as it is necessary for creating a correct header for the DICOM file
                if(fiducials->get_sop_instance_uid().empty())
                {
                    fiducials->new_sop_instance();
                }

                write_spatial_fiducials(*fiducials, filepath);
            }
        }
        else if(const auto& model_series = std::dynamic_pointer_cast<data::model_series>(series))
        {
            if(auto fiducials = model_series->get_fiducials(); !fiducials->get_fiducial_sets().empty())
            {
                // Make sure to include this field as it is necessary for creating a correct header for the DICOM file
                if(fiducials->get_sop_instance_uid().empty())
                {
                    fiducials->new_sop_instance();
                }

                write_spatial_fiducials(*fiducials, filepath);
            }
        }

        m_pimpl->progress(progress_step);
        ++index;
    }
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
    m_pimpl->m_job->done_work(10);
}

//------------------------------------------------------------------------------

void file::force_cpu(bool _force)
{
    m_pimpl->m_force_cpu = _force;
}

//------------------------------------------------------------------------------

void file::set_transfer_syntax(file::transfer_syntax _transfer_syntax)
{
    m_pimpl->m_transfer_syntax = _transfer_syntax;
}

} // namespace sight::io::dicom::writer
