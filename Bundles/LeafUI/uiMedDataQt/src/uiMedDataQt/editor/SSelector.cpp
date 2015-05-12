/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QVBoxLayout>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/helper/Vector.hpp>
#include <fwComEd/helper/SeriesDB.hpp>
#include <fwComEd/SeriesDBMsg.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include "uiMedDataQt/editor/SSelector.hpp"
#include "uiMedDataQt/widget/Selector.hpp"


namespace uiMedData
{

namespace editor
{
//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiMedData::editor::SSelector, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SSelector::s_SERIES_DOUBLE_CLICKED_SIG = "seriesDoubleClicked";

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

#ifdef COM_LOG
    ::fwCom::HasSignals::m_signals.setID();
#endif
}

//------------------------------------------------------------------------------

SSelector::~SSelector() throw()
{
}

//------------------------------------------------------------------------------

void SSelector::info(std::ostream &_sstream )
{
    // Update message
    _sstream << std::string("SSelector");
}

//------------------------------------------------------------------------------

void SSelector::starting() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_selectorWidget = new ::uiMedData::widget::Selector();
    m_selectorWidget->setSeriesIcons(m_seriesIcons);
    m_selectorWidget->setSelectionMode(m_selectionMode);
    m_selectorWidget->setAllowedRemove(m_allowedRemove);
    m_selectorWidget->setInsertMode(m_insertMode);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_selectorWidget);
    container->setLayout(layout);

    QObject::connect(m_selectorWidget, SIGNAL(selectSeries(QVector< ::fwMedData::Series::sptr >,
                                                           QVector< ::fwMedData::Series::sptr >)),
                     this, SLOT(onSelectedSeries(QVector< ::fwMedData::Series::sptr >,
                                                 QVector< ::fwMedData::Series::sptr >)));

    if(!m_insertMode)
    {
        QObject::connect(m_selectorWidget, SIGNAL(doubleClicked(const QModelIndex &)),
                         this, SLOT(onDoubleClick(const QModelIndex &)));
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

void SSelector::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::SeriesDBMsg::csptr seriesDBMsg = ::fwComEd::SeriesDBMsg::dynamicConstCast(msg);

    if ( seriesDBMsg && seriesDBMsg->hasEvent( ::fwComEd::SeriesDBMsg::ADDED_OBJECTS ) )
    {
        ::fwData::Vector::sptr addedObject = seriesDBMsg->getAddedSeries();
        BOOST_FOREACH( ::fwData::Object::sptr obj, addedObject->getContainer() )
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
            if(series)
            {
                m_selectorWidget->addSeries(series);
            }
        }
    }
    if ( seriesDBMsg && seriesDBMsg->hasEvent( ::fwComEd::SeriesDBMsg::REMOVED_OBJECTS ) )
    {
        ::fwData::Vector::sptr removedObject = seriesDBMsg->getRemovedSeries();
        BOOST_FOREACH( ::fwData::Object::sptr obj, removedObject->getContainer() )
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
            if(series)
            {
                m_selectorWidget->removeSeries(series);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SSelector::stopping() throw(::fwTools::Failed)
{
    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SSelector::updating() throw(::fwTools::Failed)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();

    m_selectorWidget->clear();

    BOOST_FOREACH(::fwMedData::Series::sptr series, seriesDB->getContainer())
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > selectionCfg = m_configuration->find("selectionId");
    SLM_ASSERT("Missing tag 'selectionId'", !selectionCfg.empty());

    m_selectionId = selectionCfg.front()->getValue();
    SLM_ASSERT("selectionId must not be empty", !m_selectionId.empty());

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

        BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, cfg)
        {
            std::string series = elt->getAttributeValue("series");
            SLM_ASSERT("'series' attribute is missing", !series.empty());

            std::string icon = elt->getAttributeValue("icon");
            SLM_ASSERT("'icon' attribute is missing", !icon.empty());

            m_seriesIcons[series] = icon;
        }
    }
}

//------------------------------------------------------------------------------

void SSelector::onSelectedSeries(QVector< ::fwMedData::Series::sptr > selection,
                                 QVector< ::fwMedData::Series::sptr > deselection)
{
    ::fwData::Vector::sptr selectionVector = this->getSelection();
    ::fwComEd::helper::Vector vectorHelper(selectionVector);

    BOOST_FOREACH( ::fwMedData::Series::sptr series, deselection)
    {
        vectorHelper.remove(series);
    }

    BOOST_FOREACH( ::fwMedData::Series::sptr series, selection)
    {
        vectorHelper.add(series);
    }

    vectorHelper.notify(this->getSptr());
}

//------------------------------------------------------------------------------

void SSelector::onDoubleClick(const QModelIndex &index)
{
    m_selectorWidget->clearSelection();
    m_selectorWidget->setCurrentIndex(index);

    ::fwData::Vector::sptr selectionVector = this->getSelection();

    if (m_selectorWidget->getItemType(index) == ::uiMedData::widget::SelectorModel::STUDY)
    {
        std::stringstream str;
        str << "Selected study. TODO";

        ::fwGui::dialog::MessageDialog::showMessageDialog("Double click",
                                                          str.str());
    }
    else if (m_selectorWidget->getItemType(index) == ::uiMedData::widget::SelectorModel::SERIES)
    {
        SLM_ASSERT("There must be only one object selected", selectionVector->size() == 1);
        ::fwData::Object::sptr obj       = selectionVector->front();
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Object must be a '::fwMedData::Series'", series);

        fwServicesNotifyMacro(this->getLightID(), m_sigSeriesDoubleClicked, (series));
    }
}

//------------------------------------------------------------------------------

void SSelector::onRemoveSeries(QVector< ::fwMedData::Series::sptr > selection)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

    // Remove duplicated series
    std::set< ::fwMedData::Series::sptr > seriesSet;
    std::copy(selection.begin(), selection.end(), std::inserter(seriesSet, seriesSet.begin()));

    BOOST_FOREACH( ::fwMedData::Series::sptr series, seriesSet)
    {
        seriesDBHelper.remove(series);
    }

    seriesDBHelper.notify(this->getSptr());
}

//------------------------------------------------------------------------------

::fwData::Vector::sptr SSelector::getSelection()
{
    SLM_ASSERT("Object " << m_selectionId << " doesn't exist", ::fwTools::fwID::exist(m_selectionId));

    ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(m_selectionId);
    ::fwData::Vector::sptr selection = ::fwData::Vector::dynamicCast(obj);
    SLM_ASSERT("Object " << m_selectionId << " is not a '::fwData::Vector'", selection);

    return selection;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedData
