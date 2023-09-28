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

#include "modules/ui/qt/calibration/SCameraConfigLauncher.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/path.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/Composite.hpp>
#include <data/Vector.hpp>

#include <io/__/service/ioTypes.hpp>
#include <io/__/service/reader.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/lexical_cast.hpp>

#include <QHBoxLayout>
#include <QInputDialog>
#include <QStringList>

namespace sight::module::ui::qt::calibration
{

SCameraConfigLauncher::SCameraConfigLauncher() noexcept =
    default;

//------------------------------------------------------------------------------

SCameraConfigLauncher::~SCameraConfigLauncher() noexcept =
    default;

//------------------------------------------------------------------------------

void SCameraConfigLauncher::configuring()
{
    this->initialize();
    service::config_t configuration = this->getConfiguration();

    SIGHT_ASSERT(
        "There must be one (and only one) <config/> element.",
        configuration.count("config") == 1
    );
    const service::config_t& srvconfig = configuration;
    const service::config_t& config    = srvconfig.get_child("config");

    const service::config_t& intrinsic = config.get_child("intrinsic");
    const service::config_t& extrinsic = config.get_child("extrinsic");

    m_intrinsicLauncher.parseConfig(intrinsic, this->get_sptr());
    m_extrinsicLauncher.parseConfig(extrinsic, this->get_sptr());
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::starting()
{
    this->create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* layout = new QHBoxLayout();

    m_cameraComboBox = new QComboBox();
    layout->addWidget(m_cameraComboBox);

    QIcon addIcon(QString::fromStdString(
                      core::runtime::get_module_resource_file_path(
                          "sight::module::ui::icons",
                          "Import.svg"
                      )
                      .string()
    ));
    m_addButton = new QPushButton(addIcon, "");
    m_addButton->setToolTip("Add a new camera.");
    layout->addWidget(m_addButton);

    QIcon importIcon(QString::fromStdString(
                         core::runtime::get_module_resource_file_path(
                             "sight::module::ui::icons",
                             "CameraSeries.svg"
                         )
                         .string()
    ));
    m_importButton = new QPushButton(importIcon, "");
    m_importButton->setToolTip("Import an intrinsic calibration.");
    layout->addWidget(m_importButton);

    QIcon removeIcon(QString::fromStdString(
                         core::runtime::get_module_resource_file_path(
                             "sight::module::ui::icons",
                             "remove.svg"
                         )
                         .string()
    ));
    m_removeButton = new QPushButton(removeIcon, "");
    m_removeButton->setToolTip("Remove the camera.");
    layout->addWidget(m_removeButton);

    m_extrinsicButton = new QPushButton("Extrinsic");
    layout->addWidget(m_extrinsicButton);
    m_extrinsicButton->setCheckable(true);

    qtContainer->setLayout(layout);

    std::size_t nbCam = 0;
    {
        const auto camera_set = m_camera_set.lock();
        SIGHT_ASSERT("Missing cameraSet.", camera_set);
        nbCam = camera_set->size();
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
        const auto cameraSet = m_camera_set.lock();
        SIGHT_ASSERT(
            "Bad index: " << index,
            index >= 0 && static_cast<std::size_t>(index) < cameraSet->size()
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
    auto vector = std::make_shared<data::Vector>();
    auto reader = sight::service::add<io::service::reader>("sight::module::io::session::SReader");
    reader->setInOut(vector, io::service::s_DATA_KEY);

    try
    {
        service::config_t config;
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
        sight::ui::dialog::message dlg;
        const auto msg = "Cannot read file: " + std::string(e.what());
        dlg.setTitle("Read error");
        dlg.setMessage(msg);
        dlg.setIcon(sight::ui::dialog::message::Icons::CRITICAL);
        SIGHT_ERROR(msg);

        throw;
    }

    sight::service::remove(reader);

    QStringList cameras;
    std::map<std::string, data::Camera::csptr> camera_map;
    std::size_t n_set = 0;

    for(const auto& object : *vector)
    {
        const auto& camera_set = std::dynamic_pointer_cast<data::CameraSet>(object);

        if(camera_set)
        {
            for(std::size_t n_camera = 0, end = camera_set->size() ; n_camera != end ; ++n_camera)
            {
                const auto& camera   = camera_set->get_camera(n_camera);
                const auto& cameraID =
                    camera->getCameraID() + " [" + std::to_string(n_set) + ", " + std::to_string(n_camera) + "]";

                camera_map.insert(std::make_pair(cameraID, camera));
                cameras << QString::fromStdString(cameraID);
            }

            ++n_set;
        }
    }

    if(n_set == 0)
    {
        sight::ui::dialog::message::show(
            "No CameraSet in file",
            "There are no CameraSet present in the loaded file.",
            sight::ui::dialog::message::CRITICAL
        );
    }
    else if(cameras.empty())
    {
        sight::ui::dialog::message::show(
            "No Cameras in file",
            "There are CameraSet present in the loaded CameraSet, but no Cameras were found",
            sight::ui::dialog::message::CRITICAL
        );
    }
    else
    {
        auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
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
            const auto selectedCamera = camera_map[selectedStd];
            const auto camIdx         = m_cameraComboBox->currentIndex();
            const auto camera_set     = m_camera_set.lock();
            auto camera               = camera_set->get_camera(std::size_t(camIdx));
            camera->deep_copy(selectedCamera);
            camera->signal<data::Camera::IntrinsicCalibratedSignalType>(
                data::Camera::INTRINSIC_CALIBRATED_SIG
            )
            ->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onRemoveClicked()
{
    const auto index = static_cast<std::size_t>(m_cameraComboBox->currentIndex());
    if(index > 0)
    {
        m_cameraComboBox->blockSignals(true);

        {
            const auto camera_set = m_camera_set.lock();
            // Remove camera
            data::Camera::sptr camera = camera_set->get_camera(index);
            camera_set->remove_camera(camera);

            auto sig = camera_set->signal<data::CameraSet::removed_camera_signal_t>(
                data::CameraSet::REMOVED_CAMERA_SIG
            );
            sig->async_emit(camera);

            // Remove calibrationInfo
            std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);
            const auto activity            = m_activity.lock();
            activity->erase(calibrationInfoKey);

            const std::size_t nbCam = camera_set->size();
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
        sight::ui::dialog::message::show("Warning", "The first camera can not be deleted");
    }
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::onExtrinsicToggled(bool checked)
{
    std::size_t index = 0;
    {
        const auto camera_set = m_camera_set.lock();
        index = static_cast<std::size_t>(m_cameraComboBox->currentIndex());
        SIGHT_ASSERT("Bad index: " << index, index < camera_set->size());
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
    sight::service::FieldAdaptorType replaceMap;
    {
        const auto camera_set     = m_camera_set.lock();
        data::Camera::sptr camera = camera_set->get_camera(index);

        std::string calibrationInfoKey = "calibrationInfo_" + std::to_string(index);

        const auto activity = m_activity.lock();
        auto calibInfo      = std::dynamic_pointer_cast<data::CalibrationInfo>((*activity)[calibrationInfoKey]);

        replaceMap["camera"]          = camera->get_id();
        replaceMap["calibrationInfo"] = calibInfo->get_id();
    }

    m_extrinsicLauncher.stopConfig();
    m_intrinsicLauncher.stopConfig();
    m_intrinsicLauncher.startConfig(this->get_sptr(), replaceMap);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::startExtrinsicConfig(std::size_t index)
{
    sight::service::FieldAdaptorType replaceMap;
    {
        const std::size_t cameraIdx = std::max(index, std::size_t(1));

        const auto camera_set = m_camera_set.lock();

        data::Camera::sptr camera1 = camera_set->get_camera(0);
        data::Camera::sptr camera2 = camera_set->get_camera(cameraIdx);

        // Check if the two cameras are calibrated
        if(!camera1->getIsCalibrated() || !camera2->getIsCalibrated())
        {
            sight::ui::dialog::message::show("Calibration", "Cameras must be intrinsically calibrated.");
            m_extrinsicButton->setChecked(false);
            return;
        }

        // cspell: ignore Extr
        // Add 2 calibration info in Activity if not exist
        std::string calibrationInfo1Key = "calibrationInfoExtr0_" + std::to_string(cameraIdx);
        std::string calibrationInfo2Key = "calibrationInfoExtr1_" + std::to_string(cameraIdx);
        const auto activity             = m_activity.lock();
        data::CalibrationInfo::sptr calibInfo1;
        data::CalibrationInfo::sptr calibInfo2;
        // Get the calibrationInfo from the activity if it exists or create it.
        if(activity->find(calibrationInfo1Key) == activity->end()
           || activity->find(calibrationInfo2Key) == activity->end())
        {
            calibInfo1 = std::make_shared<data::CalibrationInfo>();
            calibInfo2 = std::make_shared<data::CalibrationInfo>();

            (*activity)[calibrationInfo1Key] = calibInfo1;
            (*activity)[calibrationInfo2Key] = calibInfo2;
        }
        else
        {
            calibInfo1 = std::dynamic_pointer_cast<data::CalibrationInfo>((*activity)[calibrationInfo1Key]);
            calibInfo2 = std::dynamic_pointer_cast<data::CalibrationInfo>((*activity)[calibrationInfo2Key]);
        }

        replaceMap["camera1"]          = camera1->get_id();
        replaceMap["camera2"]          = camera2->get_id();
        replaceMap["calibrationInfo1"] = calibInfo1->get_id();
        replaceMap["calibrationInfo2"] = calibInfo2->get_id();
        replaceMap["camIndex"]         = std::to_string(index);
    }

    m_extrinsicLauncher.stopConfig();
    m_intrinsicLauncher.stopConfig();
    m_extrinsicLauncher.startConfig(this->get_sptr(), replaceMap);
}

//------------------------------------------------------------------------------

void SCameraConfigLauncher::addCamera()
{
    std::size_t nbCam = 0;
    {
        const auto camera_set = m_camera_set.lock();
        nbCam = camera_set->size();

        data::Camera::sptr camera = std::make_shared<data::Camera>();

        // Add the CalibrationInfo in activity to be saved in activity
        std::string calibrationInfoKey        = "calibrationInfo_" + std::to_string(nbCam);
        data::CalibrationInfo::sptr calibInfo = std::make_shared<data::CalibrationInfo>();

        const auto activity = m_activity.lock();
        (*activity)[calibrationInfoKey] = calibInfo;

        // Add the camera
        camera_set->add_camera(camera);
        auto sig = camera_set->signal<data::CameraSet::added_camera_signal_t>(
            data::CameraSet::ADDED_CAMERA_SIG
        );
        sig->async_emit(camera);
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
