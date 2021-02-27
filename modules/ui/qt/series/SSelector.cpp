/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include <data/SeriesDB.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/helper/Vector.hpp>

#include <service/macros.hpp>

#include <QVBoxLayout>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace sight::module::ui::qt
{

namespace series
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

static const service::IService::KeyType s_SERIES_DB_INOUT = "seriesDB";
static const service::IService::KeyType s_SELECTION_INOUT = "selection";

//------------------------------------------------------------------------------

SSelector::SSelector()
{
    m_sigSeriesDoubleClicked = newSignal<SeriesDoubleClickedSignalType>(s_SERIES_DOUBLE_CLICKED_SIG);

    newSlot(s_ADD_SERIES_SLOT, &SSelector::addSeries, this);
    m_slotRemoveSeries = newSlot(s_REMOVE_SERIES_SLOT, &SSelector::removeSeries, this);
}

//------------------------------------------------------------------------------

SSelector::~SSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SSelector::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();

    // Deprecated configuration.

    std::vector < core::runtime::ConfigurationElement::sptr > selectionModeCfg =
        m_configuration->find(s_SELECTION_MODE_CONFIG);
    if(!selectionModeCfg.empty())
    {
        FW_DEPRECATED_MSG("'" << s_SELECTION_MODE_CONFIG << "' is deprecated, use it in a <config> tag.", "22.0");

        const std::string& selectionMode = selectionModeCfg.front()->getValue();

        if(!selectionMode.empty())
        {
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
                SLM_WARN("value " + selectionMode + " is not managed for <selectionMode>");
            }
        }
    }

    std::vector < core::runtime::ConfigurationElement::sptr > allowedRemoveCfg =
        m_configuration->find(s_ALLOWED_REMOVE_CONFIG);
    if(!allowedRemoveCfg.empty())
    {
        FW_DEPRECATED_MSG("'" << s_ALLOWED_REMOVE_CONFIG << "' is deprecated, use it in a <config> tag.", "22.0");

        const std::string& allowedRemove = allowedRemoveCfg.front()->getValue();

        if(allowedRemove == "yes")
        {
            m_allowedRemove = true;
        }
        else if(allowedRemove == "no")
        {
            m_allowedRemove = false;
        }
        else
        {
            SLM_WARN("value " + allowedRemove + " is not managed for <allowedRemove>");
        }
    }

    std::vector < core::runtime::ConfigurationElement::sptr > insertCfg =
        m_configuration->find(s_INSERT_MODE_CONFIG);
    if(!insertCfg.empty())
    {
        FW_DEPRECATED_MSG("'" << s_INSERT_MODE_CONFIG << "' is deprecated, use it in a <config> tag.", "22.0");

        const std::string& insert = insertCfg.front()->getValue();

        if(insert == "yes")
        {
            m_insertMode = true;
        }
        else if(insert == "no")
        {
            m_insertMode = false;
        }
        else
        {
            SLM_WARN("value " + insert + " is not managed for <insertMode>");
        }
    }

    // Better configuration.

    std::vector < core::runtime::ConfigurationElement::sptr > iconsCfg = m_configuration->find(s_ICONS_CONFIG);
    if (!iconsCfg.empty())
    {
        SLM_ASSERT("Only one 'config' tag is allowed for SSelector configuration", iconsCfg.size() == 1);

        std::vector < core::runtime::ConfigurationElement::sptr > cfg = iconsCfg.front()->find(s_ICON_CONFIG);

        for(core::runtime::ConfigurationElement::sptr elt :  cfg)
        {
            const std::string series = elt->getAttributeValue("series");
            SLM_ASSERT("'series' attribute is missing", !series.empty());

            const std::string icon = elt->getAttributeValue("icon");
            SLM_ASSERT("'icon' attribute is missing", !icon.empty());

            const auto file = core::runtime::getResourceFilePath(icon);
            m_seriesIcons[series] = file.string();
        }
    }

    const ConfigType tree = this->getConfigTree();
    const auto config     = tree.get_child_optional("config");

    if(config)
    {
        const auto configAttr = config->get_child_optional("<xmlattr>");

        const auto removeStudyIconCfg = configAttr->get_optional< std::string >(s_REMOVE_STUDY_ICON_CONFIG);
        if(removeStudyIconCfg)
        {
            m_removeStudyIcon = core::runtime::getModuleResourceFilePath(removeStudyIconCfg.value());
        }

        const auto removeSerieIconCfg = configAttr->get_optional< std::string >(s_REMOVE_SERIE_ICON_CONFIG);
        if(removeSerieIconCfg)
        {
            m_removeSerieIcon = core::runtime::getModuleResourceFilePath(removeSerieIconCfg.value());
        }

        const auto selectionMode = configAttr->get< std::string >(s_SELECTION_MODE_CONFIG, "extended");
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
            SLM_WARN("value " + selectionMode + " is not managed for '" + s_SELECTION_MODE_CONFIG + "'");
        }

        m_allowedRemove = configAttr->get< bool >(s_ALLOWED_REMOVE_CONFIG, m_allowedRemove);
        m_insertMode    = configAttr->get< bool >(s_INSERT_MODE_CONFIG, m_insertMode);
    }
}

