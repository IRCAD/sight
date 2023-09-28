/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/runtime/path.hpp>

#include <data/Object.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/Preferences.hpp>
#include <ui/qt/container/widget.hpp>

#include <QByteArray>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include <filesystem>

namespace sight::module::ui::qt::video
{

static const core::com::signals::key_t CONFIGURED_CAMERAS_SIG = "configuredCameras";
static const core::com::signals::key_t CONFIGURED_DEVICE_SIG  = "configuredDevice";
static const core::com::signals::key_t CONFIGURED_FILE_SIG    = "configuredFile";
static const core::com::signals::key_t CONFIGURED_STREAM_SIG  = "configuredStream";

static const core::com::slots::key_t CONFIGURE_DEVICE_SLOT = "configureDevice";
static const core::com::slots::key_t CONFIGURE_FILE_SLOT   = "configureFile";
static const core::com::slots::key_t CONFIGURE_STREAM_SLOT = "configureStream";

static const std::string s_VIDEO_SUPPORT_CONFIG        = "videoSupport";
static const std::string s_USE_ABSOLUTE_PATH           = "useAbsolutePath";
static const std::string s_CREATE_CAMERA_NUMBER_CONFIG = "createCameraNumber";
static const std::string s_LABEL_CONFIG                = "label";
static const std::string s_RESOLUTION_CONFIG           = "resolution";

const std::string SCamera::s_RESOLUTION_PREF_KEY = "camera_resolution";

//------------------------------------------------------------------------------

SCamera::SCamera() :
    m_sigConfiguredCameras(new_signal<ConfiguredSignalType>(CONFIGURED_CAMERAS_SIG))
{
    new_signal<ConfiguredSignalType>(CONFIGURED_DEVICE_SIG);
    new_signal<ConfiguredSignalType>(CONFIGURED_FILE_SIG);
    new_signal<ConfiguredSignalType>(CONFIGURED_STREAM_SIG);

    new_slot(CONFIGURE_DEVICE_SLOT, &SCamera::onChooseDevice, this);
    new_slot(CONFIGURE_FILE_SLOT, &SCamera::onChooseFile, this);
    new_slot(CONFIGURE_STREAM_SLOT, &SCamera::onChooseStream, this);
}

//------------------------------------------------------------------------------

void SCamera::configuring()
{
    const service::config_t config = this->getConfiguration();

    m_bVideoSupport    = config.get<bool>(s_VIDEO_SUPPORT_CONFIG, false);
    m_useAbsolutePath  = config.get<bool>(s_USE_ABSOLUTE_PATH, false);
    m_numCreateCameras = config.get<std::size_t>(s_CREATE_CAMERA_NUMBER_CONFIG, m_numCreateCameras);
    m_label            = config.get<std::string>(s_LABEL_CONFIG, m_label);

    m_resolution = config.get<std::string>(s_RESOLUTION_CONFIG, "preferences");

    this->initialize();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->create();

    const auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );

    QPointer<QHBoxLayout> layout = new QHBoxLayout();

    if(!m_label.empty())
    {
        const QPointer<QLabel> sourceLabel = new QLabel(QString::fromStdString(m_label));
        layout->addWidget(sourceLabel);
    }

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    m_devicesComboBox = new QComboBox();
    m_devicesComboBox->setObjectName(serviceID);
    layout->addWidget(m_devicesComboBox);

    m_devicesComboBox->addItem("Device...", "device");

    // Add video file
    if(m_bVideoSupport)
    {
        m_devicesComboBox->addItem("File...", "file");
        m_devicesComboBox->addItem("Stream...", "stream");
    }

    // Add button to edit the preferences when and set `m_preferenceMode` to true
    if(m_resolution == "preferences")
    {
        auto path = core::runtime::get_module_resource_path("sight::module::ui::flaticons");
        // Add preference setting button
        QPointer<QPushButton> setPrefButton = new QPushButton();
        setPrefButton->setProperty("class", "signal-button");
        setPrefButton->setIcon(QIcon(QString::fromStdString((path / "BlueParametersCamera.svg").string())));
        setPrefButton->setToolTip("Set camera resolution preference");
        layout->addWidget(setPrefButton);
        m_preferenceMode = true;

        QObject::connect(setPrefButton, SIGNAL(clicked()), this, SLOT(setPreference()));
    }

    qtContainer->setLayout(layout);

    ::QObject::connect(m_devicesComboBox, qOverload<int>(&QComboBox::activated), this, &SCamera::onActivated);
    ::QObject::connect(m_devicesComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &SCamera::onApply);

