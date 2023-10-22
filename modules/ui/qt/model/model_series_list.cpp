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
#include <data/helper/field.hpp>
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
    m_sigReconstructionSelected = new_signal<reconstruction_selected_signal_t>(RECONSTRUCTION_SELECTED_SIG);
    m_sigEmptiedSelection       = new_signal<emptied_selection_signal_t>(EMPTIED_SELECTION_SIG);

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
    for(auto it_col = columns.begin() ; it_col != columns.end() ; ++it_col)
    {
        m_headers << QString::fromStdString(it_col->first);
    }
}

//------------------------------------------------------------------------------

void model_series_list::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    qt_container->getQtContainer()->setObjectName(service_id);

    auto* layout        = new QVBoxLayout;
    auto* layout_button = new QHBoxLayout;
    layout->addLayout(layout_button);

    m_tree = new QTreeWidget();

    m_tree->setColumnCount(m_headers.size());
    m_tree->setHeaderLabels(m_headers);

    if(m_enableHideAll)
    {
        // check box "show"
        m_showCheckBox = new QCheckBox(tr("Hide all organs"));
        m_showCheckBox->setObjectName(service_id + "/" + m_showCheckBox->text());
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layout_button->addWidget(m_showCheckBox, 0);
        QObject::connect(m_showCheckBox, &QCheckBox::stateChanged, this, &model_series_list::onShowReconstructions);

        m_checkAllButton = new QPushButton(tr("Check all"));
        m_checkAllButton->setObjectName(service_id + "/" + m_checkAllButton->text());
        layout_button->addWidget(m_checkAllButton, 0);
        QObject::connect(m_checkAllButton, &QPushButton::clicked, this, &model_series_list::onCheckAllCheckBox);

        m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
        m_unCheckAllButton->setObjectName(service_id + "/" + m_unCheckAllButton->text());
        layout_button->addWidget(m_unCheckAllButton, 0);
        QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this, &model_series_list::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        m_deleteAllButton = new QPushButton(tr("Delete all"));
        m_deleteAllButton->setObjectName(service_id + "/" + m_deleteAllButton->text());
        layout_button->addWidget(m_deleteAllButton, 0);
        QObject::connect(m_deleteAllButton, &QPushButton::clicked, this, &model_series_list::onDeleteAllCheckBox);
    }

    layout->addWidget(m_tree, 1);

    qt_container->setLayout(layout);

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
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* const container = qt_container->getQtContainer();

    SIGHT_ASSERT("container not instanced", container);

    auto model_series        = m_modelSeries.lock();
    bool has_reconstructions = !model_series->getReconstructionDB().empty();
    container->setEnabled(has_reconstructions);

    this->fillTree(model_series);
    if(has_reconstructions)
    {
        if(m_showCheckBox != nullptr)
        {
            m_showCheckBox->blockSignals(true);
            const bool show_all_rec =
                model_series->get_field("ShowReconstructions", std::make_shared<data::boolean>(true))->value();
            m_showCheckBox->setCheckState(show_all_rec ? Qt::Unchecked : Qt::Checked);
            m_showCheckBox->blockSignals(false);
        }
    }
}

//------------------------------------------------------------------------------

void model_series_list::fillTree(const data::mt::locked_ptr<data::model_series>& _model_series)
{
    const auto& reconstructions = _model_series->getReconstructionDB();

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
                info << QString::fromStdString(reconstruction->get_structure_type());
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

    const bool item_is_checked = (_item->checkState(0) == Qt::Checked);

    if(rec->getIsVisible() != item_is_checked)
    {
        rec->setIsVisible(item_is_checked);

        data::reconstruction::visibility_modified_signal_t::sptr sig;
        sig = rec->signal<data::reconstruction::visibility_modified_signal_t>(
            data::reconstruction::VISIBILITY_MODIFIED_SIG
        );
        sig->async_emit(item_is_checked);
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
        auto model_series = m_modelSeries.lock();
        data::helper::field helper(model_series.get_shared());
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
    auto model_series = m_modelSeries.lock();

    // Remove all reconstructions.
    data::model_series::reconstruction_vector_t reconstructions = model_series->getReconstructionDB();
    model_series->setReconstructionDB(data::model_series::reconstruction_vector_t());

    // Send the signals.
    auto sig = model_series->signal<data::model_series::reconstructions_removed_signal_t>(
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
        auto* const delete_action = new QAction("Delete");
        QObject::connect(
            delete_action,
            &QAction::triggered,
            this,
            [index, this]()
            {
                auto model_series = m_modelSeries.lock();

                data::model_series::reconstruction_vector_t deleted_reconstructions;

                // Remove reconstruction.
                data::model_series::reconstruction_vector_t reconstructions =
                    model_series->getReconstructionDB();
                const auto rec_it =
                    reconstructions.begin() + index.row();
                const data::reconstruction::sptr reconstruction = *rec_it;
                reconstructions.erase(rec_it);
                model_series->setReconstructionDB(reconstructions);

                // Send the signals.
                deleted_reconstructions.push_back(reconstruction);
                auto sig = model_series->signal<data::model_series::reconstructions_removed_signal_t>(
                    data::model_series::RECONSTRUCTIONS_REMOVED_SIG
                );
                sig->async_emit(deleted_reconstructions);
            });

        QMenu context_menu;
        context_menu.addAction(delete_action);
        context_menu.exec(QCursor::pos());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::model
