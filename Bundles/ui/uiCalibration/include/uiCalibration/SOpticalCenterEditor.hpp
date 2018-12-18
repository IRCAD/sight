/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "uiCalibration/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace uiCalibration
{

/**
 * @brief   This editor shows sliders to configure an intrinsic camera calibration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="::uiCalibration::SOpticalCenterEditor">
         <in key="camera" uid="..." />
         <inout key="matrix" uid="..." />
     </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera to edit.
 * @subsection In-Out In-Out
 * - \b matrix [::fwData::TransformationMatrix3D]: output matrix holding the delta values.
 *
 * This service takes a camera calibration and outputs a matrix holding the difference between
 * the input parameters and the camera parameters set by the user. Those differences are stored like this:
 * @verbatim
   dFx 0   dCx 0
   0   dFy dCy 0
   0   0   0   0
   0   0   0   0
 */
class UICALIBRATION_CLASS_API SOpticalCenterEditor : public QObject,
                                                     public ::fwGui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SOpticalCenterEditor)(::fwGui::editor::IEditor) );

    /// Constructor.
    UICALIBRATION_API SOpticalCenterEditor() noexcept;

    /// Destructor. Does nothing
    UICALIBRATION_API virtual ~SOpticalCenterEditor() noexcept;

protected:

    /// Does nothing
    UICALIBRATION_API virtual void configuring() override;

    /// Create UI.
    UICALIBRATION_API virtual void starting() override;

    /// Destroy UI.
    UICALIBRATION_API virtual void stopping() override;

    /// Does nothing.
    UICALIBRATION_API virtual void updating() override;

    /// Defines the connection betwwen camera and matrix and this editor
    UICALIBRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

private Q_SLOTS:

    void onCxSliderChanged(int value);
    void onCySliderChanged(int value);
    void onFySliderChanged(int value);

private:

    QPointer<QSlider> m_cxSlider; ///< Slider to modify cx
    QPointer<QSlider> m_cySlider; ///< Slider to modify cy
    QPointer<QSlider> m_fySlider; ///< Slider to modify fy
    QPointer<QLabel> m_cxLabel;   ///< Label for cx value
    QPointer<QLabel> m_cyLabel;   ///< Label for cy value
    QPointer<QLabel> m_fyLabel;   ///< Label for fy value
};

} // uiCalibration
