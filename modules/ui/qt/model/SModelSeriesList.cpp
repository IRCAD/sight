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

#include "SModelSeriesList.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/id.hpp>

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/helper/Field.hpp>
#include <data/Integer.hpp>
#include <data/Reconstruction.hpp>
#include <data/String.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <ui/qt/container/widget.hpp>

#include <boost/format.hpp>

#include <QAction>
#include <QCheckBox>
#include <QGroupBox>
#include <QListWidgetItem>
#include <QMenu>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace sight::module::ui::qt::model
{

//------------------------------------------------------------------------------

static const core::com::signals::key_t RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
static const core::com::signals::key_t EMPTIED_SELECTION_SIG       = "emptiedSelection";
static const core::com::slots::key_t SHOW_RECONSTRUCTIONS_SLOT     = "showReconstructions";

//------------------------------------------------------------------------------

SModelSeriesList::SModelSeriesList() noexcept
{
    m_sigReconstructionSelected = new_signal<ReconstructionSelectedSignalType>(RECONSTRUCTION_SELECTED_SIG);
    m_sigEmptiedSelection       = new_signal<EmptiedSelectionSignalType>(EMPTIED_SELECTION_SIG);

    new_slot(SHOW_RECONSTRUCTIONS_SLOT, &SModelSeriesList::showReconstructions, this);
}

//------------------------------------------------------------------------------

void SModelSeriesList::configuring()
{
    this->initialize();

    const ConfigType configType = this->getConfiguration();
    const auto config           = configType.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_enableHideAll = config->get<bool>("enable_hide_all", m_enableHideAll);
        m_enableDelete  = config->get<bool>("enableDelete", m_enableDelete);
    }

    auto columns = configType.get_child("columns");
    for(auto itCol = columns.begin() ; itCol != columns.end() ; ++itCol)
    {
        m_headers << QString::fromStdString(itCol->first);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::starting()
{
    this->create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    qtContainer->getQtContainer()->setObjectName(serviceID);

    auto* layout       = new QVBoxLayout;
    auto* layoutButton = new QHBoxLayout;
    layout->addLayout(layoutButton);

    m_tree = new QTreeWidget();

    m_tree->setColumnCount(m_headers.size());
    m_tree->setHeaderLabels(m_headers);

    if(m_enableHideAll)
    {
        // check box "show"
        m_showCheckBox = new QCheckBox(tr("Hide all organs"));
        m_showCheckBox->setObjectName(serviceID + "/" + m_showCheckBox->text());
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layoutButton->addWidget(m_showCheckBox, 0);
        QObject::connect(m_showCheckBox, &QCheckBox::stateChanged, this, &SModelSeriesList::onShowReconstructions);

        m_checkAllButton = new QPushButton(tr("Check all"));
        m_checkAllButton->setObjectName(serviceID + "/" + m_checkAllButton->text());
        layoutButton->addWidget(m_checkAllButton, 0);
        QObject::connect(m_checkAllButton, &QPushButton::clicked, this, &SModelSeriesList::onCheckAllCheckBox);

        m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
        m_unCheckAllButton->setObjectName(serviceID + "/" + m_unCheckAllButton->text());
        layoutButton->addWidget(m_unCheckAllButton, 0);
        QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this, &SModelSeriesList::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        m_deleteAllButton = new QPushButton(tr("Delete all"));
        m_deleteAllButton->setObjectName(serviceID + "/" + m_deleteAllButton->text());
        layoutButton->addWidget(m_deleteAllButton, 0);
        QObject::connect(m_deleteAllButton, &QPushButton::clicked, this, &SModelSeriesList::onDeleteAllCheckBox);
    }

    layout->addWidget(m_tree, 1);

    qtContainer->setLayout(layout);

    QObject::connect(
        m_tree,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*))
    );

    this->updating();

    QObject::connect(
        m_tree,
        SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,int))
    );

    if(m_enableDelete)
    {
        m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &SModelSeriesList::onCustomContextMenuRequested
        );
    }
}

