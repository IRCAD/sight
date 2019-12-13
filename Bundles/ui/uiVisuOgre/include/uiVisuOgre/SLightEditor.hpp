/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "uiVisuOgre/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwRenderOgre/ILight.hpp>

#include <OGRE/OgreColourValue.h>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

namespace uiVisuOgre
{

/**
 * @brief User interface to edit a light adaptor.
 *
 * @section Slots Slots
 * - \b editLight(::fwRenderOgre::ILight::sptr): loads the editor with the parameters from the selected light.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid=SLightEditorUid" type="uiVisuOgre::SLightEditor" />
 */
class UIVISUOGRE_CLASS_API SLightEditor : public QObject,
                                          public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(SLightEditor, ::fwGui::editor::IEditor)

    /// Creates the service.
    UIVISUOGRE_API SLightEditor() noexcept;

    /// Destroys the service.
    UIVISUOGRE_API virtual ~SLightEditor() noexcept;

protected:

    /// Configure the service.
    UIVISUOGRE_API virtual void configuring() override;

    /// Sets the connections and the UI elements.
    UIVISUOGRE_API virtual void starting() override;

    /// Destroys the connections and cleans the container.
    UIVISUOGRE_API virtual void stopping() override;

    /// Does nothing.
    UIVISUOGRE_API virtual void updating() override;

protected Q_SLOTS:

    /// Slot: called when the light diffuse color button is clicked.
    /// Opens a color picker and lets the user choose a new diffuse color.
    void onEditDiffuseColor(bool);

    /// Slot: called when the light specular color button is clicked.
    /// Opens a color picker and lets the user choose a new specular color.
    void onEditSpecularColor(bool);

    /**
     * @brief Called when the theta offset slider value is modified.
     * Sets the new theta offset value on the light adaptor accurately.
     * @param _value Value of the current theta offset.
     */
    void onEditThetaOffset(int _value);

    /**
     * @brief Called when the theta offset slider value is modified.
     * Sets the new phi offset value on the light adaptor accurately.
     * @param _value Value of the current phi offset.
     */
    void onEditPhiOffset(int _value);

    /**
     * @brief Called when the type box value is modified..
     * Sets the new type on the light adaptor accurately.
     * @param _value Value of the current type.
     */
    void onEditType(const QString& _type);

    /**
     * @brief Called when the x slider value is modified..
     * Sets the new position on the light adaptor accurately.
     * @param _value Value of the x translation type.
     */
    void onEditXTranslation(int _value);

    /**
     * @brief Called when the y slider value is modified..
     * Sets the new position on the light adaptor accurately.
     * @param _value Value of the y translation type.
     */
    void onEditYTranslation(int _value);

    /**
     * @brief Called when the z slider value is modified..
     * Sets the new position on the light adaptor accurately.
     * @param _value Value of the z translation type.
     */
    void onEditZTranslation(int _value);

    /// Slot: called when the reset button of the X translation is clicked.
    /// Reset the X translation of the light.
    void onResetXTranslation(bool);

    /// Slot: called when the reset button of the Y translation is clicked.
    /// Reset the Y translation of the light.
    void onResetYTranslation(bool);

    /// Slot: called when the reset button of the Z translation is clicked.
    /// Reset the Z translation of the light.
    void onResetZTranslation(bool);

private:

    /**
     * @brief Get the current light node.
     * @return Ths node where the current light is attached.
     */
    ::Ogre::Node* getLightNode() const;

    /**
     * @brief Sets the current light adaptor to edit.
     * @param _lightAdaptor The light adaptor to edit.
     */
    void editLight(::fwRenderOgre::ILight::sptr _lightAdaptor);

    /**
     * @brief Opens a QColorDialog to pick a new color that is returned.
     * @param _currentColor The curent light color.
     * @param _title The title of the dialog.
     */
    ::Ogre::ColourValue editColor(const ::Ogre::ColourValue& _currentColor, const std::string& _title);

    /// Name of the light.
    QPointer<QLabel> m_lightNameLbl;

    QPointer<QComboBox> m_lightTypeBox;

    QPointer<QPushButton> m_diffuseColorBtn;
    QPointer<QPushButton> m_specularColorBtn;

    QPointer<QSlider> m_thetaSlider;
    QPointer<QSlider> m_phiSlider;

    QPointer<QSlider> m_xTranslation;
    QPointer<QLineEdit> m_xLabel;
    QPointer<QPushButton> m_xReset;

    QPointer<QSlider> m_yTranslation;
    QPointer<QLineEdit> m_yLabel;
    QPointer<QPushButton> m_yReset;

    QPointer<QSlider> m_zTranslation;
    QPointer<QLineEdit> m_zLabel;
    QPointer<QPushButton> m_zReset;

    ::fwRenderOgre::ILight::sptr m_currentLight;
};

} // namespace uiVisuOgre
