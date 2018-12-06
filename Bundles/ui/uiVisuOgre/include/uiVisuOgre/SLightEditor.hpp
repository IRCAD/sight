/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
#include <QPointer>
#include <QPushButton>
#include <QSlider>

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

    fwCoreServiceClassDefinitionsMacro( (SLightEditor)(::fwGui::editor::IEditor) )

    /**
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_EDIT_LIGHT_SLOT;
    /** @} */

    UIVISUOGRE_API SLightEditor() noexcept;
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
    void onEditDiffuseColor(bool _checked);

    /// Slot: called when the light specular color button is clicked.
    /// Opens a color picker and lets the user choose a new specular color.
    void onEditSpecularColor(bool _checked);

    /// Slot: called when the theta offset slider value is modified.
    /// Sets the new theta offset value on the light adaptor accurately.
    void onEditThetaOffset(int _value);

    /// Slot: called when the theta offset slider value is modified.
    /// Sets the new phi offset value on the light adaptor accurately.
    void onEditPhiOffset(int _value);

private:

    void editLight(::fwRenderOgre::ILight::sptr _lightAdaptor);

    /// Opens a QColorDialog to pick a new color that is returned.
    ::Ogre::ColourValue editColor(const ::Ogre::ColourValue& _currentColor, const std::string& _title);

    QPointer<QLabel> m_lightNameLbl;

    QPointer<QComboBox> m_lightTypeBox;

    QPointer<QPushButton> m_diffuseColorBtn;
    QPointer<QPushButton> m_specularColorBtn;

    QPointer<QSlider> m_thetaSlider;
    QPointer<QSlider> m_phiSlider;

    ::fwRenderOgre::ILight::sptr m_currentLight;
};

} // namespace uiVisuOgre
