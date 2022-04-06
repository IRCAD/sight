/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SSlicedImageSeriesWriter.hpp"

#include "modules/io/itk/SImageWriter.hpp"

#include <core/base.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>

#include <io/base/service/IWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

SSlicedImageSeriesWriter::SSlicedImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

SSlicedImageSeriesWriter::~SSlicedImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SSlicedImageSeriesWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SSlicedImageSeriesWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SSlicedImageSeriesWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
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

void SSlicedImageSeriesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SSlicedImageSeriesWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SSlicedImageSeriesWriter::info(std::ostream& _sstream)
{
    _sstream << "SSlicedImageSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SSlicedImageSeriesWriter::updating()
{
    if(this->hasLocationDefined())
    {
        const auto data        = m_data.lock();
        const auto imageSeries = std::dynamic_pointer_cast<const data::ImageSeries>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", imageSeries);

        SIGHT_ASSERT("Image from image series is not instanced", imageSeries->getImage());

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        SImageWriter::saveImage(this->getFolder(), imageSeries->getImage());
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
