/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "modules/ui/qt/reconstruction/representation_editor.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/path.hpp>

#include <data/mesh.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/qt/container/widget.hpp>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

namespace sight::module::ui::qt::reconstruction
{

//------------------------------------------------------------------------------

representation_editor::representation_editor() noexcept =
    default;

//------------------------------------------------------------------------------

representation_editor::~representation_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void representation_editor::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(base_id());

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    auto* layout = new QVBoxLayout();

    auto* group_box = new QGroupBox(tr("Representation"));
    group_box->setObjectName(service_id + "/" + group_box->title());
    auto* layout_group_box = new QVBoxLayout();
    group_box->setLayout(layout_group_box);

    m_button_group = new QButtonGroup(group_box);
    m_button_group->setObjectName(service_id + "/buttonGroup");

    auto* button_surface = new QRadioButton(tr("Surface"), group_box);
    button_surface->setObjectName(service_id + "/" + button_surface->text());
    button_surface->setMinimumSize(button_surface->sizeHint());
    m_button_group->addButton(button_surface, 0);
    layout_group_box->addWidget(button_surface);
    button_surface->setChecked(true);

    auto* button_point = new QRadioButton(tr("Point"), group_box);
    button_point->setObjectName(service_id + "/" + button_point->text());
    button_point->setMinimumSize(button_point->sizeHint());
    m_button_group->addButton(button_point, 1);
    layout_group_box->addWidget(button_point);

    auto* button_wireframe = new QRadioButton(tr("Wireframe"), group_box);
    button_wireframe->setObjectName(service_id + "/" + button_wireframe->text());
    button_wireframe->setMinimumSize(button_wireframe->sizeHint());
    m_button_group->addButton(button_wireframe, 2);
    layout_group_box->addWidget(button_wireframe);

    auto* button_edge = new QRadioButton(tr("Edge"), group_box);
    button_edge->setObjectName(service_id + "/" + button_edge->text());
    button_edge->setMinimumSize(button_edge->sizeHint());
    m_button_group->addButton(button_edge, 3);
    layout_group_box->addWidget(button_edge);

    // shading group box
    auto* group_box_shading = new QGroupBox(tr("shading"));
    group_box_shading->setObjectName(service_id + "/" + group_box_shading->title());
    auto* layout_group_box_shading = new QVBoxLayout();
    group_box_shading->setLayout(layout_group_box_shading);
    m_button_group_shading = new QButtonGroup(group_box_shading);
    m_button_group_shading->setObjectName(service_id + "/buttonGroupShading");

    auto* button_ambient = new QRadioButton(tr("Ambient"), group_box_shading);
    button_ambient->setObjectName(service_id + "/" + button_ambient->text());
    button_ambient->setMinimumSize(button_ambient->sizeHint());
    m_button_group_shading->addButton(button_ambient, 0);
    layout_group_box_shading->addWidget(button_ambient);
    button_ambient->setChecked(true);

    auto* button_flat = new QRadioButton(tr("Flat"), group_box_shading);
    button_flat->setObjectName(service_id + "/" + button_flat->text());
    button_flat->setMinimumSize(button_flat->sizeHint());
    m_button_group_shading->addButton(button_flat, 1);
    layout_group_box_shading->addWidget(button_flat);
    button_flat->setChecked(true);

    auto* button_phong = new QRadioButton(tr("Phong"), group_box_shading);
    button_phong->setObjectName(service_id + "/" + button_phong->text());
    button_phong->setMinimumSize(button_phong->sizeHint());
    m_button_group_shading->addButton(button_phong, 2);
    layout_group_box_shading->addWidget(button_phong);

    layout->addWidget(group_box);
    layout->addWidget(group_box_shading);

    auto* group_box_normals = new QGroupBox(tr("Normals"));
    group_box_normals->setObjectName(service_id + "/" + group_box_normals->title());
    auto* layout_group_box_normals = new QVBoxLayout(group_box_normals);
    m_options_radio_box = new QButtonGroup();
    m_options_radio_box->setObjectName(service_id + "/normalsRadioBox");
    auto* point_normals_button = new QRadioButton(tr("Show point normals"));
    point_normals_button->setObjectName(service_id + "/" + point_normals_button->text());
    auto* cell_normals_button = new QRadioButton(tr("Show cell normals"));
    cell_normals_button->setObjectName(service_id + "/" + cell_normals_button->text());
    auto* selected_button = new QRadioButton(tr("Selected"));
    selected_button->setObjectName(service_id + "/" + selected_button->text());
    auto* hide_normals_button = new QRadioButton(tr("Hide normals"));
    hide_normals_button->setObjectName(service_id + "/" + hide_normals_button->text());

    m_options_radio_box->addButton(point_normals_button, 1);
    m_options_radio_box->addButton(cell_normals_button, 2);
    m_options_radio_box->addButton(selected_button, 3);
    m_options_radio_box->addButton(hide_normals_button, 0);

    layout_group_box_normals->addWidget(point_normals_button);
    layout_group_box_normals->addWidget(cell_normals_button);
    layout_group_box_normals->addWidget(selected_button);
    layout_group_box_normals->addWidget(hide_normals_button);

    layout->addWidget(group_box_normals);

    QObject::connect(m_options_radio_box, &QButtonGroup::idClicked, this, &self_t::on_show_options);

