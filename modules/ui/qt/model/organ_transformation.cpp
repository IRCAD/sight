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

#include <data/helper/Field.hpp>
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
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* layout = new QVBoxLayout();

    auto* groupBox = new QGroupBox(tr("Organs"));
    layout->addWidget(groupBox);

    auto* layoutGroupBox = new QVBoxLayout();
    groupBox->setLayout(layoutGroupBox);

    m_selectAllCheckBox     = new QCheckBox(tr("Select All"));
    m_reconstructionListBox = new QListWidget(groupBox);
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

    layoutGroupBox->addWidget(m_selectAllCheckBox, 0);
    layoutGroupBox->addWidget(m_reconstructionListBox, 1);
    layoutGroupBox->addWidget(m_resetButton, 0);
    layoutGroupBox->addWidget(m_saveButton, 0);
    layoutGroupBox->addWidget(m_saveSelectionComboBox, 0);
    layoutGroupBox->addWidget(m_loadButton, 0);

    qtContainer->setLayout(layout);

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

    auto qtContainer         = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    bool hasReconstructions = !series->getReconstructionDB().empty();
    container->setEnabled(hasReconstructions);

    if(hasReconstructions)
    {
        const auto pComposite = m_composite.lock();

        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            m_reconstructionMap[rec->getOrganName()] = rec;
        }

        for(auto& it : m_reconstructionMap)
        {
            std::string organName = it.first;
            auto* item            = new QListWidgetItem(QString::fromStdString(organName), m_reconstructionListBox);
            if(pComposite && pComposite->find(organName) != pComposite->end())
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

void organ_transformation::notifyTransformationMatrix(data::matrix4::sptr aTransMat)
{
    auto sig = aTransMat->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void organ_transformation::onReconstructionCheck(QListWidgetItem* currentItem)
{
    const auto pComposite = m_composite.lock();
    if(pComposite)
    {
        std::string item_name                      = currentItem->text().toStdString();
        data::reconstruction::sptr pReconstruction = m_reconstructionMap[item_name];
        data::mesh::sptr pMesh                     = pReconstruction->getMesh();

        const auto scoped_emitter = pComposite->scoped_emit();
        if((currentItem->checkState()) == Qt::Checked)
        {
            pComposite->insert_or_assign(item_name, pMesh);
        }
        else
        {
            pComposite->erase(item_name);
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
        data::mesh::sptr pTmpTrMesh = rec->getMesh();

        data::matrix4::sptr pTmpMat =
            pTmpTrMesh->getField<data::matrix4>(s_MATRIX_FIELD_NAME);
        if(pTmpMat)
        {
            geometry::data::identity(*pTmpMat);
            this->notifyTransformationMatrix(pTmpMat);
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onSaveClick()
{
    InnerMatMappingType matMap;

    const auto series = m_modelSeries.lock();

    if(!series->getReconstructionDB().empty())
    {
        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            data::mesh::sptr pTmpTrMesh = rec->getMesh();
            data::matrix4::sptr pTmpMat =
                pTmpTrMesh->getField<data::matrix4>(s_MATRIX_FIELD_NAME);
            if(pTmpMat)
            {
                data::matrix4::sptr pCpyTmpMat;
                pCpyTmpMat                   = data::object::copy(pTmpMat);
                matMap[pTmpTrMesh->get_id()] = pCpyTmpMat;
            }
        }

        std::stringstream tmpSaveName;
        tmpSaveName << "save_" << m_saveCount;
        m_saveListing[tmpSaveName.str()] = matMap;
        m_saveSelectionComboBox->addItem(QString::fromStdString(tmpSaveName.str()));
        m_saveCount++;
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onLoadClick()
{
    if(m_saveSelectionComboBox->count() != 0)
    {
        InnerMatMappingType matMap = m_saveListing[m_saveSelectionComboBox->currentText().toStdString()];

        const auto series = m_modelSeries.lock();

        //search the corresponding triangular mesh
        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            data::mesh::sptr pTmpTrMesh = rec->getMesh();
            if(matMap.find(pTmpTrMesh->get_id()) != matMap.end())
            {
                data::matrix4::sptr pTmpMat =
                    pTmpTrMesh->getField<data::matrix4>(s_MATRIX_FIELD_NAME);
                if(pTmpMat)
                {
                    pTmpMat->shallow_copy(matMap[pTmpTrMesh->get_id()]);
                    this->notifyTransformationMatrix(pTmpMat);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void organ_transformation::onSelectAllChanged(int state)
{
    const auto pComposite     = m_composite.lock();
    const auto scoped_emitter = pComposite->scoped_emit();

    if(state == Qt::Checked)
    {
        m_reconstructionListBox->setEnabled(false);

        const auto series = m_modelSeries.lock();

        for(const data::reconstruction::sptr& rec : series->getReconstructionDB())
        {
            pComposite->insert({rec->getOrganName(), rec->getMesh()});
        }
    }
    else if(state == Qt::Unchecked)
    {
        m_reconstructionListBox->setEnabled(true);

        QList<QListWidgetItem*> itemList = m_reconstructionListBox->findItems("", Qt::MatchContains);
        for(QListWidgetItem* item : itemList)
        {
            if(item->checkState() == Qt::Unchecked)
            {
                pComposite->erase(item->text().toStdString());
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

        if(!mesh->getField(s_MATRIX_FIELD_NAME))
        {
            data::helper::Field fieldHelper(mesh);
            fieldHelper.setField(s_MATRIX_FIELD_NAME, std::make_shared<data::matrix4>());
            fieldHelper.notify();
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
