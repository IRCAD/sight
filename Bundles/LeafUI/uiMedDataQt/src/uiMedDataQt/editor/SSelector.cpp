/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QVBoxLayout>

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

fwServicesRegisterMacro( ::gui::editor::IEditor , ::uiMedData::editor::SSelector , ::fwMedData::SeriesDB ) ;

//------------------------------------------------------------------------------

SSelector::SSelector()
{
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

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_selectorWidget = new ::uiMedData::widget::Selector();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_selectorWidget);
    container->setLayout(layout);

    QObject::connect(m_selectorWidget, SIGNAL(selectSeries(QVector< ::fwMedData::Series::sptr >,
                                                           QVector< ::fwMedData::Series::sptr >)),
                     this, SLOT(onSelectedSeries(QVector< ::fwMedData::Series::sptr >,
                                                 QVector< ::fwMedData::Series::sptr >)));

    QObject::connect(m_selectorWidget, SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(onDoubleClick(const QModelIndex &)));

    QObject::connect(m_selectorWidget, SIGNAL(removeSeries(QVector< ::fwMedData::Series::sptr >)),
                     this, SLOT(onRemoveSeries(QVector< ::fwMedData::Series::sptr >)));
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

    std::stringstream str;
    if (m_selectorWidget->getItemType(index) == ::uiMedData::widget::SelectorModel::STUDY)
    {
        str << "Selected study. TODO";
    }
    else if (m_selectorWidget->getItemType(index) == ::uiMedData::widget::SelectorModel::SERIES)
    {
        str << "Selected series : ";
        SLM_ASSERT("There must be only one object selected", selectionVector->size() == 1);
        str << selectionVector->getContainer()[0]->getID() << ".";
    }

    ::fwGui::dialog::MessageDialog::showMessageDialog("Double click",
                                                      str.str());
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

    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_selectionId);
    ::fwData::Vector::sptr selection = ::fwData::Vector::dynamicCast(obj);
    SLM_ASSERT("Object " << m_selectionId << " is not a '::fwData::Vector'", selection);

    return selection;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedData
