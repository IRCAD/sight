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

#pragma once

#include <sight/io/dicom/config.hpp>

#include <core/jobs/job.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/series_set.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::io::dicom::writer
{

class SIGHT_IO_DICOM_CLASS_API file final : public sight::io::writer::generic_object_writer<data::series_set>,
                                            public core::location::single_folder,
                                            public core::location::single_file,
                                            public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        file,
        io::writer::generic_object_writer<data::series_set>,
        io::writer::factory::make<file>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default constructors and assignment operators
    file(const file&)            = delete;
    file(file&&)                 = delete;
    file& operator=(const file&) = delete;
    file& operator=(file&&)      = delete;

    /// Constructor/Destructor
    SIGHT_IO_DICOM_API file();
    SIGHT_IO_DICOM_API ~file() noexcept override;

    /// Main writing method from generic_object_writer
    SIGHT_IO_DICOM_API void write() override;

    /// Return the extension to use, by default, or the one from file set by single_file::set_file(), if valid
    /// @return an extension as string
    [[nodiscard]] SIGHT_IO_DICOM_API std::string extension() const override;

    /// Set/get the current job
    SIGHT_IO_DICOM_API core::jobs::base::sptr get_job() const override;
    SIGHT_IO_DICOM_API void set_job(core::jobs::job::sptr _job);

    /// Allow to force the use of a cpu backend, even if SIGHT_ENABLE_NVJPEG2K is set.
    /// This can be useful in case of an unit test executed on a machine without GPU.
    /// @arg force: true to force CPU backend, false to use GPU and throw an exception if not available
    SIGHT_IO_DICOM_API void force_cpu(bool _force);

    /// Allowed transfer syntax. The default will depends of the SOP classes.
    /// @note All transfer syntaxes are not supported by all SOP classes.
    /// @note this is a simplified version of the DICOM standard, only the most used are supported.
    enum class transfer_syntax : std::uint8_t
    {
        sop_default = 0,
        raw,
        rle,
        jpeg,
        jpeg_lossless,
        jpeg_ls_lossless,
        jpeg_ls_nearlossless,
        jpe_g2000,
        jpe_g2000_lossless
    };

    static constexpr std::string_view transfer_syntax_to_string(transfer_syntax _transfer_syntax);
    static constexpr transfer_syntax string_to_transfer_syntax(const std::string_view& _string);

    /// Set the transfer syntax to use when writing the DICOM file
    /// @param _transfer_syntax the transfer syntax
    SIGHT_IO_DICOM_API void set_transfer_syntax(transfer_syntax _transfer_syntax);

private:

    /// PImpl
    class writer_impl;
    std::unique_ptr<writer_impl> m_pimpl;
};

//------------------------------------------------------------------------------

inline std::string file::extension() const
{
    return ".dcm";
}

//------------------------------------------------------------------------------

constexpr std::string_view file::transfer_syntax_to_string(file::transfer_syntax _transfer_syntax)
{
    switch(_transfer_syntax)
    {
        case transfer_syntax::raw:
            return "RAW";

        case transfer_syntax::rle:
            return "RLE";

        case transfer_syntax::jpeg:
            return "JPEG";

        case transfer_syntax::jpeg_lossless:
            return "JPEG_LOSSLESS";

        case transfer_syntax::jpeg_ls_lossless:
            return "JPEG_LS_LOSSLESS";

        case transfer_syntax::jpeg_ls_nearlossless:
            return "JPEG_LS_NEARLOSSLESS";

        case transfer_syntax::jpe_g2000:
            return "JPEG2000";

        case transfer_syntax::jpe_g2000_lossless:
            return "JPEG2000_LOSSLESS";

        default:
            return "SOP_DEFAULT";
    }
}

//------------------------------------------------------------------------------

constexpr file::transfer_syntax file::string_to_transfer_syntax(const std::string_view& _string)
{
    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::raw); _string == ts)
    {
        return transfer_syntax::raw;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::rle); _string == ts)
    {
        return transfer_syntax::rle;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::jpeg); _string == ts)
    {
        return transfer_syntax::jpeg;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::jpeg_lossless); _string == ts)
    {
        return transfer_syntax::jpeg_lossless;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::jpeg_ls_lossless); _string == ts)
    {
        return transfer_syntax::jpeg_ls_lossless;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::jpeg_ls_nearlossless); _string == ts)
    {
        return transfer_syntax::jpeg_ls_nearlossless;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::jpe_g2000); _string == ts)
    {
        return transfer_syntax::jpe_g2000;
    }

    if(constexpr auto ts = transfer_syntax_to_string(transfer_syntax::jpe_g2000_lossless); _string == ts)
    {
        return transfer_syntax::jpe_g2000_lossless;
    }

    return transfer_syntax::sop_default;
}

} // namespace sight::io::dicom::writer
