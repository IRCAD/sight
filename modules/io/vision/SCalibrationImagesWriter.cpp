/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "SCalibrationImagesWriter.hpp"

#include <data/CalibrationInfo.hpp>
#include <data/Image.hpp>
#include <data/location/Folder.hpp>

#include <io/opencv/Image.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <opencv2/opencv.hpp>

#include <filesystem>
#include <sstream>

namespace sight::module::io::vision
{

//------------------------------------------------------------------------------

SCalibrationImagesWriter::SCalibrationImagesWriter() noexcept
{
}

//------------------------------------------------------------------------------

SCalibrationImagesWriter::~SCalibrationImagesWriter() noexcept
{

}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SCalibrationImagesWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SCalibrationImagesWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SCalibrationImagesWriter::openLocationDialog()
{
    static std::filesystem::path s_defaultPath;

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the images" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(s_defaultPath) );
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result = data::location::Folder::dynamicCast(dialogFile.show());

    if (result)
    {
        s_defaultPath = result->getFolder().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(s_defaultPath) );
        this->setFolder(result->getFolder());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SCalibrationImagesWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();

    const auto configTree = this->getConfigTree();
    m_fileExtension = configTree.get("format", ".tiff");
}

//------------------------------------------------------------------------------

void SCalibrationImagesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SCalibrationImagesWriter::updating()
{
    if( !m_fileExtension.empty() && this->hasLocationDefined() )
    {
        data::CalibrationInfo::csptr calibInfo =
            this->getInput< data::CalibrationInfo >(sight::io::base::service::s_DATA_KEY);
        SIGHT_ASSERT("Missing calibration info input.", calibInfo);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        size_t count(0);
        for(const auto& calibImg : calibInfo->getImageContainer())
        {
            std::ostringstream imageNumber;
            imageNumber << std::setw(4) << std::setfill('0') << count++;

            const std::string filename       = "img_" + imageNumber.str() + m_fileExtension;
            const std::filesystem::path path = this->getFolder() / filename;

            ::cv::Mat cvImg = sight::io::opencv::Image::copyToCv(calibImg);

            if(cvImg.dims == 3)
            {
                // Ensure that we have a true depth-less 2D image.
                cvImg = cvImg.reshape(0, 2, cvImg.size + 1);
            }

            try
            {
                if (cvImg.type() == CV_8UC3 || cvImg.type() == CV_8UC4)
                {
                    // convert the image from BGR to RGB
                    const auto colConvType = cvImg.type() == CV_8UC3 ? ::cv::COLOR_BGR2RGB : ::cv::COLOR_BGRA2RGBA;
                    ::cv::cvtColor(cvImg, cvImg, colConvType);
                }

                ::cv::imwrite(path.string(), cvImg);
            }
            catch(const ::cv::Exception& e)
            {
                m_writeFailed = true;
                sight::ui::base::dialog::MessageDialog::show("Error writing calibration images.",
                                                             e.what(),
                                                             sight::ui::base::dialog::MessageDialog::CRITICAL);
            }
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void SCalibrationImagesWriter::stopping()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
