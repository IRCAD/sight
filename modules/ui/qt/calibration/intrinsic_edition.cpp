/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/ui/qt/calibration/intrinsic_edition.hpp"

#include <core/base.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>
#include <core/tools/object.hpp>

#include <data/camera.hpp>

#include <service/macros.hpp>

namespace sight::module::ui::qt::calibration
{

// -------------------------------------------------------------------------

intrinsic_edition::intrinsic_edition() :
    m_dialog(new update_intrinsic_dialog())
{
    core::com::has_slots::m_slots.set_worker(this->worker());

    QObject::connect(
        m_dialog,
        SIGNAL(newCalibration(std::array<double,12>&)),
        this,
        SLOT(onNewCalibration(std::array<double,12>&))
    );
}

// -------------------------------------------------------------------------

intrinsic_edition::~intrinsic_edition()
{
    QObject::disconnect(
        m_dialog,
        SIGNAL(newCalibration(std::array<double,12>&)),
        this,
        SLOT(onNewCalibration(std::array<double,12>&))
    );
}

// -------------------------------------------------------------------------

void intrinsic_edition::onNewCalibration(std::array<double, 12>& cal)
{
    m_calibration = cal;

    this->updateCalibration();
}

// -------------------------------------------------------------------------

void intrinsic_edition::updateCalibration()
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("The inout key '" << s_CAMERA << "' is not correctly set.", camera);

    camera->setWidth(std::size_t(m_calibration[0]));
    camera->setHeight(std::size_t(m_calibration[1]));

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

    data::camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal<data::camera::IntrinsicCalibratedSignalType>(
        data::camera::INTRINSIC_CALIBRATED_SIG
    );

    sig->async_emit();
}

// -------------------------------------------------------------------------

void intrinsic_edition::configuring()
{
    this->readCalibration();

    m_dialog->setParameters(m_calibration);
}

// -------------------------------------------------------------------------

void intrinsic_edition::readCalibration()
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("The inout key '" << s_CAMERA << "' is not correctly set.", camera);

    m_calibration[0] = double(camera->getWidth());
    m_calibration[1] = double(camera->getHeight());

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

void intrinsic_edition::starting()
{
}

// -------------------------------------------------------------------------

void intrinsic_edition::stopping()
{
}

// -------------------------------------------------------------------------

void intrinsic_edition::updating()
{
    this->readCalibration();
    m_dialog->setParameters(m_calibration);
    m_dialog->show();
}

// -------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
