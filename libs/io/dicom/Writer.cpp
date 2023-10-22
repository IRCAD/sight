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

#include "Writer.hpp"

#include "codec/NvJpeg2K.hpp"

#include "data/model_series.hpp"

#include <core/macros.hpp>

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>

#include <io/bitmap/Writer.hpp>

#include <gdcmImageChangeTransferSyntax.h>
#include <gdcmImageWriter.h>

#include <filesystem>
#include <iomanip>
#include <sstream>

// cspell:ignore JPEGLS NEARLOSSLESS

namespace sight::io::dicom
{

//------------------------------------------------------------------------------

inline static void write_enhanced_us_volume(
    const data::image_series& _image_series,
    const std::string& _filepath,
    Writer::TransferSyntax _transfer_syntax,
    [[maybe_unused]] bool _force_cpu
)
{
    // Create the writer
    gdcm::ImageWriter writer;
    writer.SetFileName(_filepath.c_str());
    writer.GetFile().SetDataSet(_image_series.getDataSet());

    auto& gdcm_image = writer.GetImage();

    const auto& image_sizes = _image_series.size();

    // Set the dimension to 3. This magically allows to write multi-frame images. Or at least, they are saved and we can
    // read them back.
    gdcm_image.SetNumberOfDimensions(3);

    // Set the image dimensions
    std::array<std::uint32_t, 3> dimensions {
        std::uint32_t(image_sizes[0]),
        std::uint32_t(image_sizes[1]),
        std::uint32_t(image_sizes[2])
    };

    gdcm_image.SetDimensions(dimensions.data());

    // Orientation
    if(const auto& orientation_volume = _image_series.getImageOrientationVolume(); orientation_volume.size() == 6)
    {
        gdcm_image.SetDirectionCosines(orientation_volume.data());
    }
    else if(const auto& orientation_patient = _image_series.getImageOrientationPatient();
            orientation_patient.size() == 6)
    {
        gdcm_image.SetDirectionCosines(orientation_patient.data());
    }

    // Position
    if(const auto& position_volume = _image_series.getImagePositionVolume(); position_volume.size() == 3)
    {
        gdcm_image.SetOrigin(position_volume.data());
    }
    else if(const auto& position_patient = _image_series.getImagePositionPatient(); position_patient.size() == 6)
    {
        gdcm_image.SetOrigin(position_patient.data());
    }

    // Pixel Format
    switch(_image_series.getType())
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
            SIGHT_THROW("Unsupported type: '" << _image_series.getType().name() << "'");
    }

    // Photometric Interpretation
    switch(_image_series.getPixelFormat())
    {
        case data::image::PixelFormat::GRAY_SCALE:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(1);
            break;

        case data::image::PixelFormat::RGB:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::RGB);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(3);
            break;

        case data::image::PixelFormat::RGBA:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::RGB);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(4);
            break;

        default:
            SIGHT_THROW("Unsupported pixel format: '" << _image_series.getPixelFormat() << "'");
    }

    // Planar Configuration is always 0 (R1G1B1 R2G2B2 ...)
    gdcm_image.SetPlanarConfiguration(0);

    // Spacing
    gdcm_image.SetSpacing(_image_series.getSpacing().data());

    // Dump the image data to GDCM
    /// @note This seems to be sub-optimal as we are going to recompress the data in JPEG2000
    /// @todo Find a way to avoid this double copying of possibly huge image data
    const auto image_locked  = _image_series.dump_lock();
    const auto size_in_bytes = _image_series.getSizeInBytes();

    SIGHT_THROW_IF("Size in Bytes is greater than 4GB", _image_series.getSizeInBytes() > 0xFFFFFFFF);

    using PIXEL_DATA = data::dicom::attribute::Attribute<data::dicom::attribute::Keyword::PixelData>;
    gdcm::DataElement pixeldata(gdcm::Tag(PIXEL_DATA::s_group, PIXEL_DATA::s_element));

    pixeldata.SetByteValue(
        reinterpret_cast<const char*>(_image_series.buffer()),
        std::uint32_t(size_in_bytes)
    );

    gdcm_image.SetDataElement(pixeldata);

    std::unique_ptr<codec::NvJpeg2K> nvjpeg2k_codec;
    gdcm::ImageChangeTransferSyntax transfer_syntax_changer;

    switch(_transfer_syntax)
    {
        case Writer::TransferSyntax::RAW:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);
            break;

        case Writer::TransferSyntax::RLE:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::RLELossless);
            break;

        case Writer::TransferSyntax::JPEG:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGBaselineProcess1);
            break;

        case Writer::TransferSyntax::JPEG_LOSSLESS:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGLosslessProcess14_1);
            break;

        case Writer::TransferSyntax::JPEG_LS_NEARLOSSLESS:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGLSNearLossless);
            break;

        case Writer::TransferSyntax::JPEG_LS_LOSSLESS:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEGLSLossless);
            break;

        case Writer::TransferSyntax::JPEG2000:
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEG2000);
            break;

        default:
        {
            // Default is JPEG2000 lossless
            transfer_syntax_changer.SetTransferSyntax(gdcm::TransferSyntax::JPEG2000Lossless);

#ifdef SIGHT_ENABLE_NVJPEG2K
            if(!_force_cpu)
            {
                SIGHT_THROW_IF(
                    "nvJPEG2000 is not available, but the support has been compiled in. "
                    "Check your nvJPEG2000 library installation",
                    !io::bitmap::nv_jpeg_2k()
                );

                nvjpeg2k_codec = std::make_unique<codec::NvJpeg2K>();
                transfer_syntax_changer.SetUserCodec(nvjpeg2k_codec.get());
            }
#endif
            break;
        }
    }

    transfer_syntax_changer.SetInput(gdcm_image);
    transfer_syntax_changer.Change();

    // This is hackish, but the only way to get the correct Photometric Interpretation without having to deep copying
    // the whole image. Anyway, this should have been done in gdcm::ImageChangeTransferSyntax.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& changed_gdcm_image = const_cast<gdcm::Image&>(transfer_syntax_changer.GetOutput());

    // Correct the Photometric Interpretation (This avoid a warning when GDCM decodes back the image)
    if(_image_series.getPixelFormat() != data::image::PixelFormat::GRAY_SCALE)
    {
        switch(_transfer_syntax)
        {
            case Writer::TransferSyntax::JPEG:
            case Writer::TransferSyntax::JPEG_LS_NEARLOSSLESS:
                changed_gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::YBR_FULL_422);
                break;

            case Writer::TransferSyntax::JPEG2000:
                changed_gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::YBR_ICT);
                break;

            case Writer::TransferSyntax::JPEG2000_LOSSLESS:
            case Writer::TransferSyntax::SOP_DEFAULT:
                changed_gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::YBR_RCT);
                break;

            default:
                // Nothing to do
                break;
        }
    }

    writer.SetImage(changed_gdcm_image);

    // Finally write the file
    writer.Write();
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
    writer.GetFile().SetDataSet(_fiducials_series.getDataSet());
    // Dummy value else GDCM complains. TransferSyntax shouldn't be required since Fiducials aren't an image.
    writer.GetFile().GetHeader().SetDataSetTransferSyntax(gdcm::TransferSyntax::JPEG2000Lossless);
    writer.Write();
}

