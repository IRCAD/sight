/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiVisuQt/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QLabel;
class QLineEdit;

namespace uiVisuQt
{
/**
 * @brief   This editor regulates the position and rotation defined in a transformation matrix.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiVisuQt::STransformEditor" autoConnect="no">
        <inout key="matrix" uid="..."/>
        <translation enabled="no" min="-300"/>
        <rotation enabled="yes" min="-180" max="180" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b matrix [::fwData::TransformationMatrix3D]: matrix modified by the editor
 *
 * @subsection Configuration Configuration
 * - \b enabled (optional): enable/disable rotation/translation edition.
 *      Can be 'yes', 'no' or a combination of [xyz] (default: yes).
 * - \b min (optional): set the minimum value for translation/rotation (default: translation=-300, rotation=-180 ).
 * - \b max (optional): set the maximum value for translation/rotation (default: translation=+300, rotation=180).
 */
class UIVISUQT_CLASS_API STransformEditor : public QObject,
                                            public ::fwGui::editor::IEditor
{
Q_OBJECT;
public:
    fwCoreServiceClassDefinitionsMacro( (STransformEditor)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API STransformEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~STransformEditor() noexcept;

protected:

    /// This method is used to configure the service parameters:
    UIVISUQT_API virtual void configuring() override;

    ///This method launches the ::fwGui::IGuiContainerSrv::create method.
    UIVISUQT_API virtual void starting() override;

    ///This method launches the ::fwGui::IGuiContainerSrv::destroy method.
    UIVISUQT_API virtual void stopping() override;

    /// Updates Slider value
    UIVISUQT_API virtual void updating() override;

    // Connect ::fwData::TransformationMatrix3D::s_MODIFIED_SIG to update slot
    UIVISUQT_API virtual KeyConnectionsMap getAutoConnections() const override;

private Q_SLOTS:

    /// Slot called when slider value changed.
    void onSliderChanged(int value);

    /// Slot called when line edit value changed.
    void onTextChanged();

private:

    /// Update the editor when the matrix changes
    void updateFromMatrix();

    /*
     * @brief This enum defines the transformation matrix entries indexes
     */
    enum SliderIndex
    {
        POSITION_X = 0,
        POSITION_Y,
        POSITION_Z,
        ROTATION_X,
        ROTATION_Y,
        ROTATION_Z,
        MAX_SLIDER_INDEX
    };

    /*
     * @brief This struct regulates a transformation matrix entry in the editor
     */
    struct SliderWidget
    {
        QPointer<QSlider> m_slider; ///< Slider to change coefficient value.
        QPointer<QLabel> m_labelMin; ///< Label to show the min value.
        QPointer<QLabel> m_labelMax; ///< Label to show the max value.
        QPointer<QLabel> m_labelDefinition; ///< Label to show the coefficient description.
        QPointer<QLineEdit> m_sliderValue; ///< Editor to show the current value of the slider.
    };

    /// Array containing the different structs to regulate the transformation matrix entries.
    SliderWidget m_sliders[ MAX_SLIDER_INDEX ];

    /// Contains a string identifying which axes [xyz] are displayed for rotation
    std::string m_rotation;

    /// Contains a string identifying which axes [xyz] are displayed for translation
    std::string m_translation;

    /// Range of translation sliders
    int m_translationRange[2];

    /// Range of rotation sliders
    int m_rotationRange[2];
};

} // namespace uiVisuQt
