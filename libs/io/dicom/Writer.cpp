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

#include <core/macros.hpp>

#include <data/ImageSeries.hpp>

#include <gdcmImageChangeTransferSyntax.h>
#include <gdcmImageWriter.h>

#include <iomanip>
#include <sstream>

namespace sight::io::dicom
{

//------------------------------------------------------------------------------

inline static void writeEnhancedUSVolume(
    const data::ImageSeries& image_series,
    const std::string& filepath
)
{
    // Create the writer
    gdcm::ImageWriter writer;
    writer.SetFileName(filepath.c_str());
    writer.GetFile().SetDataSet(image_series.getDataSet());

    auto& gdcm_image = writer.GetImage();

    const auto& image_sizes = image_series.getSize();

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
    if(const auto& orientation_volume = image_series.getImageOrientationVolume(); orientation_volume.size() == 6)
    {
        gdcm_image.SetDirectionCosines(orientation_volume.data());
    }
    else if(const auto& orientation_patient = image_series.getImageOrientationPatient();
            orientation_patient.size() == 6)
    {
        gdcm_image.SetDirectionCosines(orientation_patient.data());
    }

    // Position
    if(const auto& position_volume = image_series.getImagePositionVolume(); position_volume.size() == 3)
    {
        gdcm_image.SetOrigin(position_volume.data());
    }
    else if(const auto& position_patient = image_series.getImagePositionPatient(); position_patient.size() == 6)
    {
        gdcm_image.SetOrigin(position_patient.data());
    }

    // Pixel Format
    switch(image_series.getType())
    {
        case core::Type::INT8:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT8);
            break;

        case core::Type::UINT8:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT8);
            break;

        case core::Type::INT16:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT16);
            break;

        case core::Type::UINT16:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT16);
            break;

        case core::Type::INT32:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT32);
            break;

        case core::Type::UINT32:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT32);
            break;

        case core::Type::INT64:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::INT64);
            break;

        case core::Type::UINT64:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::UINT64);
            break;

        case core::Type::FLOAT:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::FLOAT32);
            break;

        case core::Type::DOUBLE:
            gdcm_image.SetPixelFormat(gdcm::PixelFormat::FLOAT64);
            break;

        default:
            SIGHT_THROW("Unsupported type: '" << image_series.getType().name() << "'");
    }

    // Photometric Interpretation
    switch(image_series.getPixelFormat())
    {
        case data::Image::PixelFormat::GRAY_SCALE:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(1);
            break;

        case data::Image::PixelFormat::RGB:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::RGB);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(3);
            break;

        case data::Image::PixelFormat::RGBA:
            gdcm_image.SetPhotometricInterpretation(gdcm::PhotometricInterpretation::RGB);
            gdcm_image.GetPixelFormat().SetSamplesPerPixel(4);
            break;

        default:
            SIGHT_THROW("Unsupported pixel format: '" << image_series.getPixelFormat() << "'");
    }

    // Planar Configuration is always 0 (R1G1B1 R2G2B2 ...)
    gdcm_image.SetPlanarConfiguration(0);

    // Spacing
    gdcm_image.SetSpacing(image_series.getSpacing().data());

    // Dump the image data to GDCM
    /// @note This seems to be sub-optimal as we are going to recompress the data in JPEG2000
    /// @todo Find a way to avoid this double copying of possibly huge image data
    const auto image_locked = image_series.dump_lock();

    using PIXEL_DATA = data::dicom::attribute::Attribute<data::dicom::attribute::Keyword::PixelData>;
    gdcm::DataElement pixeldata(gdcm::Tag(PIXEL_DATA::s_group, PIXEL_DATA::s_element));
    pixeldata.SetByteValue(
        reinterpret_cast<const char*>(image_series.getBuffer()),
        std::uint32_t(image_series.getSizeInBytes())
    );
    gdcm_image.SetDataElement(pixeldata);

    // Change data as JPEG2000 lossless
    gdcm::ImageChangeTransferSyntax transferSyntaxChanger;
    transferSyntaxChanger.SetTransferSyntax(gdcm::TransferSyntax::JPEG2000Lossless);
#ifdef SIGHT_ENABLE_NVJPEG2K
    codec::NvJpeg2K codec;
    transferSyntaxChanger.SetUserCodec(&codec);
#endif
    transferSyntaxChanger.SetInput(gdcm_image);
    transferSyntaxChanger.Change();
    writer.SetImage(transferSyntaxChanger.GetOutput());

    // Finally write the file
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
    inline explicit WriterImpl(Writer* const writer) noexcept :
        m_writer(writer)
    {
    }

    /// Default destructor
    inline ~WriterImpl() noexcept = default;

    /// Pointer to the public interface
    Writer* const m_writer;

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

    /// The default job. Allows to watch for cancellation and report progress.
    core::jobs::Job::sptr m_job;
};

Writer::Writer(io::base::writer::IObjectWriter::Key /*unused*/) :
    core::location::SingleFolder(),
    m_pimpl(std::make_unique<WriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Writer::~Writer() noexcept = default;

//------------------------------------------------------------------------------

void Writer::write()
{
    // Get the destination folder
    auto folder = getFolder();

    if(folder.empty() && getFile().has_parent_path())
    {
        folder = getFile().parent_path();
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
    const auto& file         = getFile();
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
        if(m_pimpl->cancelRequested())
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
            const auto& image_series = data::ImageSeries::dynamicCast(series);

            SIGHT_THROW_IF(
                "The series '" + series->getSeriesInstanceUID() + "' is not an image series.",
                !image_series
            );

            writeEnhancedUSVolume(*image_series, filepath.string());
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

        m_pimpl->progress(progress_step);
        ++index;
    }
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr Writer::getJob() const
{
    return m_pimpl->m_job;
}

//------------------------------------------------------------------------------

void Writer::setJob(core::jobs::Job::sptr job)
{
    SIGHT_ASSERT("Some work have already be reported.", job->getDoneWorkUnits() == 0);
    m_pimpl->m_job = job;
    m_pimpl->m_job->setTotalWorkUnits(100);
    m_pimpl->m_job->doneWork(10);
}

} // namespace sight::io::dicom
