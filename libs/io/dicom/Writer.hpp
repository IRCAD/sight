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

#pragma once

#include "io/dicom/config.hpp"

#include <core/jobs/Job.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <data/SeriesSet.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

namespace sight::io::dicom
{

class IO_DICOM_CLASS_API Writer final : public base::writer::GenericObjectWriter<data::SeriesSet>,
                                        public core::location::SingleFolder,
                                        public core::location::SingleFile,
                                        public core::tools::ProgressAdviser
{
public:

    SIGHT_DECLARE_CLASS(
        Writer,
        base::writer::GenericObjectWriter<data::SeriesSet>,
        base::writer::factory::New<Writer>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default constructors and assignment operators
    Writer()                         = delete;
    Writer(const Writer&)            = delete;
    Writer(Writer&&)                 = delete;
    Writer& operator=(const Writer&) = delete;
    Writer& operator=(Writer&&)      = delete;

    /// Constructor/Destructor
    IO_DICOM_API Writer(io::base::writer::IObjectWriter::Key key);
    IO_DICOM_API ~Writer() noexcept override;

    /// Main writing method from GenericObjectWriter
    IO_DICOM_API void write() override;

    /// Return the extension to use, by default, or the one from file set by SingleFile::setFile(), if valid
    /// @return an extension as string
    [[nodiscard]] IO_DICOM_API std::string extension() const override;

    /// Set/get the current job
    IO_DICOM_API core::jobs::IJob::sptr getJob() const override;
    IO_DICOM_API void setJob(core::jobs::Job::sptr job);

    /// Allow to force the use of a cpu backend, even if SIGHT_ENABLE_NVJPEG2K is set.
    /// This can be useful in case of an unit test executed on a machine without GPU.
    /// @arg force: true to force CPU backend, false to use GPU and throw an exception if not available
    IO_DICOM_API void forceCPU(bool force);

    /// Allowed transfer syntax. The default will depends of the SOP classes.
    /// @note All transfer syntaxes are not supported by all SOP classes.
    /// @note this is a simplified version of the DICOM standard, only the most used are supported.
    enum class TransferSyntax : std::uint8_t
    {
        SOP_DEFAULT = 0,
        RAW,
        RLE,
        JPEG,
        JPEG_LOSSLESS,
        JPEG_LS_LOSSLESS,
        JPEG_LS_NEARLOSSLESS,
        JPEG2000,
        JPEG2000_LOSSLESS
    };

    static constexpr std::string_view transferSyntaxToString(TransferSyntax transferSyntax);
    static constexpr TransferSyntax stringToTransferSyntax(const std::string_view& string);

    /// Set the transfer syntax to use when writing the DICOM file
    /// @param transferSyntax the transfer syntax
    IO_DICOM_API void setTransferSyntax(TransferSyntax transferSyntax);

private:

    /// PImpl
    class WriterImpl;
    std::unique_ptr<WriterImpl> m_pimpl;
};

//------------------------------------------------------------------------------

inline std::string Writer::extension() const
{
    return ".dcm";
}

//------------------------------------------------------------------------------

constexpr std::string_view Writer::transferSyntaxToString(Writer::TransferSyntax transferSyntax)
{
    switch(transferSyntax)
    {
        case TransferSyntax::RAW:
            return "RAW";

        case TransferSyntax::RLE:
            return "RLE";

        case TransferSyntax::JPEG:
            return "JPEG";

        case TransferSyntax::JPEG_LOSSLESS:
            return "JPEG_LOSSLESS";

        case TransferSyntax::JPEG_LS_LOSSLESS:
            return "JPEG_LS_LOSSLESS";

        case TransferSyntax::JPEG_LS_NEARLOSSLESS:
            return "JPEG_LS_NEARLOSSLESS";

        case TransferSyntax::JPEG2000:
            return "JPEG2000";

        case TransferSyntax::JPEG2000_LOSSLESS:
            return "JPEG2000_LOSSLESS";

        default:
            return "SOP_DEFAULT";
    }
}

//------------------------------------------------------------------------------

constexpr Writer::TransferSyntax Writer::stringToTransferSyntax(const std::string_view& string)
{
    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::RAW); string == ts)
    {
        return TransferSyntax::RAW;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::RLE); string == ts)
    {
        return TransferSyntax::RLE;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG); string == ts)
    {
        return TransferSyntax::JPEG;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG_LOSSLESS); string == ts)
    {
        return TransferSyntax::JPEG_LOSSLESS;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG_LS_LOSSLESS); string == ts)
    {
        return TransferSyntax::JPEG_LS_LOSSLESS;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG_LS_NEARLOSSLESS); string == ts)
    {
        return TransferSyntax::JPEG_LS_NEARLOSSLESS;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG2000); string == ts)
    {
        return TransferSyntax::JPEG2000;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG2000_LOSSLESS); string == ts)
    {
        return TransferSyntax::JPEG2000_LOSSLESS;
    }

    return TransferSyntax::SOP_DEFAULT;
}

} // namespace sight::io::dicom
