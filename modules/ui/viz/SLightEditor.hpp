/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/viz/config.hpp"

#include <viz/scene3d/ILight.hpp>

#include <OGRE/OgreColourValue.h>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include <ui/base/IEditor.hpp>

namespace sight::modules::ui::viz
{

/**
 * @brief This service creates a user interface to manage a light adaptor.
 *
 * @section Slots Slots
 * - \b editLight(viz::scene3d::ILight::sptr): loads the editor with the parameters from the selected light.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
 *  <service uid=SLightEditorUid" type="modules::ui::viz::SLightEditor" />
 */
class MODULE_UI_VIZ_CLASS_API SLightEditor : public QObject,
                                             public sight::ui::base::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(SLightEditor, sight::ui::base::IEditor)

    /// Creates the service.
    MODULE_UI_VIZ_API SLightEditor() noexcept;

    /// Destroys the service.
    MODULE_UI_VIZ_API virtual ~SLightEditor() noexcept;

private:

    /// Configures the service.
    MODULE_UI_VIZ_API virtual void configuring() override final;

    /// Sets the connections and the UI elements.
    MODULE_UI_VIZ_API virtual void starting() override final;

    /// Does nothing.
    MODULE_UI_VIZ_API virtual void updating() override final;

    /// Destroys the connections and cleans the container.
    MODULE_UI_VIZ_API virtual void stopping() override final;

    /**
     * @brief Gets the current light node.
     * @return The node where the current light is attached.
     */
    ::Ogre::Node* getLightNode() const;

    /**
     * @brief SLOT: sets the current light adaptor to edit.
     * @param _lightAdaptor The light adaptor to edit.
     */
    void editLight(sight::viz::scene3d::ILight::sptr _lightAdaptor);

    /**
     * @brief Opens a QColorDialog to pick a new color that is returned.
     * @param _currentColor the curent light color.
     * @param _title the title of the dialog.
     */
    ::Ogre::ColourValue editColor(const ::Ogre::ColourValue& _currentColor, const std::string& _title);

    /// Contains the name of the light.
    QPointer<QLabel> m_lightNameLabel;

    /// Contains a list of each possible light type.
    QPointer<QComboBox> m_lightTypeBox;

    /// Contains a button to show or hide the visual feedback of the light.
    QPointer<QPushButton> m_visualFeedback;

    /// Contains a button that manage the light diffuse color.
    QPointer<QPushButton> m_diffuseColorBtn;

    /// Contains a button that manage the light specular color.
    QPointer<QPushButton> m_specularColorBtn;

    /// Contains a slider used to edit the theta value of directional lights.
    QPointer<QSlider> m_thetaSlider;

    /// Contains a slider used to edit the phi value of directional lights.
    QPointer<QSlider> m_phiSlider;

    /// Contains a slider used to edit the X translation value of directional lights.
    QPointer<QSlider> m_xTranslation;
    QPointer<QLineEdit> m_xLabel;
    QPointer<QPushButton> m_xReset;

    /// Contains a slider used to edit the Y translation value of directional lights.
    QPointer<QSlider> m_yTranslation;
    QPointer<QLineEdit> m_yLabel;
    QPointer<QPushButton> m_yReset;

    /// Contains a slider used to edit the Z translation value of directional lights.
    QPointer<QSlider> m_zTranslation;
    QPointer<QLineEdit> m_zLabel;
    QPointer<QPushButton> m_zReset;

    /// Contains the current selected light.
    sight::viz::scene3d::ILight::sptr m_currentLight;

private Q_SLOTS:

    /**
     * @brief Opens a color picker and lets the user choose a new diffuse color.
     * @see m_diffuseColorBtn.
     */
    void onEditDiffuseColor(bool);

    /**
     * @brief Opens a color picker and lets the user choose a new specular color.
     * @see m_specularColorBtn.
     */
    void onEditSpecularColor(bool);

    /**
     * @brief Sets the new theta offset value on the light adaptor accurately.
     * @param _value value of the current theta offset.
     * @see m_thetaSlider.
     */
    void onEditThetaOffset(int _value);

    /**
     * @brief the new phi offset value on the light adaptor accurately.
     * @param _value value of the current phi offset.
     * @see m_phiSlider.
     */
    void onEditPhiOffset(int _value);

    /**
     * @brief Sets the new type on the light adaptor accurately.
     * @param _value value of the current type.
     * @see m_lightTypeBox
     */
    void onEditType(const QString& _type);

    /**
     * @brief Toggles the visual feedback of the light.
     * @param _enable value of the pressed button.
     * @see m_visualFeedback
     */
    void onToggleFeedback(bool _enable);

    /**
     * @brief Sets the new position on the light adaptor accurately.
     * @param _value value of the x translation type.
     * @see m_xTranslation
     */
    void onEditXTranslation(int _value);

    /**
     * @brief Sets the new position on the light adaptor accurately.
     * @param _value value of the y translation type.
     * @see m_yTranslation
     */
    void onEditYTranslation(int _value);

    /**
     * @brief Sets the new position on the light adaptor accurately.
     * @param _value value of the z translation type.
     * @see m_zTranslation
     */
    void onEditZTranslation(int _value);

    /**
     * @brief Reset the X translation of the light.
     * @see m_xReset.
     */
    void onResetXTranslation(bool);

    /**
     * @brief Reset the Y translation of the light.
     * @see m_yReset.
     */
    void onResetYTranslation(bool);

    /**
     * @brief Reset the Z translation of the light.
     * @see m_zReset.
     */
    void onResetZTranslation(bool);

};

} // namespace sight::modules::ui::viz
