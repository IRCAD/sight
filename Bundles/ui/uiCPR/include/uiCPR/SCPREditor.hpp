/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiCPR/config.hpp"

#include <fwCom/Signals.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QObject>
#include <QPointer>

#include <string>
#include <vector>

class QSlider;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;

namespace uiCPR
{

/**
 * @brief Creates a GUI to set parameters for a curved planar reformation (CPR).
 *  @code{.xml}
        <service type="::uiCPR::SCPREditor">
            <in key="image" uid="..." />
        </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: source image used to generate CPR.
 */
class UICPR_CLASS_API SCPREditor : public QObject,
                                   public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro((SCPREditor)(::fwGui::editor::IEditor));

    /**
     * @name Constructor/Destructor.
     * @{ */
    UICPR_API SCPREditor () noexcept;
    UICPR_API virtual ~SCPREditor () noexcept;
    /**  @} */

    /**
     * @name Signal keys.
     * @{ */
    UICPR_API static const ::fwCom::Signals::SignalKeyType s_HEIGHT_CHANGED_SIG;
    UICPR_API static const ::fwCom::Signals::SignalKeyType s_SPACING_CHANGED_SIG;
    UICPR_API static const ::fwCom::Signals::SignalKeyType s_SLIDER_PROGRESSED_SIG;
    /**  @} */

    /**
     * @name Signal types.
     * @{ */
    typedef ::fwCom::Signal < void (double) > HeightChangedSignalType;
    typedef ::fwCom::Signal < void (double) > SpacingChangedSignalType;
    typedef ::fwCom::Signal < void (double) > SliderProgressedSignalType;
    /**  @} */

protected:

    /**
     * @name Overrides
     * @{ */
    virtual void starting() override;
    virtual void stopping() override;
    virtual void updating() override;
    /**  @} */

    /**
     * @brief Overrides IService::configuring().
     * @throw fwTools::Failed
     */
    virtual void configuring() override;

    /// Signal emitted when the height value changes.
    HeightChangedSignalType::sptr m_sigHeightChanged;

    /// Signal emitted when the spacing value changes.
    SpacingChangedSignalType::sptr m_sigSpacingChanged;

    /// Signal emitted when the slider value changes.
    SliderProgressedSignalType::sptr m_sigSliderProgressed;

protected Q_SLOTS:

    /// Triggered when the slider value changes.
    void onChangeSliderValue(int newSliderValue);

    /// Triggered when the height value changes.
    void onChangeHeightValue (double newHeight);

    /// Triggered when the spacing value changes.
    void onChangeSpacingValue (double newSpacing);

    /// Triggered when the "Compute" button is clicked.
    void onClickComputeSlotType();

private:

    /// CPR current rotation angle.
    double m_angle;

    /// CPR current spacing.
    double m_spacing;

    /// CPR minimum spacing, relative to image spacing.
    double m_minSpacing;

    /// CPR maximum spacing, relative to image spacing.
    double m_maxSpacing;

    /// CPR current height.
    double m_height;

    /// CPR minimum height.
    static const double s_MIN_HEIGHT;

    /// CPR maximum height, relative to image size.
    double m_maxHeight;

    /// Widget to adjust CPR height.
    QPointer< QDoubleSpinBox > m_heightSpinBox;

    /// Widget to adjust CPR spacing.
    QPointer< QDoubleSpinBox > m_spacingSpinBox;

    /// Widget to adjust CPR rotation angle.
    QPointer< QSlider > m_rotationSlider;

    /// Show current CPR rotation angle.
    QPointer< QLineEdit > m_angleText;

    /// Widget to trigger CPR computation.
    QPointer< QPushButton> m_computeButton;
};

} // uiCPR
