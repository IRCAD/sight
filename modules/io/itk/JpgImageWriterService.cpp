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

#include "JpgImageWriterService.hpp"

#include <core/base.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Image.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/itk/JpgImageWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

JpgImageWriterService::JpgImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

JpgImageWriterService::~JpgImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType JpgImageWriterService::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void JpgImageWriterService::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::ILocationDialog::FOLDER);

    auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());
    if(result)
    {
        this->setFolder(result->getFolder());
        defaultDirectory = result;
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void JpgImageWriterService::starting()
{
}

//------------------------------------------------------------------------------

void JpgImageWriterService::stopping()
{
}

//------------------------------------------------------------------------------

void JpgImageWriterService::info(std::ostream& _sstream)
{
    _sstream << "JpgImageWriterService::info";
}

//------------------------------------------------------------------------------

void JpgImageWriterService::saveImage(const std::filesystem::path& imgPath, const CSPTR(data::Image)& img)
{
    auto writer = sight::io::itk::JpgImageWriter::New();
    sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving image... ");

    writer->setFolder(imgPath);
    writer->setObject(img);

    try
    {
        writer->addHandler(progressMeterGUI);
        writer->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during saving",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//------------------------------------------------------------------------------

void JpgImageWriterService::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        data::Image::csptr image = this->getInput<data::Image>(sight::io::base::service::s_DATA_KEY);
        SIGHT_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", image);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        saveImage(this->getFolder(), image);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
