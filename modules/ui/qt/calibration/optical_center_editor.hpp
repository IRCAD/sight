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

#pragma once

#include "modules/ui/qt/config.hpp"

#include <data/camera.hpp>
#include <data/matrix4.hpp>

#include <ui/__/editor.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief   This editor shows sliders to configure an intrinsic camera calibration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="sight::module::ui::qt::calibration::optical_center_editor">
         <in key="camera" uid="..." />
         <inout key="matrix" uid="..." />
     </service>
   @endcode
 * @subsection Input Input
 * - \b camera [sight::data::camera]: camera to edit.
 * @subsection In-Out In-Out
 * - \b matrix [sight::data::matrix4]: output matrix holding the delta values.
 *
 * This service takes a camera calibration and outputs a matrix holding the difference between
 * the input parameters and the camera parameters set by the user. Those differences are stored like this:
 * @verbatim
   dFx 0   dCx 0
   0   dFy dCy 0
   0   0   0   0
   0   0   0   0
 */
class MODULE_UI_QT_CLASS_API optical_center_editor : public QObject,
                                                     public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(optical_center_editor, sight::ui::editor);

    /// Constructor.
    MODULE_UI_QT_API optical_center_editor() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_QT_API ~optical_center_editor() noexcept override;

protected:

    /// Does nothing
    MODULE_UI_QT_API void configuring() override;

    /// Create UI.
    MODULE_UI_QT_API void starting() override;

    /// Destroy UI.
    MODULE_UI_QT_API void stopping() override;

    /// Does nothing.
    MODULE_UI_QT_API void updating() override;

    /// Defines the connection between camera and matrix and this editor
    MODULE_UI_QT_API connections_t auto_connections() const override;

private Q_SLOTS:

    void on_cx_slider_changed(int _value);
    void on_cy_slider_changed(int _value);
    void on_fy_slider_changed(int _value);

private:

    QPointer<QSlider> m_cx_slider; ///< Slider to modify cx
    QPointer<QSlider> m_cy_slider; ///< Slider to modify cy
    QPointer<QSlider> m_fy_slider; ///< Slider to modify fy
    QPointer<QLabel> m_cx_label;   ///< Label for cx value
    QPointer<QLabel> m_cy_label;   ///< Label for cy value
    QPointer<QLabel> m_fy_label;   ///< Label for fy value

    static constexpr std::string_view CAMERA = "camera";
    static constexpr std::string_view MATRIX = "matrix";

    data::ptr<data::camera, data::access::in> m_camera {this, CAMERA, true};
    data::ptr<data::matrix4, data::access::inout> m_matrix {this, MATRIX, true};
};

} // namespace sight::module::ui::qt::calibration
