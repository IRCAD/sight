/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/SCamera.hpp"

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include <arData/CameraSeries.hpp>

#include <arPreferences/preferences.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
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

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::videoQt::editor::SCamera );

const ::fwCom::Signals::SignalKeyType SCamera::s_CONFIGURED_CAMERAS_SIG = "configuredCameras";

const ::fwCom::Signals::SignalKeyType SCamera::s_CONFIGURED_DEVICE_SIG = "configuredDevice";
const ::fwCom::Signals::SignalKeyType SCamera::s_CONFIGURED_FILE_SIG   = "configuredFile";
const ::fwCom::Signals::SignalKeyType SCamera::s_CONFIGURED_STREAM_SIG = "configuredStream";

const ::fwCom::Slots::SlotKeyType SCamera::s_CONFIGURE_DEVICE_SLOT = "configureDevice";
const ::fwCom::Slots::SlotKeyType SCamera::s_CONFIGURE_FILE_SLOT   = "configureFile";
const ::fwCom::Slots::SlotKeyType SCamera::s_CONFIGURE_STREAM_SLOT = "configureStream";

//------------------------------------------------------------------------------

SCamera::SCamera() noexcept :
    m_bVideoSupport(false),
    m_numCreateCameras(0),
    m_sigConfiguredCameras(newSignal<ConfiguredCamerasSignalType>(s_CONFIGURED_CAMERAS_SIG))
{
    newSignal< SourceConfiguredSignal >(s_CONFIGURED_DEVICE_SIG);
    newSignal< SourceConfiguredSignal >(s_CONFIGURED_FILE_SIG);
    newSignal< SourceConfiguredSignal >(s_CONFIGURED_STREAM_SIG);

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
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_bVideoSupport    = (config.get<std::string>("videoSupport", "no") == "yes");
    m_numCreateCameras = config.get<size_t>("createCameraNumber", 0);

    this->initialize();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    QPointer<QLabel> sourceLabel = new QLabel(QObject::tr("Video source: "));
    m_devicesComboBox = new QComboBox();
    layout->addWidget(sourceLabel);
    layout->addWidget(m_devicesComboBox);

    m_devicesComboBox->addItem("Device...", "device");

    // Add video file
    if(m_bVideoSupport)
    {
        m_devicesComboBox->addItem("File...", "file");
        m_devicesComboBox->addItem("Stream...", "stream");
    }

    qtContainer->setLayout(layout);

    QObject::connect(m_devicesComboBox, SIGNAL(activated(int)), this, SLOT(onApply(int)));

    // Create camera data if necessary
    auto cameraSeries = this->getInOut< ::arData::CameraSeries >("cameraSeries");
    if(cameraSeries)
    {
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
                auto sig = cameraSeries->signal< ::arData::CameraSeries::AddedCameraSignalType >(
                    ::arData::CameraSeries::s_ADDED_CAMERA_SIG);
                sig->asyncEmit(camera);
            }
            OSLM_INFO("No camera data in the CameraSeries, " << m_numCreateCameras << " will be created.");
        }
        else
        {
            SLM_ASSERT("CameraSeries contains camera data but the service is configured to create " <<
                       m_numCreateCameras <<" cameras.", m_numCreateCameras == 0);
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

void SCamera::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SCamera::onApply(int index)
{
    switch(index)
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
    dialogFile.addFilter("videos", "*.avi *.m4v *.mkv *.mp4 *.ogv");
    dialogFile.addFilter("images", "*.bmp *.jpeg *.jpg *.png *.tiff");
    dialogFile.addFilter("All files", "*.*");
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

                auto findValidImagePath = [&](std::set<std::string> _folders)
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
                ::boost::filesystem::path videoRelativePath;
                videoRelativePath = ::fwTools::getPathDifference(videoDirPreferencePath, videoPath);

                ::boost::filesystem::path concatenatedPath = videoDirPreferencePath / videoRelativePath;
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
            camera->setCameraSource(::arData::Camera::FILE);
            camera->setVideoFile(videoPath.string());

            ::arData::Camera::ModifiedSignalType::sptr sig;
            sig = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
            sig->asyncEmit();

            this->signal< SourceConfiguredSignal >(s_CONFIGURED_FILE_SIG)->asyncEmit();
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
        std::string streamSource;

        inputDialog.setTitle("Enter stream url for video source #" + std::to_string(count++));

        streamSource = inputDialog.getInput();
        if(!streamSource.empty())
        {
            camera->setCameraSource(::arData::Camera::STREAM);
            camera->setStreamUrl(streamSource);

            ::arData::Camera::ModifiedSignalType::sptr sig;
            sig = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
            sig->asyncEmit();

            this->signal< SourceConfiguredSignal >(s_CONFIGURED_STREAM_SIG)->asyncEmit();
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
        camDialog.exec();

        bool isSelected = camDialog.getSelectedCamera(camera);
        if(isSelected)
        {
            ::arData::Camera::ModifiedSignalType::sptr sig;
            sig = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
            sig->asyncEmit();

            this->signal< SourceConfiguredSignal >(s_CONFIGURED_DEVICE_SIG)->asyncEmit();
        }
    }
    m_sigConfiguredCameras->asyncEmit();
}

//------------------------------------------------------------------------------

std::vector< ::arData::Camera::sptr > SCamera::getCameras() const
{
    std::vector< ::arData::Camera::sptr > cameras;

    auto cameraSeries = this->getInOut< ::arData::CameraSeries >("cameraSeries");
    if(cameraSeries)
    {
        const size_t numCameras = cameraSeries->getNumberOfCameras();
        for(size_t i = 0; i < numCameras; ++i)
        {
            cameras.push_back(cameraSeries->getCamera(i));
        }
    }
    else
    {
        cameras.push_back(this->getInOut< ::arData::Camera >("camera"));
    }

    return cameras;
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace videoQt
