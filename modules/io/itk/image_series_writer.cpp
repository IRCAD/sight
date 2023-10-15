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

#include "image_series_writer.hpp"

#include "image_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_file.hpp>
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

image_series_writer::image_series_writer() noexcept =
    default;

//------------------------------------------------------------------------------

image_series_writer::~image_series_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_series_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_series_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void image_series_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an image file to save image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialogFile.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        this->set_file(result->get_file());
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void image_series_writer::starting()
{
}

//------------------------------------------------------------------------------

void image_series_writer::stopping()
{
}

//------------------------------------------------------------------------------

void image_series_writer::info(std::ostream& _sstream)
{
    _sstream << "image_series_writer::info";
}

//------------------------------------------------------------------------------

void image_series_writer::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        const auto data         = m_data.lock();
        const auto image_series = std::dynamic_pointer_cast<const data::image_series>(data.get_shared());
        SIGHT_ASSERT(
            "The input key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.",
            image_series
        );

        sight::ui::BusyCursor cursor;
        image_writer::saveImage(this->get_file(), image_series);
        m_writeFailed = false;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
