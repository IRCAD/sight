/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "uiMedDataQt/editor/SOrganTransformation.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/tools/fwID.hpp>

#include <data/Composite.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/tools/helper/Composite.hpp>
#include <data/tools/helper/Field.hpp>

#include <services/macros.hpp>

#include <geometry/data/TransformationMatrix3D.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>

#include <ui/qt/container/QtContainer.hpp>

#include <map>

namespace uiMedDataQt
{
namespace editor
{

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::uiMedDataQt::editor::SOrganTransformation )

static const services::IService::KeyType s_MODEL_SERIES_INOUT = "modelSeries";
static const services::IService::KeyType s_COMPOSITE_INOUT = "composite";

static const std::string s_MATRIX_FIELD_NAME = "TransformMatrix";

SOrganTransformation::SOrganTransformation() noexcept :
    m_saveButton( 0 ),
    m_loadButton( 0 ),
    m_resetButton( 0 ),
    m_reconstructionListBox( 0 ),
    m_saveCount( 0 )
{
}

//------------------------------------------------------------------------------

SOrganTransformation::~SOrganTransformation() noexcept
{
}

//------------------------------------------------------------------------------

void SOrganTransformation::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SOrganTransformation::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast( this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();

    QGroupBox* groupBox = new QGroupBox(tr("Organs") );
    layout->addWidget( groupBox);

    QVBoxLayout* layoutGroupBox = new QVBoxLayout();
    groupBox->setLayout(layoutGroupBox);

    m_selectAllCheckBox     = new QCheckBox(tr("Select All")  );
    m_reconstructionListBox = new QListWidget( groupBox);
    m_resetButton           = new QPushButton(tr("Reset") );
    m_saveButton            = new QPushButton(tr("Save") );
    m_loadButton            = new QPushButton(tr("Load") );
    m_saveSelectionComboBox = new QComboBox();

    QObject::connect(m_reconstructionListBox,
                     SIGNAL(itemClicked(QListWidgetItem*)),
                     this,
                     SLOT(onReconstructionCheck(QListWidgetItem*)));
    QObject::connect(m_resetButton, SIGNAL(clicked()), this, SLOT(onResetClick()));
    QObject::connect(m_saveButton, SIGNAL(clicked()), this, SLOT(onSaveClick()));
    QObject::connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadClick()));
    QObject::connect(m_selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAllChanged(int)));

    layoutGroupBox->addWidget( m_selectAllCheckBox, 0);
    layoutGroupBox->addWidget( m_reconstructionListBox, 1);
    layoutGroupBox->addWidget( m_resetButton, 0);
    layoutGroupBox->addWidget( m_saveButton, 0);
    layoutGroupBox->addWidget( m_saveSelectionComboBox, 0);
    layoutGroupBox->addWidget( m_loadButton, 0);

    qtContainer->setLayout( layout );

    this->updating();
}

//------------------------------------------------------------------------------

