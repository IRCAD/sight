/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

#include <ui/base/editor/IEditor.hpp>

#include <viz/ogre/ILight.hpp>
#include <viz/ogre/Layer.hpp>

#include <string>
#include <vector>

namespace uiVisuOgre
{

/**
 * @brief User interface to select light adaptor from a selected layer or create a new one.
 *
 * @section Signals Signals
 * - \b lightSelected(viz::ogre::ILight::sptr): Emitted when a light is selected in the list widget.
 *
 * @section Slots Slots
 * - \b initLightList(viz::ogre::Layer::sptr): populates the list of available light adaptors for the current
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="SLightSelectorUid" type="::uiVisuOgre::SLightSelector" />
 */
class UIVISUOGRE_CLASS_API SLightSelector final :
    public QObject,
    public sight::ui::base::editor::IEditor
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLightSelector, sight::ui::base::editor::IEditor)

    /// Initializes signals and slots.
    UIVISUOGRE_API SLightSelector() noexcept;

    /// Destroys the service.
    UIVISUOGRE_API virtual ~SLightSelector() noexcept;

protected:

    /// Configures the service.
    UIVISUOGRE_API void configuring() override;

    /// Sets the connections and the UI elements.
    UIVISUOGRE_API void starting() override;

    /// Does nothing.
    UIVISUOGRE_API void updating() override;

    /// Destroys the connections and cleans the container.
    UIVISUOGRE_API void stopping() override;

private Q_SLOTS:

    /// SLOT: called when a layer is selected.
    /// Sets the current layer and initializes the light adaptors list.
    void onSelectedLayerItem(int _index);

    /// SLOT: called when a light is selected.
    /// Loads the selected light parameters in the light editor.
    void onSelectedLightItem(QListWidgetItem* _item, QListWidgetItem* _previous);

    /// SLOT: called when a light is checked.
    /// Switched on or off the light according to its current state.
    void onCheckedLightItem(QListWidgetItem* _item);

    /// SLOT: called when the add light button is clicked.
    /// Adds a new light to the current scene.
    void onAddLight(bool);

    /// SLOT: called when the remove light button is clicked.
    /// Removes the selected light.
    void onRemoveLight(bool);

    /// SLOT: called when the scene ambient color button is clicked.
    /// Opens a color picker and lets the user choose a new ambient color.
    void onEditAmbientColor(bool);

    /// SLOT: called when the "check all" button is clicked.
    /// Call onCheckAllBoxes(true).
    void onCheckAllCheckBox();

    /// SLOT: called when the "uncheck all" button is clicked.
    /// Call onCheckAllBoxes(false).
    void onUnCheckAllCheckBox();

private:

    typedef core::com::Signal<void (viz::ogre::ILight::sptr)> LightSelectedSignalType;

    /// Checks or unchecks all item in m_lightsList.
    void onCheckAllBoxes(bool visible);

    void initLightList(viz::ogre::Layer::sptr _layer);

    /// Retrieves all the layers from the application thanks to the render services.
    void refreshLayers();

    /// Retrieves light adaptors used in the current layer and stores them in the list widget.
    void updateLightsList();

    /// Creates a new light adaptor.
    void createLightAdaptor(const std::string& _name);

    /// Finds the light adaptor with the specified name.
    viz::ogre::ILight::sptr retrieveLightAdaptor(const std::string& _name) const;

    QPointer<QComboBox>   m_layersBox;

    QPointer<QListWidget> m_lightsList;

    QPointer<QPushButton> m_checkAllButton;

    QPointer<QPushButton> m_unCheckAllButton;

    QPointer<QPushButton> m_addLightBtn;

    QPointer<QPushButton> m_removeLightBtn;

    QPointer<QPushButton> m_ambientColorBtn;

    std::vector< viz::ogre::Layer::wptr > m_layers;
    viz::ogre::Layer::wptr m_currentLayer;

    /// Stores all light adaptors (existing in the configuration and those created by this editor).
    std::vector< viz::ogre::ILight::sptr > m_lightAdaptors;

    /// Stores a light adaptor and it's data to keep a reference on them.
    struct Light
    {
        viz::ogre::ILight::sptr m_light;
        data::Color::sptr m_diffuse;
        data::Color::sptr m_specular;
    };

    /// Stores adaptors managed by this editor.
    std::vector< Light > m_managedLightAdaptors;

    /// Defines the current selected light.
    viz::ogre::ILight::sptr m_currentLight;

    /// Handles connections with the layer.
    core::com::helper::SigSlotConnection m_connections;

};

//------------------------------------------------------------------------------

class NewLightDialog final : public QDialog
{

Q_OBJECT

public:

    NewLightDialog(QWidget* parent = 0);

    ~NewLightDialog();

private Q_SLOTS:

    void onOkBtn(bool _checked);

private:

    QPointer<QLabel> m_lightNameLbl;

    QPointer<QLineEdit> m_lightNameEdit;

    QPointer<QPushButton> m_okBtn;

};

} // namespace uiVisuOgre.
