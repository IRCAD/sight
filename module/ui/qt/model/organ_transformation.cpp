/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "organ_transformation.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/id.hpp>

#include <data/helper/field.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>

#include <map>

namespace sight::module::ui::qt::model
{

static const std::string MATRIX_FIELD_NAME = "TransformMatrix";

organ_transformation::organ_transformation() noexcept :
    m_save_button(nullptr),
    m_load_button(nullptr),
    m_reset_button(nullptr),
    m_reconstruction_list_box(nullptr)
{
}

//------------------------------------------------------------------------------

organ_transformation::~organ_transformation() noexcept =
    default;

//------------------------------------------------------------------------------

void organ_transformation::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void organ_transformation::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* layout = new QVBoxLayout();

    auto* group_box = new QGroupBox(tr("Organs"));
    layout->addWidget(group_box);

    auto* layout_group_box = new QVBoxLayout();
    group_box->setLayout(layout_group_box);

    m_select_all_check_box     = new QCheckBox(tr("Select All"));
    m_reconstruction_list_box  = new QListWidget(group_box);
    m_reset_button             = new QPushButton(tr("Reset"));
    m_save_button              = new QPushButton(tr("Save"));
    m_load_button              = new QPushButton(tr("Load"));
    m_save_selection_combo_box = new QComboBox();

    QObject::connect(m_reconstruction_list_box, &QListWidget::itemClicked, this, &self_t::on_reconstruction_check);
    QObject::connect(m_reset_button, &QPushButton::clicked, this, &self_t::on_reset_click);
    QObject::connect(m_save_button, &QPushButton::clicked, this, &self_t::on_save_click);
    QObject::connect(m_load_button, &QPushButton::clicked, this, &self_t::on_load_click);
    QObject::connect(m_select_all_check_box, &QCheckBox::stateChanged, this, &self_t::on_select_all_changed);

    layout_group_box->addWidget(m_select_all_check_box, 0);
    layout_group_box->addWidget(m_reconstruction_list_box, 1);
    layout_group_box->addWidget(m_reset_button, 0);
    layout_group_box->addWidget(m_save_button, 0);
    layout_group_box->addWidget(m_save_selection_combo_box, 0);
    layout_group_box->addWidget(m_load_button, 0);

