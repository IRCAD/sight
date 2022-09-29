/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/ui/qt/series/Selector.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/operations.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType s_SERIES_DOUBLE_CLICKED_SIG = "seriesDoubleClicked";

static const core::com::Slots::SlotKeyType s_ADD_SERIES_SLOT    = "addSeries";
static const core::com::Slots::SlotKeyType s_REMOVE_SERIES_SLOT = "removeSeries";

static const std::string s_SELECTION_MODE_CONFIG    = "selectionMode";
static const std::string s_ALLOWED_REMOVE_CONFIG    = "allowedRemove";
static const std::string s_INSERT_MODE_CONFIG       = "insertMode";
static const std::string s_ICONS_CONFIG             = "icons";
static const std::string s_ICON_CONFIG              = "icon";
static const std::string s_REMOVE_STUDY_ICON_CONFIG = "removeStudyIcon";
static const std::string s_REMOVE_SERIE_ICON_CONFIG = "removeSerieIcon";

//------------------------------------------------------------------------------

SSelector::SSelector()
{
    m_sigSeriesDoubleClicked = newSignal<SeriesDoubleClickedSignalType>(s_SERIES_DOUBLE_CLICKED_SIG);

    newSlot(s_ADD_SERIES_SLOT, &SSelector::addSeries, this);
    m_slotRemoveSeries = newSlot(s_REMOVE_SERIES_SLOT, &SSelector::removeSeries, this);
}

//------------------------------------------------------------------------------

SSelector::~SSelector() noexcept =
    default;

//------------------------------------------------------------------------------

void SSelector::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();

    std::vector<core::runtime::ConfigurationElement::sptr> iconsCfg = m_configuration->find(s_ICONS_CONFIG);
    if(!iconsCfg.empty())
    {
        SIGHT_ASSERT("Only one 'config' tag is allowed for SSelector configuration", iconsCfg.size() == 1);

        std::vector<core::runtime::ConfigurationElement::sptr> cfg = iconsCfg.front()->find(s_ICON_CONFIG);

        for(const core::runtime::ConfigurationElement::sptr& elt : cfg)
        {
            const std::string series = elt->getAttributeValue("series");
            SIGHT_ASSERT("'series' attribute is missing", !series.empty());

            const std::string icon = elt->getAttributeValue("icon");
            SIGHT_ASSERT("'icon' attribute is missing", !icon.empty());

            const auto file = core::runtime::getResourceFilePath(icon);
            m_seriesIcons[series] = file.string();
        }
    }

    const ConfigType tree = this->getConfigTree();
    const auto config     = tree.get_child_optional("config");

    if(config)
    {
        const auto configAttr = config->get_child_optional("<xmlattr>");

        const auto removeStudyIconCfg = configAttr->get_optional<std::string>(s_REMOVE_STUDY_ICON_CONFIG);
        if(removeStudyIconCfg)
        {
            m_removeStudyIcon = core::runtime::getModuleResourceFilePath(removeStudyIconCfg.value());
        }

        const auto removeSerieIconCfg = configAttr->get_optional<std::string>(s_REMOVE_SERIE_ICON_CONFIG);
        if(removeSerieIconCfg)
        {
            m_removeSerieIcon = core::runtime::getModuleResourceFilePath(removeSerieIconCfg.value());
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

        m_allowedRemove = configAttr->get<bool>(s_ALLOWED_REMOVE_CONFIG, m_allowedRemove);
        m_insertMode    = configAttr->get<bool>(s_INSERT_MODE_CONFIG, m_insertMode);
    }
}

//------------------------------------------------------------------------------

void SSelector::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );

    m_selectorWidget = new Selector();
    m_selectorWidget->setSeriesIcons(m_seriesIcons);
    m_selectorWidget->setSelectionMode(m_selectionMode);
    m_selectorWidget->setAllowedRemove(m_allowedRemove);
    m_selectorWidget->setInsertMode(m_insertMode);
    m_selectorWidget->setRemoveStudyIcon(m_removeStudyIcon);
    m_selectorWidget->setRemoveSerieIcon(m_removeSerieIcon);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_selectorWidget);
    qtContainer->setLayout(layout);

    QObject::connect(
        m_selectorWidget,
        SIGNAL(
            selectSeries(
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

    if(m_allowedRemove)
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

service::IService::KeyConnectionsMap SSelector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_SERIES_SET,data::SeriesSet::s_ADDED_OBJECTS_SIG,s_ADD_SERIES_SLOT);
    connections.push(s_SERIES_SET,data::SeriesSet::s_REMOVED_OBJECTS_SIG,s_REMOVE_SERIES_SLOT);

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

    if(m_selectorWidget->getItemType(_index) == SelectorModel::SERIES)
    {
        SIGHT_ASSERT("There must be only one object selected",selectionVector->size() == 1);
        data::Object::sptr obj    = selectionVector->front();
        data::Series::sptr series = data::Series::dynamicCast(obj);
        SIGHT_ASSERT("Object must be a 'data::Series'",series);

        m_sigSeriesDoubleClicked->asyncEmit(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::onRemoveSeries(QVector<data::Series::sptr> _selection)
{
    const auto series_set = m_series_set.lock();

    auto sig = series_set->signal<data::SeriesSet::removed_signal_t>(data::SeriesSet::s_REMOVED_OBJECTS_SIG);
    core::com::Connection::Blocker block(sig->getConnection(m_slotRemoveSeries));

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
