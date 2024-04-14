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

#include "selector.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>
#include <ui/qt/series/selector.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

static const core::com::signals::key_t SERIES_DOUBLE_CLICKED_SIG = "series_double_clicked";

static const core::com::slots::key_t ADD_SERIES_SLOT    = "addSeries";
static const core::com::slots::key_t REMOVE_SERIES_SLOT = "removeSeries";

static const std::string SELECTION_MODE_CONFIG    = "selectionMode";
static const std::string ALLOWED_REMOVE_CONFIG    = "allowedRemove";
static const std::string INSERT_MODE_CONFIG       = "insertMode";
static const std::string REMOVE_STUDY_ICON_CONFIG = "removeStudyIcon";
static const std::string REMOVE_SERIE_ICON_CONFIG = "removeSerieIcon";
static const std::string DISPLAYED_COLUMN_CONFIG  = "displayedColumns";

//------------------------------------------------------------------------------

selector::selector()
{
    m_sig_series_double_clicked = new_signal<series_double_clicked_signal_t>(SERIES_DOUBLE_CLICKED_SIG);

    new_slot(ADD_SERIES_SLOT, &selector::add_series, this);
    m_slot_remove_series = new_slot(REMOVE_SERIES_SLOT, &selector::remove_series, this);
}

//------------------------------------------------------------------------------

selector::~selector() noexcept =
    default;

//------------------------------------------------------------------------------

void selector::configuring()
{
    this->sight::ui::service::initialize();

    const config_t tree = this->get_config();

    if(const auto& icons = tree.get_child_optional("icons"); icons.has_value())
    {
        for(const auto& elt : boost::make_iterator_range(icons->equal_range("icon")))
        {
            const auto series = elt.second.get<std::string>("<xmlattr>.series");
            SIGHT_ASSERT("'series' attribute is missing", !series.empty());

            const auto icon = elt.second.get<std::string>("<xmlattr>.icon");
            SIGHT_ASSERT("'icon' attribute is missing", !icon.empty());

            const auto file = core::runtime::get_resource_file_path(icon);
            m_series_icons[series] = file.string();
        }
    }

    const auto config = tree.get_child_optional("config");

    if(config)
    {
        const auto config_attr = config->get_child_optional("<xmlattr>");

        const auto remove_study_icon_cfg = config_attr->get_optional<std::string>(REMOVE_STUDY_ICON_CONFIG);
        if(remove_study_icon_cfg)
        {
            m_remove_study_icon = core::runtime::get_module_resource_file_path(remove_study_icon_cfg.value());
        }

        const auto remove_serie_icon_cfg = config_attr->get_optional<std::string>(REMOVE_SERIE_ICON_CONFIG);
        if(remove_serie_icon_cfg)
        {
            m_remove_series_icon = core::runtime::get_module_resource_file_path(remove_serie_icon_cfg.value());
        }

        const auto selection_mode = config_attr->get<std::string>(SELECTION_MODE_CONFIG, "extended");
        if(selection_mode == "single")
        {
            m_selection_mode = QAbstractItemView::SingleSelection;
        }
        else if(selection_mode == "extended")
        {
            m_selection_mode = QAbstractItemView::ExtendedSelection;
        }
        else
        {
            SIGHT_WARN(
                std::string("value ") + selection_mode + " is not managed for '"
                + SELECTION_MODE_CONFIG + "'"
            );
        }

        m_remove_allowed = config_attr->get<bool>(ALLOWED_REMOVE_CONFIG, m_remove_allowed);
        m_insert_mode    = config_attr->get<bool>(INSERT_MODE_CONFIG, m_insert_mode);

        m_displayed_columns = config_attr->get(DISPLAYED_COLUMN_CONFIG, m_displayed_columns);
    }
}

//------------------------------------------------------------------------------

