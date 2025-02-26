/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <filesystem>

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

series_set::series_set() :
    m_fiducials_export_mode(io::dicom::writer::series::spatial_fiducials)
{
}

//------------------------------------------------------------------------------

void series_set::write()
{
//    // Disable GDCM Warnings
//    gdcm::Trace::SetWarning(false);

    auto series_set = this->get_concrete_object();
    SIGHT_ASSERT("series_set not instanced", series_set);

    auto writer = std::make_shared<io::dicom::writer::series>();
    writer->set_fiducials_export_mode(m_fiducials_export_mode);

    // Copy and sort container in order to write ImageSeries before ModelSeries
    std::vector<data::series::csptr> series_container(series_set->size());
    std::partial_sort_copy(
        series_set->cbegin(),
        series_set->cend(),
        series_container.begin(),
        series_container.end(),
        [](const data::series::csptr& _lhs,
           const data::series::csptr& _rhs)
        {
            return std::dynamic_pointer_cast<const data::model_series>(
                _lhs
            ) && !std::dynamic_pointer_cast<const data::model_series>(_rhs);
        });

    // Write all patients
    for(const auto& series : series_container)
    {
        // Create a new directory
        const std::filesystem::path& series_path = this->get_folder() / series->get_series_instance_uid();
        std::filesystem::create_directories(series_path);
        writer->set_object(series);
        writer->set_folder(series_path);

        // Forward event progress to its parents
        core::tools::progress_adviser::progress_handler handler =
            [this](auto&& _p_h1, auto&& _p_h2, auto&& ...)
            {
                notify_progress(std::forward<decltype(_p_h1)>(_p_h1), std::forward<decltype(_p_h2)>(_p_h2));
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
