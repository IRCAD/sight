/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SOrganTransformation.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/tools/fwID.hpp>

#include <data/helper/Field.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

#include <geometry/data/Matrix4.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

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

SOrganTransformation::SOrganTransformation() noexcept :
    m_saveButton(nullptr),
    m_loadButton(nullptr),
    m_resetButton(nullptr),
    m_reconstructionListBox(nullptr)
{
}

//------------------------------------------------------------------------------

SOrganTransformation::~SOrganTransformation() noexcept =
    default;

//------------------------------------------------------------------------------

void SOrganTransformation::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SOrganTransformation::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

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

void SOrganTransformation::stopping()
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

void SOrganTransformation::updating()
{
    this->addMeshTransform();
    this->refresh();
}

//------------------------------------------------------------------------------

void SOrganTransformation::info(std::ostream& /*sstream*/)
{
}

//------------------------------------------------------------------------------

void SOrganTransformation::refresh()
{
    m_reconstructionMap.clear();
    m_reconstructionListBox->clear();

    const auto series = m_modelSeries.lock();

    auto qtContainer         = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    bool hasReconstructions = !series->getReconstructionDB().empty();
    container->setEnabled(hasReconstructions);

    if(hasReconstructions)
    {
        const auto pComposite = m_composite.lock();

        for(const data::Reconstruction::sptr& rec : series->getReconstructionDB())
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

void SOrganTransformation::notifyTransformationMatrix(data::Matrix4::sptr aTransMat)
{
    auto sig = aTransMat->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SOrganTransformation::onReconstructionCheck(QListWidgetItem* currentItem)
{
    const auto pComposite = m_composite.lock();
    if(pComposite)
    {
        std::string item_name                      = currentItem->text().toStdString();
        data::Reconstruction::sptr pReconstruction = m_reconstructionMap[item_name];
        data::Mesh::sptr pMesh                     = pReconstruction->getMesh();

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

void SOrganTransformation::onResetClick()
{
    const auto series = m_modelSeries.lock();

    //search the corresponding triangular mesh
    for(const data::Reconstruction::sptr& rec : series->getReconstructionDB())
    {
        data::Mesh::sptr pTmpTrMesh = rec->getMesh();

        data::Matrix4::sptr pTmpMat =
            pTmpTrMesh->getField<data::Matrix4>(s_MATRIX_FIELD_NAME);
        if(pTmpMat)
        {
            geometry::data::identity(*pTmpMat);
            this->notifyTransformationMatrix(pTmpMat);
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onSaveClick()
{
    InnerMatMappingType matMap;

    const auto series = m_modelSeries.lock();

    if(!series->getReconstructionDB().empty())
    {
        for(const data::Reconstruction::sptr& rec : series->getReconstructionDB())
        {
            data::Mesh::sptr pTmpTrMesh = rec->getMesh();
            data::Matrix4::sptr pTmpMat =
                pTmpTrMesh->getField<data::Matrix4>(s_MATRIX_FIELD_NAME);
            if(pTmpMat)
            {
                data::Matrix4::sptr pCpyTmpMat;
                pCpyTmpMat                  = data::Object::copy(pTmpMat);
                matMap[pTmpTrMesh->getID()] = pCpyTmpMat;
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

void SOrganTransformation::onLoadClick()
{
    if(m_saveSelectionComboBox->count() != 0)
    {
        InnerMatMappingType matMap = m_saveListing[m_saveSelectionComboBox->currentText().toStdString()];

        const auto series = m_modelSeries.lock();

        //search the corresponding triangular mesh
        for(const data::Reconstruction::sptr& rec : series->getReconstructionDB())
        {
            data::Mesh::sptr pTmpTrMesh = rec->getMesh();
            if(matMap.find(pTmpTrMesh->getID()) != matMap.end())
            {
                data::Matrix4::sptr pTmpMat =
                    pTmpTrMesh->getField<data::Matrix4>(s_MATRIX_FIELD_NAME);
                if(pTmpMat)
                {
                    pTmpMat->shallowCopy(matMap[pTmpTrMesh->getID()]);
                    this->notifyTransformationMatrix(pTmpMat);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onSelectAllChanged(int state)
{
    const auto pComposite     = m_composite.lock();
    const auto scoped_emitter = pComposite->scoped_emit();

    if(state == Qt::Checked)
    {
        m_reconstructionListBox->setEnabled(false);

        const auto series = m_modelSeries.lock();

        for(const data::Reconstruction::sptr& rec : series->getReconstructionDB())
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

void SOrganTransformation::addMeshTransform()
{
    const auto series = m_modelSeries.lock();

    for(const data::Reconstruction::sptr& rec : series->getReconstructionDB())
    {
        data::Mesh::sptr mesh = rec->getMesh();

        if(!mesh->getField(s_MATRIX_FIELD_NAME))
        {
            data::helper::Field fieldHelper(mesh);
            fieldHelper.setField(s_MATRIX_FIELD_NAME, data::Matrix4::New());
            fieldHelper.notify();
        }
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SOrganTransformation::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MODEL_SERIES, data::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES, data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES, data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE, data::Composite::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE, data::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE, data::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE, data::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::model
