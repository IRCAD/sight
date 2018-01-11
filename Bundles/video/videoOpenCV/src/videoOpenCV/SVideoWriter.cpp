/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOpenCV/SVideoWriter.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
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

fwServicesRegisterMacro( ::io::IWriter, ::videoOpenCV::SVideoWriter, ::arData::FrameTL);

static const ::fwCom::Slots::SlotKeyType s_SAVE_FRAME   = "saveFrame";
static const ::fwCom::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const ::fwCom::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";

//------------------------------------------------------------------------------

SVideoWriter::SVideoWriter() noexcept :
    m_imageType(0)
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

::io::IOPathType SVideoWriter::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SVideoWriter::configuring()
{
    ::io::IWriter::configuring();
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

void SVideoWriter::saveFrame(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (m_writer && m_writer->isOpened())
    {
        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(::io::s_DATA_KEY);

        // Get the buffer of the copied timeline
        CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

        if (buffer)
        {
            // Save RGB image
            int width  = static_cast<int>( frameTL->getWidth() );
            int height = static_cast<int>( frameTL->getHeight() );

            const std::uint8_t* imageBuffer = &buffer->getElement(0);

            ::cv::Mat image(::cv::Size(width, height), m_imageType, (void*)imageBuffer, ::cv::Mat::AUTO_STEP);
            if (m_imageType == CV_16UC1)
            {
                // Convert the image to a RGB image
                ::cv::Mat img8bit;
                ::cv::Mat imgColor;
                image.convertTo(img8bit, CV_8UC1, 1/100.0);
                ::cv::cvtColor(img8bit, imgColor, ::cv::COLOR_GRAY2RGBA, 4);

                m_writer->write(imgColor);
            }
            else
            {
                m_writer->write(image);
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
        ::boost::filesystem::path path = this->getFile();

        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(::io::s_DATA_KEY);
        int width  = static_cast<int>( frameTL->getWidth() );
        int height = static_cast<int>( frameTL->getHeight() );

        if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if (frameTL->getType() == ::fwTools::Type::s_UINT16 && frameTL->getNumberOfComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            OSLM_ERROR("This type of frame : " + frameTL->getType().string() + " with " +
                       std::to_string(frameTL->getNumberOfComponents()) + " is not supported");
            return;
        }

        m_writer = std::unique_ptr< ::cv::VideoWriter >(
            new ::cv::VideoWriter(path.string(), CV_FOURCC('M', 'J', 'P', 'G'), 30, cvSize(width, height), true));

        if (!m_writer->isOpened())
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Video recording", "Unable to write the video in the file: "
                                                              + path.string());
            m_writer.reset();
        }
    }
}

//------------------------------------------------------------------------------

void SVideoWriter::stopRecord()
{
    if(m_writer)
    {
        m_writer->release();
        m_writer.reset();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVideoWriter::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(::io::s_DATA_KEY, ::arData::FrameTL::s_OBJECT_PUSHED_SIG, s_SAVE_FRAME);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace videoOpenCV
