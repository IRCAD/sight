/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QString>
#include <QVBoxLayout>
#include <QGroupBox>

#include <boost/format.hpp>

#include <boost/format.hpp>

#include <fwCore/base.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/Reconstruction.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Float.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/ModelSeriesMsg.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiMedDataQt/editor/SModelSeriesList.hpp"

namespace uiMedData
{
namespace editor
{

class ValueView
{
public:
    virtual std::string apply(::fwData::Object::sptr obj)
    {
        if(obj->isA("::fwData::String"))
        {
            ::fwData::String::sptr fwValue = ::fwData::String::dynamicCast(obj);
            return fwValue->getValue();
        }
        else if(obj->isA("::fwData::Integer"))
        {
            ::fwData::Integer::sptr fwValue = ::fwData::Integer::dynamicCast(obj);
            return ::boost::lexical_cast<std::string>(fwValue->getValue());
        }
        else if(obj->isA("::fwData::Float"))
        {
            ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(obj);
            return ::boost::lexical_cast<std::string>(fwValue->getValue());
        }
        else
        {
            SLM_WARN(obj->getClassname() +  " is not a printable object  : ");
            return "";
        }
    }
};

class PositiveView : public ValueView
{

public:
    virtual std::string apply(::fwData::Object::sptr obj)
    {
        if(obj->isA("::fwData::Integer"))
        {
            ::fwData::Integer::sptr fwValue = ::fwData::Integer::dynamicCast(obj);

            if(fwValue->getValue() > 0)
            {
                std::stringstream ss;
                ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(obj);
                ss << ::boost::format("%11.2f") % (fwValue->getValue());
                return ss.str();
            }
            return "Unknown";
        }
        else if(obj->isA("::fwData::Float"))
        {
            ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(obj);
            if(fwValue->getValue() > 0)
            {
                std::stringstream ss;
                ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(obj);
                ss << ::boost::format("%11.2f") % (fwValue->getValue());
                return ss.str();
            }
            return "Unknown";
        }
        else
        {
            SLM_WARN(obj->getClassname() +  " is not a printable object  : ");
            return "";
        }
    }
};


fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiMedData::editor::SModelSeriesList, ::fwMedData::ModelSeries);

SModelSeriesList::SModelSeriesList() throw() : m_tree(new QTreeWidget()), m_enableHideAll(true)
{
}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() throw()
{
    BOOST_FOREACH(auto cIt, m_displayedInfo)
    {
        delete cIt.second;
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);


    if (m_enableHideAll)
    {
        m_showCheckBox = new QCheckBox( tr("Hide all organs"));
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layout->addWidget( m_showCheckBox, 0 );
        QObject::connect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    }

    layout->addWidget( m_tree, 1 );

    container->setLayout( layout );

    QObject::connect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    this->updating();

    QObject::connect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, int )));
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if(m_showCheckBox)
    {
        QObject::disconnect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    }

    QObject::disconnect(m_tree, SIGNAL(itemChanged (QTreeWidgetItem * )),
                        this, SLOT(onOrganChoiceVisibility(QTreeWidgetItem *)));
    QObject::disconnect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
                        this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));


    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SModelSeriesList::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    if( m_configuration->findConfigurationElement( "enable_hide_all" ) )
    {
        const std::string& hide = m_configuration->findConfigurationElement("enable_hide_all")->getValue();
        SLM_ASSERT("'enable_hide_all' attribute value must be 'true' or 'false' (found '" + hide + "')",
                   hide == "true" || hide == "false");
        m_enableHideAll = (hide == "true");
    }

    const ::fwRuntime::ConfigurationElement::sptr& columns = m_configuration->findConfigurationElement( "columns" );
    if(columns)
    {
        ::fwRuntime::ConfigurationElement::Container::const_iterator cIt = columns->begin();
        m_tree->setColumnCount(columns->size());
        QStringList header;
        for(; cIt != columns->end(); cIt++)
        {
            ValueView* view;
            ::fwRuntime::ConfigurationElement::AttributePair configView = (*cIt)->getSafeAttributeValue("view");
            if(!configView.first)
            {
                view = new ValueView();
            }
            else if (configView.second ==("positive"))
            {
                view = new PositiveView();
            }

            m_displayedInfo.insert(DisplayedInformation::value_type((*cIt)->getValue(), view));
            header << QString::fromStdString((*cIt)->getName());
        }
        m_tree->setHeaderLabels(header);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::updating() throw(::fwTools::Failed)
{
    QObject::disconnect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
                        this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, int )));

    this->updateReconstructions();
    this->refreshVisibility();

    QObject::connect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, int )));

}

