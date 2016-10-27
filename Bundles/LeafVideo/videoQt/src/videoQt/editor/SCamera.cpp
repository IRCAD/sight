/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/SCamera.hpp"

#include "videoQt/editor/CameraDeviceDlg.hpp"

#include <arData/Camera.hpp>

#include <arPreferences/preferences.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/Object.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
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

fwServicesRegisterMacro( ::gui::editor::IEditor, ::videoQt::editor::SCamera, ::arData::Camera );

//------------------------------------------------------------------------------

SCamera::SCamera() throw() : m_bVideoSupport(false)
{
}

//------------------------------------------------------------------------------

SCamera::~SCamera() throw()
{
}

//------------------------------------------------------------------------------

void SCamera::starting() throw(::fwTools::Failed)
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

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

    container->setLayout(layout);

    QObject::connect(m_devicesComboBox, SIGNAL(activated(int)), this, SLOT(onApply(int)));
}

//------------------------------------------------------------------------------

void SCamera::stopping() throw(::fwTools::Failed)
{
    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SCamera::configuring() throw(fwTools::Failed)
{
    ConfigurationType cfgVideo = m_configuration->findConfigurationElement("videoSupport");

    if(cfgVideo)
    {
        std::string str = cfgVideo->getValue();
        m_bVideoSupport = (str == "yes");
    }

    this->initialize();
}

//------------------------------------------------------------------------------

void SCamera::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCamera::swapping() throw(::fwTools::Failed)
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
    ::arData::Camera::sptr camera;
    if (this->isVersion2())
    {
        camera = this->getInOut< ::arData::Camera >("camera");
    }
    else
    {
        camera = this->getObject< ::arData::Camera >();
    }

    // Check preferences
    const ::boost::filesystem::path videoDirPreferencePath(::arPreferences::getVideoDir());

    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load a video");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("mp4","*.mp4");
    dialogFile.addFilter("avi","*.avi");
    dialogFile.addFilter("m4v","*.m4v");
    dialogFile.addFilter("mkv","*.mkv");
    dialogFile.addFilter("All files","*.*");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    ::boost::filesystem::path videoPath;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        videoPath = result->getPath();
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
    }
}

//------------------------------------------------------------------------------

void SCamera::onChooseStream()
{
    ::arData::Camera::sptr camera;
    if (this->isVersion2())
    {
        camera = this->getInOut< ::arData::Camera >("camera");
    }
    else
    {
        camera = this->getObject< ::arData::Camera >();
    }

    ::fwGui::dialog::InputDialog inputDialog;
    std::string streamSource;

    inputDialog.setTitle ("Enter stream source");
    streamSource = inputDialog.getInput();
    if(!streamSource.empty())
    {
        camera->setCameraSource(::arData::Camera::STREAM);
        camera->setStreamUrl(streamSource);

        ::arData::Camera::ModifiedSignalType::sptr sig;
        sig = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SCamera::onChooseDevice()
{
    ::videoQt::editor::CameraDeviceDlg camDialog;
    camDialog.exec();

    ::arData::Camera::sptr camera;
    if (this->isVersion2())
    {
        camera = this->getInOut< ::arData::Camera >("camera");
    }
    else
    {
        camera = this->getObject< ::arData::Camera >();
    }
    bool isSelected = camDialog.getSelectedCamera(camera);
    if(isSelected)
    {
        ::arData::Camera::ModifiedSignalType::sptr sig;
        sig = camera->signal< ::arData::Camera::ModifiedSignalType >( ::arData::Camera::s_MODIFIED_SIG );
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace videoQt

