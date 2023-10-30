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
    QObject::connect(m_dialog, &update_intrinsic_dialog::new_calibration, this, &intrinsic_edition::on_new_calibration);
}

// -------------------------------------------------------------------------

intrinsic_edition::~intrinsic_edition()
{
    QObject::disconnect(
        m_dialog,
        &update_intrinsic_dialog::new_calibration,
        this,
        &intrinsic_edition::on_new_calibration
    );
}

// -------------------------------------------------------------------------

void intrinsic_edition::on_new_calibration(std::array<double, 12>& _cal)
{
    m_calibration = _cal;

    this->update_calibration();
}

// -------------------------------------------------------------------------

void intrinsic_edition::update_calibration()
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("The inout key '" << CAMERA << "' is not correctly set.", camera);

    camera->set_width(std::size_t(m_calibration[0]));
    camera->set_height(std::size_t(m_calibration[1]));

    camera->set_fx(m_calibration[2]);
    camera->set_fy(m_calibration[3]);
    camera->set_cx(m_calibration[4]);
    camera->set_cy(m_calibration[5]);

    camera->set_distortion_coefficient(
        m_calibration[6],
        m_calibration[7],
        m_calibration[8],
        m_calibration[9],
        m_calibration[10]
    );

    camera->set_skew(m_calibration[11]);

    data::camera::intrinsic_calibrated_signal_t::sptr sig;
    sig = camera->signal<data::camera::intrinsic_calibrated_signal_t>(
        data::camera::INTRINSIC_CALIBRATED_SIG
    );

    sig->async_emit();
}

// -------------------------------------------------------------------------

void intrinsic_edition::configuring()
{
    this->read_calibration();

    m_dialog->set_parameters(m_calibration);
}

// -------------------------------------------------------------------------

void intrinsic_edition::read_calibration()
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("The inout key '" << CAMERA << "' is not correctly set.", camera);

    m_calibration[0] = double(camera->get_width());
    m_calibration[1] = double(camera->get_height());

    m_calibration[2] = camera->get_fx();
    m_calibration[3] = camera->get_fy();
    m_calibration[4] = camera->get_cx();
    m_calibration[5] = camera->get_cy();

    m_dist_parameters = camera->get_distortion_coefficient();

    m_calibration[6]  = m_dist_parameters[0];
    m_calibration[7]  = m_dist_parameters[1];
    m_calibration[8]  = m_dist_parameters[2];
    m_calibration[9]  = m_dist_parameters[3];
    m_calibration[10] = m_dist_parameters[4];
    m_calibration[11] = camera->get_skew();
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
    this->read_calibration();
    m_dialog->set_parameters(m_calibration);
    m_dialog->show();
}

// -------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
