/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "uiMedDataQt/editor/SSelector.hpp"

#include "uiMedDataQt/widget/Selector.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwDataTools/helper/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <QVBoxLayout>

namespace uiMedDataQt
{

namespace editor
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiMedDataQt::editor::SSelector, ::fwMedData::SeriesDB )

//------------------------------------------------------------------------------

static const ::fwCom::Signals::SignalKeyType s_SERIES_DOUBLE_CLICKED_SIG = "seriesDoubleClicked";

static const ::fwCom::Slots::SlotKeyType s_ADD_SERIES_SLOT    = "addSeries";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_SERIES_SLOT = "removeSeries";

static const std::string s_SELECTION_MODE_CONFIG    = "selectionMode";
static const std::string s_ALLOWED_REMOVE_CONFIG    = "allowedRemove";
static const std::string s_INSERT_MODE_CONFIG       = "insertMode";
static const std::string s_ICONS_CONFIG             = "icons";
static const std::string s_ICON_CONFIG              = "icon";
static const std::string s_REMOVE_STUDY_ICON_CONFIG = "removeStudyIcon";
static const std::string s_REMOVE_SERIE_ICON_CONFIG = "removeSerieIcon";

static const ::fwServices::IService::KeyType s_SERIES_DB_INOUT = "seriesDB";
static const ::fwServices::IService::KeyType s_SELECTION_INOUT = "selection";

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
    this->::fwGui::IGuiContainerSrv::initialize();

    // Deprecated configuration.

    std::vector < ::fwRuntime::ConfigurationElement::sptr > selectionModeCfg =
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

    std::vector < ::fwRuntime::ConfigurationElement::sptr > allowedRemoveCfg =
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

    std::vector < ::fwRuntime::ConfigurationElement::sptr > insertCfg =
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

    std::vector < ::fwRuntime::ConfigurationElement::sptr > iconsCfg = m_configuration->find(s_ICONS_CONFIG);
    if (!iconsCfg.empty())
    {
        SLM_ASSERT("Only one 'config' tag is allowed for SSelector configuration", iconsCfg.size() == 1);

        std::vector < ::fwRuntime::ConfigurationElement::sptr > cfg = iconsCfg.front()->find(s_ICON_CONFIG);

        for(::fwRuntime::ConfigurationElement::sptr elt :  cfg)
        {
            const std::string series = elt->getAttributeValue("series");
            SLM_ASSERT("'series' attribute is missing", !series.empty());

            const std::string icon = elt->getAttributeValue("icon");
            SLM_ASSERT("'icon' attribute is missing", !icon.empty());

            const auto file = ::fwRuntime::getResourceFilePath(icon);
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
            m_removeStudyIcon = ::fwRuntime::getModuleResourceFilePath(removeStudyIconCfg.value());
        }

        const auto removeSerieIconCfg = configAttr->get_optional< std::string >(s_REMOVE_SERIE_ICON_CONFIG);
        if(removeSerieIconCfg)
        {
            m_removeSerieIcon = ::fwRuntime::getModuleResourceFilePath(removeSerieIconCfg.value());
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
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    m_selectorWidget = new ::uiMedDataQt::widget::Selector();
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

    QObject::connect(m_selectorWidget, SIGNAL(selectSeries(QVector< ::fwMedData::Series::sptr >,
                                                           QVector< ::fwMedData::Series::sptr >)),
                     this, SLOT(onSelectedSeries(QVector< ::fwMedData::Series::sptr >,
                                                 QVector< ::fwMedData::Series::sptr >)));

    if(!m_insertMode)
    {
        QObject::connect(m_selectorWidget, SIGNAL(doubleClicked(const QModelIndex&)),
                         this, SLOT(onDoubleClick(const QModelIndex&)));
    }

    if(m_allowedRemove)
    {
        QObject::connect(m_selectorWidget, SIGNAL(removeSeries(QVector< ::fwMedData::Series::sptr >)),
                         this, SLOT(onRemoveSeries(QVector< ::fwMedData::Series::sptr >)));
    }

    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSelector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_SERIES_DB_INOUT, ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG, s_ADD_SERIES_SLOT);
    connections.push(s_SERIES_DB_INOUT, ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG, s_REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SSelector::updating()
{
    const auto seriesDB = this->getLockedInOut< ::fwMedData::SeriesDB >(s_SERIES_DB_INOUT);

    m_selectorWidget->clear();

    for(::fwMedData::Series::sptr series : seriesDB->getContainer())
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

void SSelector::onSelectedSeries(QVector< ::fwMedData::Series::sptr > _selection,
                                 QVector< ::fwMedData::Series::sptr > _deselection)
{
    const auto selectionVector = this->getLockedInOut< ::fwData::Vector >(s_SELECTION_INOUT);
    ::fwDataTools::helper::Vector vectorHelper(selectionVector.get_shared());

    for( ::fwMedData::Series::sptr series :  _deselection)
    {
        vectorHelper.remove(series);
    }

    for( ::fwMedData::Series::sptr series :  _selection)
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

    const auto selectionVector = this->getLockedInOut< ::fwData::Vector >(s_SELECTION_INOUT);

    if(m_selectorWidget->getItemType(_index) == ::uiMedDataQt::widget::SelectorModel::SERIES)
    {
        SLM_ASSERT("There must be only one object selected", selectionVector->size() == 1);
        ::fwData::Object::sptr obj       = selectionVector->front();
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Object must be a '::fwMedData::Series'", series);

        m_sigSeriesDoubleClicked->asyncEmit(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::onRemoveSeries(QVector< ::fwMedData::Series::sptr > _selection)
{
    const auto seriesDB = this->getLockedInOut< ::fwMedData::SeriesDB >(s_SERIES_DB_INOUT);
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB.get_shared());

    // Remove duplicated series
    std::set< ::fwMedData::Series::sptr > seriesSet;
    std::copy(_selection.begin(), _selection.end(), std::inserter(seriesSet, seriesSet.begin()));

    for( ::fwMedData::Series::sptr series :  seriesSet)
    {
        seriesDBHelper.remove(series);
    }

    {
        auto sig = seriesDB->signal< ::fwMedData::SeriesDB::RemovedSeriesSignalType >(
            ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG );
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotRemoveSeries));
        seriesDBHelper.notify();
    }
}

//------------------------------------------------------------------------------

void SSelector::addSeries(::fwMedData::SeriesDB::ContainerType _addedSeries)
{
    for(::fwMedData::Series::sptr series : _addedSeries)
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::removeSeries(::fwMedData::SeriesDB::ContainerType _removedSeries)
{
    for(::fwMedData::Series::sptr series : _removedSeries)
    {
        m_selectorWidget->removeSeries(series);
    }
}

//------------------------------------------------------------------------------

} // namespace editor.
} // namespace uiMedDataQt.