    qt_container->set_layout(layout);
    qt_container->set_enabled(false);

    QObject::connect(m_button_group, &QButtonGroup::idClicked, this, &self_t::on_change_representation);
    QObject::connect(m_button_group_shading, &QButtonGroup::idClicked, this, &self_t::on_change_shading);

    this->updating();
}

//------------------------------------------------------------------------------

void representation_editor::stopping()
{
    QObject::disconnect(m_button_group, &QButtonGroup::idClicked, this, &self_t::on_change_representation);
    QObject::disconnect(m_button_group_shading, &QButtonGroup::idClicked, this, &self_t::on_change_shading);

    QObject::disconnect(m_options_radio_box, &QButtonGroup::idClicked, this, &self_t::on_show_options);

    this->destroy();
}

//------------------------------------------------------------------------------

void representation_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void representation_editor::updating()
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    QWidget* const container = qt_container->get_qt_container();
    SIGHT_ASSERT("container not instanced", container);

    SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();
    m_material = reconstruction->get_material();
    container->setEnabled(!reconstruction->get_organ_name().empty());

    this->refresh_representation();
    this->refresh_options();
    this->refresh_shading();
}

//------------------------------------------------------------------------------

void representation_editor::on_change_representation(int _id)
{
    data::material::representation_t selected_mode = data::material::surface;

    switch(_id)
    {
        case 1:
            selected_mode = data::material::point;
            break;

        case 2:
            selected_mode = data::material::wireframe;
            break;

        case 3:
            selected_mode = data::material::edge;
            break;

        default: // 0 or other
            selected_mode = data::material::surface;
    }

    m_material->set_representation_mode(selected_mode);
    this->notify_material();
}

//------------------------------------------------------------------------------

void representation_editor::on_change_shading(int _id)
{
    data::material::shading_t selected_mode = data::material::shading_t::phong;

    switch(_id)
    {
        case 0:
            selected_mode = data::material::shading_t::ambient;
            break;

        case 1:
            selected_mode = data::material::shading_t::flat;
            break;

        default: // 2 or other
            selected_mode = data::material::shading_t::phong;
    }

    m_material->set_shading_mode(selected_mode);
    this->notify_material();
}

//------------------------------------------------------------------------------

void representation_editor::refresh_representation()
{
    int representation_mode = m_material->get_representation_mode();
    QAbstractButton* button = nullptr;

    switch(representation_mode)
    {
        case data::material::surface:
            button = m_button_group->button(0);
            button->setChecked(true);
            break;

        case data::material::point:
            button = m_button_group->button(1);
            button->setChecked(true);
            break;

        case data::material::wireframe:
            button = m_button_group->button(2);
            button->setChecked(true);
            break;

        case data::material::edge:
            button = m_button_group->button(3);
            button->setChecked(true);
            break;

        default:
            button = m_button_group->button(0);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void representation_editor::refresh_shading()
{
    auto shading_mode       = static_cast<data::material::shading_t>(m_material->get_shading_mode());
    QAbstractButton* button = nullptr;

    switch(shading_mode)
    {
        case data::material::shading_t::ambient:
            button = m_button_group_shading->button(0);
            button->setChecked(true);
            break;

        case data::material::shading_t::flat:
            button = m_button_group_shading->button(1);
            button->setChecked(true);
            break;

        case data::material::shading_t::phong:
            button = m_button_group_shading->button(2);
            button->setChecked(true);
            break;

        default:
            button = m_button_group_shading->button(2);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void representation_editor::refresh_options()
{
    QAbstractButton* button_hide = m_options_radio_box->button(0);
    button_hide->setChecked(m_material->get_options_mode() == data::material::standard);
    QAbstractButton* button_normals = m_options_radio_box->button(1);
    button_normals->setChecked(m_material->get_options_mode() == data::material::normals);
    QAbstractButton* selected = m_options_radio_box->button(2);
    selected->setChecked(m_material->get_options_mode() == data::material::selected);
}

//------------------------------------------------------------------------------

void representation_editor::on_show_options(int _state)
{
    switch(_state)
    {
        case 0:
            m_material->set_options_mode(data::material::standard);
            break;

        case 1:
            m_material->set_options_mode(data::material::normals);
            break;

        case 2:
            m_material->set_options_mode(data::material::cells_normals);
            break;

        case 3:
            m_material->set_options_mode(data::material::selected);
            break;

        default:
            SIGHT_ASSERT("Invalid state: " << _state, false);
    }

    this->notify_material();

    SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    // In VTK backend the normals is handled by the mesh and not by the material
    auto sig = reconstruction->signal<data::reconstruction::mesh_changed_signal_t>(
        data::reconstruction::MESH_CHANGED_SIG
    );
    sig->async_emit(reconstruction->get_mesh());
}

//------------------------------------------------------------------------------

void representation_editor::notify_material()
{
    SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    data::object::modified_signal_t::sptr sig;
    sig = reconstruction->get_material()->signal<data::object::modified_signal_t>(
        data::object::MODIFIED_SIG
    );
    sig->async_emit();
}

//------------------------------------------------------------------------------

service::connections_t representation_editor::auto_connections() const
{
    connections_t connections;
    connections.push(RECONSTRUCTION, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::reconstruction
