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

#include "model_series_list.hpp"

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

#include <data/boolean.hpp>
#include <data/helper/Field.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/reconstruction.hpp>
#include <data/string.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

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

model_series_list::model_series_list() noexcept
{
    m_sigReconstructionSelected = new_signal<ReconstructionSelectedSignalType>(RECONSTRUCTION_SELECTED_SIG);
    m_sigEmptiedSelection       = new_signal<EmptiedSelectionSignalType>(EMPTIED_SELECTION_SIG);

    new_slot(SHOW_RECONSTRUCTIONS_SLOT, &model_series_list::showReconstructions, this);
}

//------------------------------------------------------------------------------

void model_series_list::configuring()
{
    this->initialize();

    const config_t config_t = this->get_config();
    const auto config       = config_t.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_enableHideAll = config->get<bool>("enable_hide_all", m_enableHideAll);
        m_enableDelete  = config->get<bool>("enableDelete", m_enableDelete);
    }

    auto columns = config_t.get_child("columns");
    for(auto itCol = columns.begin() ; itCol != columns.end() ; ++itCol)
    {
        m_headers << QString::fromStdString(itCol->first);
    }
}

//------------------------------------------------------------------------------

void model_series_list::starting()
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
        QObject::connect(m_showCheckBox, &QCheckBox::stateChanged, this, &model_series_list::onShowReconstructions);

        m_checkAllButton = new QPushButton(tr("Check all"));
        m_checkAllButton->setObjectName(serviceID + "/" + m_checkAllButton->text());
        layoutButton->addWidget(m_checkAllButton, 0);
        QObject::connect(m_checkAllButton, &QPushButton::clicked, this, &model_series_list::onCheckAllCheckBox);

        m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
        m_unCheckAllButton->setObjectName(serviceID + "/" + m_unCheckAllButton->text());
        layoutButton->addWidget(m_unCheckAllButton, 0);
        QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this, &model_series_list::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        m_deleteAllButton = new QPushButton(tr("Delete all"));
        m_deleteAllButton->setObjectName(serviceID + "/" + m_deleteAllButton->text());
        layoutButton->addWidget(m_deleteAllButton, 0);
        QObject::connect(m_deleteAllButton, &QPushButton::clicked, this, &model_series_list::onDeleteAllCheckBox);
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
            &model_series_list::onCustomContextMenuRequested
        );
    }
}

//------------------------------------------------------------------------------

service::connections_t model_series_list::auto_connections() const
{
    return {
        {s_MODEL_SERIES, data::model_series::MODIFIED_SIG, service::slots::UPDATE},
        {s_MODEL_SERIES, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE},
        {s_MODEL_SERIES, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void model_series_list::updating()
{
    m_tree->blockSignals(true);

    this->updateReconstructions();
    this->refreshVisibility();

    m_tree->blockSignals(false);
}

//------------------------------------------------------------------------------

void model_series_list::stopping()
{
    if(m_enableHideAll)
    {
        QObject::disconnect(m_showCheckBox, &QCheckBox::stateChanged, this, &model_series_list::onShowReconstructions);
        QObject::disconnect(m_checkAllButton, &QPushButton::clicked, this, &model_series_list::onCheckAllCheckBox);
        QObject::disconnect(m_unCheckAllButton, &QPushButton::clicked, this, &model_series_list::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        QObject::disconnect(m_deleteAllButton, &QPushButton::clicked, this, &model_series_list::onDeleteAllCheckBox);
        QObject::disconnect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &model_series_list::onCustomContextMenuRequested
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

void model_series_list::updateReconstructions()
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
                modelSeries->getField("ShowReconstructions", std::make_shared<data::boolean>(true))->value();
            m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked);
            m_showCheckBox->blockSignals(false);
        }
    }
}

//------------------------------------------------------------------------------

void model_series_list::fillTree(const data::mt::locked_ptr<data::model_series>& _modelSeries)
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

void model_series_list::onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem* /*unused*/)
{
    SIGHT_ASSERT("Current selected item is null", _current);
    std::string id = _current->data(0, Qt::UserRole).toString().toStdString();

    data::reconstruction::sptr rec = std::dynamic_pointer_cast<data::reconstruction>(core::tools::id::get_object(id));

    m_sigReconstructionSelected->async_emit(rec);
}

//------------------------------------------------------------------------------

void model_series_list::onCurrentItemChanged(QTreeWidgetItem* _current, int _column)
{
    sight::module::ui::qt::model::model_series_list::onOrganChoiceVisibility(_current, _column);
}

//------------------------------------------------------------------------------

void model_series_list::onOrganChoiceVisibility(QTreeWidgetItem* _item, int /*unused*/)
{
    std::string id                 = _item->data(0, Qt::UserRole).toString().toStdString();
    data::reconstruction::sptr rec = std::dynamic_pointer_cast<data::reconstruction>(core::tools::id::get_object(id));
    SIGHT_ASSERT("rec not instanced", rec);

    const bool itemIsChecked = (_item->checkState(0) == Qt::Checked);

    if(rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(itemIsChecked);

        data::reconstruction::VisibilityModifiedSignalType::sptr sig;
        sig = rec->signal<data::reconstruction::VisibilityModifiedSignalType>(
            data::reconstruction::VISIBILITY_MODIFIED_SIG
        );
        sig->async_emit(itemIsChecked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::onShowReconstructions(int _state)
{
    const bool visible = static_cast<bool>(_state);

    m_checkAllButton->setEnabled(!visible);
    m_unCheckAllButton->setEnabled(!visible);
    m_tree->setEnabled(!visible);

    {
        auto modelSeries = m_modelSeries.lock();
        data::helper::Field helper(modelSeries.get_shared());
        helper.addOrSwap("ShowReconstructions", std::make_shared<data::boolean>(_state == Qt::Unchecked));
    }
}

//------------------------------------------------------------------------------

void model_series_list::refreshVisibility()
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item          = m_tree->topLevelItem(i);
        std::string id                 = item->data(0, Qt::UserRole).toString().toStdString();
        data::reconstruction::sptr rec =
            std::dynamic_pointer_cast<data::reconstruction>(core::tools::id::get_object(id));
        item->setCheckState(0, rec->getIsVisible() ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::showReconstructions(bool _show)
{
    if(m_showCheckBox != nullptr)
    {
        m_showCheckBox->setCheckState(_show ? Qt::Unchecked : Qt::Checked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::onCheckAllCheckBox()
{
    this->onCheckAllBoxes(true);
}

//------------------------------------------------------------------------------

void model_series_list::onUnCheckAllCheckBox()
{
    this->onCheckAllBoxes(false);
}

//------------------------------------------------------------------------------

void model_series_list::onCheckAllBoxes(bool _visible)
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item = m_tree->topLevelItem(i);
        item->setCheckState(0, _visible ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::onDeleteAllCheckBox()
{
    auto modelSeries = m_modelSeries.lock();

    // Remove all reconstructions.
    data::model_series::ReconstructionVectorType reconstructions = modelSeries->getReconstructionDB();
    modelSeries->setReconstructionDB(data::model_series::ReconstructionVectorType());

    // Send the signals.
    auto sig = modelSeries->signal<data::model_series::ReconstructionsRemovedSignalType>(
        data::model_series::RECONSTRUCTIONS_REMOVED_SIG
    );
    sig->async_emit(reconstructions);
}

//------------------------------------------------------------------------------

void model_series_list::onCustomContextMenuRequested(const QPoint& _pos)
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

                data::model_series::ReconstructionVectorType deletedReconstructions;

                // Remove reconstruction.
                data::model_series::ReconstructionVectorType reconstructions =
                    modelSeries->getReconstructionDB();
                const auto recIt =
                    reconstructions.begin() + index.row();
                const data::reconstruction::sptr reconstruction = *recIt;
                reconstructions.erase(recIt);
                modelSeries->setReconstructionDB(reconstructions);

                // Send the signals.
                deletedReconstructions.push_back(reconstruction);
                auto sig = modelSeries->signal<data::model_series::ReconstructionsRemovedSignalType>(
                    data::model_series::RECONSTRUCTIONS_REMOVED_SIG
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
