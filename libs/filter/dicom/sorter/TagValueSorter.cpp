/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "filter/dicom/sorter/TagValueSorter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::sorter::TagValueSorter);

namespace sight::filter::dicom::sorter
{

const std::string TagValueSorter::s_FILTER_NAME        = "Tag value sorter";
const std::string TagValueSorter::s_FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

TagValueSorter::TagValueSorter() :
    m_tag(DCM_UndefinedTagKey)
{
}

//-----------------------------------------------------------------------------

TagValueSorter::~TagValueSorter()
= default;

//-----------------------------------------------------------------------------

std::string TagValueSorter::getName() const
{
    return TagValueSorter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueSorter::getDescription() const
{
    return TagValueSorter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueSorter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

TagValueSorter::DicomSeriesContainerType TagValueSorter::apply(
    const data::DicomSeries::sptr& series,
    const core::log::logger::sptr& logger
)
const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw sight::filter::dicom::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    data::DicomSeries::DicomContainerType sortedDicom;

    OFCondition status;
    for(const auto& item : series->getDicomContainer())
    {
        const core::memory::buffer_object::sptr bufferObj = item.second;
        const std::size_t buffSize                        = bufferObj->size();
        core::memory::buffer_object::lock_t lock(bufferObj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        DcmDataset* dataset = fileFormat.getDataset();

        Sint32 index = 0;
        dataset->findAndGetSint32(m_tag, index);

        sortedDicom[std::size_t(index)] = bufferObj;
    }

    if(sortedDicom.size() != series->getDicomContainer().size())
    {
        const std::string msg = "Unable to sort the series using the specified tag. The tag may be missing in "
                                "some instances or several instances may have the same tag value.";
        throw sight::filter::dicom::exceptions::FilterFailure(msg);
    }

    series->clearDicomContainer();
    for(const auto& item : sortedDicom)
    {
        series->addBinary(item.first, item.second);
    }

    result.push_back(series);

    std::stringstream ss;
    ss << "The instances have been sorted using the value of tag ("
    << std::hex << std::setfill('0') << std::setw(4) << m_tag.getGroup() << ","
    << std::hex << std::setfill('0') << std::setw(4) << m_tag.getElement() << ").";
    logger->information(ss.str());

    return result;
}

} // namespace sight::filter::dicom::sorter