void selector::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    m_selector_widget = new sight::ui::qt::series::selector(m_displayed_columns);
    m_selector_widget->set_series_icons(m_series_icons);
    m_selector_widget->setSelectionMode(m_selection_mode);
    m_selector_widget->allow_remove(m_remove_allowed);
    m_selector_widget->set_insert_mode(m_insert_mode);
    m_selector_widget->set_remove_study_icon(m_remove_study_icon);
    m_selector_widget->set_remove_series_icon(m_remove_series_icon);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_selector_widget);
    qt_container->set_layout(layout);

    QObject::connect(
        m_selector_widget,
        SIGNAL(
            series_selected(
                QVector<data::series::sptr>,
                QVector<data::series::sptr>
            )
        ),
        this,
        SLOT(
            on_selected_series(
                QVector<data::series::sptr>,
                QVector<data::series::sptr>
            )
        )
    );

    if(!m_insert_mode)
    {
        QObject::connect(
            m_selector_widget,
            SIGNAL(doubleClicked(const QModelIndex&)),
            this,
            SLOT(on_double_click(const QModelIndex&))
        );
    }

    if(m_remove_allowed)
    {
        QObject::connect(
            m_selector_widget,
            SIGNAL(remove_series(QVector<data::series::sptr>)),
            this,
            SLOT(on_remove_series(QVector<data::series::sptr>))
        );
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t selector::auto_connections() const
{
    connections_t connections;
    connections.push(SERIES_SET,data::series_set::ADDED_OBJECTS_SIG,ADD_SERIES_SLOT);
    connections.push(SERIES_SET,data::series_set::REMOVED_OBJECTS_SIG,REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void selector::updating()
{
    const auto series_set = m_series_set.lock();

    m_selector_widget->clear();

    for(const auto& series : *series_set)
    {
        m_selector_widget->add_series(series);
    }
}

//------------------------------------------------------------------------------

void selector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void selector::on_selected_series(
    QVector<data::series::sptr> _selection,
    QVector<data::series::sptr> _deselection
)
{
    const auto selection_vector = m_selection.lock();
    const auto scoped_emitter   = selection_vector->scoped_emit();

    for(const auto& series : _deselection)
    {
        selection_vector->remove(series);
    }

    for(const auto& series : _selection)
    {
        selection_vector->push_back(series);
    }
}

//------------------------------------------------------------------------------

void selector::on_double_click(const QModelIndex& _index)
{
    m_selector_widget->clearSelection();
    m_selector_widget->setCurrentIndex(_index);

    const auto selection_vector = m_selection.lock();

    if(m_selector_widget->get_item_type(_index) == sight::ui::qt::series::selector_model::series)
    {
        SIGHT_ASSERT("There must be only one object selected",selection_vector->size() == 1);
        data::object::sptr obj    = selection_vector->front();
        data::series::sptr series = std::dynamic_pointer_cast<data::series>(obj);
        SIGHT_ASSERT("Object must be a 'data::series'",series);

        m_sig_series_double_clicked->async_emit(series);
    }
}

//------------------------------------------------------------------------------

void selector::on_remove_series(QVector<data::series::sptr> _selection)
{
    const auto series_set = m_series_set.lock();

    auto sig = series_set->signal<data::series_set::removed_signal_t>(data::series_set::REMOVED_OBJECTS_SIG);
    core::com::connection::blocker block(sig->get_connection(m_slot_remove_series));

    {
        const auto scoped_emitter = series_set->scoped_emit();

        for(const auto& series : _selection)
        {
            series_set->remove(series);
        }
    }
}

//------------------------------------------------------------------------------

void selector::add_series(data::series_set::container_t _added_series)
{
    for(const auto& series : _added_series)
    {
        m_selector_widget->add_series(series);
    }
}

//------------------------------------------------------------------------------

void selector::remove_series(data::series_set::container_t _removed_series)
{
    for(const auto& series : _removed_series)
    {
        m_selector_widget->remove_series(series);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
