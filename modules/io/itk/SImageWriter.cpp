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

#include "SImageWriter.hpp"

#include <core/base.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Image.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/itk/InrImageWriter.hpp>
#include <io/itk/JpgImageWriter.hpp>
#include <io/itk/NiftiImageWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::itk
{

/**
 * Do not mark `nifti`  as incorrect.
 * cspell:ignore nifti
 */

//------------------------------------------------------------------------------

SImageWriter::SImageWriter() noexcept
{
}

//------------------------------------------------------------------------------

SImageWriter::~SImageWriter() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SImageWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SImageWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialogFile.addFilter("NIfTI (.nii)", "*.nifti");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

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

void SImageWriter::starting()
{
}

//------------------------------------------------------------------------------

void SImageWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SImageWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageWriter::info(std::ostream& _sstream)
{
    _sstream << "SImageWriter::info";
}

//------------------------------------------------------------------------------

bool SImageWriter::saveImage(const std::filesystem::path& imgSavePath, const data::Image::csptr& image)
{
    sight::io::base::writer::IObjectWriter::sptr myWriter;
    std::string ext = imgSavePath.extension().string();
    boost::algorithm::to_lower(ext);

    if(imgSavePath.string().find(".inr.gz") != std::string::npos)
    {
        auto inrWriter = sight::io::itk::InrImageWriter::New();
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving images... ");
        inrWriter->addHandler(progressMeterGUI);
        inrWriter->setFile(imgSavePath);
        myWriter = inrWriter;
    }
    else if(ext == ".nii")
    {
        auto niftiWriter = sight::io::itk::NiftiImageWriter::New();
        niftiWriter->setFile(imgSavePath);
        myWriter = niftiWriter;
    }
    else if(std::filesystem::is_directory(imgSavePath))
    {
        auto jpgWriter = sight::io::itk::JpgImageWriter::New();
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving images... ");
        jpgWriter->addHandler(progressMeterGUI);
        jpgWriter->setFolder(imgSavePath);
        myWriter = jpgWriter;
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::Failed(
                "Unsupported " + ext + " format (Available formats: "
                + ".inr.gz, .nii, .jpg, .jpeg)"
            )
        );
    }

    myWriter->setObject(image);

    try
    {
        myWriter->write();
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
        return false;
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during saving",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------

void SImageWriter::updating()
{
    m_writeFailed = true;
    if(this->hasLocationDefined())
    {
        const auto data  = m_data.lock();
        const auto image = std::dynamic_pointer_cast<const data::Image>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", image);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        try
        {
            saveImage(this->getFile(), image);
            m_writeFailed = false;
        }
        catch(core::tools::Failed& e)
        {
            SIGHT_THROW_EXCEPTION(e);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
