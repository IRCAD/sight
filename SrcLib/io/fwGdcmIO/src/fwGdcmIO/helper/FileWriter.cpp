/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwGdcmIO/helper/FileWriter.hpp"

#include "fwGdcmIO/exception/Failed.hpp"

#include <gdcmFileMetaInformation.h>
#include <gdcmWriter.h>

namespace fwGdcmIO
{
namespace helper
{
//------------------------------------------------------------------------------

void FileWriter::write(const ::boost::filesystem::path& filename,
                       const SPTR(::gdcm::Writer)& writer)
{
    // Set file header
    ::gdcm::FileMetaInformation& metaInformation = writer->GetFile().GetHeader();

    // Transfer syntax - Type 1
    metaInformation.SetDataSetTransferSyntax(::gdcm::TransferSyntax::ExplicitVRLittleEndian);

    // Initialize the file
    writer->SetFileName(filename.string().c_str()); // NOTE: Must be called when file is ready to be written

    // Write data
    if (!writer->Write())
    {
        const std::string msg = "Unable to write the file " + filename.string();
        throw ::fwGdcmIO::exception::Failed(msg);
    }
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO

