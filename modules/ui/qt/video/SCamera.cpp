/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SCamera.hpp"

#include "modules/ui/qt/video/CameraDeviceDlg.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/runtime/operations.hpp>

#include <data/Object.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/preferences/preferences.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QByteArray>
#include <QCamera>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <filesystem>

namespace sight::module::ui::qt
{

namespace video
{

static const core::com::Signals::SignalKeyType s_CONFIGURED_CAMERAS_SIG = "configuredCameras";
static const core::com::Signals::SignalKeyType s_CONFIGURED_DEVICE_SIG  = "configuredDevice";
static const core::com::Signals::SignalKeyType s_CONFIGURED_FILE_SIG    = "configuredFile";
static const core::com::Signals::SignalKeyType s_CONFIGURED_STREAM_SIG  = "configuredStream";

static const core::com::Slots::SlotKeyType s_CONFIGURE_DEVICE_SLOT = "configureDevice";
static const core::com::Slots::SlotKeyType s_CONFIGURE_FILE_SLOT   = "configureFile";
static const core::com::Slots::SlotKeyType s_CONFIGURE_STREAM_SLOT = "configureStream";

static const std::string s_VIDEO_SUPPORT_CONFIG        = "videoSupport";
static const std::string s_USE_ABSOLUTE_PATH           = "useAbsolutePath";
static const std::string s_CREATE_CAMERA_NUMBER_CONFIG = "createCameraNumber";
static const std::string s_LABEL_CONFIG                = "label";

//------------------------------------------------------------------------------

SCamera::SCamera() :
    m_sigConfiguredCameras(newSignal<ConfiguredSignalType>(s_CONFIGURED_CAMERAS_SIG))
{
    newSignal<ConfiguredSignalType>(s_CONFIGURED_DEVICE_SIG);
    newSignal<ConfiguredSignalType>(s_CONFIGURED_FILE_SIG);
    newSignal<ConfiguredSignalType>(s_CONFIGURED_STREAM_SIG);

    newSlot(s_CONFIGURE_DEVICE_SLOT, &SCamera::onChooseDevice, this);
    newSlot(s_CONFIGURE_FILE_SLOT, &SCamera::onChooseFile, this);
    newSlot(s_CONFIGURE_STREAM_SLOT, &SCamera::onChooseStream, this);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() noexcept
{
}

//------------------------------------------------------------------------------

void SCamera::configuring()
{
    const service::IService::ConfigType config = this->getConfigTree();

    m_bVideoSupport    = config.get<bool>(s_VIDEO_SUPPORT_CONFIG, false);
    m_useAbsolutePath  = config.get<bool>(s_USE_ABSOLUTE_PATH, false);
    m_numCreateCameras = config.get<size_t>(s_CREATE_CAMERA_NUMBER_CONFIG, m_numCreateCameras);
    m_label            = config.get<std::string>(s_LABEL_CONFIG, m_label);

    this->initialize();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->create();

    const auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );

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
    auto cameraSeries = m_cameraSeries.lock();
    if(cameraSeries)
    {
        const size_t numCameras = cameraSeries->getNumberOfCameras();
        if(numCameras == 0)
        {
            SIGHT_ASSERT("No camera data in the CameraSeries.", m_numCreateCameras != 0);

            for(size_t i = 0 ; i < m_numCreateCameras ; ++i)
            {
                data::Camera::sptr camera = data::Camera::New();
                const size_t index        = cameraSeries->getNumberOfCameras();
                cameraSeries->addCamera(camera);
                cameraSeries->setExtrinsicMatrix(index, data::Matrix4::New());
                const auto sig = cameraSeries->signal<data::CameraSeries::AddedCameraSignalType>(
                    data::CameraSeries::s_ADDED_CAMERA_SIG
                );
                sig->asyncEmit(camera);
            }

            SIGHT_INFO("No camera data in the CameraSeries, " << m_numCreateCameras << " will be created.");
        }
        else
        {
            SIGHT_WARN_IF(
                "CameraSeries contains camera data but the service is configured to create "
                << m_numCreateCameras << " cameras.",
                m_numCreateCameras != 0
            );
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
    std::vector<data::Camera::sptr> cameras = this->getCameras();

    // Check preferences
    const std::filesystem::path videoDirPreferencePath(sight::ui::base::preferences::getVideoDir());

    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("All files", "*.*");
    dialogFile.addFilter("videos", "*.avi *.m4v *.mkv *.mp4 *.ogv");
    dialogFile.addFilter("images", "*.bmp *.jpeg *.jpg *.png *.tiff");
    dialogFile.addFilter("realsense files (*.bag)", "*.bag *.rosbag");
    dialogFile.setOption(sight::ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(sight::ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    size_t count = 0;
    for(auto& camera : cameras)
    {
        std::filesystem::path videoPath;

        if(count == 1 && cameras.size() == 2)
        {
            // Try to guess the second stream path for RGBD cameras
            auto file = cameras[0]->getVideoFile();

            if(std::filesystem::is_directory(videoDirPreferencePath))
            {
                file = videoDirPreferencePath / file;
                file = file.lexically_normal();
            }

            const auto dir = file.parent_path();

            if(!dir.empty())
            {
                const auto parentDir = dir.parent_path();
                const auto curDir    = *(--dir.end());

                const auto findValidImagePath = [&](std::set<std::string> _folders)
                                                {
                                                    for(const auto& leafDir : _folders)
                                                    {
                                                        const auto dir = parentDir / leafDir;

                                                        if(std::filesystem::exists(dir))
                                                        {
                                                            std::filesystem::directory_iterator currentEntry(dir);
                                                            std::filesystem::directory_iterator endEntry;
                                                            while(currentEntry != endEntry)
                                                            {
                                                                std::filesystem::path entryPath = *currentEntry;
                                                                if(entryPath.has_stem())
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

                                                    return std::filesystem::path();
                                                };

                static const std::set<std::string> s_DEPTH_FOLDERS = {{"d", "D", "depth", "Depth", "DEPTH"}};
                static const std::set<std::string> s_COLOR_FOLDERS = {{"c", "C", "color", "Color", "COLOR", "RGB"}};

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

            auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
            if(result)
            {
                defaultDirectory->setFolder(result->getFile().parent_path());
                dialogFile.saveDefaultLocation(defaultDirectory);
                videoPath = result->getFile();
            }
        }

        if(!videoPath.empty())
        {
            if(std::filesystem::is_directory(videoDirPreferencePath))
            {
                if(!m_useAbsolutePath)
                {
                    const auto videoRelativePath = std::filesystem::relative(
                        videoPath,
                        videoDirPreferencePath
                    );
                    const std::filesystem::path concatenatedPath = videoDirPreferencePath / videoRelativePath;
                    if(std::filesystem::exists(concatenatedPath))
                    {
                        videoPath = videoRelativePath;
                    }
                    else
                    {
                        SIGHT_WARN(
                            "Relative path '" + videoRelativePath.string()
                            + "' generated from preferences is not valid."
                        );
                    }
                }
            }
            else
            {
                SIGHT_WARN(
                    "Video directory '" + videoDirPreferencePath.string()
                    + "' stored in preference is not valid."
                );
            }

            {
                data::mt::locked_ptr<data::Camera> lock(camera);
                camera->setCameraSource(data::Camera::FILE);
                camera->setVideoFile(videoPath.string());
            }
            const data::Camera::ModifiedSignalType::sptr sig =
                camera->signal<data::Camera::ModifiedSignalType>(data::Camera::s_MODIFIED_SIG);
            sig->asyncEmit();

            this->signal<ConfiguredSignalType>(s_CONFIGURED_FILE_SIG)->asyncEmit();
        }
    }

    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

void SCamera::onChooseStream()
{
    std::vector<data::Camera::sptr> cameras = this->getCameras();

    size_t count = 0;
    for(auto& camera : cameras)
    {
        sight::ui::base::dialog::InputDialog inputDialog;
        inputDialog.setTitle("Enter stream url for video source #" + std::to_string(count++));

        const std::string streamSource = inputDialog.getInput();
        if(!streamSource.empty())
        {
            {
                data::mt::locked_ptr<data::Camera> lock(camera);
                camera->setCameraSource(data::Camera::STREAM);
                camera->setStreamUrl(streamSource);
            }
            const data::Camera::ModifiedSignalType::sptr sig =
                camera->signal<data::Camera::ModifiedSignalType>(data::Camera::s_MODIFIED_SIG);
            sig->asyncEmit();

            this->signal<ConfiguredSignalType>(s_CONFIGURED_STREAM_SIG)->asyncEmit();
        }
    }

    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

void SCamera::onChooseDevice()
{
    std::vector<data::Camera::sptr> cameras = this->getCameras();

    size_t count = 0;
    for(auto& camera : cameras)
    {
        module::ui::qt::video::CameraDeviceDlg camDialog;
        camDialog.setWindowTitle(QString("Camera device selector for video source #%1").arg(count++));

        if(camDialog.exec() != QDialog::Accepted)
        {
            return;
        }

        bool isSelected = false;
        {
            data::mt::locked_ptr<data::Camera> lock(camera);
            isSelected = camDialog.getSelectedCamera(camera);
        }

        if(isSelected)
        {
            const data::Camera::ModifiedSignalType::sptr sig =
                camera->signal<data::Camera::ModifiedSignalType>(data::Camera::s_MODIFIED_SIG);
            sig->asyncEmit();

            this->signal<ConfiguredSignalType>(s_CONFIGURED_DEVICE_SIG)->asyncEmit();
        }
    }

    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

std::vector<data::Camera::sptr> SCamera::getCameras() const
{
    std::vector<data::Camera::sptr> cameras;

    const auto cameraSeries = m_cameraSeries.lock();
    if(cameraSeries)
    {
        const size_t numCameras = cameraSeries->getNumberOfCameras();
        for(size_t i = 0 ; i < numCameras ; ++i)
        {
            cameras.push_back(cameraSeries->getCamera(i));
        }
    }
    else
    {
        const auto camera = m_camera.lock();
        SIGHT_ASSERT("'" << s_CAMERA << "' does not exist.", camera);
        cameras.push_back(camera.get_shared());
    }

    return cameras;
}

//------------------------------------------------------------------------------

} //namespace video

} //namespace sight::module::ui::qt
