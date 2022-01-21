/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/ui/qt/calibration/SCameraConfigLauncher.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/runtime/operations.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/Composite.hpp>
#include <data/SeriesDB.hpp>

#include <io/base/service/ioTypes.hpp>
#include <io/base/service/IReader.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QInputDialog>
#include <QStringList>

namespace sight::module::ui::qt::calibration
{

SCameraConfigLauncher::SCameraConfigLauncher() noexcept
{
}

//------------------------------------------------------------------------------

SCameraConfigLauncher::~SCameraConfigLauncher() noexcept
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::configuring()
{
    this->initialize();
    service::IService::ConfigType configuration = this->getConfigTree();

    SIGHT_ASSERT(
        "There must be one (and only one) <config/> element.",
        configuration.count("config") == 1
    );
    const service::IService::ConfigType& srvconfig = configuration;
    const service::IService::ConfigType& config    = srvconfig.get_child("config");

    const service::IService::ConfigType& intrinsic = config.get_child("intrinsic");
    const service::IService::ConfigType& extrinsic = config.get_child("extrinsic");

    m_intrinsicLauncher.parseConfig(intrinsic, this->getSptr());
    m_extrinsicLauncher.parseConfig(extrinsic, this->getSptr());
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    m_cameraComboBox = new QComboBox();
    layout->addWidget(m_cameraComboBox);

    QIcon addIcon(QString::fromStdString(
                      core::runtime::getModuleResourceFilePath(
                          "sight::module::ui::icons",
                          "Import.svg"
                      ).string()
    ));
    m_addButton = new QPushButton(addIcon, "");
    m_addButton->setToolTip("Add a new camera.");
    layout->addWidget(m_addButton);

    QIcon importIcon(QString::fromStdString(
                         core::runtime::getModuleResourceFilePath(
                             "sight::module::ui::icons",
                             "CameraSeries.svg"
                         ).string()
    ));
    m_importButton = new QPushButton(importIcon, "");
    m_importButton->setToolTip("Import an intrinsic calibration.");
    layout->addWidget(m_importButton);

    QIcon removeIcon(QString::fromStdString(
                         core::runtime::getModuleResourceFilePath(
                             "sight::module::ui::icons",
                             "remove.svg"
                         ).string()
    ));
    m_removeButton = new QPushButton(removeIcon, "");
    m_removeButton->setToolTip("Remove the camera.");
    layout->addWidget(m_removeButton);

    m_extrinsicButton = new QPushButton("Extrinsic");
    layout->addWidget(m_extrinsicButton);
    m_extrinsicButton->setCheckable(true);

    qtContainer->setLayout(layout);

    std::size_t nbCam;
    {
        const auto cameraSeries = m_cameraSeries.lock();
        SIGHT_ASSERT("Missing cameraSeries.", cameraSeries);
        nbCam = cameraSeries->numCameras();
    }
    if(nbCam == 0)
    {
        this->addCamera();

        m_extrinsicButton->setEnabled(false);
        m_removeButton->setEnabled(false);
    }
    else
    {
        for(std::size_t i = 0 ; i < nbCam ; ++i)
        {
            m_cameraComboBox->addItem(QString("Camera %1").arg(i + 1));
        }

        const bool moreThanOneCamera = (nbCam > 1);

        m_extrinsicButton->setEnabled(moreThanOneCamera);
        m_removeButton->setEnabled(moreThanOneCamera);

        this->startIntrinsicConfig(0);
    }

    QObject::connect(m_cameraComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCameraChanged(int)));
    QObject::connect(m_addButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    QObject::connect(m_importButton, SIGNAL(clicked()), this, SLOT(onImportClicked()));
    QObject::connect(m_removeButton, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));
    QObject::connect(m_extrinsicButton, SIGNAL(toggled(bool)), this, SLOT(onExtrinsicToggled(bool)));
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::stopping()
{
    m_intrinsicLauncher.stopConfig();
    m_extrinsicLauncher.stopConfig();

    this->destroy();
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::updating()
{
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onCameraChanged(int index)
{
    {
        const auto cameraSeries = m_cameraSeries.lock();
        SIGHT_ASSERT(
            "Bad index: " << index,
            index >= 0 && static_cast<std::size_t>(index) < cameraSeries->numCameras()
        );
    }

    if(index == 0)
    {
        m_extrinsicButton->setChecked(false);
        m_extrinsicButton->setEnabled(false);
    }
    else
    {
        m_extrinsicButton->setEnabled(true);
    }

    if(m_extrinsicButton->isChecked())
    {
        this->startExtrinsicConfig(static_cast<std::size_t>(index));
    }
    else
    {
        this->startIntrinsicConfig(static_cast<std::size_t>(index));
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

void SCameraConfigLauncher::onImportClicked()
{
    auto sdb    = data::SeriesDB::New();
    auto reader = service::add<io::base::service::IReader>("sight::module::io::session::SReader");
    reader->setInOut(sdb, io::base::service::s_DATA_KEY);

    try
    {
        service::IService::ConfigType config;
        config.add("dialog.<xmlattr>.extension", ".cam");
        config.add("dialog.<xmlattr>.description", "Cameras");
        reader->configure(config);

        reader->start();
        reader->openLocationDialog();
        reader->update();
        reader->stop();
    }
    catch(std::exception const& e)
    {
        sight::ui::base::dialog::MessageDialog dlg;
        const auto msg = "Cannot read file: " + std::string(e.what());
        dlg.setTitle("Read error");
        dlg.setMessage(msg);
        dlg.setIcon(sight::ui::base::dialog::IMessageDialog::Icons::CRITICAL);
        SIGHT_ERROR(msg);

        throw;
    }
    service::OSR::unregisterService(reader);

    auto series             = sdb->getContainer();
    auto cameraSeriesVector = std::vector<data::CameraSeries::sptr>();

    for(auto& series_ : series)
    {
        auto cameraSeries_ = data::CameraSeries::dynamicCast(series_);
        if(cameraSeries_ != nullptr)
        {
            cameraSeriesVector.push_back(cameraSeries_);
        }
    }

    if(cameraSeriesVector.size() == 0)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "No CameraSeries in SDB",
            "There are no CameraSeries present in the loaded SeriesDB",
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    else
    {
        QStringList cameras;
        std::map<std::string, data::Camera::sptr> map;
        for(auto nSeries = 0 ; nSeries != cameraSeriesVector.size() ; ++nSeries)
        {
            auto cameraSeries_ = cameraSeriesVector[nSeries];
            for(auto nCam = 0 ; nCam != cameraSeries_->numCameras() ; ++nCam)
            {
                auto cam      = cameraSeries_->getCamera(nCam);
                auto cameraID =
                    cam->getCameraID() + " [" + std::to_string(nSeries) + ", " + std::to_string(nCam) + "]";
                map.insert(std::make_pair(cameraID, cam));
                cameras << QString(cameraID.data());
            }
        }

        if(cameras.size() == 0)
        {
            sight::ui::base::dialog::MessageDialog::show(
                "No Cameras in SDB",
                "There are CameraSeries present in the loaded SeriesDB, but no Cameras were found",
                sight::ui::base::dialog::IMessageDialog::CRITICAL
            );
        }
        else
        {
            auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
            bool ok          = false;
            auto selected    = QInputDialog::getItem(
                qtContainer->getQtContainer(),
                "Please select a camera",
                "Camera",
                cameras,
                0,
                false,
                &ok
            );
            if(ok)
            {
                const auto selectedStd    = selected.toStdString();
                const auto selectedCamera = map[selectedStd];
                const auto camIdx         = m_cameraComboBox->currentIndex();
                const auto cameraSeries   = m_cameraSeries.lock();
                auto camera               = cameraSeries->getCamera(camIdx);
                camera->deepCopy(selectedCamera);
                camera->signal<data::Camera::IntrinsicCalibratedSignalType>(
                    data::Camera::s_INTRINSIC_CALIBRATED_SIG
                )
                ->asyncEmit();
            }
        }
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onRemoveClicked()
{
    const std::size_t index = static_cast<std::size_t>(m_cameraComboBox->currentIndex());
    if(index > 0)
    {
        m_cameraComboBox->blockSignals(true);

        {
            const auto cameraSeries = m_cameraSeries.lock();
            // Remove camera
            data::Camera::sptr camera = cameraSeries->getCamera(index);
            cameraSeries->removeCamera(camera);
            data::CameraSeries::RemovedCameraSignalType::sptr sig;
            sig = cameraSeries->signal<data::CameraSeries::RemovedCameraSignalType>(
                data::CameraSeries::s_REMOVED_CAMERA_SIG
            );
            sig->asyncEmit(camera);

            // Remove calibrationInfo
            std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);
            const auto activitySeries      = m_activitySeries.lock();
            activitySeries->getData()->getContainer().erase(calibrationInfoKey);

            const std::size_t nbCam = cameraSeries->numCameras();
            if(nbCam == 1)
            {
                m_extrinsicButton->setEnabled(false);
                m_removeButton->setEnabled(false);
            }

            // Renamed all items from 1 to nbCam
            m_cameraComboBox->clear();
            for(std::size_t i = 0 ; i < nbCam ; ++i)
            {
                m_cameraComboBox->addItem(QString("Camera %1").arg(i + 1));
            }
        }

        // select first camera
        m_cameraComboBox->setCurrentIndex(0);
        this->startIntrinsicConfig(0);
        m_cameraComboBox->blockSignals(false);
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show("Warning", "The first camera can not be deleted");
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onExtrinsicToggled(bool checked)
{
    std::size_t index;
    {
        const auto cameraSeries = m_cameraSeries.lock();
        index = static_cast<std::size_t>(m_cameraComboBox->currentIndex());
        SIGHT_ASSERT("Bad index: " << index, index < cameraSeries->numCameras());
    }
    if(checked)
    {
        this->startExtrinsicConfig(index);
    }
    else
    {
        this->startIntrinsicConfig(index);
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startIntrinsicConfig(std::size_t index)
{
    service::FieldAdaptorType replaceMap;
    {
        const auto cameraSeries   = m_cameraSeries.lock();
        data::Camera::sptr camera = cameraSeries->getCamera(index);

        std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);

        const auto activitySeries             = m_activitySeries.lock();
        data::Composite::sptr data            = activitySeries->getData();
        data::CalibrationInfo::sptr calibInfo =
            data::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfoKey]);

        replaceMap["camera"]          = camera->getID();
        replaceMap["calibrationInfo"] = calibInfo->getID();
    }

    m_extrinsicLauncher.stopConfig();
    m_intrinsicLauncher.stopConfig();
    m_intrinsicLauncher.startConfig(this->getSptr(), replaceMap);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startExtrinsicConfig(std::size_t index)
{
    service::FieldAdaptorType replaceMap;
    {
        const std::size_t cameraIdx = std::max(index, std::size_t(1));

        const auto cameraSeries = m_cameraSeries.lock();

        data::Camera::sptr camera1 = cameraSeries->getCamera(0);
        data::Camera::sptr camera2 = cameraSeries->getCamera(cameraIdx);

        // Check if the two cameras are calibrated
        if(!camera1->getIsCalibrated() || !camera2->getIsCalibrated())
        {
            sight::ui::base::dialog::MessageDialog::show("Calibration", "Cameras must be intrinsically calibrated.");
            m_extrinsicButton->setChecked(false);
            return;
        }

        // Add 2 calibration info in ActivitySeries if not exist
        std::string calibrationInfo1Key = "calibrationInfoExtr0_" + boost::lexical_cast<std::string>(cameraIdx);
        std::string calibrationInfo2Key = "calibrationInfoExtr1_" + boost::lexical_cast<std::string>(cameraIdx);

        const auto activitySeries  = m_activitySeries.lock();
        data::Composite::sptr data = activitySeries->getData();
        data::CalibrationInfo::sptr calibInfo1;
        data::CalibrationInfo::sptr calibInfo2;
        // Get the calibrationInfo from the activity series if it exists or create it.
        if(data->find(calibrationInfo1Key) == data->end() || data->find(calibrationInfo2Key) == data->end())
        {
            calibInfo1 = data::CalibrationInfo::New();
            calibInfo2 = data::CalibrationInfo::New();

            data->getContainer()[calibrationInfo1Key] = calibInfo1;
            data->getContainer()[calibrationInfo2Key] = calibInfo2;
        }
        else
        {
            calibInfo1 = data::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfo1Key]);
            calibInfo2 = data::CalibrationInfo::dynamicCast(data->getContainer()[calibrationInfo2Key]);
        }

        replaceMap["camera1"]          = camera1->getID();
        replaceMap["camera2"]          = camera2->getID();
        replaceMap["calibrationInfo1"] = calibInfo1->getID();
        replaceMap["calibrationInfo2"] = calibInfo2->getID();
        replaceMap["camIndex"]         = std::to_string(index);
    }

    m_extrinsicLauncher.stopConfig();
    m_intrinsicLauncher.stopConfig();
    m_extrinsicLauncher.startConfig(this->getSptr(), replaceMap);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::addCamera()
{
    std::size_t nbCam;
    {
        const auto cameraSeries = m_cameraSeries.lock();
        nbCam = cameraSeries->numCameras();

        data::Camera::sptr camera = data::Camera::New();

        // Add the CalibrationInfo in activitySeries to be saved in activity
        std::string calibrationInfoKey        = "calibrationInfo_" + std::to_string(nbCam);
        data::CalibrationInfo::sptr calibInfo = data::CalibrationInfo::New();

        const auto activitySeries = m_activitySeries.lock();
        activitySeries->getData()->getContainer()[calibrationInfoKey] = calibInfo;

        // Add the camera
        cameraSeries->addCamera(camera);
        data::CameraSeries::AddedCameraSignalType::sptr sig;
        sig = cameraSeries->signal<data::CameraSeries::AddedCameraSignalType>(
            data::CameraSeries::s_ADDED_CAMERA_SIG
        );
        sig->asyncEmit(camera);
    }

    m_cameraComboBox->blockSignals(true);
    m_cameraComboBox->addItem(QString("Camera %1").arg(nbCam + 1));
    m_cameraComboBox->setCurrentIndex(static_cast<int>(nbCam));
    m_extrinsicButton->setChecked(false);
    this->startIntrinsicConfig(nbCam);
    m_cameraComboBox->blockSignals(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