/// Private Writer implementation
class Writer::WriterImpl
{
public:

    /// Delete default constructors and assignment operators
    WriterImpl(const WriterImpl&)            = delete;
    WriterImpl(WriterImpl&&)                 = delete;
    WriterImpl& operator=(const WriterImpl&) = delete;
    WriterImpl& operator=(WriterImpl&&)      = delete;

    /// Constructor
    inline explicit WriterImpl(Writer* const _writer) noexcept :
        M_WRITER(_writer)
    {
    }

    /// Default destructor
    inline ~WriterImpl() noexcept = default;

    /// Pointer to the public interface
    Writer* const M_WRITER;

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

    /// The default job. Allows to watch for cancellation and report progress.
    core::jobs::job::sptr m_job;

    /// True to disable GPU codec
    bool m_force_cpu {false};

    /// The overriden transfer syntax
    TransferSyntax m_transfer_syntax {TransferSyntax::SOP_DEFAULT};
};

Writer::Writer() :
    core::location::single_folder(),
    m_pimpl(std::make_unique<WriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Writer::~Writer() noexcept = default;

//------------------------------------------------------------------------------

void Writer::write()
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
    const auto& series_set = getConcreteObject();

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

        if(const auto& sop_keyword = series->getSOPKeyword();
           sop_keyword == data::dicom::sop::Keyword::EnhancedUSVolumeStorage)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);

            SIGHT_THROW_IF(
                "The series '" + series->getSeriesInstanceUID() + "' is not an image series.",
                !image_series
            );

            write_enhanced_us_volume(
                *image_series,
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
            if(image_series->hasFiducials())
            {
                write_spatial_fiducials(*image_series->getFiducials(), filepath);
            }
        }
        else if(const auto& model_series = std::dynamic_pointer_cast<data::model_series>(series))
        {
            if(model_series->hasFiducials())
            {
                write_spatial_fiducials(*model_series->getFiducials(), filepath);
            }
        }

        m_pimpl->progress(progress_step);
        ++index;
    }
}

//------------------------------------------------------------------------------

core::jobs::base::sptr Writer::getJob() const
{
    return m_pimpl->m_job;
}

//------------------------------------------------------------------------------

void Writer::setJob(core::jobs::job::sptr _job)
{
    SIGHT_ASSERT("Some work have already be reported.", _job->get_done_work_units() == 0);
    m_pimpl->m_job = _job;
    m_pimpl->m_job->set_total_work_units(100);
    m_pimpl->m_job->done_work(10);
}

//------------------------------------------------------------------------------

void Writer::forceCPU(bool _force)
{
    m_pimpl->m_force_cpu = _force;
}

//------------------------------------------------------------------------------

void Writer::setTransferSyntax(Writer::TransferSyntax _transfer_syntax)
{
    m_pimpl->m_transfer_syntax = _transfer_syntax;
}

} // namespace sight::io::dicom