    qt_container->set_layout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

void organ_transformation::stopping()
{
    QObject::disconnect(m_reconstruction_list_box, &QListWidget::itemClicked, this, &self_t::on_reconstruction_check);
    QObject::disconnect(m_reset_button, &QPushButton::clicked, this, &self_t::on_reset_click);
    QObject::disconnect(m_save_button, &QPushButton::clicked, this, &self_t::on_save_click);
    QObject::disconnect(m_load_button, &QPushButton::clicked, this, &self_t::on_load_click);
    QObject::disconnect(m_select_all_check_box, &QCheckBox::stateChanged, this, &self_t::on_select_all_changed);

    this->destroy();
}

//------------------------------------------------------------------------------

void organ_transformation::updating()
{
    this->add_mesh_transform();
    this->refresh();
}

//------------------------------------------------------------------------------

void organ_transformation::info(std::ostream& /*sstream*/)
{
}

//------------------------------------------------------------------------------

void organ_transformation::refresh()
{
    m_reconstruction_map.clear();
    m_reconstruction_list_box->clear();

    const auto series = m_model_series.lock();

    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QWidget* const container = qt_container->get_qt_container();
    SIGHT_ASSERT("container not instanced", container);

    bool has_reconstructions = !series->get_reconstruction_db().empty();
    container->setEnabled(has_reconstructions);

    if(has_reconstructions)
    {
        const auto p_map = m_map.lock();

        for(const data::reconstruction::sptr& rec : series->get_reconstruction_db())
        {
            m_reconstruction_map[rec->get_organ_name()] = rec;
        }

        for(auto& it : m_reconstruction_map)
        {
            std::string organ_name = it.first;
            auto* item             = new QListWidgetItem(QString::fromStdString(organ_name), m_reconstruction_list_box);
            if(p_map && p_map->find(organ_name) != p_map->end())
            {
                item->setCheckState(Qt::Checked);
            }
            else
            {
                item->setCheckState(Qt::Unchecked);
            }

            m_reconstruction_list_box->addItem(item);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::notify_transformation_matrix(data::matrix4::sptr _a_trans_mat)
{
    auto sig = _a_trans_mat->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void organ_transformation::on_reconstruction_check(QListWidgetItem* _current_item)
{
    const auto p_map = m_map.lock();
    if(p_map)
    {
        std::string item_name                       = _current_item->text().toStdString();
        data::reconstruction::sptr p_reconstruction = m_reconstruction_map[item_name];
        data::mesh::sptr p_mesh                     = p_reconstruction->get_mesh();

        const auto scoped_emitter = p_map->scoped_emit();
        if((_current_item->checkState()) == Qt::Checked)
        {
            p_map->insert_or_assign(item_name, p_mesh);
        }
        else
        {
            p_map->erase(item_name);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::on_reset_click()
{
    const auto series = m_model_series.lock();

    //search the corresponding triangular mesh
    for(const data::reconstruction::sptr& rec : series->get_reconstruction_db())
    {
        data::mesh::sptr p_tmp_tr_mesh = rec->get_mesh();

        data::matrix4::sptr p_tmp_mat =
            p_tmp_tr_mesh->get_field<data::matrix4>(MATRIX_FIELD_NAME);
        if(p_tmp_mat)
        {
            geometry::data::identity(*p_tmp_mat);
            notify_transformation_matrix(p_tmp_mat);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::on_save_click()
{
    inner_mat_mapping_t mat_map;

    const auto series = m_model_series.lock();

    if(!series->get_reconstruction_db().empty())
    {
        for(const data::reconstruction::sptr& rec : series->get_reconstruction_db())
        {
            data::mesh::sptr p_tmp_tr_mesh = rec->get_mesh();
            data::matrix4::sptr p_tmp_mat  =
                p_tmp_tr_mesh->get_field<data::matrix4>(MATRIX_FIELD_NAME);
            if(p_tmp_mat)
            {
                data::matrix4::sptr p_cpy_tmp_mat;
                p_cpy_tmp_mat                    = data::object::copy(p_tmp_mat);
                mat_map[p_tmp_tr_mesh->get_id()] = p_cpy_tmp_mat;
            }
        }

        std::stringstream tmp_save_name;
        tmp_save_name << "save_" << m_save_count;
        m_save_listing[tmp_save_name.str()] = mat_map;
        m_save_selection_combo_box->addItem(QString::fromStdString(tmp_save_name.str()));
        m_save_count++;
    }
}

//------------------------------------------------------------------------------

void organ_transformation::on_load_click()
{
    if(m_save_selection_combo_box->count() != 0)
    {
        inner_mat_mapping_t mat_map = m_save_listing[m_save_selection_combo_box->currentText().toStdString()];

        const auto series = m_model_series.lock();

        //search the corresponding triangular mesh
        for(const data::reconstruction::sptr& rec : series->get_reconstruction_db())
        {
            data::mesh::sptr p_tmp_tr_mesh = rec->get_mesh();
            if(mat_map.find(p_tmp_tr_mesh->get_id()) != mat_map.end())
            {
                data::matrix4::sptr p_tmp_mat =
                    p_tmp_tr_mesh->get_field<data::matrix4>(MATRIX_FIELD_NAME);
                if(p_tmp_mat)
                {
                    p_tmp_mat->shallow_copy(mat_map[p_tmp_tr_mesh->get_id()]);
                    notify_transformation_matrix(p_tmp_mat);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::on_select_all_changed(int _state)
{
    const auto p_map          = m_map.lock();
    const auto scoped_emitter = p_map->scoped_emit();

    if(_state == Qt::Checked)
    {
        m_reconstruction_list_box->setEnabled(false);

        const auto series = m_model_series.lock();

        for(const data::reconstruction::sptr& rec : series->get_reconstruction_db())
        {
            p_map->insert({rec->get_organ_name(), rec->get_mesh()});
        }
    }
    else if(_state == Qt::Unchecked)
    {
        m_reconstruction_list_box->setEnabled(true);

        QList<QListWidgetItem*> item_list = m_reconstruction_list_box->findItems("", Qt::MatchContains);
        for(QListWidgetItem* item : item_list)
        {
            if(item->checkState() == Qt::Unchecked)
            {
                p_map->erase(item->text().toStdString());
            }
        }

        this->refresh();
    }
}

//------------------------------------------------------------------------------

void organ_transformation::add_mesh_transform()
{
    const auto series = m_model_series.lock();

    for(const data::reconstruction::sptr& rec : series->get_reconstruction_db())
    {
        data::mesh::sptr mesh = rec->get_mesh();

        if(!mesh->get_field(MATRIX_FIELD_NAME))
        {
            data::helper::field field_helper(mesh);
            field_helper.set_field(MATRIX_FIELD_NAME, std::make_shared<data::matrix4>());
            field_helper.notify();
        }
    }
}

//------------------------------------------------------------------------------

service::connections_t organ_transformation::auto_connections() const
{
    connections_t connections;
    connections.push(MODEL_SERIES, data::model_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(MODEL_SERIES, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE);
    connections.push(MODEL_SERIES, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE);
    connections.push(MAP, data::map::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(MAP, data::map::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(MAP, data::map::CHANGED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(MAP, data::map::REMOVED_OBJECTS_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::model