//------------------------------------------------------------------------------

service::connections_t SModelSeriesList::getAutoConnections() const
{
    return {
        {s_MODEL_SERIES, data::ModelSeries::MODIFIED_SIG, service::slots::UPDATE},
        {s_MODEL_SERIES, data::ModelSeries::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE},
        {s_MODEL_SERIES, data::ModelSeries::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void SModelSeriesList::updating()
{
    m_tree->blockSignals(true);

    this->updateReconstructions();
    this->refreshVisibility();

    m_tree->blockSignals(false);
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping()
{
    if(m_enableHideAll)
    {
        QObject::disconnect(m_showCheckBox, &QCheckBox::stateChanged, this, &SModelSeriesList::onShowReconstructions);
        QObject::disconnect(m_checkAllButton, &QPushButton::clicked, this, &SModelSeriesList::onCheckAllCheckBox);
        QObject::disconnect(m_unCheckAllButton, &QPushButton::clicked, this, &SModelSeriesList::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        QObject::disconnect(m_deleteAllButton, &QPushButton::clicked, this, &SModelSeriesList::onDeleteAllCheckBox);
        QObject::disconnect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &SModelSeriesList::onCustomContextMenuRequested
        );
    }

    QObject::disconnect(
        m_tree,
        SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,int))
    );

    QObject::disconnect(
        m_tree,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*))
    );

    this->destroy();
}

//------------------------------------------------------------------------------