//------------------------------------------------------------------------------

void SSelector::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );

    m_selectorWidget = new Selector();
    m_selectorWidget->setSeriesIcons(m_seriesIcons);
    m_selectorWidget->setSelectionMode(m_selectionMode);
    m_selectorWidget->setAllowedRemove(m_allowedRemove);
    m_selectorWidget->setInsertMode(m_insertMode);
    m_selectorWidget->setRemoveStudyIcon(m_removeStudyIcon);
    m_selectorWidget->setRemoveSerieIcon(m_removeSerieIcon);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_selectorWidget);
    qtContainer->setLayout(layout);

    QObject::connect(m_selectorWidget, SIGNAL(selectSeries(QVector< data::Series::sptr >,
                                                           QVector< data::Series::sptr >)),
                     this, SLOT(onSelectedSeries(QVector< data::Series::sptr >,
                                                 QVector< data::Series::sptr >)));

    if(!m_insertMode)
    {
        QObject::connect(m_selectorWidget, SIGNAL(doubleClicked(const QModelIndex&)),
                         this, SLOT(onDoubleClick(const QModelIndex&)));
    }

    if(m_allowedRemove)
    {
        QObject::connect(m_selectorWidget, SIGNAL(removeSeries(QVector< data::Series::sptr >)),
                         this, SLOT(onRemoveSeries(QVector< data::Series::sptr >)));
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSelector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_SERIES_DB_INOUT, data::SeriesDB::s_ADDED_SERIES_SIG, s_ADD_SERIES_SLOT);
    connections.push(s_SERIES_DB_INOUT, data::SeriesDB::s_REMOVED_SERIES_SIG, s_REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SSelector::updating()
{
    const auto seriesDB = this->getLockedInOut< data::SeriesDB >(s_SERIES_DB_INOUT);

    m_selectorWidget->clear();

    for(data::Series::sptr series : seriesDB->getContainer())
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

void SSelector::onSelectedSeries(QVector< data::Series::sptr > _selection,
                                 QVector< data::Series::sptr > _deselection)
{
    const auto selectionVector = this->getLockedInOut< data::Vector >(s_SELECTION_INOUT);
    data::helper::Vector vectorHelper(selectionVector.get_shared());

    for( data::Series::sptr series :  _deselection)
    {
        vectorHelper.remove(series);
    }

    for( data::Series::sptr series :  _selection)
    {
        vectorHelper.add(series);
    }

    vectorHelper.notify();
}

//------------------------------------------------------------------------------

void SSelector::onDoubleClick(const QModelIndex& _index)
{
    m_selectorWidget->clearSelection();
    m_selectorWidget->setCurrentIndex(_index);

    const auto selectionVector = this->getLockedInOut< data::Vector >(s_SELECTION_INOUT);

    if(m_selectorWidget->getItemType(_index) == SelectorModel::SERIES)
    {
        SLM_ASSERT("There must be only one object selected", selectionVector->size() == 1);
        data::Object::sptr obj    = selectionVector->front();
        data::Series::sptr series = data::Series::dynamicCast(obj);
        SLM_ASSERT("Object must be a 'data::Series'", series);

        m_sigSeriesDoubleClicked->asyncEmit(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::onRemoveSeries(QVector< data::Series::sptr > _selection)
{
    const auto seriesDB = this->getLockedInOut< data::SeriesDB >(s_SERIES_DB_INOUT);
    data::helper::SeriesDB seriesDBHelper(seriesDB.get_shared());

    // Remove duplicated series
    std::set< data::Series::sptr > seriesSet;
    std::copy(_selection.begin(), _selection.end(), std::inserter(seriesSet, seriesSet.begin()));

    for( data::Series::sptr series :  seriesSet)
    {
        seriesDBHelper.remove(series);
    }

    {
        auto sig = seriesDB->signal< data::SeriesDB::RemovedSeriesSignalType >(
            data::SeriesDB::s_REMOVED_SERIES_SIG );
        core::com::Connection::Blocker block(sig->getConnection(m_slotRemoveSeries));
        seriesDBHelper.notify();
    }
}

//------------------------------------------------------------------------------

void SSelector::addSeries(data::SeriesDB::ContainerType _addedSeries)
{
    for(data::Series::sptr series : _addedSeries)
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::removeSeries(data::SeriesDB::ContainerType _removedSeries)
{
    for(data::Series::sptr series : _removedSeries)
    {
        m_selectorWidget->removeSeries(series);
    }
}

//------------------------------------------------------------------------------

} // namespace series.
} // namespace sight::module::ui::qt.