//------------------------------------------------------------------------------

void SModelSeriesList::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void SModelSeriesList::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::ModelSeriesMsg::csptr acquisitionMsg = ::fwComEd::ModelSeriesMsg::dynamicConstCast( msg );
    if ( acquisitionMsg )
    {
        if ( acquisitionMsg->hasEvent(::fwComEd::ModelSeriesMsg::SHOW_RECONSTRUCTIONS) )
        {
            this->updating();
        }
        else if ( acquisitionMsg->hasEvent(::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION) )
        {
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SModelSeriesList::updateReconstructions()
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();

    SLM_ASSERT("container not instanced", container);
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    bool hasReconstructions = !modelSeries->getReconstructionDB().empty();
    container->setEnabled( hasReconstructions );

    if(hasReconstructions)
    {
        this->fillTree();
        if(m_showCheckBox)
        {
            const bool showAllRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();
            m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked );
        }
    }
}


void SModelSeriesList::fillTree()
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
    auto& reconstructions = modelSeries->getReconstructionDB();

    m_tree->clear();

    // Create items
    BOOST_FOREACH(auto const& reconstruction, reconstructions)
    {
        QStringList info;
        BOOST_FOREACH(auto const& cIt, m_displayedInfo)
        {
            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(reconstruction, cIt.first);
            OSLM_ASSERT("Invalid seshat path : '"<< cIt.first <<"'", obj);
            info << QString::fromStdString(cIt.second->apply(obj));
        }
        QTreeWidgetItem* item = new QTreeWidgetItem(info);
        item->setCheckState(0, Qt::Unchecked);
        m_tree->addTopLevelItem(item);
        item->setData(0, Qt::UserRole, QString::fromStdString(reconstruction->getID()));
    }


    for(int i = 0; i< m_tree->topLevelItemCount(); i++)
    {
        m_tree->resizeColumnToContents(i);
    }
}



//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
    SLM_ASSERT( "Current selected item is null", current );
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    std::string id = current->data(0, Qt::UserRole).toString().toStdString();
    ::fwComEd::ModelSeriesMsg::sptr msg = ::fwComEd::ModelSeriesMsg::New();
    msg->addEvent( ::fwComEd::ModelSeriesMsg::NEW_RECONSTRUCTION_SELECTED, ::fwData::String::New(id));
    ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);
}


void SModelSeriesList::onCurrentItemChanged ( QTreeWidgetItem * current, int column )
{
    this->onOrganChoiceVisibility(current, column);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganChoiceVisibility(QTreeWidgetItem * item, int column)
{
    std::string id = item->data(0, Qt::UserRole).toString().toStdString();
    ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));
    SLM_ASSERT("rec not instanced", rec);

    bool itemIsChecked = (item->checkState(0) == Qt::Checked);

    if (rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(item->checkState(0));

        ::fwComEd::ReconstructionMsg::sptr msg = ::fwComEd::ReconstructionMsg::New();
        msg->addEvent( ::fwComEd::ReconstructionMsg::VISIBILITY );
        ::fwServices::IEditionService::notify(this->getSptr(), rec, msg);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onShowReconstructions(int state )
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
    modelSeries->setField("ShowReconstructions",  ::fwData::Boolean::New(state == Qt::Unchecked) );

    ::fwComEd::ModelSeriesMsg::sptr msg = ::fwComEd::ModelSeriesMsg::New();
    msg->addEvent( ::fwComEd::ModelSeriesMsg::SHOW_RECONSTRUCTIONS );
    ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);
}

//------------------------------------------------------------------------------

void SModelSeriesList::refreshVisibility()
{
    for( int i = 0; i < m_tree->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem *item = m_tree->topLevelItem( i );
        std::string id        = item->data(0, Qt::UserRole).toString().toStdString();
        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));
        item->setCheckState(0, rec->getIsVisible() ? Qt::Checked : Qt::Unchecked );
    }
}

} // namespace editor
} // namespace uiMedData
