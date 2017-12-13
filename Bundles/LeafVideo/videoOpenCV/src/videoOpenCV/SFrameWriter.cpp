/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoOpenCV/SFrameWriter.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace videoOpenCV
{

fwServicesRegisterMacro( ::io::IWriter, ::videoOpenCV::SFrameWriter, ::arData::FrameTL);

static const ::fwCom::Slots::SlotKeyType s_SAVE_FRAME   = "saveFrame";
static const ::fwCom::Slots::SlotKeyType s_START_RECORD = "startRecord";
static const ::fwCom::Slots::SlotKeyType s_STOP_RECORD  = "stopRecord";
static const ::fwCom::Slots::SlotKeyType s_WRITE        = "write";

//------------------------------------------------------------------------------

SFrameWriter::SFrameWriter() noexcept :
    m_imageType(0),
    m_isRecording(false)
{
    newSlot(s_SAVE_FRAME, &SFrameWriter::saveFrame, this);
    newSlot(s_START_RECORD, &SFrameWriter::startRecord, this);
    newSlot(s_STOP_RECORD, &SFrameWriter::stopRecord, this);
    newSlot(s_WRITE, &SFrameWriter::write, this);
}

//------------------------------------------------------------------------------

SFrameWriter::~SFrameWriter() noexcept
{
}

//------------------------------------------------------------------------------

::io::IOPathType SFrameWriter::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

void SFrameWriter::configuring()
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SFrameWriter::starting()
{
}

//------------------------------------------------------------------------------

void SFrameWriter::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the frames" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());
    }
    else
    {
        this->clearLocations();
    }

}

//------------------------------------------------------------------------------

void SFrameWriter::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SFrameWriter::updating()
{

    ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    this->saveFrame(timestamp);
}

//------------------------------------------------------------------------------

void SFrameWriter::saveFrame(::fwCore::HiResClock::HiResClockType _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SFrameWriter::write(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (m_isRecording)
    {
        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(::io::s_DATA_KEY);
        // The following lock causes the service to drop frames if under heavy load. This prevents desynchronization
        // between frames and timestamps.
        // TODO: experiment with queuing frames and writing them from a worker thread.
        const auto sig = frameTL->signal< ::arData::FrameTL::ObjectPushedSignalType>(
            ::arData::FrameTL::s_OBJECT_PUSHED_SIG);
        ::fwCom::Connection::Blocker writeBlocker(sig->getConnection(m_slots[s_WRITE]));

        // Get the buffer of the copied timeline
        CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

        if (buffer)
        {
            int width  = static_cast<int>( frameTL->getWidth() );
            int height = static_cast<int>( frameTL->getHeight() );

            const std::uint8_t* imageBuffer = &buffer->getElement(0);

            ::cv::Mat image(::cv::Size(width, height), m_imageType, (void*)imageBuffer, ::cv::Mat::AUTO_STEP);

            size_t time = static_cast<size_t>(timestamp);
            std::string filename( "img_" + std::to_string(time) + ".tiff");
            ::boost::filesystem::path path = this->getFolder() / filename;

            if (image.type() == CV_8UC3)
            {
                // convert the read image from BGR to RGB
                ::cv::Mat imageRgb;
                ::cv::cvtColor(image, imageRgb, ::cv::COLOR_BGR2RGB);
                ::cv::imwrite(path.string(), imageRgb);
            }
            else if (image.type() == CV_8UC4)
            {
                // convert the read image from BGRA to RGBA
                ::cv::Mat imageRgb;
                ::cv::cvtColor(image, imageRgb, ::cv::COLOR_BGRA2RGBA);
                ::cv::imwrite(path.string(), imageRgb);
            }
            else
            {
                ::cv::imwrite(path.string(), image);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::startRecord()
{
    if (!this->hasLocationDefined())
    {
        this->configureWithIHM();
    }

    if (this->hasLocationDefined())
    {
        ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(::io::s_DATA_KEY);

        if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if (frameTL->getType() == ::fwTools::Type::s_UINT8 && frameTL->getNumberOfComponents() == 1)
        {
            m_imageType = CV_8UC1;
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

        ::boost::filesystem::path path = this->getFolder();

        if (!::boost::filesystem::exists(path))
        {
            ::boost::filesystem::create_directories(path);
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::stopRecord()
{
    m_isRecording = false;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrameWriter::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(::io::s_DATA_KEY, ::arData::FrameTL::s_OBJECT_PUSHED_SIG, s_WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace videoOpenCV
