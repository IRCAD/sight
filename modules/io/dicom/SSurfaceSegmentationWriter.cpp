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

#include "SSurfaceSegmentationWriter.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/helper/Fiducial.hpp>
#include <io/dicom/writer/Series.hpp>
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

SSurfaceSegmentationWriter::SSurfaceSegmentationWriter() noexcept =
    default;

//------------------------------------------------------------------------------

SSurfaceSegmentationWriter::~SSurfaceSegmentationWriter() noexcept =
    default;

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::openLocationDialog()
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

void SSurfaceSegmentationWriter::starting()
{
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::updating()
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
        const auto model = std::dynamic_pointer_cast<const data::ModelSeries>(data.get_shared());

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

void SSurfaceSegmentationWriter::saveSurfaceSegmentation(
    const std::filesystem::path filename,
    const data::ModelSeries::csptr& model
)
{
    auto writer = std::make_shared<sight::io::dicom::writer::SurfaceSegmentation>();

    writer->setObject(model);
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

sight::io::service::IOPathType SSurfaceSegmentationWriter::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

} // namespace sight::module::io::dicom
