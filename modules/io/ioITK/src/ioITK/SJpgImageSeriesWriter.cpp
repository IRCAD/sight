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

#include "ioITK/SJpgImageSeriesWriter.hpp"

#include "ioITK/JpgImageWriterService.hpp"

#include <core/base.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/location/Folder.hpp>

#include <fwIO/IWriter.hpp>

#include <gui/Cursor.hpp>
#include <gui/dialog/LocationDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/dialog/ProgressDialog.hpp>

#include <services/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioITK::SJpgImageSeriesWriter, ::sight::data::ImageSeries )

//------------------------------------------------------------------------------

SJpgImageSeriesWriter::SJpgImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

SJpgImageSeriesWriter::~SJpgImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SJpgImageSeriesWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    gui::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
    dialog.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialog.setOption(gui::dialog::ILocationDialog::WRITE);
    dialog.setType(gui::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result;

    while (result = data::location::Folder::dynamicCast( dialog.show() ))
    {
        if( std::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        gui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(gui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(gui::dialog::IMessageDialog::YES);
        messageBox.addButton(gui::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == gui::dialog::IMessageDialog::YES)
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

void SJpgImageSeriesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::info(std::ostream& _sstream )
{
    _sstream << "SJpgImageSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::updating()
{

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::ImageSeries::csptr imageSeries = this->getInput< data::ImageSeries >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", imageSeries);

        SLM_ASSERT("Image from image series is not instanced", imageSeries->getImage());

        gui::Cursor cursor;
        cursor.setCursor(gui::ICursor::BUSY);
        JpgImageWriterService::saveImage(this->getFolder(), imageSeries->getImage());
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
