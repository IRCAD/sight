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

#include "io/dicom/writer/SeriesSet.hpp"

#include <core/base.hpp>

#include <data/ModelSeries.hpp>

#include <io/base/writer/registry/macros.hpp>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::writer::SeriesSet);

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

SeriesSet::SeriesSet(io::base::writer::IObjectWriter::Key /*unused*/) :
    m_fiducialsExportMode(io::dicom::writer::Series::SPATIAL_FIDUCIALS)
{
}

//------------------------------------------------------------------------------

void SeriesSet::write()
{
//    // Disable GDCM Warnings
//    gdcm::Trace::SetWarning(false);

    auto series_set = this->getConcreteObject();
    SIGHT_ASSERT("SeriesSet not instanced", series_set);

    auto writer = io::dicom::writer::Series::New();
    writer->setFiducialsExportMode(m_fiducialsExportMode);

    // Copy and sort container in order to write ImageSeries before ModelSeries
    std::vector<data::Series::csptr> seriesContainer(series_set->size());
    std::partial_sort_copy(
        series_set->cbegin(),
        series_set->cend(),
        seriesContainer.begin(),
        seriesContainer.end(),
        [](const data::Series::csptr& lhs, const data::Series::csptr& rhs)
        {
            return data::ModelSeries::dynamicCast(lhs) && !data::ModelSeries::dynamicCast(rhs);
        });

    // Write all patients
    for(const auto& series : seriesContainer)
    {
        // Create a new directory
        const std::filesystem::path& seriesPath = this->getFolder() / series->getSeriesInstanceUID();
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

std::string SeriesSet::extension() const
{
    return {};
}

} // namespace sight::io::dicom::writer
