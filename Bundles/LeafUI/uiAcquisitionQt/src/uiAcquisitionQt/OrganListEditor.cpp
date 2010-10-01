/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QString>
#include <QVBoxLayout>
#include <QGroupBox>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/fwID.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>

#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/AcquisitionMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiAcquisitionQt/OrganListEditor.hpp"

namespace uiAcquisition
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiAcquisition::OrganListEditor , ::fwData::Acquisition ) ;


OrganListEditor::OrganListEditor() throw()
{
    addNewHandledEvent("ShowReconstructions");
    addNewHandledEvent("SelectReconstruction");
    addNewHandledEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION);
}

//------------------------------------------------------------------------------

OrganListEditor::~OrganListEditor() throw()
{}

//------------------------------------------------------------------------------

void OrganListEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    assert( container ) ;

    QVBoxLayout* layout = new QVBoxLayout(container);

    QGroupBox* groupBox = new QGroupBox(tr("Organs"), container );
    layout->addWidget( groupBox);

    QVBoxLayout* layoutGroupBox = new QVBoxLayout(container);
    groupBox->setLayout(layoutGroupBox);

    m_showCheckBox = new QCheckBox( tr("Hide all organs"), groupBox);
    m_showCheckBox->setToolTip(tr("Show or hide all organs"));
    m_organChoice = new QListWidget( groupBox) ;

    layoutGroupBox->addWidget( m_showCheckBox, 0 );
    layoutGroupBox->addWidget( m_organChoice, 1);

    container->setLayout( layout );

    QObject::connect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    QObject::connect(m_organChoice, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(onOrganChoiceVisibility(QListWidgetItem *)));
    QObject::connect(m_organChoice, SIGNAL(itemSelectionChanged()), this, SLOT(onOrganChoiceSelection()));
    this->updating();
}

//------------------------------------------------------------------------------

void OrganListEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    QObject::disconnect(m_organChoice, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(onOrganChoiceVisibility(QListWidgetItem *)));
    QObject::disconnect(m_organChoice, SIGNAL(itemSelectionChanged()), this, SLOT(onOrganChoiceSelection()));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void OrganListEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void OrganListEditor::updating() throw(::fwTools::Failed)
{
    this->updateReconstructions();
    this->refreshVisibility();
}

//------------------------------------------------------------------------------

void OrganListEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void OrganListEditor::updating( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::AcquisitionMsg::csptr acquisitionMsg = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg ) ;
    if ( acquisitionMsg )
    {
        if ( acquisitionMsg->hasEvent("ShowReconstructions") ||
                acquisitionMsg->hasEvent("SelectReconstruction") ||
                acquisitionMsg->hasEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION) )
        {
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void OrganListEditor::updateReconstructions()
{
    m_organChoice->clear();
    m_map.clear();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();

    assert( container ) ;
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    container->setEnabled(acq->getReconstructions().first != acq->getReconstructions().second);

    if(acq->getReconstructions().first != acq->getReconstructions().second)
    {
        ::fwData::Acquisition::ReconstructionIterator iter =  acq->getReconstructions().first;

        for (; iter!=  acq->getReconstructions().second ; ++iter )
        {
            m_map[ (*iter)->getOrganName() ] = (*iter);
        }

        for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter )
        {
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString((*iter).first), m_organChoice);
            item->setCheckState(Qt::Unchecked);
            m_organChoice->addItem (item);
        }

        bool showAllRec = true;
        if (acq->getFieldSize("ShowReconstructions"))
        {
            showAllRec = acq->getFieldSingleElement< ::fwData::Boolean >("ShowReconstructions")->value();
        }
        m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked );
        m_organChoice->setEnabled(m_showCheckBox->checkState() == Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::onOrganChoiceSelection()
{
    this->notifyOrganChoiceSelection();
}

//------------------------------------------------------------------------------

void OrganListEditor::notifyOrganChoiceSelection()
{
    std::string organSelected = m_organChoice->currentItem()->text().toStdString();

    if(m_map.find(organSelected) != m_map.end())
    {
        ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
        ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;

        ::fwComEd::AcquisitionMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::AcquisitionMsg::NEW_RECONSTRUCTION_SELECTED, ::fwData::String::New( rec->getID() ) ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::onOrganChoiceVisibility(QListWidgetItem * item )
{
    std::string organSelected = item->text().toStdString();
    ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;
    assert(rec) ;

    bool itemIsChecked = (item->checkState() == Qt::Checked);

    if (rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(item->checkState());

        ::fwComEd::ReconstructionMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::ReconstructionMsg::VISIBILITY ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), rec, msg);
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::onShowReconstructions(int state )
{
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    acq->setFieldSingleElement("ShowReconstructions",  ::fwData::Boolean::NewSptr(state == Qt::Unchecked) );

    ::fwComEd::AcquisitionMsg::NewSptr msg;
    msg->addEvent( "ShowReconstructions" );
    ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);

    m_organChoice->setEnabled(state == Qt::Unchecked);
}

//------------------------------------------------------------------------------

void OrganListEditor::refreshVisibility()
{
    int item=0;
    for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter, ++item )
    {
        QListWidgetItem *  currentItem = m_organChoice->item (item);
        currentItem->setCheckState( iter->second->getIsVisible() ? Qt::Checked : Qt::Unchecked );
    }
}

}

