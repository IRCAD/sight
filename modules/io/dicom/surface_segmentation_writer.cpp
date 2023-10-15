/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "surface_segmentation_writer.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/dicom_series.hpp>
#include <data/series.hpp>
#include <data/vector.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/helper/Fiducial.hpp>
#include <io/dicom/writer/series.hpp>
#include <io/dicom/writer/SurfaceSegmentation.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>
#include <ui/__/dialog/selector.hpp>

#include <string>

namespace sight::module::io::dicom
{

//------------------------------------------------------------------------------

surface_segmentation_writer::surface_segmentation_writer() noexcept =
    default;

//------------------------------------------------------------------------------

surface_segmentation_writer::~surface_segmentation_writer() noexcept =
    default;

//------------------------------------------------------------------------------

void surface_segmentation_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_folder());
        this->set_folder(result->get_folder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::starting()
{
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::stopping()
{
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::updating()
{
    if(this->hasLocationDefined())
    {
        const std::filesystem::path& folder = this->get_folder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::dialog::message dialog;
            dialog.setMessage(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(ui::dialog::message::QUESTION);
            dialog.addButton(sight::ui::dialog::message::YES_NO);
            sight::ui::dialog::message::Buttons button = dialog.show();

            if(button == sight::ui::dialog::message::NO)
            {
                m_writeFailed = true;
                return;
            }
        }
        else
        {
            m_writeFailed = true;
        }

        // Retrieve dataStruct associated with this service
        const auto data  = m_data.lock();
        const auto model = std::dynamic_pointer_cast<const data::model_series>(data.get_shared());

        if(!model->getDicomReference())
        {
            m_writeFailed = true;
            sight::ui::dialog::message::show(
                "Warning",
                "DICOM image reference is missing, DICOM Surface Segmentation cannot be generated",
                sight::ui::dialog::message::WARNING
            );
            return;
        }

        /* Build up the filename */
        std::filesystem::path outputPath = this->get_folder() / "imSeg";

        /* Write the data */
        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);
        saveSurfaceSegmentation(outputPath, model);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::saveSurfaceSegmentation(
    const std::filesystem::path filename,
    const data::model_series::csptr& model
)
{
    auto writer = std::make_shared<sight::io::dicom::writer::SurfaceSegmentation>();

    writer->set_object(model);
    writer->set_file(filename);

    try
    {
        writer->write();
    }
    catch(const std::exception& e)
    {
        m_writeFailed = true;
        std::stringstream ss;
        ss << "Warning during saving: " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
    }
    catch(...)
    {
        m_writeFailed = true;
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::service::IOPathType surface_segmentation_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

} // namespace sight::module::io::dicom
