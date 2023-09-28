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

#include "SSelector.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>
#include <ui/qt/series/Selector.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

static const core::com::signals::key_t SERIES_DOUBLE_CLICKED_SIG = "seriesDoubleClicked";

static const core::com::slots::key_t ADD_SERIES_SLOT    = "addSeries";
static const core::com::slots::key_t REMOVE_SERIES_SLOT = "removeSeries";

static const std::string s_SELECTION_MODE_CONFIG    = "selectionMode";
static const std::string s_ALLOWED_REMOVE_CONFIG    = "allowedRemove";
static const std::string s_INSERT_MODE_CONFIG       = "insertMode";
static const std::string s_REMOVE_STUDY_ICON_CONFIG = "removeStudyIcon";
static const std::string s_REMOVE_SERIE_ICON_CONFIG = "removeSerieIcon";
static const std::string s_DISPLAYED_COLUMN_CONFIG  = "displayedColumns";

//------------------------------------------------------------------------------

SSelector::SSelector()
{
    m_sigSeriesDoubleClicked = new_signal<SeriesDoubleClickedSignalType>(SERIES_DOUBLE_CLICKED_SIG);

    new_slot(ADD_SERIES_SLOT, &SSelector::addSeries, this);
    m_slotRemoveSeries = new_slot(REMOVE_SERIES_SLOT, &SSelector::removeSeries, this);
}

//------------------------------------------------------------------------------

SSelector::~SSelector() noexcept =
    default;

//------------------------------------------------------------------------------

void SSelector::configuring()
{
    this->sight::ui::service::initialize();

    const ConfigType tree = this->getConfiguration();

    if(const auto& icons = tree.get_child_optional("icons"); icons.has_value())
    {
        for(const auto& elt : boost::make_iterator_range(icons->equal_range("icon")))
        {
            const auto series = elt.second.get<std::string>("<xmlattr>.series");
            SIGHT_ASSERT("'series' attribute is missing", !series.empty());

            const auto icon = elt.second.get<std::string>("<xmlattr>.icon");
            SIGHT_ASSERT("'icon' attribute is missing", !icon.empty());

            const auto file = core::runtime::get_resource_file_path(icon);
            m_seriesIcons[series] = file.string();
        }
    }

    const auto config = tree.get_child_optional("config");

    if(config)
    {
        const auto configAttr = config->get_child_optional("<xmlattr>");

        const auto removeStudyIconCfg = configAttr->get_optional<std::string>(s_REMOVE_STUDY_ICON_CONFIG);
        if(removeStudyIconCfg)
        {
            m_removeStudyIcon = core::runtime::get_module_resource_file_path(removeStudyIconCfg.value());
        }

        const auto removeSerieIconCfg = configAttr->get_optional<std::string>(s_REMOVE_SERIE_ICON_CONFIG);
        if(removeSerieIconCfg)
        {
            m_removeSeriesIcon = core::runtime::get_module_resource_file_path(removeSerieIconCfg.value());
        }

        const auto selectionMode = configAttr->get<std::string>(s_SELECTION_MODE_CONFIG, "extended");
        if(selectionMode == "single")
        {
            m_selectionMode = QAbstractItemView::SingleSelection;
        }
        else if(selectionMode == "extended")
        {
            m_selectionMode = QAbstractItemView::ExtendedSelection;
        }
        else
        {
            SIGHT_WARN(
                std::string("value ") + selectionMode + " is not managed for '"
                + s_SELECTION_MODE_CONFIG + "'"
            );
        }

        m_removeAllowed = configAttr->get<bool>(s_ALLOWED_REMOVE_CONFIG, m_removeAllowed);
        m_insertMode    = configAttr->get<bool>(s_INSERT_MODE_CONFIG, m_insertMode);

        m_displayedColumns = configAttr->get(s_DISPLAYED_COLUMN_CONFIG, m_displayedColumns);
    }
}

//------------------------------------------------------------------------------

