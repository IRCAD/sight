/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/editor/SCamera.hpp"
#include "videoQt/helper/preferences.hpp"

#include <fwCore/base.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Object.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <arData/Camera.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGui/dialog/InputDialog.hpp>

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

SCamera::SCamera() throw() :
    m_bVideoSupport(false)
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
    QPointer<QLabel> deviceLabel = new QLabel(QObject::tr("Device: "));
    m_devicesComboBox = new QComboBox();
    layout->addWidget(deviceLabel);
    layout->addWidget(m_devicesComboBox);

    // Add cameras
    const QList<QByteArray> devices = QCamera::availableDevices();
    for (int i = 0; i < devices.size(); ++i)
    {
        m_devicesComboBox->addItem(QCamera::deviceDescription(devices.at(i)), QString(devices.at(i)));
    }

    // Add video file
    if(m_bVideoSupport)
    {
        m_devicesComboBox->addItem("Open file...", "file");
        m_devicesComboBox->addItem("Open stream...", "stream");
    }


    container->setLayout(layout);

    QObject::connect(m_devicesComboBox, SIGNAL(activated(int)), this, SLOT(onApply(int)));

    if(!devices.empty())
    {
        ::arData::Camera::sptr cam = this->getObject< ::arData::Camera >();
        std::string cameraID = cam->getCameraID();
        //select default camera device
        if(cameraID.empty())
        {
            this->onApply(0);
        }
        //TODO : Manage selection with cameraID
    }
    this->updating();
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

        if(str == "yes")
        {
            m_bVideoSupport = true;
        }
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
    if(index >= 0)
    {
        QString device = m_devicesComboBox->currentData().toString();

        ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();

        if(device == "file")
        {
            // Check preferences
            std::string videoDir = helper::getVideoDir();

            static ::boost::filesystem::path _sDefaultPath;

            ::fwGui::dialog::LocationDialog dialogFile;
            dialogFile.setTitle("Choose a file to load a video");
            dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
            dialogFile.addFilter("mp4","*.mp4");
            dialogFile.addFilter("avi","*.avi");
            dialogFile.addFilter("m4v","*.m4v");
            dialogFile.addFilter("mkv","*.mkv");
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
                ::boost::filesystem::path videoDirPath(videoDir);
                videoPath = ::fwTools::getPathDifference(videoDirPath, videoPath);
            }
            else
            {
                videoPath = "";
            }

            camera->setCameraID("file");
            camera->setDescription(videoPath.string());
        }
        else if (device == "stream")
        {
            ::fwGui::dialog::InputDialog inputDialog;
            std::string streamSource;

            inputDialog.setTitle ("Enter stream source");
            streamSource = inputDialog.getInput();
            camera->setCameraID("stream");
            camera->setDescription(streamSource);
        }
        else
        {
            QString description = QCamera::deviceDescription(m_devicesComboBox->currentData().toByteArray());
            camera->setCameraID(device.toStdString());
            camera->setDescription(description.toStdString());
        }

        ::arData::Camera::IdModifiedSignalType::sptr sig;
        sig = camera->signal< ::arData::Camera::IdModifiedSignalType >( ::arData::Camera::s_ID_MODIFIED_SIG );

        fwServicesNotifyMacro( camera->getLightID(), sig, (device.toStdString()));
    }
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace videoQt

