/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/ui/qt/calibration/SIntrinsicEdition.hpp"

#include <core/base.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/Worker.hpp>
#include <core/tools/Object.hpp>

#include <data/Camera.hpp>

#include <service/macros.hpp>

namespace sight::module::ui::qt::calibration
{

static const service::IService::KeyType s_CAMERA_INOUT = "camera";

// -------------------------------------------------------------------------

SIntrinsicEdition::SIntrinsicEdition()
{
    core::com::HasSlots::m_slots.setWorker(m_associatedWorker);
    m_dialog = new SUpdateIntrinsicDialog();

    QObject::connect(
        m_dialog,
        SIGNAL(newCalibration(std::array<double,12>&)),
        this,
        SLOT(onNewCalibration(std::array<double,12>&))
    );
}

// -------------------------------------------------------------------------

SIntrinsicEdition::~SIntrinsicEdition()
{
    QObject::disconnect(
        m_dialog,
        SIGNAL(newCalibration(std::array<double,12>&)),
        this,
        SLOT(onNewCalibration(std::array<double,12>&))
    );
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::onNewCalibration(std::array<double, 12>& cal)
{
    m_calibration = cal;

    this->updateCalibration();
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::updateCalibration()
{
    data::Camera::sptr camera = this->getInOut<data::Camera>(s_CAMERA_INOUT);
    SIGHT_ASSERT("The inout key '" + s_CAMERA_INOUT + "' is not correctly set.", camera);

    camera->setWidth(m_calibration[0]);
    camera->setHeight(m_calibration[1]);

    camera->setFx(m_calibration[2]);
    camera->setFy(m_calibration[3]);
    camera->setCx(m_calibration[4]);
    camera->setCy(m_calibration[5]);

    camera->setDistortionCoefficient(
        m_calibration[6],
        m_calibration[7],
        m_calibration[8],
        m_calibration[9],
        m_calibration[10]
    );

    camera->setSkew(m_calibration[11]);

    data::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal<data::Camera::IntrinsicCalibratedSignalType>(
        data::Camera::s_INTRINSIC_CALIBRATED_SIG
    );

    sig->asyncEmit();
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::configuring()
{
    this->readCalibration();

    m_dialog->setParameters(m_calibration);
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::readCalibration()
{
    data::Camera::sptr camera = this->getInOut<data::Camera>(s_CAMERA_INOUT);
    SIGHT_ASSERT("The inout key '" + s_CAMERA_INOUT + "' is not correctly set.", camera);

    m_calibration[0] = camera->getWidth();
    m_calibration[1] = camera->getHeight();

    m_calibration[2] = camera->getFx();
    m_calibration[3] = camera->getFy();
    m_calibration[4] = camera->getCx();
    m_calibration[5] = camera->getCy();

    m_distParameters = camera->getDistortionCoefficient();

    m_calibration[6]  = m_distParameters[0];
    m_calibration[7]  = m_distParameters[1];
    m_calibration[8]  = m_distParameters[2];
    m_calibration[9]  = m_distParameters[3];
    m_calibration[10] = m_distParameters[4];
    m_calibration[11] = camera->getSkew();
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::starting()
{
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::stopping()
{
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::updating()
{
    this->readCalibration();
    m_dialog->setParameters(m_calibration);
    m_dialog->show();
}

// -------------------------------------------------------------------------

} // uiCalibration