    // Create camera data if necessary
    auto camera_set = m_camera_set.lock();
    if(camera_set)
    {
        const std::size_t numCameras = camera_set->size();
        if(numCameras == 0)
        {
            SIGHT_ASSERT("No camera data in the CameraSet.", m_numCreateCameras != 0);

            for(std::size_t i = 0 ; i < m_numCreateCameras ; ++i)
            {
                auto camera = std::make_shared<data::Camera>();
                camera_set->add_camera(camera);

                const auto sig = camera_set->signal<data::CameraSet::added_camera_signal_t>(
                    data::CameraSet::ADDED_CAMERA_SIG
                );

                sig->async_emit(camera);
            }

            SIGHT_INFO("No camera data in the CameraSet, " << m_numCreateCameras << " will be created.");
        }
        else
        {
            SIGHT_WARN_IF(
                "CameraSet contains camera data but the service is configured to create "
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

        default:
            SIGHT_ASSERT("Invalid index: " << _index, false);
    }
}

//------------------------------------------------------------------------------
void SCamera::onActivated(int _index)
{
    // If the current index did change, onCurrentIndexChanged will be called, we wouldn't want onApply to be called
    // twice
    if(oldIndex == _index)
    {
        onApply(_index);
    }

    oldIndex = _index;
}

//------------------------------------------------------------------------------

void SCamera::onChooseFile()
{
    std::vector<data::Camera::sptr> cameras = this->getCameras();

    // Check preferences
    const std::filesystem::path videoDirPreferencePath(sight::ui::Preferences().get(
                                                           "VIDEO_DIR_PREF",
                                                           std::string()
    ));

    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("All files", "*.*");
    dialogFile.addFilter("videos", "*.avi *.m4v *.mkv *.mp4 *.ogv");
    dialogFile.addFilter("images", "*.bmp *.jpeg *.jpg *.png *.tiff");
    dialogFile.addFilter("realsense files (*.bag)", "*.bag *.rosbag");
    dialogFile.setOption(sight::ui::dialog::location::READ);
    dialogFile.setOption(sight::ui::dialog::location::FILE_MUST_EXIST);

    std::size_t count = 0;
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

                                                                ++currentEntry;
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

            auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
            if(result)
            {
                defaultDirectory->set_folder(result->get_file().parent_path());
                dialogFile.saveDefaultLocation(defaultDirectory);
                videoPath = result->get_file();
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
                camera->signal<data::Camera::ModifiedSignalType>(data::Camera::MODIFIED_SIG);
            sig->async_emit();

            this->signal<ConfiguredSignalType>(CONFIGURED_FILE_SIG)->async_emit();
        }
    }

    m_sigConfiguredCameras->async_emit();
}

//------------------------------------------------------------------------------

void SCamera::onChooseStream()
{
    std::vector<data::Camera::sptr> cameras = this->getCameras();

    std::size_t count = 0;
    for(auto& camera : cameras)
    {
        sight::ui::dialog::input input;
        input.setTitle("Enter stream url for video source #" + std::to_string(count++));

        const auto& [streamSource, ok] = input.getInput();
        if(ok && !streamSource.empty())
        {
            {
                data::mt::locked_ptr<data::Camera> lock(camera);
                camera->setCameraSource(data::Camera::STREAM);
                camera->setStreamUrl(streamSource);
            }
            const data::Camera::ModifiedSignalType::sptr sig =
                camera->signal<data::Camera::ModifiedSignalType>(data::Camera::MODIFIED_SIG);
            sig->async_emit();

            this->signal<ConfiguredSignalType>(CONFIGURED_STREAM_SIG)->async_emit();
        }
    }

    m_sigConfiguredCameras->async_emit();
}

//------------------------------------------------------------------------------

void SCamera::onChooseDevice()
{
    if(m_preferenceMode)
    {
        try
        {
            sight::ui::Preferences resolutionPreference;
            m_cameraResolutionPreference = resolutionPreference.get<std::string>(s_RESOLUTION_PREF_KEY);
        }
        catch(boost::property_tree::ptree_error&)
        {
            SIGHT_ERROR("Couldn't get preference. The required key doesn't exist.");
        }
        if(!m_cameraResolutionPreference.empty())
        {
            m_resolution = m_cameraResolutionPreference;
        }
    }

    std::vector<data::Camera::sptr> cameras = this->getCameras();
    const QList<QCameraInfo> devices        = QCameraInfo::availableCameras();
    if(devices.isEmpty())
    {
        auto* errorMessageBox = new QMessageBox(
            QMessageBox::Critical,
            "Error",
            "No device available. Please connect a camera device and relaunch the application."
        );
        errorMessageBox->exec();
    }
    else
    {
        bool configured   = true;
        std::size_t count = 0;
        for(auto& camera : cameras)
        {
            module::ui::qt::video::CameraDeviceDlg camDialog(m_resolution);
            camDialog.setWindowTitle(QString("Camera device selector for video source #%1").arg(count++));

            if(((devices.size() > 1 && m_resolution != "preferences") || m_resolution == "prompt")
               && camDialog.exec() != QDialog::Accepted)
            {
                return;
            }

            bool isSelected = false;
            {
                data::mt::locked_ptr<data::Camera> lock(camera);

                isSelected  = camDialog.getSelectedCamera(camera, m_resolution);
                configured &= isSelected;
            }

            if(isSelected)
            {
                const data::Camera::ModifiedSignalType::sptr sig =
                    camera->signal<data::Camera::ModifiedSignalType>(data::Camera::MODIFIED_SIG);
                sig->async_emit();

                this->signal<ConfiguredSignalType>(CONFIGURED_DEVICE_SIG)->async_emit();
            }
            else if(m_preferenceMode)
            {
                QMetaObject::invokeMethod(this, "setPreference", Qt::QueuedConnection);
            }
        }

        if(configured)
        {
            m_sigConfiguredCameras->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

std::vector<data::Camera::sptr> SCamera::getCameras() const
{
    std::vector<data::Camera::sptr> cameras;

    auto camera_set = m_camera_set.lock();
    if(camera_set)
    {
        for(std::size_t i = 0, numCameras = camera_set->size() ; i < numCameras ; ++i)
        {
            cameras.push_back(camera_set->get_camera(i));
        }
    }
    else
    {
        const auto camera = m_camera.lock();
        if(camera)
        {
            cameras.push_back(camera.get_shared());
        }
    }

    return cameras;
}

//------------------------------------------------------------------------------

void SCamera::setPreference()
{
    // set m_resolution to "prompt" mode in order to display the camera selection dialog anytime the button is clicked
    m_resolution     = "prompt";
    m_preferenceMode = false;
    this->onChooseDevice();
    m_preferenceMode = true;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::video
