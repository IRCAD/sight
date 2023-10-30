/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/ui/viz/config.hpp"

#include <ui/__/editor.hpp>

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/light_adaptor.hpp>

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

namespace sight::module::ui::viz
{

/**
 * @brief User interface to select light adaptor from a selected layer or create a new one.
 *
 * @section Signals Signals
 * - \b lightSelected(viz::scene3d::light_adaptor::sptr): Emitted when a light is selected in the list widget.
 *
 * @section Slots Slots
 * - \b initLightList(viz::scene3d::layer::sptr): populates the list of available light adaptors for the current
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="light_selectorUid" type="sight::module::ui::viz::light_selector" />
 */
class MODULE_UI_VIZ_CLASS_API light_selector final :
    public QObject,
    public sight::ui::editor
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(light_selector, sight::ui::editor);

    /// Initializes signals and slots.
    MODULE_UI_VIZ_API light_selector() noexcept;

    /// Destroys the service.
    MODULE_UI_VIZ_API ~light_selector() noexcept override;

protected:

    /// Configures the service.
    MODULE_UI_VIZ_API void configuring() override;

    /// Sets the connections and the UI elements.
    MODULE_UI_VIZ_API void starting() override;

    /// Does nothing.
    MODULE_UI_VIZ_API void updating() override;

    /// Destroys the connections and cleans the container.
    MODULE_UI_VIZ_API void stopping() override;

private Q_SLOTS:

    /// SLOT: called when a layer is selected.
    /// Sets the current layer and initializes the light adaptors list.
    void on_selected_layer_item(int _index);

    /// SLOT: called when a light is selected.
    /// Loads the selected light parameters in the light editor.
    void on_selected_light_item(QListWidgetItem* _item, QListWidgetItem* _previous);

    /// SLOT: called when a light is checked.
    /// Switched on or off the light according to its current state.
    void on_checked_light_item(QListWidgetItem* _item);

    /// SLOT: called when the add light button is clicked.
    /// Adds a new light to the current scene.
    void on_add_light(bool /*unused*/);

    /// SLOT: called when the remove light button is clicked.
    /// Removes the selected light.
    void on_remove_light(bool /*unused*/);

    /// SLOT: called when the scene ambient color button is clicked.
    /// Opens a color picker and lets the user choose a new ambient color.
    void on_edit_ambient_color(bool /*unused*/);

    /// SLOT: called when the "check all" button is clicked.
    /// Call onCheckAllBoxes(true).
    void on_check_all_check_box();

    /// SLOT: called when the "uncheck all" button is clicked.
    /// Call onCheckAllBoxes(false).
    void on_un_check_all_check_box();

private:

    using light_selected_signal_t = core::com::signal<void (sight::viz::scene3d::light_adaptor::sptr)>;

    /// Checks or unchecks all item in m_lightsList.
    void on_check_all_boxes(bool _visible);

    void init_light_list(sight::viz::scene3d::layer::sptr _layer);

    /// Retrieves all the layers from the application thanks to the render services.
    void refresh_layers();

    /// Retrieves light adaptors used in the current layer and stores them in the list widget.
    void update_lights_list();

    /// Creates a new light adaptor.
    void create_light_adaptor(const std::string& _name);

    /// Finds the light adaptor with the specified name.
    sight::viz::scene3d::light_adaptor::sptr retrieve_light_adaptor(const std::string& _name) const;

    QPointer<QComboBox> m_layers_box;

    QPointer<QListWidget> m_lights_list;

    QPointer<QPushButton> m_check_all_button;

    QPointer<QPushButton> m_un_check_all_button;

    QPointer<QPushButton> m_add_light_btn;

    QPointer<QPushButton> m_remove_light_btn;

    QPointer<QPushButton> m_ambient_color_btn;

    std::vector<sight::viz::scene3d::layer::wptr> m_layers;
    sight::viz::scene3d::layer::wptr m_current_layer;

    /// Stores all light adaptors (existing in the configuration and those created by this editor).
    std::vector<sight::viz::scene3d::light_adaptor::sptr> m_light_adaptors;

    /// Stores a light adaptor and it's data to keep a reference on them.
    struct light
    {
        sight::viz::scene3d::light_adaptor::sptr m_light;
        data::color::sptr m_diffuse;
        data::color::sptr m_specular;
    };

    /// Stores adaptors managed by this editor.
    std::vector<light> m_managed_light_adaptors;

    /// Defines the current selected light.
    sight::viz::scene3d::light_adaptor::sptr m_current_light;

    /// Handles connections with the layer.
    core::com::helper::sig_slot_connection m_connections;
};

//------------------------------------------------------------------------------

class new_light_dialog final : public QDialog
{
Q_OBJECT

public:

    new_light_dialog(QWidget* _parent = nullptr);

    ~new_light_dialog() override;

private Q_SLOTS:

    void on_ok_btn(bool _checked);

private:

    QPointer<QLabel> m_light_name_lbl;

    QPointer<QLineEdit> m_light_name_edit;

    QPointer<QPushButton> m_ok_btn;
};

} // namespace sight::module::ui::viz.
