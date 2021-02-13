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

#include "InrImageWriterService.hpp"

#include <core/base.hpp>

#include <data/Image.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <service/macros.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/itk/ImageWriter.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::modules::io::itk
{

fwServicesRegisterMacro( ::sight::io::base::service::IWriter, ::sight::modules::io::itk::InrImageWriterService,
                         ::sight::data::Image )

//------------------------------------------------------------------------------

InrImageWriterService::InrImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

InrImageWriterService::~InrImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType InrImageWriterService::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void InrImageWriterService::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void InrImageWriterService::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void InrImageWriterService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an inrimage file to save image" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        this->setFile( result->getPath() );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void InrImageWriterService::starting()
{
}

//------------------------------------------------------------------------------

void InrImageWriterService::stopping()
{
}

//------------------------------------------------------------------------------

void InrImageWriterService::info(std::ostream& _sstream )
{
    _sstream << "InrImageWriterService::info";
}

//------------------------------------------------------------------------------

void InrImageWriterService::saveImage( const std::filesystem::path& inrFile, const data::Image::csptr& image )
{
    auto myWriter = sight::io::itk::ImageWriter::New();

    myWriter->setObject(image);
    myWriter->setFile(inrFile);

    try
    {
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter->addHandler( progressMeterGUI );
        myWriter->write();

    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::base::dialog::MessageDialog::show("Warning",
                                                     ss.str(),
                                                     sight::ui::base::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        sight::ui::base::dialog::MessageDialog::show("Warning",
                                                     "Warning during saving",
                                                     sight::ui::base::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void InrImageWriterService::updating()
{

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::Image::csptr image = this->getInput< data::Image >(sight::io::base::service::s_DATA_KEY);
        SLM_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", image);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        saveImage(this->getFile(), image);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::modules::io::itk
