/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "filter/dicom/splitter/SOPClassUIDSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro(sight::filter::dicom::splitter::SOPClassUIDSplitter);

namespace sight::filter::dicom
{

namespace splitter
{

const std::string SOPClassUIDSplitter::s_FILTER_NAME        = "SOPClassUID splitter";
const std::string SOPClassUIDSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to <i>SOPClassUID</i> tag.";

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::SOPClassUIDSplitter(filter::dicom::IFilter::Key key) :
    filter::dicom::splitter::TagValueSplitter(key)
{
    this->setTag(DCM_SOPClassUID);
}

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::~SOPClassUIDSplitter()
{
}

//-----------------------------------------------------------------------------

std::string SOPClassUIDSplitter::getName() const
{
    return SOPClassUIDSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string SOPClassUIDSplitter::getDescription() const
{
    return SOPClassUIDSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool SOPClassUIDSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::DicomSeriesContainerType SOPClassUIDSplitter::apply(
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr& logger
) const
{
    const DicomSeriesContainerType result = filter::dicom::splitter::TagValueSplitter::apply(series, logger);

    for(const data::DicomSeries::sptr& dicomSeries : result)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;
        OFString data;

        // Open first instance
        const auto firstItem                             = dicomSeries->getDicomContainer().begin();
        const core::memory::BufferObject::sptr bufferObj = firstItem->second;
        const size_t buffSize                            = bufferObj->getSize();
        const std::string dicomPath                      = bufferObj->getStreamInfo().fsFile.string();
        core::memory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast<char*>(lock.getBuffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << dicomPath << "' "
                << "(slice: '" << firstItem->first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        dataset = fileFormat.getDataset();

        // Read SOPClassUID
        dataset = fileFormat.getDataset();
        status  = dataset->findAndGetOFStringArray(DCM_SOPClassUID, data);
        SIGHT_THROW_IF("Unable to read tags: \"" + dicomPath + "\"", status.bad());

        data::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer;
        sopClassUIDContainer.insert(data.c_str());
        dicomSeries->setSOPClassUIDs(sopClassUIDContainer);
    }

    if(result.size() > 1)
    {
        logger->warning(
            "The same series instance UID has been used for several instances "
            "with different SOP class UID. The series has been split."
        );
    }

    return result;
}

} // namespace splitter

} // namespace sight::filter::dicom
