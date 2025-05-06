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

#include "model_series_list.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/id.hpp>
#include <core/runtime/path.hpp>

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

static const core::com::signals::key_t RECONSTRUCTION_SELECTED_SIG = "reconstruction_selected";
static const core::com::signals::key_t EMPTIED_SELECTION_SIG       = "emptied_selection";
static const core::com::slots::key_t SHOW_RECONSTRUCTIONS_SLOT     = "showReconstructions";

//------------------------------------------------------------------------------

model_series_list::model_series_list() noexcept
{
    m_sig_reconstruction_selected = new_signal<reconstruction_selected_signal_t>(RECONSTRUCTION_SELECTED_SIG);
    m_sig_emptied_selection       = new_signal<emptied_selection_signal_t>(EMPTIED_SELECTION_SIG);

    new_slot(SHOW_RECONSTRUCTIONS_SLOT, &model_series_list::show_reconstructions, this);
}

//------------------------------------------------------------------------------

void model_series_list::configuring()
{
    this->initialize();

    const config_t config_t = this->get_config();
    const auto config       = config_t.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_enable_hide_all = config->get<bool>("enable_hide_all", m_enable_hide_all);
        m_enable_delete   = config->get<bool>("enable_delete", m_enable_delete);
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

    const QString service_id = QString::fromStdString(base_id());

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    auto* layout        = new QVBoxLayout;
    auto* layout_button = new QHBoxLayout;
    layout->addLayout(layout_button);

    m_tree = new QTreeWidget();

    m_tree->setColumnCount(static_cast<int>(m_headers.size()));
    m_tree->setHeaderLabels(m_headers);

    if(m_enable_hide_all)
    {
        // check box "show"
        m_show_check_box = new QCheckBox(tr("Hide all organs"));
        m_show_check_box->setObjectName(service_id + "/" + m_show_check_box->text());
        m_show_check_box->setToolTip(tr("Show or hide all organs"));
        layout_button->addWidget(m_show_check_box, 0);
        QObject::connect(m_show_check_box, &QCheckBox::stateChanged, this, &self_t::on_show_reconstructions);

        m_check_all_button = new QPushButton(tr("Check all"));
        m_check_all_button->setObjectName(service_id + "/" + m_check_all_button->text());
        layout_button->addWidget(m_check_all_button, 0);
        QObject::connect(m_check_all_button, &QPushButton::clicked, this, &self_t::on_check_all_check_box);

        m_un_check_all_button = new QPushButton(tr("UnCheck all"));
        m_un_check_all_button->setObjectName(service_id + "/" + m_un_check_all_button->text());
        layout_button->addWidget(m_un_check_all_button, 0);
        QObject::connect(m_un_check_all_button, &QPushButton::clicked, this, &self_t::on_un_check_all_check_box);
    }

    if(m_enable_delete)
    {
        m_delete_all_button = new QPushButton(tr("Delete all"));
        m_delete_all_button->setObjectName(service_id + "/" + m_delete_all_button->text());
        layout_button->addWidget(m_delete_all_button, 0);
        QObject::connect(m_delete_all_button, &QPushButton::clicked, this, &self_t::on_delete_all_check_box);
    }

    layout->addWidget(m_tree, 1);

    qt_container->set_layout(layout);

    QObject::connect(
        m_tree,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this,
        SLOT(on_current_item_changed(QTreeWidgetItem*,QTreeWidgetItem*))
    );

    this->updating();

    QObject::connect(
        m_tree,
        SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this,
        SLOT(on_current_item_changed(QTreeWidgetItem*,int))
    );

    if(m_enable_delete)
    {
        m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &model_series_list::on_custom_context_menu_requested
        );
    }
}

//------------------------------------------------------------------------------

