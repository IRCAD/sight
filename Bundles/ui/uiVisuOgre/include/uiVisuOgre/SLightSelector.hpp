/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiVisuOgre/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwRenderOgre/ILight.hpp>
#include <fwRenderOgre/Layer.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

#include <string>
#include <vector>

namespace uiVisuOgre
{

/**
 * @brief User interface to select light adaptor from a selected layer or create a new one.
 *
 * @section Signals Signals
 * - \b lightSelected(::fwRenderOgre::ILight::sptr): Emitted when a light is selected in the list widget.
 *
 * @section Slots Slots
 * - \b initLightList(::fwRenderOgre::Layer::sptr): populates the list of available light adaptors for the current
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="SLightSelectorUid" type="::uiVisuOgre::SLightSelector" />
 */
class UIVISUOGRE_CLASS_API SLightSelector : public QObject,
                                            public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SLightSelector)(::fwGui::editor::IEditor) )

    /**
     * @name Signals API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Signals::SignalKeyType s_LIGHT_SELECTED_SIG;
    typedef ::fwCom::Signal<void (::fwRenderOgre::ILight::sptr)> LightSelectedSignalType;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_INIT_LIGHT_LIST_SLOT;
    /** @} */

    /// Initializes signals and slots.
    UIVISUOGRE_API SLightSelector() noexcept;
    UIVISUOGRE_API virtual ~SLightSelector() noexcept;

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

    /// Slot: called when a layer is selected.
    /// Sets the current layer and initializes the light adaptors list.
    void onSelectedLayerItem(int _index);

    /// Slot: called when a light is selected.
    /// Loads the selected light parameters in the light editor.
    void onSelectedLightItem(QListWidgetItem* _item, QListWidgetItem* _previous);

    /// Slot: called when a light is checked.
    /// Switched on or off the light according to its current state.
    void onCheckedLightItem(QListWidgetItem* _item);

    /// Slot: called when the add light button is clicked.
    /// Adds a new light to the current scene.
    void onAddLight(bool _checked);

    /// Slot: called when the remove light button is clicked.
    /// Removes the selected light.
    void onRemoveLight(bool _checked);

    /// Slot: called when the scene ambient color button is clicked.
    /// Opens a color picker and lets the user choose a new ambient color.
    void onEditAmbientColor(bool _checked);

    /// Slot: called when the "check all" button is clicked.
    /// Call onCheckAllBoxes(true).
    void onCheckAllCheckBox();

    /// Slot: called when the "uncheck all" button is clicked.
    /// Call onCheckAllBoxes(false).
    void onUnCheckAllCheckBox();

private:

    /// check or uncheck all item in m_lightsList
    void onCheckAllBoxes(bool visible);

    void initLightList(::fwRenderOgre::Layer::sptr _layer);

    /// Retrieves all the layers from the application thanks to the render services.
    void refreshLayers();

    /// Retrieves light adaptors used in the current layer and stores them in the list widget.
    void updateLightsList();

    /// Creates a new light adaptor.
    void createLightAdaptor(const std::string& _name);

    /// Finds the light adaptor with the specified name.
    ::fwRenderOgre::ILight::sptr retrieveLightAdaptor(const std::string& _name) const;

    QPointer<QComboBox>   m_layersBox;
    QPointer<QListWidget> m_lightsList;
    QPointer<QPushButton> m_checkAllButton;
    QPointer<QPushButton> m_unCheckAllButton;

    QPointer<QPushButton> m_addLightBtn;
    QPointer<QPushButton> m_removeLightBtn;
    QPointer<QPushButton> m_ambientColorBtn;

    std::vector< ::fwRenderOgre::Layer::wptr > m_layers;
    ::fwRenderOgre::Layer::wptr m_currentLayer;

    /// List of all light adaptors (existing in the configuration and those created by this editor)
    std::vector< ::fwRenderOgre::ILight::sptr > m_lightAdaptors;

    /// List of adaptors managed by this editor
    std::vector< ::fwRenderOgre::ILight::sptr > m_managedLightAdaptors;

    /// Currently selected light
    ::fwRenderOgre::ILight::sptr m_currentLight;

    /// Connection service, needed for slot/signal association
    ::fwCom::helper::SigSlotConnection m_connections;
};

//------------------------------------------------------------------------------

class NewLightDialog : public QDialog
{
Q_OBJECT

public:

    NewLightDialog(QWidget* parent = 0);
    ~NewLightDialog();

protected Q_SLOTS:
    void onOkBtn(bool _checked);

private:

    QPointer<QLabel> m_lightNameLbl;
    QPointer<QLineEdit> m_lightNameEdit;
    QPointer<QPushButton> m_okBtn;
};

} // namespace uiVisuOgre
