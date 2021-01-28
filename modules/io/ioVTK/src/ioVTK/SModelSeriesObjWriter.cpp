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

#include "ioVTK/SModelSeriesObjWriter.hpp"

#include "ioVTK/SMeshWriter.hpp"

#include <core/base.hpp>
#include <core/tools/UUID.hpp>

#include <data/location/Folder.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <fwVtkIO/ModelSeriesObjWriter.hpp>

#include <services/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <filesystem>

namespace ioVTK
{

fwServicesRegisterMacro( io::base::services::IWriter, ::ioVTK::SModelSeriesObjWriter, ::sight::data::ModelSeries )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesObjWriter::SModelSeriesObjWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::io::base::services::IOPathType SModelSeriesObjWriter::getIOPathType() const
{
    return io::base::services::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ui::base::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialog.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialog.setType(ui::base::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result;

    while (result = data::location::Folder::dynamicCast( dialog.show() ))
    {
        if( std::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::YES);
        messageBox.addButton(ui::base::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == ui::base::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialog.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());
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
    io::base::services::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::info(std::ostream& _sstream )
{
    _sstream << "SModelSeriesObjWriter::info";
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::updating()
{

    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::ModelSeries::csptr modelSeries = this->getInput< data::ModelSeries >(io::base::services::s_DATA_KEY);
        SLM_ASSERT("The input key '" + io::base::services::s_DATA_KEY + "' is not correctly set.", modelSeries);

        ::fwVtkIO::ModelSeriesObjWriter::sptr writer = ::fwVtkIO::ModelSeriesObjWriter::New();
        writer->setObject(modelSeries);
        writer->setFolder(this->getFolder());

        ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        try
        {
            m_sigJobCreated->emit(writer->getJob());
            writer->write();
        }
        catch (const std::exception& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        catch( ... )
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving.";

            ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
