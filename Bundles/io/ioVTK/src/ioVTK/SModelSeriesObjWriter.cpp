/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "ioVTK/SModelSeriesObjWriter.hpp"

#include "ioVTK/SMeshWriter.hpp"

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/UUID.hpp>

#include <fwVtkIO/ModelSeriesObjWriter.hpp>

#include <boost/filesystem/operations.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioVTK::SModelSeriesObjWriter, ::fwMedData::ModelSeries );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesObjWriter::SModelSeriesObjWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SModelSeriesObjWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
    dialog.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialog.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialog.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;

    while (result = ::fwData::location::Folder::dynamicCast( dialog.show() ))
    {
        if( ::boost::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == ::fwGui::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialog.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());
    }
    else
    {
        m_writeFailed = true;
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::info(std::ostream& _sstream )
{
    _sstream << "SModelSeriesObjWriter::info";
}

//------------------------------------------------------------------------------

void SModelSeriesObjWriter::updating()
{
    SLM_TRACE_FUNC();

    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ModelSeries::csptr modelSeries = this->getInput< ::fwMedData::ModelSeries >(::fwIO::s_DATA_KEY);

        if(!modelSeries)
        {
            m_writeFailed = true;
        }

        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", modelSeries);

        ::fwVtkIO::ModelSeriesObjWriter::sptr writer = ::fwVtkIO::ModelSeriesObjWriter::New();
        writer->setObject(modelSeries);
        writer->setFolder(this->getFolder());

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

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

            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        catch( ... )
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving.";

            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
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
