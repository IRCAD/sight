/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "videoOpenCV/SVideoWriter.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace videoOpenCV
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::videoOpenCV::SVideoWriter, ::arData::FrameTL)

static const ::fwCom::Slots::SlotKeyType s_SAVE_FRAME = "saveFrame";
static const ::fwCom::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const ::fwCom::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";

//------------------------------------------------------------------------------

SVideoWriter::SVideoWriter() noexcept
{
    newSlot(s_SAVE_FRAME, &SVideoWriter::saveFrame, this);
    newSlot(s_START_RECORD, &SVideoWriter::startRecord, this);
    newSlot(s_STOP_RECORD, &SVideoWriter::stopRecord, this);
}

//------------------------------------------------------------------------------

SVideoWriter::~SVideoWriter() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SVideoWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SVideoWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SVideoWriter::starting()
{
}

//------------------------------------------------------------------------------

void SVideoWriter::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an file to save the video" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("avi", "*.avi");
    dialogFile.addFilter("mp4", "*.mp4");
    dialogFile.addFilter("m4v", "*.m4v");
    dialogFile.addFilter("mkv", "*.mkv");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }

}

//------------------------------------------------------------------------------

void SVideoWriter::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SVideoWriter::updating()
{
}

//------------------------------------------------------------------------------

void SVideoWriter::writeBuffer(int width, int height, CSPTR(::arData::FrameTL::BufferType)buffer)
{
    SLM_ASSERT("OpenCV video writer not initialized", m_writer);
    const std::uint8_t* imageBuffer = &buffer->getElement(0);

    ::cv::Mat image(::cv::Size(width, height), m_imageType, (void*)imageBuffer, ::cv::Mat::AUTO_STEP);
    if (m_imageType == CV_16UC1)
    {
        // Convert the image to a RGB image
        ::cv::Mat img8bit;
        ::cv::Mat imgColor;
        image.convertTo(img8bit, CV_8UC1, 1/100.0);
        ::cv::cvtColor(img8bit, imgColor, ::cv::COLOR_GRAY2RGB);

        m_writer->write(imgColor);
    }
    else if (m_imageType == CV_8UC3)
    {
        // convert the image from RGB to BGR
        ::cv::Mat imageBGR;
        ::cv::cvtColor(image, imageBGR, ::cv::COLOR_RGB2BGR);
        m_writer->write(imageBGR);
    }
    else if (m_imageType == CV_8UC4)
    {
        // convert the image from RGBA to BGR
        ::cv::Mat imageBGR;
        ::cv::cvtColor(image, imageBGR, ::cv::COLOR_RGBA2BGR);
        m_writer->write(imageBGR);
    }
    else
    {
        m_writer->write(image);
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::saveFrame(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (m_isRecording)
    {
        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(::fwIO::s_DATA_KEY);
        if(m_writer && m_writer->isOpened())
        {
            // Get the buffer of the copied timeline
            CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);
            if (buffer)
            {
                const int width  = static_cast<int>( frameTL->getWidth() );
                const int height = static_cast<int>( frameTL->getHeight() );
                this->writeBuffer(width, height, buffer);
            }
        }
        else
        {
            if(m_timestamps.size() >= 5 )
            {
                // computes number of fps
                const double fps = 1000 * m_timestamps.size() / (m_timestamps.back() - m_timestamps.front());
                OSLM_TRACE("Estimated FPS: " << fps);
                const int width                      = static_cast<int>( frameTL->getWidth() );
                const int height                     = static_cast<int>( frameTL->getHeight() );
                const ::boost::filesystem::path path = this->getFile();

                m_writer = std::make_unique< ::cv::VideoWriter >(path.string(), CV_FOURCC('M', 'J', 'P', 'G'),
                                                                 fps, cvSize(width, height), true);

                if (!m_writer->isOpened())
                {
                    ::fwGui::dialog::MessageDialog::showMessageDialog(
                        "Video recording", "Unable to write the video in the file: " + path.string());
                    this->stopRecord();
                }
                else
                {
                    for(const auto& oldTimestamp : m_timestamps)
                    {
                        // writes the old frames used to compute the number of fps
                        CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(oldTimestamp);
                        if (buffer)
                        {
                            this->writeBuffer(width, height, buffer);
                        }
                    }
                }
            }
            else
            {
                m_timestamps.push_back(timestamp);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::startRecord()
{
    if (!this->hasLocationDefined())
    {
        this->configureWithIHM();
    }

    if (this->hasLocationDefined())
    {
        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(::fwIO::s_DATA_KEY);

        if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if (frameTL->getType() == ::fwTools::Type::s_UINT16 && frameTL->getNumberOfComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SLM_ERROR("This type of frame : " + frameTL->getType().string() + " with " +
                      std::to_string(frameTL->getNumberOfComponents()) + " components is not supported");
            return;
        }
        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::stopRecord()
{
    m_isRecording = false;
    m_timestamps.clear();
    if(m_writer)
    {
        m_writer->release();
        m_writer.reset();
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVideoWriter::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(::fwIO::s_DATA_KEY, ::arData::FrameTL::s_OBJECT_PUSHED_SIG, s_SAVE_FRAME);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace videoOpenCV
