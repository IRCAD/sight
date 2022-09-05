/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/dicom/writer/SeriesDB.hpp"

#include <core/base.hpp>

#include <data/ModelSeries.hpp>

#include <io/base/writer/registry/macros.hpp>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::writer::SeriesDB);

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(io::base::writer::IObjectWriter::Key /*key*/)
{
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
= default;

//------------------------------------------------------------------------------

void SeriesDB::write()
{
//    // Disable GDCM Warnings
//    gdcm::Trace::SetWarning(false);

    data::SeriesDB::csptr seriesDB = this->getConcreteObject();
    SIGHT_ASSERT("SeriesDB not instanced", seriesDB);

    io::dicom::writer::Series::sptr writer = io::dicom::writer::Series::New();
    writer->setFiducialsExportMode(m_fiducialsExportMode);

    // Copy and sort container in order to write ImageSeries before ModelSeries
    data::SeriesDB::ContainerType seriesContainer = seriesDB->getContainer();
    std::sort(seriesContainer.begin(), seriesContainer.end(), SeriesDB::seriesComparator);

    // Write all patients
    for(const data::Series::sptr& series : seriesContainer)
    {
        // Create a new directory
        const std::filesystem::path& seriesPath = this->getFolder() / series->getInstanceUID();
        std::filesystem::create_directories(seriesPath);
        writer->setObject(series);
        writer->setFolder(seriesPath);

        // Forward event progress to its parents
        core::tools::ProgressAdviser::ProgessHandler handler =
            [this](auto&& PH1, auto&& PH2, auto&& ...)
            {
                notifyProgress(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
            };
        writer->addHandler(handler);

        // Write a series
        writer->write();
    }
}

//------------------------------------------------------------------------------

std::string SeriesDB::extension() const
{
    return {""};
}

//------------------------------------------------------------------------------

bool SeriesDB::seriesComparator(
    const data::Series::csptr& a,
    const data::Series::csptr& b
)
{
    data::ModelSeries::csptr ma = data::ModelSeries::dynamicCast(a);
    data::ModelSeries::csptr mb = data::ModelSeries::dynamicCast(b);
    return mb && !ma;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer
