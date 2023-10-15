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

#include "modules/io/vtk/model_series_obj_writer.hpp"

#include "modules/io/vtk/mesh_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/uuid.hpp>

#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <io/vtk/ModelSeriesObjWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

model_series_obj_writer::model_series_obj_writer() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType model_series_obj_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void model_series_obj_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation(defaultDirectory);
    dialog.setOption(ui::dialog::location::WRITE);
    dialog.setType(ui::dialog::location::FOLDER);

    core::location::single_folder::sptr result;

    while((result = std::dynamic_pointer_cast<core::location::single_folder>(dialog.show())))
    {
        if(std::filesystem::is_empty(result->get_folder()))
        {
            break;
        }

        // message box
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(ui::dialog::message::QUESTION);
        messageBox.addButton(ui::dialog::message::YES);
        messageBox.addButton(ui::dialog::message::CANCEL);
        if(messageBox.show() == sight::ui::dialog::message::YES)
        {
            break;
        }
    }

    if(result)
    {
        this->set_folder(result->get_folder());
        defaultDirectory->set_folder(result->get_folder().parent_path());
        dialog.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void model_series_obj_writer::starting()
{
}

//------------------------------------------------------------------------------

void model_series_obj_writer::stopping()
{
}

//------------------------------------------------------------------------------

void model_series_obj_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void model_series_obj_writer::info(std::ostream& _sstream)
{
    _sstream << "model_series_obj_writer::info";
}

//------------------------------------------------------------------------------

void model_series_obj_writer::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked      = m_data.lock();
        const auto modelSeries = std::dynamic_pointer_cast<const data::model_series>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::model_series::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            modelSeries
        );

        auto writer = std::make_shared<sight::io::vtk::ModelSeriesObjWriter>();
        writer->set_object(modelSeries);
        writer->set_folder(this->get_folder());

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        try
        {
            m_sigJobCreated->emit(writer->getJob());
            writer->write();

            m_writeFailed = false;
        }
        catch(const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            sight::ui::dialog::message messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage(ss.str());
            messageBox.setIcon(ui::dialog::message::WARNING);
            messageBox.addButton(ui::dialog::message::OK);
            messageBox.show();
        }
        catch(...)
        {
            std::stringstream ss;
            ss << "Warning during saving.";

            sight::ui::dialog::message messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage(ss.str());
            messageBox.setIcon(ui::dialog::message::WARNING);
            messageBox.addButton(ui::dialog::message::OK);
            messageBox.show();
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
