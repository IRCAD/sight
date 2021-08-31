/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/io/vtk/SModelSeriesObjWriter.hpp"

#include "modules/io/vtk/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/UUID.hpp>

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <io/vtk/ModelSeriesObjWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesObjWriter::SModelSeriesObjWriter() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SModelSeriesObjWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation(defaultDirectory);
    dialog.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialog.setType(ui::base::dialog::ILocationDialog::FOLDER);

    core::location::SingleFolder::sptr result;

    while((result = core::location::SingleFolder::dynamicCast(dialog.show())))
    {
        if(std::filesystem::is_empty(result->getFolder()))
        {
            break;
        }

        // message box
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::YES);
        messageBox.addButton(ui::base::dialog::IMessageDialog::CANCEL);
        if(messageBox.show() == sight::ui::base::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if(result)
    {
        this->setFolder(result->getFolder());
        defaultDirectory->setFolder(result->getFolder().parent_path());
        dialog.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::starting()
{
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::info(std::ostream& _sstream)
{
    _sstream << "SModelSeriesObjWriter::info";
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked      = m_data.lock();
        const auto modelSeries = std::dynamic_pointer_cast<const data::ModelSeries>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::ModelSeries::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            modelSeries
        );

        auto writer = sight::io::vtk::ModelSeriesObjWriter::New();
        writer->setObject(modelSeries);
        writer->setFolder(this->getFolder());

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

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

            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage(ss.str());
            messageBox.setIcon(ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        catch(...)
        {
            std::stringstream ss;
            ss << "Warning during saving.";

            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage(ss.str());
            messageBox.setIcon(ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
