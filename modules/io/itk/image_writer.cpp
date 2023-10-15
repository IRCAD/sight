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

#include "image_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/image.hpp>

#include <io/__/service/writer.hpp>
#include <io/itk/InrImageWriter.hpp>
#include <io/itk/JpgImageWriter.hpp>
#include <io/itk/NiftiImageWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::itk
{

/**
 * Do not mark `nifti`  as incorrect.
 * cspell:ignore nifti
 */

//------------------------------------------------------------------------------

image_writer::image_writer() noexcept =
    default;

//------------------------------------------------------------------------------

image_writer::~image_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
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

void image_writer::starting()
{
}

//------------------------------------------------------------------------------

void image_writer::stopping()
{
}

//------------------------------------------------------------------------------

void image_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void image_writer::info(std::ostream& _sstream)
{
    _sstream << "image_writer::info";
}

//------------------------------------------------------------------------------

bool image_writer::saveImage(const std::filesystem::path& imgSavePath, const data::image::csptr& image)
{
    sight::io::writer::object_writer::sptr myWriter;
    std::string ext = imgSavePath.extension().string();
    boost::algorithm::to_lower(ext);

    if(boost::algorithm::ends_with(imgSavePath.string(), ".inr.gz"))
    {
        auto inrWriter = std::make_shared<sight::io::itk::InrImageWriter>();
        sight::ui::dialog::progress progressMeterGUI("Saving images... ");
        inrWriter->add_handler(progressMeterGUI);
        inrWriter->set_file(imgSavePath);
        myWriter = inrWriter;
    }
    else if(ext == ".nii" || boost::algorithm::ends_with(imgSavePath.string(), ".nii.gz"))
    {
        auto niftiWriter = std::make_shared<sight::io::itk::NiftiImageWriter>();
        niftiWriter->set_file(imgSavePath);
        myWriter = niftiWriter;
    }
    else if(std::filesystem::is_directory(imgSavePath))
    {
        auto jpgWriter = std::make_shared<sight::io::itk::JpgImageWriter>();
        sight::ui::dialog::progress progressMeterGUI("Saving images... ");
        jpgWriter->add_handler(progressMeterGUI);
        jpgWriter->set_folder(imgSavePath);
        myWriter = jpgWriter;
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Unsupported " + ext + " format (Available formats: "
                + ".inr.gz, .nii, .jpg, .jpeg)"
            )
        );
    }

    myWriter->set_object(image);

    try
    {
        myWriter->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        return false;
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::WARNING
        );
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------

void image_writer::updating()
{
    m_writeFailed = true;
    if(this->hasLocationDefined())
    {
        const auto data  = m_data.lock();
        const auto image = std::dynamic_pointer_cast<const data::image>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", image);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);
        try
        {
            saveImage(this->get_file(), image);
            m_writeFailed = false;
        }
        catch(core::tools::failed& e)
        {
            SIGHT_THROW_EXCEPTION(e);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
