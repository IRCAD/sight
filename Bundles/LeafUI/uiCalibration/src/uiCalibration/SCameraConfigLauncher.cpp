/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SCameraConfigLauncher.hpp"

#include <arData/CalibrationInfo.hpp>
#include <arData/Camera.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>

namespace uiCalibration
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SCameraConfigLauncher, ::fwData::Composite);

SCameraConfigLauncher::SCameraConfigLauncher() throw()
{
    m_intrinsicLauncher = ::fwServices::helper::ConfigLauncher::New();
    m_extrinsicLauncher = ::fwServices::helper::ConfigLauncher::New();
}

//------------------------------------------------------------------------------

SCameraConfigLauncher::~SCameraConfigLauncher() throw()
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::configuring() throw(fwTools::Failed)
{
    this->initialize();
    ::fwServices::IService::ConfigType configuration = this->getConfigTree();

    SLM_ASSERT("There must be one (and only one) <config/> element.",
               configuration.get_child("service").count("config") == 1 );
    const ::fwServices::IService::ConfigType& srvconfig = configuration.get_child("service");
    const ::fwServices::IService::ConfigType& config    = srvconfig.get_child("config");

    const ::fwServices::IService::ConfigType& intrinsic = config.get_child("intrinsic");

    const ::fwServices::IService::ConfigType& extrinsic = config.get_child("extrinsic");

    // ConfigLauncher needs ptree configuration with service.config.appConfig
    ::fwServices::IService::ConfigType cameraConfig;
    ::fwServices::IService::ConfigType extrinsicConfig;
    cameraConfig.add_child("service.config", intrinsic);
    extrinsicConfig.add_child("service.config", extrinsic);

    m_intrinsicLauncher->parseConfig(cameraConfig);
    m_extrinsicLauncher->parseConfig(extrinsicConfig);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::starting() throw(::fwTools::Failed)
{
    this->create();

    m_cameraSeries = this->getInOut< ::arData::CameraSeries >("cameraSeries");
    SLM_ASSERT("Missing CameraSeries: " + m_cameraSeriesKey, m_cameraSeries);

    m_activitySeries = this->getInOut< ::fwMedData::ActivitySeries >("activitySeries");
    SLM_ASSERT("Missing ActivitySeries: " + m_activitySeriesKey, m_activitySeries);

    auto qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QHBoxLayout* layout = new QHBoxLayout();

    m_cameraComboBox = new QComboBox();
    layout->addWidget(m_cameraComboBox);

    QIcon addIcon(QString(BUNDLE_PREFIX) + QString("/media_0-1/icons/Import.svg"));
    m_addButton = new QPushButton(addIcon, "");
    m_addButton->setToolTip("Add a new camera.");
    layout->addWidget(m_addButton);

    QIcon removeIcon(QString(BUNDLE_PREFIX) + QString("/arMedia_0-1/icons/remove.svg"));
    m_removeButton = new QPushButton(removeIcon, "");
    m_removeButton->setToolTip("Remove the camera.");
    layout->addWidget(m_removeButton);

    m_extrinsicButton = new QPushButton("Extrinsic");
    layout->addWidget(m_extrinsicButton);
    m_extrinsicButton->setCheckable(true);

    qtContainer->setLayout( layout );

    size_t nbCam = m_cameraSeries->getNumberOfCameras();

    if (nbCam == 0)
    {
        this->addCamera();

        m_extrinsicButton->setEnabled(false);
        m_removeButton->setEnabled(false);
    }
    else
    {
        for (size_t i = 0; i < nbCam; ++i)
        {
            m_cameraComboBox->addItem(QString("Camera %1").arg(i+1));
        }

        const bool moreThanOneCamera = (nbCam > 1);

        m_extrinsicButton->setEnabled(moreThanOneCamera);
        m_removeButton->setEnabled(moreThanOneCamera);
    }

    QObject::connect(m_cameraComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCameraChanged(int)));
    QObject::connect(m_addButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    QObject::connect(m_removeButton, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));
    QObject::connect(m_extrinsicButton, SIGNAL(toggled(bool)), this, SLOT(onExtrinsicToggled(bool)));

    this->startIntrinsicConfig(0);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::stopping() throw(::fwTools::Failed)
{
    m_intrinsicLauncher->stopConfig();
    m_extrinsicLauncher->stopConfig();

    this->destroy();
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::swapping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onCameraChanged(int index)
{
    OSLM_ASSERT("Bad index: " << index,
                index >= 0 && static_cast<size_t>(index) < m_cameraSeries->getNumberOfCameras());

    if (index == 0)
    {
        m_extrinsicButton->setChecked(false);
        m_extrinsicButton->setEnabled(false);
    }
    else
    {
        m_extrinsicButton->setEnabled(true);
    }

    if (m_extrinsicButton->isChecked())
    {
        this->startExtrinsicConfig(static_cast<size_t>(index));
    }
    else
    {
        this->startIntrinsicConfig(static_cast<size_t>(index));
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onAddClicked()
{
    m_extrinsicButton->setEnabled(true);
    m_removeButton->setEnabled(true);

    this->addCamera();
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onRemoveClicked()
{
    const size_t index = static_cast<size_t>(m_cameraComboBox->currentIndex());
    if (index > 0)
    {
        m_cameraComboBox->blockSignals(true);

        // Remove camera
        ::arData::Camera::sptr camera = m_cameraSeries->getCamera(index);
        m_cameraSeries->removeCamera(camera);
        ::arData::CameraSeries::RemovedCameraSignalType::sptr sig;
        sig = m_cameraSeries->signal< ::arData::CameraSeries::RemovedCameraSignalType >(
            ::arData::CameraSeries::s_REMOVED_CAMERA_SIG);
        sig->asyncEmit(camera);

        // Remove calibrationInfo
        std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);
        m_activitySeries->getData()->getContainer().erase(calibrationInfoKey);

        const size_t nbCam = m_cameraSeries->getNumberOfCameras();
        if (nbCam == 1)
        {
            m_extrinsicButton->setEnabled(false);
            m_removeButton->setEnabled(false);
        }
        // Renamed all items from 1 to nbCam
        m_cameraComboBox->clear();
        for (size_t i = 0; i < nbCam; ++i)
        {
            m_cameraComboBox->addItem(QString("Camera %1").arg(i+1));
        }
        // select first camera
        m_cameraComboBox->setCurrentIndex(0);
        this->startIntrinsicConfig(0);
        m_cameraComboBox->blockSignals(false);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", "The first camera can not be deleted");
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onExtrinsicToggled(bool checked)
{
    const size_t index = static_cast<size_t>(m_cameraComboBox->currentIndex());
    OSLM_ASSERT("Bad index: " << index, index >= 0 && index < m_cameraSeries->getNumberOfCameras());
    if (checked)
    {
        this->startExtrinsicConfig(index);
    }
    else
    {
        this->startIntrinsicConfig(index);
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startIntrinsicConfig(size_t index)
{
    ::fwServices::helper::ConfigLauncher::FieldAdaptorType replaceMap;

    ::arData::Camera::sptr camera = m_cameraSeries->getCamera(index);

    std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);
    ::fwData::Composite::sptr data            = m_activitySeries->getData();
    ::arData::CalibrationInfo::sptr calibInfo =
        ::arData::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfoKey]);

    replaceMap["camera"]          = camera->getID();
    replaceMap["calibrationInfo"] = calibInfo->getID();

    m_extrinsicLauncher->stopConfig();
    m_intrinsicLauncher->stopConfig();
    m_intrinsicLauncher->startConfig(this->getSptr(), replaceMap);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startExtrinsicConfig(size_t index)
{
    size_t cameraIdx = std::max(index, size_t(1));

    ::arData::Camera::sptr camera1 = m_cameraSeries->getCamera(0);
    ::arData::Camera::sptr camera2 = m_cameraSeries->getCamera(cameraIdx);

    // Check if the two cameras are calibrated
    if (camera1->getIsCalibrated() && camera2->getIsCalibrated())
    {
        // Add 2 calibration info in ActivitySeries if not exist
        std::string calibrationInfo1Key = "calibrationInfoExtr0_" + ::boost::lexical_cast<std::string>(cameraIdx);
        std::string calibrationInfo2Key = "calibrationInfoExtr1_" + ::boost::lexical_cast<std::string>(cameraIdx);

        ::fwData::Composite::sptr data = m_activitySeries->getData();
        ::arData::CalibrationInfo::sptr calibInfo1;
        ::arData::CalibrationInfo::sptr calibInfo2;
        // Get the calibrationInfo from the activity series if it exists or create it.
        if (data->find(calibrationInfo1Key) == data->end() || data->find(calibrationInfo2Key) == data->end())
        {
            calibInfo1 = ::arData::CalibrationInfo::New();
            calibInfo2 = ::arData::CalibrationInfo::New();

            data->getContainer()[calibrationInfo1Key] = calibInfo1;
            data->getContainer()[calibrationInfo2Key] = calibInfo2;
        }
        else
        {
            calibInfo1 = ::arData::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfo1Key]);
            calibInfo2 = ::arData::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfo2Key]);
        }

        ::fwServices::registry::FieldAdaptorType replaceMap;

        replaceMap["camera1"]          = camera1->getID();
        replaceMap["camera2"]          = camera2->getID();
        replaceMap["calibrationInfo1"] = calibInfo1->getID();
        replaceMap["calibrationInfo2"] = calibInfo2->getID();
        replaceMap["camIndex"]         = std::to_string(index);

        m_extrinsicLauncher->stopConfig();
        m_intrinsicLauncher->stopConfig();
        m_extrinsicLauncher->startConfig(this->getSptr(), replaceMap);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Calibration", "Cameras must be intrinsically calibrated.");
        m_extrinsicButton->setChecked(false);
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::addCamera()
{
    const size_t nbCam = m_cameraSeries->getNumberOfCameras();

    ::arData::Camera::sptr camera = ::arData::Camera::New();

    // Add the CalibrationInfo in activitySeries to be saved in activity
    std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(nbCam);
    ::arData::CalibrationInfo::sptr calibInfo                       = ::arData::CalibrationInfo::New();
    m_activitySeries->getData()->getContainer()[calibrationInfoKey] = calibInfo;

    // Add the camera
    m_cameraSeries->addCamera(camera);
    ::arData::CameraSeries::AddedCameraSignalType::sptr sig;
    sig = m_cameraSeries->signal< ::arData::CameraSeries::AddedCameraSignalType >(
        ::arData::CameraSeries::s_ADDED_CAMERA_SIG);
    sig->asyncEmit(camera);

    m_cameraComboBox->blockSignals(true);
    m_cameraComboBox->addItem(QString("Camera %1").arg(nbCam+1));
    m_cameraComboBox->setCurrentIndex(static_cast<int>(nbCam));
    m_extrinsicButton->setChecked(false);
    this->startIntrinsicConfig(nbCam);
    m_cameraComboBox->blockSignals(false);
}

//------------------------------------------------------------------------------

} // namespace uiCalibration

