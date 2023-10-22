/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/tools/id.hpp>

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

static const std::string s_MATRIX_FIELD_NAME = "TransformMatrix";

organ_transformation::organ_transformation() noexcept :
    m_saveButton(nullptr),
    m_loadButton(nullptr),
    m_resetButton(nullptr),
    m_reconstructionListBox(nullptr)
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
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* layout = new QVBoxLayout();

    auto* group_box = new QGroupBox(tr("Organs"));
    layout->addWidget(group_box);

    auto* layout_group_box = new QVBoxLayout();
    group_box->setLayout(layout_group_box);

    m_selectAllCheckBox     = new QCheckBox(tr("Select All"));
    m_reconstructionListBox = new QListWidget(group_box);
    m_resetButton           = new QPushButton(tr("Reset"));
    m_saveButton            = new QPushButton(tr("Save"));
    m_loadButton            = new QPushButton(tr("Load"));
    m_saveSelectionComboBox = new QComboBox();

    QObject::connect(
        m_reconstructionListBox,
        SIGNAL(itemClicked(QListWidgetItem*)),
        this,
        SLOT(onReconstructionCheck(QListWidgetItem*))
    );
    QObject::connect(m_resetButton, SIGNAL(clicked()), this, SLOT(onResetClick()));
    QObject::connect(m_saveButton, SIGNAL(clicked()), this, SLOT(onSaveClick()));
    QObject::connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadClick()));
    QObject::connect(m_selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAllChanged(int)));

    layout_group_box->addWidget(m_selectAllCheckBox, 0);
    layout_group_box->addWidget(m_reconstructionListBox, 1);
    layout_group_box->addWidget(m_resetButton, 0);
    layout_group_box->addWidget(m_saveButton, 0);
    layout_group_box->addWidget(m_saveSelectionComboBox, 0);
    layout_group_box->addWidget(m_loadButton, 0);

    qt_container->setLayout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