void SOrganTransformation::stopping()
{
    QObject::disconnect(m_reconstructionListBox,
                        SIGNAL(itemClicked(QListWidgetItem*)),
                        this,
                        SLOT(onReconstructionCheck(QListWidgetItem*)));
    QObject::disconnect(m_resetButton, SIGNAL(clicked()), this, SLOT(onResetClick()));
    QObject::disconnect(m_saveButton, SIGNAL(clicked()), this, SLOT(onSaveClick()));
    QObject::disconnect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadClick()));
    QObject::disconnect(m_selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onSelectAllChanged(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void SOrganTransformation::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SOrganTransformation::updating()
{
    this->addMeshTransform();
    this->refresh();
}

//------------------------------------------------------------------------------

void SOrganTransformation::info( ::std::ostream& sstream )
{
}

//------------------------------------------------------------------------------

void SOrganTransformation::refresh()
{
    m_reconstructionMap.clear();
    m_reconstructionListBox->clear();

    data::ModelSeries::sptr series = this->getInOut< data::ModelSeries >(s_MODEL_SERIES_INOUT);

    auto qtContainer         = sight::ui::qt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    bool hasReconstructions = !series->getReconstructionDB().empty();
    container->setEnabled( hasReconstructions );

    if(hasReconstructions)
    {
        data::Composite::sptr pComposite = this->getInOut< data::Composite>(s_COMPOSITE_INOUT);

        for(data::Reconstruction::sptr rec :  series->getReconstructionDB())
        {
            m_reconstructionMap[ rec->getOrganName() ] = rec;
        }

        for( ReconstructionMapType::iterator it = m_reconstructionMap.begin(); it != m_reconstructionMap.end(); ++it )
        {
            std::string organName = it->first;
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(organName), m_reconstructionListBox);
            if( pComposite && pComposite->find(organName) != pComposite->end())
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

void SOrganTransformation::notitfyTransformationMatrix(data::TransformationMatrix3D::sptr aTransMat)
{
    auto sig = aTransMat->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SOrganTransformation::onReconstructionCheck(QListWidgetItem* currentItem)
{
    data::Composite::sptr pComposite = this->getInOut< data::Composite>(s_COMPOSITE_INOUT);
    if (pComposite != nullptr)
    {
        ::std::string item_name = currentItem->text().toStdString();
        data::Reconstruction::sptr pReconstruction = m_reconstructionMap[item_name];
        data::Mesh::sptr pMesh                     = pReconstruction->getMesh();

        data::tools::helper::Composite aCompositeHelper(pComposite);
        if ((currentItem->checkState()) == Qt::Checked)
        {
            if (pComposite->find(item_name) == pComposite->end())
            {
                aCompositeHelper.add(item_name, pMesh);
            }
            else
            {
                aCompositeHelper.swap(item_name, pMesh);
            }
        }
        else
        {
            if (pComposite->find(item_name) != pComposite->end())
            {
                aCompositeHelper.remove(item_name);
            }
        }
        aCompositeHelper.notify();
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onResetClick()
{
    data::ModelSeries::sptr series = this->getInOut< data::ModelSeries >(s_MODEL_SERIES_INOUT);

    //search the corresponding triangular mesh
    for(data::Reconstruction::sptr rec :  series->getReconstructionDB())
    {
        data::Mesh::sptr pTmpTrMesh = rec->getMesh();

        data::TransformationMatrix3D::sptr pTmpMat =
            pTmpTrMesh->getField< data::TransformationMatrix3D>( s_MATRIX_FIELD_NAME );
        if (pTmpMat)
        {
            geometry::data::identity(pTmpMat);
            this->notitfyTransformationMatrix(pTmpMat);
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onSaveClick()
{
    InnerMatMappingType matMap;

    data::ModelSeries::sptr series = this->getInOut< data::ModelSeries >(s_MODEL_SERIES_INOUT);

    if(!series->getReconstructionDB().empty())
    {
        for(data::Reconstruction::sptr rec :  series->getReconstructionDB())
        {
            data::Mesh::sptr pTmpTrMesh                = rec->getMesh();
            data::TransformationMatrix3D::sptr pTmpMat =
                pTmpTrMesh->getField< data::TransformationMatrix3D>( s_MATRIX_FIELD_NAME );
            if (pTmpMat)
            {
                data::TransformationMatrix3D::sptr pCpyTmpMat;
                pCpyTmpMat                  = data::Object::copy(pTmpMat);
                matMap[pTmpTrMesh->getID()] = pCpyTmpMat;
            }
        }

        ::std::stringstream tmpSaveName;
        tmpSaveName << "save_" << m_saveCount;
        m_saveListing[tmpSaveName.str()] = matMap;
        m_saveSelectionComboBox->addItem(QString::fromStdString(tmpSaveName.str()));
        m_saveCount++;
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onLoadClick()
{
    if (m_saveSelectionComboBox->count() != 0)
    {
        InnerMatMappingType matMap = m_saveListing[m_saveSelectionComboBox->currentText().toStdString()];

        data::ModelSeries::sptr series = this->getInOut< data::ModelSeries >(s_MODEL_SERIES_INOUT);

        //search the corresponding triangular mesh
        for(data::Reconstruction::sptr rec :  series->getReconstructionDB())
        {
            data::Mesh::sptr pTmpTrMesh = rec->getMesh();
            if (matMap.find(pTmpTrMesh->getID()) != matMap.end())
            {
                data::TransformationMatrix3D::sptr pTmpMat =
                    pTmpTrMesh->getField< data::TransformationMatrix3D>( s_MATRIX_FIELD_NAME );
                if (pTmpMat)
                {
                    pTmpMat->shallowCopy(matMap[pTmpTrMesh->getID()]);
                    this->notitfyTransformationMatrix(pTmpMat);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onSelectAllChanged(int state)
{

    data::Composite::sptr composite = this->getInOut< data::Composite>(s_COMPOSITE_INOUT);
    data::tools::helper::Composite compositeHelper(composite);

    if(state == Qt::Checked)
    {
        m_reconstructionListBox->setEnabled(false);

        data::ModelSeries::sptr series = this->getInOut< data::ModelSeries >(s_MODEL_SERIES_INOUT);

        for(data::Reconstruction::sptr rec :  series->getReconstructionDB())
        {
            if(composite->find(rec->getOrganName()) == composite->end())
            {
                compositeHelper.add(rec->getOrganName(), rec->getMesh());
            }
        }

    }
    else if(state == Qt::Unchecked)
    {
        m_reconstructionListBox->setEnabled(true);

        QList<QListWidgetItem*> itemList = m_reconstructionListBox->findItems("", Qt::MatchContains);
        for(QListWidgetItem* item :  itemList)
        {
            if(item->checkState() == Qt::Unchecked)
            {
                compositeHelper.remove(item->text().toStdString());
            }
        }

        this->refresh();
    }
    compositeHelper.notify();

}

//------------------------------------------------------------------------------

void SOrganTransformation::addMeshTransform()
{
    data::ModelSeries::sptr series = this->getInOut< data::ModelSeries >(s_MODEL_SERIES_INOUT);

    for(const data::Reconstruction::sptr& rec :  series->getReconstructionDB())
    {
        data::Mesh::sptr mesh = rec->getMesh();

        if (!mesh->getField( s_MATRIX_FIELD_NAME ))
        {
            data::tools::helper::Field fieldHelper(mesh);
            fieldHelper.setField(s_MATRIX_FIELD_NAME, data::TransformationMatrix3D::New());
            fieldHelper.notify();
        }
    }
}

//------------------------------------------------------------------------------

services::IService::KeyConnectionsMap SOrganTransformation::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE_INOUT, data::Composite::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE_INOUT, data::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE_INOUT, data::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_COMPOSITE_INOUT, data::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedDataQt
