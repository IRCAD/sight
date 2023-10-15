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

#include "sliced_image_series_writer.hpp"

#include "modules/io/itk/image_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_folder.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <io/__/service/writer.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

sliced_image_series_writer::sliced_image_series_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sliced_image_series_writer::~sliced_image_series_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType sliced_image_series_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
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

void sliced_image_series_writer::starting()
{
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::stopping()
{
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::info(std::ostream& _sstream)
{
    _sstream << "sliced_image_series_writer::info";
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::updating()
{
    m_writeFailed = true;
    if(this->hasLocationDefined())
    {
        const auto data        = m_data.lock();
        const auto imageSeries = std::dynamic_pointer_cast<const data::image_series>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", imageSeries);

        sight::ui::BusyCursor cursor;
        image_writer::saveImage(this->get_folder(), imageSeries);
        m_writeFailed = false;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
