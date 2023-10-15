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

#include "io/dicom/writer/series_set.hpp"

#include <core/base.hpp>

#include <data/model_series.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::writer::series_set);

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

series_set::series_set() :
    m_fiducialsExportMode(io::dicom::writer::series::SPATIAL_FIDUCIALS)
{
}

//------------------------------------------------------------------------------

void series_set::write()
{
//    // Disable GDCM Warnings
//    gdcm::Trace::SetWarning(false);

    auto series_set = this->getConcreteObject();
    SIGHT_ASSERT("series_set not instanced", series_set);

    auto writer = std::make_shared<io::dicom::writer::series>();
    writer->setFiducialsExportMode(m_fiducialsExportMode);

    // Copy and sort container in order to write ImageSeries before ModelSeries
    std::vector<data::series::csptr> seriesContainer(series_set->size());
    std::partial_sort_copy(
        series_set->cbegin(),
        series_set->cend(),
        seriesContainer.begin(),
        seriesContainer.end(),
        [](const data::series::csptr& lhs,
           const data::series::csptr& rhs)
        {
            return std::dynamic_pointer_cast<const data::model_series>(
                lhs
            ) && !std::dynamic_pointer_cast<const data::model_series>(rhs);
        });

    // Write all patients
    for(const auto& series : seriesContainer)
    {
        // Create a new directory
        const std::filesystem::path& seriesPath = this->get_folder() / series->getSeriesInstanceUID();
        std::filesystem::create_directories(seriesPath);
        writer->set_object(series);
        writer->set_folder(seriesPath);

        // Forward event progress to its parents
        core::tools::progress_adviser::progress_handler handler =
            [this](auto&& PH1, auto&& PH2, auto&& ...)
            {
                notify_progress(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
            };
        writer->add_handler(handler);

        // Write a series
        writer->write();
    }
}

//------------------------------------------------------------------------------

std::string series_set::extension() const
{
    return {};
}

} // namespace sight::io::dicom::writer
