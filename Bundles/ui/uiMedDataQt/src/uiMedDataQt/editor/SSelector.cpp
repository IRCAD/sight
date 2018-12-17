/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiMedDataQt::editor::SSelector, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SSelector::s_SERIES_DOUBLE_CLICKED_SIG = "seriesDoubleClicked";

const ::fwCom::Slots::SlotKeyType SSelector::s_ADD_SERIES_SLOT    = "addSeries";
const ::fwCom::Slots::SlotKeyType SSelector::s_REMOVE_SERIES_SLOT = "removeSeries";

//------------------------------------------------------------------------------

SSelector::SSelector() :
    m_allowedRemove(true),
    m_selectionMode(QAbstractItemView::ExtendedSelection),
    m_insertMode(false)
{
    // Init
    m_sigSeriesDoubleClicked = SeriesDoubleClickedSignalType::New();

    // Register
    m_signals( s_SERIES_DOUBLE_CLICKED_SIG,  m_sigSeriesDoubleClicked);

    newSlot(s_ADD_SERIES_SLOT, &SSelector::addSeries, this);
    m_slotRemoveSeries = newSlot(s_REMOVE_SERIES_SLOT, &SSelector::removeSeries, this);

}

//------------------------------------------------------------------------------

SSelector::~SSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SSelector::info(std::ostream& _sstream )
{
    // Update message
    _sstream << std::string("SSelector");
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

    QVBoxLayout* layout = new QVBoxLayout();
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
        SLM_TRACE("CONNECT remove series slot");
        QObject::connect(m_selectorWidget, SIGNAL(removeSeries(QVector< ::fwMedData::Series::sptr >)),
                         this, SLOT(onRemoveSeries(QVector< ::fwMedData::Series::sptr >)));
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SSelector::updating()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >("seriesDB");

    m_selectorWidget->clear();

    for(::fwMedData::Series::sptr series :  seriesDB->getContainer())
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > selectionModeCfg = m_configuration->find("selectionMode");
    if(!selectionModeCfg.empty())
    {
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

    std::vector < ::fwRuntime::ConfigurationElement::sptr > allowedRemoveCfg = m_configuration->find("allowedRemove");
    if(!allowedRemoveCfg.empty())
    {
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

    std::vector < ::fwRuntime::ConfigurationElement::sptr > insertCfg = m_configuration->find("insertMode");
    if(!insertCfg.empty())
    {
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

    std::vector < ::fwRuntime::ConfigurationElement::sptr > iconsCfg = m_configuration->find("icons");
    if (!iconsCfg.empty())
    {
        SLM_ASSERT("Only one 'config' tag is allowed for SSelector configuration", iconsCfg.size() == 1);

        std::vector < ::fwRuntime::ConfigurationElement::sptr > cfg = iconsCfg.front()->find("icon");

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
}

//------------------------------------------------------------------------------

void SSelector::onSelectedSeries(QVector< ::fwMedData::Series::sptr > selection,
                                 QVector< ::fwMedData::Series::sptr > deselection)
{
    ::fwData::Vector::sptr selectionVector = this->getSelection();
    ::fwDataTools::helper::Vector vectorHelper(selectionVector);

    for( ::fwMedData::Series::sptr series :  deselection)
    {
        vectorHelper.remove(series);
    }

    for( ::fwMedData::Series::sptr series :  selection)
    {
        vectorHelper.add(series);
    }

    vectorHelper.notify();
}

//------------------------------------------------------------------------------

void SSelector::onDoubleClick(const QModelIndex& index)
{
    m_selectorWidget->clearSelection();
    m_selectorWidget->setCurrentIndex(index);

    ::fwData::Vector::sptr selectionVector = this->getSelection();

    if (m_selectorWidget->getItemType(index) == ::uiMedDataQt::widget::SelectorModel::STUDY)
    {
        std::stringstream str;
        str << "Selected study. TODO";

        ::fwGui::dialog::MessageDialog::showMessageDialog("Double click",
                                                          str.str());
    }
    else if (m_selectorWidget->getItemType(index) == ::uiMedDataQt::widget::SelectorModel::SERIES)
    {
        SLM_ASSERT("There must be only one object selected", selectionVector->size() == 1);
        ::fwData::Object::sptr obj       = selectionVector->front();
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Object must be a '::fwMedData::Series'", series);

        m_sigSeriesDoubleClicked->asyncEmit(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::onRemoveSeries(QVector< ::fwMedData::Series::sptr > selection)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >("seriesDB");
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);

    // Remove duplicated series
    std::set< ::fwMedData::Series::sptr > seriesSet;
    std::copy(selection.begin(), selection.end(), std::inserter(seriesSet, seriesSet.begin()));

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

::fwData::Vector::sptr SSelector::getSelection()
{
    ::fwData::Vector::sptr selection = this->getInOut< ::fwData::Vector >("selection");
    SLM_ASSERT("Object " << m_selectionId << " is not a '::fwData::Vector'", selection);

    return selection;
}

//------------------------------------------------------------------------------

void SSelector::addSeries(::fwMedData::SeriesDB::ContainerType addedSeries)
{
    for( ::fwMedData::Series::sptr series :  addedSeries )
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::removeSeries(::fwMedData::SeriesDB::ContainerType removedSeries)
{
    for( ::fwMedData::Series::sptr series :  removedSeries )
    {
        m_selectorWidget->removeSeries(series);
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSelector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( "seriesDB", ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG, s_ADD_SERIES_SLOT );
    connections.push( "seriesDB", ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG, s_REMOVE_SERIES_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedDataQt
