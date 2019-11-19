/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "videoQt/editor/SCamera.hpp"

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include <arData/CameraSeries.hpp>

#include <arPreferences/preferences.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Object.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/pathDifference.hpp>

#include <QByteArray>
#include <QCamera>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace videoQt
{
namespace editor
{

static const ::fwCom::Signals::SignalKeyType s_CONFIGURED_CAMERAS_SIG = "configuredCameras";
static const ::fwCom::Signals::SignalKeyType s_CONFIGURED_DEVICE_SIG  = "configuredDevice";
static const ::fwCom::Signals::SignalKeyType s_CONFIGURED_FILE_SIG    = "configuredFile";
static const ::fwCom::Signals::SignalKeyType s_CONFIGURED_STREAM_SIG  = "configuredStream";

static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_DEVICE_SLOT = "configureDevice";
static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_FILE_SLOT   = "configureFile";
static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_STREAM_SLOT = "configureStream";

static const ::fwServices::IService::KeyType s_CAMERA_SERIES_INOUT = "cameraSeries";
static const ::fwServices::IService::KeyType s_CAMERA_INOUT        = "camera";

static const std::string s_VIDEO_SUPPORT_CONFIG        = "videoSupport";
static const std::string s_CREATE_CAMERA_NUMBER_CONFIG = "createCameraNumber";
static const std::string s_LABEL_CONFIG                = "label";

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::videoQt::editor::SCamera )

//------------------------------------------------------------------------------

SCamera::SCamera() :
    m_sigConfiguredCameras(newSignal< ConfiguredSignalType >(s_CONFIGURED_CAMERAS_SIG))
{
    newSignal< ConfiguredSignalType >(s_CONFIGURED_DEVICE_SIG);
    newSignal< ConfiguredSignalType >(s_CONFIGURED_FILE_SIG);
    newSignal< ConfiguredSignalType >(s_CONFIGURED_STREAM_SIG);

    newSlot(s_CONFIGURE_DEVICE_SLOT, &SCamera::onChooseDevice, this );
    newSlot(s_CONFIGURE_FILE_SLOT, &SCamera::onChooseFile, this );
    newSlot(s_CONFIGURE_STREAM_SLOT, &SCamera::onChooseStream, this );
}

//------------------------------------------------------------------------------

SCamera::~SCamera() noexcept
{
}

//------------------------------------------------------------------------------

void SCamera::configuring()
{
    const ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_bVideoSupport    = (config.get<std::string>(s_VIDEO_SUPPORT_CONFIG, "no") == "yes");
    m_numCreateCameras = config.get<size_t>(s_CREATE_CAMERA_NUMBER_CONFIG, m_numCreateCameras);
    m_label            = config.get<std::string>(s_LABEL_CONFIG, m_label);

    this->initialize();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->create();

    const ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QPointer<QHBoxLayout> layout = new QHBoxLayout();

    if(!m_label.empty())
    {
        const QPointer<QLabel> sourceLabel = new QLabel(QString::fromStdString(m_label));
        layout->addWidget(sourceLabel);
    }

    m_devicesComboBox = new QComboBox();
    layout->addWidget(m_devicesComboBox);

    m_devicesComboBox->addItem("Device...", "device");

    // Add video file
    if(m_bVideoSupport)
    {
        m_devicesComboBox->addItem("File...", "file");
        m_devicesComboBox->addItem("Stream...", "stream");
    }

    qtContainer->setLayout(layout);

    ::QObject::connect(m_devicesComboBox, qOverload<int>(&QComboBox::activated), this, &SCamera::onApply);

    // Create camera data if necessary
    auto cameraSeries = this->getInOut< ::arData::CameraSeries >(s_CAMERA_SERIES_INOUT);
    if(cameraSeries)
    {
        const ::fwData::mt::ObjectWriteLock lock(cameraSeries);

        const size_t numCameras = cameraSeries->getNumberOfCameras();
        if(numCameras == 0)
        {
            SLM_ASSERT("No camera data in the CameraSeries.", m_numCreateCameras != 0);

            for(size_t i = 0; i < m_numCreateCameras; ++i)
            {
                ::arData::Camera::sptr camera = ::arData::Camera::New();
                const size_t index = cameraSeries->getNumberOfCameras();
                cameraSeries->addCamera(camera);
                cameraSeries->setExtrinsicMatrix(index, ::fwData::TransformationMatrix3D::New());
                const auto sig = cameraSeries->signal< ::arData::CameraSeries::AddedCameraSignalType >(
                    ::arData::CameraSeries::s_ADDED_CAMERA_SIG);
                sig->asyncEmit(camera);
            }
            OSLM_INFO("No camera data in the CameraSeries, " << m_numCreateCameras << " will be created.");
        }
        else
        {
            OSLM_WARN_IF("CameraSeries contains camera data but the service is configured to create " <<
                         m_numCreateCameras <<" cameras.", m_numCreateCameras != 0);
        }
    }
}

//------------------------------------------------------------------------------

void SCamera::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SCamera::updating()
{
}

//------------------------------------------------------------------------------

void SCamera::onApply(int _index)
{
    switch(_index)
    {
        case 0:
            this->onChooseDevice();
            break;
        case 1:
            this->onChooseFile();
            break;
        case 2:
            this->onChooseStream();
            break;
    }
}

//------------------------------------------------------------------------------

void SCamera::onChooseFile()
{
    std::vector< ::arData::Camera::sptr > cameras = this->getCameras();

    // Check preferences
    const ::boost::filesystem::path videoDirPreferencePath(::arPreferences::getVideoDir());

    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("All files", "*.*");
    dialogFile.addFilter("videos", "*.avi *.m4v *.mkv *.mp4 *.ogv");
    dialogFile.addFilter("images", "*.bmp *.jpeg *.jpg *.png *.tiff");
    dialogFile.addFilter("realsense files (*.bag)", "*.bag *.rosbag");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    size_t count = 0;
    for(auto& camera : cameras)
    {
        ::boost::filesystem::path videoPath;

        if(count == 1 && cameras.size() == 2)
        {
            // Try to guess the second stream path for RGBD cameras
            auto file = cameras[0]->getVideoFile();

            if(::boost::filesystem::is_directory(videoDirPreferencePath))
            {
                file = videoDirPreferencePath / file;
                file = file.lexically_normal();
            }
            const auto dir = file.parent_path();

            if(!dir.empty())
            {
                const auto parentDir = dir.parent_path();
                const auto curDir    = *(dir.rbegin());

                const auto findValidImagePath = [&](std::set<std::string> _folders)
                                                {
                                                    for(const auto& leafDir : _folders)
                                                    {
                                                        const auto dir = parentDir / leafDir;

                                                        if(::boost::filesystem::exists(dir))
                                                        {
                                                            ::boost::filesystem::directory_iterator currentEntry(dir);
                                                            ::boost::filesystem::directory_iterator endEntry;
                                                            while(currentEntry != endEntry)
                                                            {
                                                                ::boost::filesystem::path entryPath = *currentEntry;
                                                                if (entryPath.has_stem())
                                                                {
                                                                    return entryPath;
                                                                }
                                                                else
                                                                {
                                                                    ++currentEntry;
                                                                }
                                                            }
                                                        }
                                                    }

                                                    return ::boost::filesystem::path();
                                                };

                static const std::set<std::string> s_DEPTH_FOLDERS = {{ "d", "D", "depth", "Depth", "DEPTH"}};
                static const std::set<std::string> s_COLOR_FOLDERS = {{ "c", "C", "color", "Color", "COLOR", "RGB"}};

                if(s_DEPTH_FOLDERS.find(curDir.string()) != s_DEPTH_FOLDERS.end())
                {
                    videoPath = findValidImagePath(s_COLOR_FOLDERS);
                }
                else if(s_COLOR_FOLDERS.find(curDir.string()) != s_COLOR_FOLDERS.end())
                {
                    videoPath = findValidImagePath(s_DEPTH_FOLDERS);
                }
            }
        }

        if(videoPath.empty())
        {
            dialogFile.setTitle("Choose a file to load for video source #" + std::to_string(count++));

            ::fwData::location::SingleFile::sptr result;
            result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
            if (result)
            {
                _sDefaultPath = result->getPath().parent_path();
                dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
                videoPath = result->getPath();
            }
        }

        if(!videoPath.empty())
        {
            if(::boost::filesystem::is_directory(videoDirPreferencePath))
            {
                const ::boost::filesystem::path videoRelativePath
                    = ::fwTools::getPathDifference(videoDirPreferencePath, videoPath);

                const ::boost::filesystem::path concatenatedPath = videoDirPreferencePath / videoRelativePath;
                if(::boost::filesystem::exists(concatenatedPath))
                {
                    videoPath = videoRelativePath;
                }
                else
                {
                    SLM_WARN("Relative path '"+videoRelativePath.string()+"' genrerated with preference is not valid.");
                }
            }
            else
            {
                SLM_WARN("Video directory '"+videoDirPreferencePath.string()+"' stored in preference is not valid.");
            }

            ::fwData::mt::ObjectWriteLock lock(camera);
            camera->setCameraSource(::arData::Camera::FILE);
            camera->setVideoFile(videoPath.string());
            lock.unlock();

            const ::arData::Camera::ModifiedSignalType::sptr sig
                = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
            sig->asyncEmit();

            this->signal< ConfiguredSignalType >(s_CONFIGURED_FILE_SIG)->asyncEmit();
        }
    }
    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

void SCamera::onChooseStream()
{
    std::vector< ::arData::Camera::sptr > cameras = this->getCameras();

    size_t count = 0;
    for(auto& camera : cameras)
    {
        ::fwGui::dialog::InputDialog inputDialog;
        inputDialog.setTitle("Enter stream url for video source #" + std::to_string(count++));

        const std::string streamSource = inputDialog.getInput();
        if(!streamSource.empty())
        {
            ::fwData::mt::ObjectWriteLock lock(camera);
            camera->setCameraSource(::arData::Camera::STREAM);
            camera->setStreamUrl(streamSource);
            lock.unlock();

            const ::arData::Camera::ModifiedSignalType::sptr sig
                = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
            sig->asyncEmit();

            this->signal< ConfiguredSignalType >(s_CONFIGURED_STREAM_SIG)->asyncEmit();
        }
    }
    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

void SCamera::onChooseDevice()
{
    std::vector< ::arData::Camera::sptr > cameras = this->getCameras();

    size_t count = 0;
    for(auto& camera : cameras)
    {
        ::videoQt::editor::CameraDeviceDlg camDialog;
        camDialog.setWindowTitle(QString("Camera device selector for video source #%1").arg(count++));

        if(camDialog.exec() != QDialog::Accepted)
        {
            return;
        }

        ::fwData::mt::ObjectWriteLock lock(camera);
        const bool isSelected = camDialog.getSelectedCamera(camera);
        lock.unlock();

        if(isSelected)
        {
            const ::arData::Camera::ModifiedSignalType::sptr sig
                = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
            sig->asyncEmit();

            this->signal< ConfiguredSignalType >(s_CONFIGURED_DEVICE_SIG)->asyncEmit();
        }
    }
    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

std::vector< ::arData::Camera::sptr > SCamera::getCameras() const
{
    std::vector< ::arData::Camera::sptr > cameras;

    auto cameraSeries = this->getInOut< ::arData::CameraSeries >(s_CAMERA_SERIES_INOUT);
    if(cameraSeries)
    {
        const ::fwData::mt::ObjectReadLock lock(cameraSeries);
        const size_t numCameras = cameraSeries->getNumberOfCameras();
        for(size_t i = 0; i < numCameras; ++i)
        {
            cameras.push_back(cameraSeries->getCamera(i));
        }
    }
    else
    {
        const auto camera = this->getInOut< ::arData::Camera >(s_CAMERA_INOUT);
        SLM_ASSERT("'" + s_CAMERA_INOUT + "' does not exist.", camera);
        cameras.push_back(camera);
    }

    return cameras;
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace videoQt
