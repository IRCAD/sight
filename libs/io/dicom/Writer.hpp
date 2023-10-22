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

#include <core/jobs/job.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/series_set.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::io::dicom
{

class IO_DICOM_CLASS_API Writer final : public io::writer::generic_object_writer<data::series_set>,
                                        public core::location::single_folder,
                                        public core::location::single_file,
                                        public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        Writer,
        io::writer::generic_object_writer<data::series_set>,
        io::writer::factory::make<Writer>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default constructors and assignment operators
    Writer(const Writer&)            = delete;
    Writer(Writer&&)                 = delete;
    Writer& operator=(const Writer&) = delete;
    Writer& operator=(Writer&&)      = delete;

    /// Constructor/Destructor
    IO_DICOM_API Writer();
    IO_DICOM_API ~Writer() noexcept override;

    /// Main writing method from generic_object_writer
    IO_DICOM_API void write() override;

    /// Return the extension to use, by default, or the one from file set by single_file::set_file(), if valid
    /// @return an extension as string
    [[nodiscard]] IO_DICOM_API std::string extension() const override;

    /// Set/get the current job
    IO_DICOM_API core::jobs::base::sptr getJob() const override;
    IO_DICOM_API void setJob(core::jobs::job::sptr _job);

    /// Allow to force the use of a cpu backend, even if SIGHT_ENABLE_NVJPEG2K is set.
    /// This can be useful in case of an unit test executed on a machine without GPU.
    /// @arg force: true to force CPU backend, false to use GPU and throw an exception if not available
    IO_DICOM_API void forceCPU(bool _force);

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

    static constexpr std::string_view transferSyntaxToString(TransferSyntax _transfer_syntax);
    static constexpr TransferSyntax stringToTransferSyntax(const std::string_view& _string);

    /// Set the transfer syntax to use when writing the DICOM file
    /// @param transferSyntax the transfer syntax
    IO_DICOM_API void setTransferSyntax(TransferSyntax _transfer_syntax);

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

constexpr std::string_view Writer::transferSyntaxToString(Writer::TransferSyntax _transfer_syntax)
{
    switch(_transfer_syntax)
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

constexpr Writer::TransferSyntax Writer::stringToTransferSyntax(const std::string_view& _string)
{
    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::RAW); _string == ts)
    {
        return TransferSyntax::RAW;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::RLE); _string == ts)
    {
        return TransferSyntax::RLE;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG); _string == ts)
    {
        return TransferSyntax::JPEG;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG_LOSSLESS); _string == ts)
    {
        return TransferSyntax::JPEG_LOSSLESS;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG_LS_LOSSLESS); _string == ts)
    {
        return TransferSyntax::JPEG_LS_LOSSLESS;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG_LS_NEARLOSSLESS); _string == ts)
    {
        return TransferSyntax::JPEG_LS_NEARLOSSLESS;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG2000); _string == ts)
    {
        return TransferSyntax::JPEG2000;
    }

    if(constexpr auto ts = transferSyntaxToString(TransferSyntax::JPEG2000_LOSSLESS); _string == ts)
    {
        return TransferSyntax::JPEG2000_LOSSLESS;
    }

    return TransferSyntax::SOP_DEFAULT;
}

} // namespace sight::io::dicom
