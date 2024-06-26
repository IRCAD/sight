/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#pragma once

#include "modules/ui/qt/calibration/update_intrinsic_dialog.hpp"

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/camera.hpp>

#include <service/base.hpp>

#include <QObject>

#include <array>
#include <list>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   intrinsic_edition service is used to set the intrinsic parameter information.
 *
 * This services displays a dialog to change the resolution of your calibration, and computes the new intrinsic
 * parameters.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::calibration::intrinsic_edition">
       <inout key="camera" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b camera [sight::data::camera]: camera on which the intrinsic parameters will be modified.
 */
class intrinsic_edition : public QObject,
                          public service::base
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(intrinsic_edition, sight::service::base);

    /**
     * @brief Constructor.
     */
    intrinsic_edition();

    /**
     * @brief Destructor.
     */
    ~intrinsic_edition() override;

    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    void stopping() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    void updating() override;

private Q_SLOTS:

    /**
     * @brief onNewCalibration
     * @param _cal  the new calibration data
     */
    void on_new_calibration(std::array<double, 12>& _cal);

private:

    void update_calibration();

    void read_calibration();

    std::array<double, 4> m_intrinsic {};
    std::array<double, 5> m_dist_parameters {};

    double m_skew {};
    std::array<double, 2> m_resolution {};

    std::array<double, 12> m_calibration {};

    update_intrinsic_dialog* m_dialog;

    static constexpr std::string_view CAMERA = "camera";
    data::ptr<data::camera, data::access::inout> m_camera {this, CAMERA};
};

} // namespace sight::module::ui::qt::calibration
