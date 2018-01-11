/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SOPTICALCENTEREDITOR_HPP__
#define __UICALIBRATION_SOPTICALCENTEREDITOR_HPP__

#include "uiCalibration/config.hpp"

#include <gui/editor/IEditor.hpp>

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
                                                     public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SOpticalCenterEditor)(::gui::editor::IEditor) );

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

#endif // __UICALIBRATION_SOPTICALCENTEREDITOR_HPP__
