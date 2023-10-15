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
    m_fiducialsExportMode(sight::io::dicom::writer::series::COMPREHENSIVE_3D_SR)
{
}

//------------------------------------------------------------------------------

void series_set_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result && this->selectFiducialsExportMode())
    {
        defaultDirectory->set_folder(result->get_folder());
        this->set_folder(result->get_folder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
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
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        const std::filesystem::path& folder = this->get_folder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::dialog::message dialog;
            dialog.setMessage(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(ui::dialog::message::QUESTION);
            dialog.addButton(sight::ui::dialog::message::YES_NO);
            sight::ui::dialog::message::Buttons button = dialog.show();

            if(button == sight::ui::dialog::message::NO)
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

        sight::ui::BusyCursor cursor;

        saveSeriesSet(folder, series_set);
    }
}

//------------------------------------------------------------------------------

void series_set_writer::saveSeriesSet(const std::filesystem::path folder, data::series_set::sptr series_set)
{
    m_writeFailed = true;

    auto writer = std::make_shared<sight::io::dicom::writer::series_set>();
    writer->set_object(series_set);
    writer->setFiducialsExportMode(m_fiducialsExportMode);
    writer->set_folder(folder);

    try
    {
        sight::ui::dialog::progress progressMeterGUI("Saving series ");
        writer->add_handler(progressMeterGUI);
        writer->write();

        m_writeFailed = false;
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::service::IOPathType series_set_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

bool series_set_writer::selectFiducialsExportMode()
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

    const bool containsLandmarks   = sight::io::dicom::helper::Fiducial::containsLandmarks(series_set);
    const bool containsDistances   = sight::io::dicom::helper::Fiducial::containsDistances(series_set);
    const bool contains3DDistances = sight::io::dicom::helper::Fiducial::contains3DDistances(series_set);

    if(containsLandmarks || containsDistances)
    {
        static const std::string fiducialIOD          = "Spatial Fiducials";
        static const std::string comprehensiveSRIOD   = "Comprehensive SR";
        static const std::string comprehensive3DSRIOD = "Comprehensive 3D SR";

        std::vector<std::string> exportModes;
        if(!containsDistances)
        {
            exportModes.push_back(fiducialIOD);
        }

        if(!contains3DDistances)
        {
            exportModes.push_back(comprehensiveSRIOD);
        }

        exportModes.push_back(comprehensive3DSRIOD);

        // Create selector
        auto selector = std::make_shared<sight::ui::dialog::selector>();

        selector->setTitle("Fiducials export mode");
        selector->set_choices(exportModes);
        const std::string mode             = selector->show()[0];
        const bool modeSelectionIsCanceled = mode.empty();

        if(mode == fiducialIOD)
        {
            m_fiducialsExportMode = sight::io::dicom::writer::series::SPATIAL_FIDUCIALS;
        }
        else if(mode == comprehensiveSRIOD)
        {
            m_fiducialsExportMode = sight::io::dicom::writer::series::COMPREHENSIVE_SR;
        }
        else
        {
            m_fiducialsExportMode = sight::io::dicom::writer::series::COMPREHENSIVE_3D_SR;
        }

        if(mode.empty())
        {
            m_writeFailed = true;
        }

        return !modeSelectionIsCanceled;
    }

    return true;
}

} // namespace sight::module::io::dicom