void SModelSeriesList::updateReconstructions()
{
    auto qtContainer         = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    SIGHT_ASSERT("container not instanced", container);

    auto modelSeries        = m_modelSeries.lock();
    bool hasReconstructions = !modelSeries->getReconstructionDB().empty();
    container->setEnabled(hasReconstructions);

    this->fillTree(modelSeries);
    if(hasReconstructions)
    {
        if(m_showCheckBox != nullptr)
        {
            m_showCheckBox->blockSignals(true);
            const bool showAllRec =
                modelSeries->getField("ShowReconstructions", std::make_shared<data::Boolean>(true))->value();
            m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked);
            m_showCheckBox->blockSignals(false);
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::fillTree(const data::mt::locked_ptr<data::ModelSeries>& _modelSeries)
{
    const auto& reconstructions = _modelSeries->getReconstructionDB();

    if(!m_tree->selectedItems().empty())
    {
        m_sigEmptiedSelection->async_emit();
    }

    m_tree->clear();

    // Create items
    for(auto const& reconstruction : reconstructions)
    {
        QStringList info;
        for(auto const& column : m_headers)
        {
            if(column == "organ_name")
            {
                info << QString::fromStdString(reconstruction->getOrganName());
            }
            else if(column == "structure_type")
            {
                info << QString::fromStdString(reconstruction->getStructureType());
            }
            else if(column == "volume")
            {
                auto volume = reconstruction->getComputedMaskVolume();
                info << (volume < 0 ? "unknown" : QString::fromStdString(std::to_string(volume)));
            }
        }

        auto* item = new QTreeWidgetItem(info);
        item->setCheckState(0, Qt::Unchecked);
        m_tree->addTopLevelItem(item);
        item->setData(0, Qt::UserRole, QString::fromStdString(reconstruction->get_id()));
    }

    for(int i = 0 ; i < m_tree->topLevelItemCount() ; i++)
    {
        m_tree->resizeColumnToContents(i);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem* /*unused*/)
{
    SIGHT_ASSERT("Current selected item is null", _current);
    std::string id = _current->data(0, Qt::UserRole).toString().toStdString();

    data::Reconstruction::sptr rec = std::dynamic_pointer_cast<data::Reconstruction>(core::tools::id::get_object(id));

    m_sigReconstructionSelected->async_emit(rec);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged(QTreeWidgetItem* _current, int _column)
{
    sight::module::ui::qt::model::SModelSeriesList::onOrganChoiceVisibility(_current, _column);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganChoiceVisibility(QTreeWidgetItem* _item, int /*unused*/)
{
    std::string id                 = _item->data(0, Qt::UserRole).toString().toStdString();
    data::Reconstruction::sptr rec = std::dynamic_pointer_cast<data::Reconstruction>(core::tools::id::get_object(id));
    SIGHT_ASSERT("rec not instanced", rec);

    const bool itemIsChecked = (_item->checkState(0) == Qt::Checked);

    if(rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(itemIsChecked);

        data::Reconstruction::VisibilityModifiedSignalType::sptr sig;
        sig = rec->signal<data::Reconstruction::VisibilityModifiedSignalType>(
            data::Reconstruction::VISIBILITY_MODIFIED_SIG
        );
        sig->async_emit(itemIsChecked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onShowReconstructions(int _state)
{
    const bool visible = static_cast<bool>(_state);

    m_checkAllButton->setEnabled(!visible);
    m_unCheckAllButton->setEnabled(!visible);
    m_tree->setEnabled(!visible);

    {
        auto modelSeries = m_modelSeries.lock();
        data::helper::Field helper(modelSeries.get_shared());
        helper.addOrSwap("ShowReconstructions", std::make_shared<data::Boolean>(_state == Qt::Unchecked));
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::refreshVisibility()
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item          = m_tree->topLevelItem(i);
        std::string id                 = item->data(0, Qt::UserRole).toString().toStdString();
        data::Reconstruction::sptr rec =
            std::dynamic_pointer_cast<data::Reconstruction>(core::tools::id::get_object(id));
        item->setCheckState(0, rec->getIsVisible() ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::showReconstructions(bool _show)
{
    if(m_showCheckBox != nullptr)
    {
        m_showCheckBox->setCheckState(_show ? Qt::Unchecked : Qt::Checked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCheckAllCheckBox()
{
    this->onCheckAllBoxes(true);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onUnCheckAllCheckBox()
{
    this->onCheckAllBoxes(false);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCheckAllBoxes(bool _visible)
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item = m_tree->topLevelItem(i);
        item->setCheckState(0, _visible ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onDeleteAllCheckBox()
{
    auto modelSeries = m_modelSeries.lock();

    // Remove all reconstructions.
    data::ModelSeries::ReconstructionVectorType reconstructions = modelSeries->getReconstructionDB();
    modelSeries->setReconstructionDB(data::ModelSeries::ReconstructionVectorType());

    // Send the signals.
    auto sig = modelSeries->signal<data::ModelSeries::ReconstructionsRemovedSignalType>(
        data::ModelSeries::RECONSTRUCTIONS_REMOVED_SIG
    );
    sig->async_emit(reconstructions);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCustomContextMenuRequested(const QPoint& _pos)
{
    QModelIndex index = m_tree->indexAt(_pos);
    if(index.isValid())
    {
        auto* const deleteAction = new QAction("Delete");
        QObject::connect(
            deleteAction,
            &QAction::triggered,
            this,
            [index, this]()
            {
                auto modelSeries = m_modelSeries.lock();

                data::ModelSeries::ReconstructionVectorType deletedReconstructions;

                // Remove reconstruction.
                data::ModelSeries::ReconstructionVectorType reconstructions =
                    modelSeries->getReconstructionDB();
                const auto recIt =
                    reconstructions.begin() + index.row();
                const data::Reconstruction::sptr reconstruction = *recIt;
                reconstructions.erase(recIt);
                modelSeries->setReconstructionDB(reconstructions);

                // Send the signals.
                deletedReconstructions.push_back(reconstruction);
                auto sig = modelSeries->signal<data::ModelSeries::ReconstructionsRemovedSignalType>(
                    data::ModelSeries::RECONSTRUCTIONS_REMOVED_SIG
                );
                sig->async_emit(deletedReconstructions);
            });

        QMenu contextMenu;
        contextMenu.addAction(deleteAction);
        contextMenu.exec(QCursor::pos());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::model
