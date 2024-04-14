/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/helper/file_writer.hpp"

#include "io/dicom/exception/failed.hpp"

#include <gdcmFileMetaInformation.h>
#include <gdcmWriter.h>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

void file_writer::write(
    const std::filesystem::path& _filename,
    const SPTR(gdcm::Writer)& _writer
)
{
    // Set file header
    gdcm::FileMetaInformation& meta_information = _writer->GetFile().GetHeader();

    // Transfer syntax - Type 1
    meta_information.SetDataSetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);

    // Initialize the file
    _writer->SetFileName(_filename.string().c_str()); // NOTE: Must be called when file is ready to be written

    // Write data
    if(!_writer->Write())
    {
        const std::string msg = "Unable to write the file " + _filename.string();
        throw io::dicom::exception::failed(msg);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