void SSelector::starting()
{
    this->sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );

    m_selectorWidget = new sight::ui::qt::series::Selector(m_displayedColumns);
    m_selectorWidget->setSeriesIcons(m_seriesIcons);
    m_selectorWidget->setSelectionMode(m_selectionMode);
    m_selectorWidget->allowRemove(m_removeAllowed);
    m_selectorWidget->setInsertMode(m_insertMode);
    m_selectorWidget->setRemoveStudyIcon(m_removeStudyIcon);
    m_selectorWidget->setRemoveSeriesIcon(m_removeSeriesIcon);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_selectorWidget);
    qtContainer->setLayout(layout);

    QObject::connect(
        m_selectorWidget,
        SIGNAL(
            seriesSelected(
                QVector<data::Series::sptr>,
                QVector<data::Series::sptr>
            )
        ),
        this,
        SLOT(
            onSelectedSeries(
                QVector<data::Series::sptr>,
                QVector<data::Series::sptr>
            )
        )
    );

    if(!m_insertMode)
    {
        QObject::connect(
            m_selectorWidget,
            SIGNAL(doubleClicked(const QModelIndex&)),
            this,
            SLOT(onDoubleClick(const QModelIndex&))
        );
    }

    if(m_removeAllowed)
    {
        QObject::connect(
            m_selectorWidget,
            SIGNAL(removeSeries(QVector<data::Series::sptr>)),
            this,
            SLOT(onRemoveSeries(QVector<data::Series::sptr>))
        );
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t SSelector::getAutoConnections() const
{
    connections_t connections;
    connections.push(s_SERIES_SET,data::SeriesSet::ADDED_OBJECTS_SIG,ADD_SERIES_SLOT);
    connections.push(s_SERIES_SET,data::SeriesSet::REMOVED_OBJECTS_SIG,REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SSelector::updating()
{
    const auto series_set = m_series_set.lock();

    m_selectorWidget->clear();

    for(const auto& series : *series_set)
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SSelector::onSelectedSeries(
    QVector<data::Series::sptr> _selection,
    QVector<data::Series::sptr> _deselection
)
{
    const auto selectionVector = m_selection.lock();
    const auto scoped_emitter  = selectionVector->scoped_emit();

    for(const auto& series : _deselection)
    {
        selectionVector->remove(series);
    }

    for(const auto& series : _selection)
    {
        selectionVector->push_back(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::onDoubleClick(const QModelIndex& _index)
{
    m_selectorWidget->clearSelection();
    m_selectorWidget->setCurrentIndex(_index);

    const auto selectionVector = m_selection.lock();

    if(m_selectorWidget->getItemType(_index) == sight::ui::qt::series::SelectorModel::SERIES)
    {
        SIGHT_ASSERT("There must be only one object selected",selectionVector->size() == 1);
        data::Object::sptr obj    = selectionVector->front();
        data::Series::sptr series = std::dynamic_pointer_cast<data::Series>(obj);
        SIGHT_ASSERT("Object must be a 'data::Series'",series);

        m_sigSeriesDoubleClicked->async_emit(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::onRemoveSeries(QVector<data::Series::sptr> _selection)
{
    const auto series_set = m_series_set.lock();

    auto sig = series_set->signal<data::SeriesSet::removed_signal_t>(data::SeriesSet::REMOVED_OBJECTS_SIG);
    core::com::connection::blocker block(sig->get_connection(m_slotRemoveSeries));

    {
        const auto scoped_emitter = series_set->scoped_emit();

        for(const auto& series : _selection)
        {
            series_set->remove(series);
        }
    }
}

//------------------------------------------------------------------------------

void SSelector::addSeries(data::SeriesSet::container_type _addedSeries)
{
    for(const auto& series : _addedSeries)
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::removeSeries(data::SeriesSet::container_type _removedSeries)
{
    for(const auto& series : _removedSeries)
    {
        m_selectorWidget->removeSeries(series);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