service::connections_t model_series_list::auto_connections() const
{
    return {
        {MODEL_SERIES, data::model_series::MODIFIED_SIG, service::slots::UPDATE},
        {MODEL_SERIES, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE},
        {MODEL_SERIES, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void model_series_list::updating()
{
    m_tree->blockSignals(true);

    this->update_reconstructions();
    this->refresh_visibility();

    m_tree->blockSignals(false);
}

//------------------------------------------------------------------------------

void model_series_list::stopping()
{
    if(m_enable_hide_all)
    {
        QObject::disconnect(m_show_check_box, &QCheckBox::stateChanged, this, &self_t::on_show_reconstructions);
        QObject::disconnect(m_check_all_button, &QPushButton::clicked, this, &self_t::on_check_all_check_box);
        QObject::disconnect(m_un_check_all_button, &QPushButton::clicked, this, &self_t::on_un_check_all_check_box);
    }

    if(m_enable_delete)
    {
        QObject::disconnect(m_delete_all_button, &QPushButton::clicked, this, &self_t::on_delete_all_check_box);
        QObject::disconnect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &self_t::on_custom_context_menu_requested
        );
    }

    QObject::disconnect(
        m_tree,
        SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this,
        SLOT(on_current_item_changed(QTreeWidgetItem*,int))
    );

    QObject::disconnect(
        m_tree,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this,
        SLOT(on_current_item_changed(QTreeWidgetItem*,QTreeWidgetItem*))
    );

    this->destroy();
}

//------------------------------------------------------------------------------

void model_series_list::update_reconstructions()
{
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QWidget* const container = qt_container->get_qt_container();

    SIGHT_ASSERT("container not instanced", container);

    auto model_series        = m_model_series.lock();
    bool has_reconstructions = !model_series->get_reconstruction_db().empty();
    container->setEnabled(has_reconstructions);

    this->fill_tree(model_series);
    if(has_reconstructions)
    {
        if(m_show_check_box != nullptr)
        {
            m_show_check_box->blockSignals(true);
            const bool show_all_rec =
                model_series->get_field("ShowReconstructions", std::make_shared<data::boolean>(true))->value();
            m_show_check_box->setCheckState(show_all_rec ? Qt::Unchecked : Qt::Checked);
            m_show_check_box->blockSignals(false);
        }
    }
}

//------------------------------------------------------------------------------

void model_series_list::fill_tree(const data::mt::locked_ptr<data::model_series>& _model_series)
{
    const auto& reconstructions = _model_series->get_reconstruction_db();

    if(!m_tree->selectedItems().empty())
    {
        m_sig_emptied_selection->async_emit();
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
                info << QString::fromStdString(reconstruction->get_organ_name());
            }
            else if(column == "structure_type")
            {
                info << QString::fromStdString(reconstruction->get_structure_type());
            }
            else if(column == "volume")
            {
                auto volume = reconstruction->get_computed_mask_volume();
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

void model_series_list::on_current_item_changed(QTreeWidgetItem* _current, QTreeWidgetItem* /*unused*/)
{
    SIGHT_ASSERT("Current selected item is null", _current);
    std::string id = _current->data(0, Qt::UserRole).toString().toStdString();

    data::reconstruction::sptr rec = std::dynamic_pointer_cast<data::reconstruction>(core::id::get_object(id));

    m_sig_reconstruction_selected->async_emit(rec);
}

//------------------------------------------------------------------------------

void model_series_list::on_current_item_changed(QTreeWidgetItem* _current, int _column)
{
    sight::module::ui::qt::model::model_series_list::on_organ_choice_visibility(_current, _column);
}

//------------------------------------------------------------------------------

void model_series_list::on_organ_choice_visibility(QTreeWidgetItem* _item, int /*unused*/)
{
    std::string id                 = _item->data(0, Qt::UserRole).toString().toStdString();
    data::reconstruction::sptr rec = std::dynamic_pointer_cast<data::reconstruction>(core::id::get_object(id));
    SIGHT_ASSERT("rec not instanced", rec);

    const bool item_is_checked = (_item->checkState(0) == Qt::Checked);

    if(rec->get_is_visible() != item_is_checked)
    {
        rec->set_is_visible(item_is_checked);

        data::reconstruction::visibility_modified_signal_t::sptr sig;
        sig = rec->signal<data::reconstruction::visibility_modified_signal_t>(
            data::reconstruction::VISIBILITY_MODIFIED_SIG
        );
        sig->async_emit(item_is_checked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::on_show_reconstructions(int _state)
{
    const bool visible = static_cast<bool>(_state);

    m_check_all_button->setEnabled(!visible);
    m_un_check_all_button->setEnabled(!visible);
    m_tree->setEnabled(!visible);

    {
        auto model_series = m_model_series.lock();
        data::helper::field helper(model_series.get_shared());
        helper.add_or_swap("ShowReconstructions", std::make_shared<data::boolean>(_state == Qt::Unchecked));
    }
}

//------------------------------------------------------------------------------

void model_series_list::refresh_visibility()
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item          = m_tree->topLevelItem(i);
        std::string id                 = item->data(0, Qt::UserRole).toString().toStdString();
        data::reconstruction::sptr rec =
            std::dynamic_pointer_cast<data::reconstruction>(core::id::get_object(id));
        item->setCheckState(0, rec->get_is_visible() ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::show_reconstructions(bool _show)
{
    if(m_show_check_box != nullptr)
    {
        m_show_check_box->setCheckState(_show ? Qt::Unchecked : Qt::Checked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::on_check_all_check_box()
{
    this->on_check_all_boxes(true);
}

//------------------------------------------------------------------------------

void model_series_list::on_un_check_all_check_box()
{
    this->on_check_all_boxes(false);
}

//------------------------------------------------------------------------------

void model_series_list::on_check_all_boxes(bool _visible)
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item = m_tree->topLevelItem(i);
        item->setCheckState(0, _visible ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void model_series_list::on_delete_all_check_box()
{
    auto model_series = m_model_series.lock();

    // Remove all reconstructions.
    data::model_series::reconstruction_vector_t reconstructions = model_series->get_reconstruction_db();
    model_series->set_reconstruction_db(data::model_series::reconstruction_vector_t());

    // Send the signals.
    auto sig = model_series->signal<data::model_series::reconstructions_removed_signal_t>(
        data::model_series::RECONSTRUCTIONS_REMOVED_SIG
    );
    sig->async_emit(reconstructions);
}

//------------------------------------------------------------------------------

void model_series_list::on_custom_context_menu_requested(const QPoint& _pos)
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
                auto model_series = m_model_series.lock();

                data::model_series::reconstruction_vector_t deleted_reconstructions;

                // Remove reconstruction.
                data::model_series::reconstruction_vector_t reconstructions =
                    model_series->get_reconstruction_db();
                const auto rec_it =
                    reconstructions.begin() + index.row();
                const data::reconstruction::sptr reconstruction = *rec_it;
                reconstructions.erase(rec_it);
                model_series->set_reconstruction_db(reconstructions);

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
