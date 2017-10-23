/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    writer->SetFileName(filename.c_str()); // NOTE: Must be called when file is ready to be written

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

