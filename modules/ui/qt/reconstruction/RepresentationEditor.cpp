/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/qt/reconstruction/RepresentationEditor.hpp"

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

RepresentationEditor::RepresentationEditor() noexcept =
    default;

//------------------------------------------------------------------------------

RepresentationEditor::~RepresentationEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void RepresentationEditor::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    qt_container->getQtContainer()->setObjectName(service_id);

    auto* layout = new QVBoxLayout();

    auto* group_box = new QGroupBox(tr("Representation"));
    group_box->setObjectName(service_id + "/" + group_box->title());
    auto* layout_group_box = new QVBoxLayout();
    group_box->setLayout(layout_group_box);

    m_buttonGroup = new QButtonGroup(group_box);
    m_buttonGroup->setObjectName(service_id + "/buttonGroup");

    auto* button_surface = new QRadioButton(tr("Surface"), group_box);
    button_surface->setObjectName(service_id + "/" + button_surface->text());
    button_surface->setMinimumSize(button_surface->sizeHint());
    m_buttonGroup->addButton(button_surface, 0);
    layout_group_box->addWidget(button_surface);
    button_surface->setChecked(true);

    auto* button_point = new QRadioButton(tr("Point"), group_box);
    button_point->setObjectName(service_id + "/" + button_point->text());
    button_point->setMinimumSize(button_point->sizeHint());
    m_buttonGroup->addButton(button_point, 1);
    layout_group_box->addWidget(button_point);

    auto* button_wireframe = new QRadioButton(tr("Wireframe"), group_box);
    button_wireframe->setObjectName(service_id + "/" + button_wireframe->text());
    button_wireframe->setMinimumSize(button_wireframe->sizeHint());
    m_buttonGroup->addButton(button_wireframe, 2);
    layout_group_box->addWidget(button_wireframe);

    auto* button_edge = new QRadioButton(tr("Edge"), group_box);
    button_edge->setObjectName(service_id + "/" + button_edge->text());
    button_edge->setMinimumSize(button_edge->sizeHint());
    m_buttonGroup->addButton(button_edge, 3);
    layout_group_box->addWidget(button_edge);

    // shading group box
    auto* group_box_shading = new QGroupBox(tr("shading"));
    group_box_shading->setObjectName(service_id + "/" + group_box_shading->title());
    auto* layout_group_box_shading = new QVBoxLayout();
    group_box_shading->setLayout(layout_group_box_shading);
    m_buttonGroupShading = new QButtonGroup(group_box_shading);
    m_buttonGroupShading->setObjectName(service_id + "/buttonGroupShading");

    auto* button_ambient = new QRadioButton(tr("Ambient"), group_box_shading);
    button_ambient->setObjectName(service_id + "/" + button_ambient->text());
    button_ambient->setMinimumSize(button_ambient->sizeHint());
    m_buttonGroupShading->addButton(button_ambient, 0);
    layout_group_box_shading->addWidget(button_ambient);
    button_ambient->setChecked(true);

    auto* button_flat = new QRadioButton(tr("Flat"), group_box_shading);
    button_flat->setObjectName(service_id + "/" + button_flat->text());
    button_flat->setMinimumSize(button_flat->sizeHint());
    m_buttonGroupShading->addButton(button_flat, 1);
    layout_group_box_shading->addWidget(button_flat);
    button_flat->setChecked(true);

    auto* button_phong = new QRadioButton(tr("Phong"), group_box_shading);
    button_phong->setObjectName(service_id + "/" + button_phong->text());
    button_phong->setMinimumSize(button_phong->sizeHint());
    m_buttonGroupShading->addButton(button_phong, 2);
    layout_group_box_shading->addWidget(button_phong);

    layout->addWidget(group_box);
    layout->addWidget(group_box_shading);

    auto* group_box_normals = new QGroupBox(tr("Normals"));
    group_box_normals->setObjectName(service_id + "/" + group_box_normals->title());
    auto* layout_group_box_normals = new QVBoxLayout(group_box_normals);
    m_normalsRadioBox = new QButtonGroup();
    m_normalsRadioBox->setObjectName(service_id + "/normalsRadioBox");
    auto* point_normals_button = new QRadioButton(tr("Show point normals"));
    point_normals_button->setObjectName(service_id + "/" + point_normals_button->text());
    auto* cell_normals_button = new QRadioButton(tr("Show cell normals"));
    cell_normals_button->setObjectName(service_id + "/" + cell_normals_button->text());
    auto* hide_normals_button = new QRadioButton(tr("Hide normals"));
    hide_normals_button->setObjectName(service_id + "/" + hide_normals_button->text());

    m_normalsRadioBox->addButton(point_normals_button, 1);
    m_normalsRadioBox->addButton(cell_normals_button, 2);
    m_normalsRadioBox->addButton(hide_normals_button, 0);

    layout_group_box_normals->addWidget(point_normals_button);
    layout_group_box_normals->addWidget(cell_normals_button);
    layout_group_box_normals->addWidget(hide_normals_button);

    layout->addWidget(group_box_normals);

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

    qt_container->setLayout(layout);
    qt_container->setEnabled(false);

    QObject::connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onChangeRepresentation(int)));
    QObject::connect(m_buttonGroupShading, SIGNAL(buttonClicked(int)), this, SLOT(onChangeShading(int)));

    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::stopping()
{
    QObject::disconnect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onChangeRepresentation(int)));
    QObject::disconnect(m_buttonGroupShading, SIGNAL(buttonClicked(int)), this, SLOT(onChangeShading(int)));

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void RepresentationEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating()
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qt_container->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();
    m_material = reconstruction->getMaterial();
    container->setEnabled(!reconstruction->getOrganName().empty());

    this->refreshRepresentation();
    this->refreshNormals();
    this->refreshShading();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeRepresentation(int _id)
{
    data::material::representation_t selected_mode = data::material::SURFACE;

    switch(_id)
    {
        case 1:
            selected_mode = data::material::POINT;
            break;

        case 2:
            selected_mode = data::material::WIREFRAME;
            break;

        case 3:
            selected_mode = data::material::EDGE;
            break;

        default: // 0 or other
            selected_mode = data::material::SURFACE;
    }

    m_material->setRepresentationMode(selected_mode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeShading(int _id)
{
    data::material::shading_t selected_mode = data::material::PHONG;

    switch(_id)
    {
        case 0:
            selected_mode = data::material::AMBIENT;
            break;

        case 1:
            selected_mode = data::material::FLAT;
            break;

        default: // 2 or other
            selected_mode = data::material::PHONG;
    }

    m_material->setShadingMode(selected_mode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshRepresentation()
{
    int representation_mode = m_material->getRepresentationMode();
    QAbstractButton* button = nullptr;

    switch(representation_mode)
    {
        case data::material::SURFACE:
            button = m_buttonGroup->button(0);
            button->setChecked(true);
            break;

        case data::material::POINT:
            button = m_buttonGroup->button(1);
            button->setChecked(true);
            break;

        case data::material::WIREFRAME:
            button = m_buttonGroup->button(2);
            button->setChecked(true);
            break;

        case data::material::EDGE:
            button = m_buttonGroup->button(3);
            button->setChecked(true);
            break;

        default:
            button = m_buttonGroup->button(0);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshShading()
{
    int shading_mode        = m_material->getShadingMode();
    QAbstractButton* button = nullptr;

    switch(shading_mode)
    {
        case data::material::AMBIENT:
            button = m_buttonGroupShading->button(0);
            button->setChecked(true);
            break;

        case data::material::FLAT:
            button = m_buttonGroupShading->button(1);
            button->setChecked(true);
            break;

        case data::material::PHONG:
            button = m_buttonGroupShading->button(2);
            button->setChecked(true);
            break;

        default:
            button = m_buttonGroupShading->button(2);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshNormals()
{
    QAbstractButton* button_hide = m_normalsRadioBox->button(0);
    button_hide->setChecked(m_material->getOptionsMode() == data::material::STANDARD);
    QAbstractButton* button_normals = m_normalsRadioBox->button(1);
    button_normals->setChecked(m_material->getOptionsMode() == data::material::NORMALS);
}

//------------------------------------------------------------------------------

void RepresentationEditor::onShowNormals(int _state)
{
    switch(_state)
    {
        case 0:
            m_material->setOptionsMode(data::material::STANDARD);
            break;

        case 1:
            m_material->setOptionsMode(data::material::NORMALS);
            break;

        case 2:
            m_material->setOptionsMode(data::material::CELLS_NORMALS);
            break;

        default:
            SIGHT_ASSERT("Invalid state: " << _state, false);
    }

    this->notifyMaterial();

    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    // In VTK backend the normals is handled by the mesh and not by the material
    auto sig = reconstruction->signal<data::reconstruction::mesh_changed_signal_t>(
        data::reconstruction::MESH_CHANGED_SIG
    );
    sig->async_emit(reconstruction->getMesh());
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMaterial()
{
    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    data::object::modified_signal_t::sptr sig;
    sig = reconstruction->getMaterial()->signal<data::object::modified_signal_t>(
        data::object::MODIFIED_SIG
    );
    sig->async_emit();
}

//------------------------------------------------------------------------------

service::connections_t RepresentationEditor::auto_connections() const
{
    connections_t connections;
    connections.push(s_RECONSTRUCTION, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::reconstruction
