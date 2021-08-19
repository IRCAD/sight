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

#include "InrImageReaderService.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Image.hpp>

#include <io/base/service/IReader.hpp>
#include <io/itk/ImageReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

InrImageReaderService::InrImageReaderService() noexcept
{
}

//------------------------------------------------------------------------------

InrImageReaderService::~InrImageReaderService() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType InrImageReaderService::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//
//------------------------------------------------------------------------------

void InrImageReaderService::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void InrImageReaderService::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an Inrimage file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        this->setFile(result->getFile());
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void InrImageReaderService::info(std::ostream& _sstream)
{
    _sstream << "InrImageReaderService::info";
}

//------------------------------------------------------------------------------

bool InrImageReaderService::createImage(
    const std::filesystem::path& inrFileDir,
    const data::Image::sptr& _pImg
)
{
    auto myLoader = sight::io::itk::ImageReader::New();
    bool ok       = true;

    myLoader->setObject(_pImg);
    myLoader->setFile(inrFileDir);

    try
    {
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Loading Image ");
        myLoader->addHandler(progressMeterGUI);
        myLoader->read();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }
    return ok;
}

//------------------------------------------------------------------------------

void InrImageReaderService::updating()
{
    if(this->hasLocationDefined())
    {
        data::Image::sptr image = this->getInOut<data::Image>(sight::io::base::service::s_DATA_KEY);
        SIGHT_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", image);

        if(this->createImage(this->getFile(), image))
        {
            sight::ui::base::Cursor cursor;
            cursor.setCursor(ui::base::ICursor::BUSY);
            notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            m_readFailed = true;
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void InrImageReaderService::notificationOfDBUpdate()
{
    data::Image::sptr image = this->getInOut<data::Image>(sight::io::base::service::s_DATA_KEY);
    SIGHT_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", image);

    auto sig = image->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