void organ_transformation::stopping()
{
    QObject::disconnect(
        m_reconstructionListBox,
        SIGNAL(itemClicked(QListWidgetItem*)),
        this,
        SLOT(onReconstructionCheck(QListWidgetItem*))
    );
    QObject::disconnect(m_resetButton, SIGNAL(clicked()), this, SLOT(onResetClick()));
    QObject::disconnect(m_saveButton, SIGNAL(clicked()), this, SLOT(onSaveClick()));
    QObject::disconnect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadClick()));
    QObject::disconnect(m_selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAllChanged(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void organ_transformation::updating()
{
    this->addMeshTransform();
    this->refresh();
}

//------------------------------------------------------------------------------

void organ_transformation::info(std::ostream& /*sstream*/)
{
}

//------------------------------------------------------------------------------

void organ_transformation::refresh()
{
    m_reconstructionMap.clear();
    m_reconstructionListBox->clear();

    const auto series = m_modelSeries.lock();

    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* const container = qt_container->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    bool has_reconstructions = !series->getReconstructionDB().empty();
    container->setEnabled(has_reconstructions);

    if(has_reconstructions)
    {
        const auto p_composite = m_composite.lock();

        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            m_reconstructionMap[rec->getOrganName()] = rec;
        }

        for(auto& it : m_reconstructionMap)
        {
            std::string organ_name = it.first;
            auto* item             = new QListWidgetItem(QString::fromStdString(organ_name), m_reconstructionListBox);
            if(p_composite && p_composite->find(organ_name) != p_composite->end())
            {
                item->setCheckState(Qt::Checked);
            }
            else
            {
                item->setCheckState(Qt::Unchecked);
            }

            m_reconstructionListBox->addItem(item);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::notifyTransformationMatrix(data::matrix4::sptr _a_trans_mat)
{
    auto sig = _a_trans_mat->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void organ_transformation::onReconstructionCheck(QListWidgetItem* _current_item)
{
    const auto p_composite = m_composite.lock();
    if(p_composite)
    {
        std::string item_name                       = _current_item->text().toStdString();
        data::reconstruction::sptr p_reconstruction = m_reconstructionMap[item_name];
        data::mesh::sptr p_mesh                     = p_reconstruction->getMesh();

        const auto scoped_emitter = p_composite->scoped_emit();
        if((_current_item->checkState()) == Qt::Checked)
        {
            p_composite->insert_or_assign(item_name, p_mesh);
        }
        else
        {
            p_composite->erase(item_name);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onResetClick()
{
    const auto series = m_modelSeries.lock();

    //search the corresponding triangular mesh
    for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
    {
        data::mesh::sptr p_tmp_tr_mesh = rec->getMesh();

        data::matrix4::sptr p_tmp_mat =
            p_tmp_tr_mesh->get_field<data::matrix4>(s_MATRIX_FIELD_NAME);
        if(p_tmp_mat)
        {
            geometry::data::identity(*p_tmp_mat);
            this->notifyTransformationMatrix(p_tmp_mat);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onSaveClick()
{
    inner_mat_mapping_t mat_map;

    const auto series = m_modelSeries.lock();

    if(!series->getReconstructionDB().empty())
    {
        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            data::mesh::sptr p_tmp_tr_mesh = rec->getMesh();
            data::matrix4::sptr p_tmp_mat  =
                p_tmp_tr_mesh->get_field<data::matrix4>(s_MATRIX_FIELD_NAME);
            if(p_tmp_mat)
            {
                data::matrix4::sptr p_cpy_tmp_mat;
                p_cpy_tmp_mat                    = data::object::copy(p_tmp_mat);
                mat_map[p_tmp_tr_mesh->get_id()] = p_cpy_tmp_mat;
            }
        }

        std::stringstream tmp_save_name;
        tmp_save_name << "save_" << m_saveCount;
        m_saveListing[tmp_save_name.str()] = mat_map;
        m_saveSelectionComboBox->addItem(QString::fromStdString(tmp_save_name.str()));
        m_saveCount++;
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onLoadClick()
{
    if(m_saveSelectionComboBox->count() != 0)
    {
        inner_mat_mapping_t mat_map = m_saveListing[m_saveSelectionComboBox->currentText().toStdString()];

        const auto series = m_modelSeries.lock();

        //search the corresponding triangular mesh
        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            data::mesh::sptr p_tmp_tr_mesh = rec->getMesh();
            if(mat_map.find(p_tmp_tr_mesh->get_id()) != mat_map.end())
            {
                data::matrix4::sptr p_tmp_mat =
                    p_tmp_tr_mesh->get_field<data::matrix4>(s_MATRIX_FIELD_NAME);
                if(p_tmp_mat)
                {
                    p_tmp_mat->shallow_copy(mat_map[p_tmp_tr_mesh->get_id()]);
                    this->notifyTransformationMatrix(p_tmp_mat);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onSelectAllChanged(int _state)
{
    const auto p_composite    = m_composite.lock();
    const auto scoped_emitter = p_composite->scoped_emit();

    if(_state == Qt::Checked)
    {
        m_reconstructionListBox->setEnabled(false);

        const auto series = m_modelSeries.lock();

        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            p_composite->insert({rec->getOrganName(), rec->getMesh()});
        }
    }
    else if(_state == Qt::Unchecked)
    {
        m_reconstructionListBox->setEnabled(true);

        QList<QListWidgetItem*> item_list = m_reconstructionListBox->findItems("", Qt::MatchContains);
        for(QListWidgetItem* item : item_list)
        {
            if(item->checkState() == Qt::Unchecked)
            {
                p_composite->erase(item->text().toStdString());
            }
        }

        this->refresh();
    }
}

//------------------------------------------------------------------------------

void organ_transformation::addMeshTransform()
{
    const auto series = m_modelSeries.lock();

    for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
    {
        data::mesh::sptr mesh = rec->getMesh();

        if(!mesh->get_field(s_MATRIX_FIELD_NAME))
        {
            data::helper::field field_helper(mesh);
            field_helper.set_field(s_MATRIX_FIELD_NAME, std::make_shared<data::matrix4>());
            field_helper.notify();
        }
    }
}

//------------------------------------------------------------------------------

service::connections_t organ_transformation::auto_connections() const
{
    connections_t connections;
    connections.push(s_MODEL_SERIES, data::model_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_MODEL_SERIES, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE);
    connections.push(s_MODEL_SERIES, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE);
    connections.push(s_COMPOSITE, data::composite::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_COMPOSITE, data::composite::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(s_COMPOSITE, data::composite::CHANGED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(s_COMPOSITE, data::composite::REMOVED_OBJECTS_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::model
