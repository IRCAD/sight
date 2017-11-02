/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SLIGHTEDITOR_HPP__
#define __UIVISUOGRE_SLIGHTEDITOR_HPP__

#include "uiVisuOgre/config.hpp"

#include <fwRenderOgre/ILight.hpp>

#include <gui/editor/IEditor.hpp>

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
                                          public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SLightEditor)(::gui::editor::IEditor) );

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

#endif // __UIVISUOGRE_SLIGHTEDITOR_HPP__
