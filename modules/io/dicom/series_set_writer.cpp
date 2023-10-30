/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "series_set_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/helper/Fiducial.hpp>
#include <io/dicom/writer/series_set.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>
#include <ui/__/dialog/selector.hpp>

namespace sight::module::io::dicom
{

//------------------------------------------------------------------------------

series_set_writer::series_set_writer() noexcept :
    m_fiducials_export_mode(sight::io::dicom::writer::series::comprehensive_3_d_sr)
{
}

//------------------------------------------------------------------------------

void series_set_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a directory for DICOM images" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::write);
    dialog_file.set_type(ui::dialog::location::folder);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result && this->select_fiducials_export_mode())
    {
        default_directory->set_folder(result->get_folder());
        this->set_folder(result->get_folder());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void series_set_writer::starting()
{
}

//------------------------------------------------------------------------------

void series_set_writer::stopping()
{
}

//------------------------------------------------------------------------------

void series_set_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void series_set_writer::updating()
{
    m_write_failed = true;

    if(this->has_location_defined())
    {
        const std::filesystem::path& folder = this->get_folder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::dialog::message dialog;
            dialog.set_message(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.set_title("Folder not empty.");
            dialog.set_icon(ui::dialog::message::question);
            dialog.add_button(sight::ui::dialog::message::yes_no);
            sight::ui::dialog::message::buttons button = dialog.show();

            if(button == sight::ui::dialog::message::no)
            {
                return;
            }
        }

        // Retrieve dataStruct associated with this service
        const auto data   = m_data.lock();
        const auto vector = std::dynamic_pointer_cast<const data::vector>(data.get_shared());

        // Create series_set
        const auto series_set = std::make_shared<data::series_set>();

        for(const auto& object : *vector)
        {
            const auto& series = std::dynamic_pointer_cast<data::series>(object);
            SIGHT_ASSERT("The container should only contain series.", series);
            series_set->push_back(series);
        }

        sight::ui::busy_cursor cursor;

        save_series_set(folder, series_set);
    }
}

//------------------------------------------------------------------------------

void series_set_writer::save_series_set(const std::filesystem::path _folder, data::series_set::sptr _series_set)
{
    m_write_failed = true;

    auto writer = std::make_shared<sight::io::dicom::writer::series_set>();
    writer->set_object(_series_set);
    writer->set_fiducials_export_mode(m_fiducials_export_mode);
    writer->set_folder(_folder);

    try
    {
        sight::ui::dialog::progress progress_meter_gui("Saving series ");
        writer->add_handler(progress_meter_gui);
        writer->write();

        m_write_failed = false;
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::warning
        );
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::warning
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::service::path_type_t series_set_writer::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

bool series_set_writer::select_fiducials_export_mode()
{
    // Retrieve dataStruct associated with this service
    const auto data   = m_data.lock();
    const auto vector = std::dynamic_pointer_cast<const data::vector>(data.get_shared());

    // Create series_set
    auto series_set = std::make_shared<data::series_set>();

    for(const auto& object : *vector)
    {
        const auto& series = std::dynamic_pointer_cast<data::series>(object);
        SIGHT_ASSERT("The container should only contain series.", series);
        series_set->push_back(series);
    }

    const bool contains_landmarks    = sight::io::dicom::helper::fiducial::contains_landmarks(series_set);
    const bool contains_distances    = sight::io::dicom::helper::fiducial::contains_distances(series_set);
    const bool contains_3d_distances = sight::io::dicom::helper::fiducial::contains_3d_distances(series_set);

    if(contains_landmarks || contains_distances)
    {
        static const std::string s_FIDUCIAL_IOD          = "Spatial Fiducials";
        static const std::string s_COMPREHENSIVE_SRIOD   = "Comprehensive SR";
        static const std::string s_COMPREHENSIVE3_DSRIOD = "Comprehensive 3D SR";

        std::vector<std::string> export_modes;
        if(!contains_distances)
        {
            export_modes.push_back(s_FIDUCIAL_IOD);
        }

        if(!contains_3d_distances)
        {
            export_modes.push_back(s_COMPREHENSIVE_SRIOD);
        }

        export_modes.push_back(s_COMPREHENSIVE3_DSRIOD);

        // Create selector
        sight::ui::dialog::selector selector;

        selector.set_title("Fiducials export mode");
        selector.set_choices(export_modes);
        const auto& modes                     = selector.show();
        const bool mode_selection_is_canceled = modes.empty();

        if(mode_selection_is_canceled)
        {
            m_write_failed = true;
            return false;
        }

        const auto& mode = modes.front();

        if(mode == s_FIDUCIAL_IOD)
        {
            m_fiducials_export_mode = sight::io::dicom::writer::series::spatial_fiducials;
        }
        else if(mode == s_COMPREHENSIVE_SRIOD)
        {
            m_fiducials_export_mode = sight::io::dicom::writer::series::comprehensive_sr;
        }
        else
        {
            m_fiducials_export_mode = sight::io::dicom::writer::series::comprehensive_3_d_sr;
        }
    }

    return true;
}

} // namespace sight::module::io::dicom
