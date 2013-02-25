/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
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

#include <fwTools/fwID.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/AcquisitionMsg.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiAcquisitionQt/SOrganSeriesListEditor.hpp"

namespace uiAcquisition
{

fwServicesRegisterMacro( ::gui::editor::IEditor , ::uiAcquisition::SOrganSeriesListEditor , ::fwMedData::ModelSeries) ;


SOrganSeriesListEditor::SOrganSeriesListEditor() throw()
{
    //addNewHandledEvent(::fwComEd::AcquisitionMsg::SHOW_RECONSTRUCTIONS);
    //addNewHandledEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION);
    //addNewHandledEvent(::fwComEd::AcquisitionMsg::REMOVED_RECONSTRUCTIONS);
}

//------------------------------------------------------------------------------

SOrganSeriesListEditor::~SOrganSeriesListEditor() throw()
{}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);

    m_showCheckBox = new QCheckBox( tr("Hide all organs"));
    m_showCheckBox->setToolTip(tr("Show or hide all organs"));
    m_organChoice = new QListWidget() ;

    layout->addWidget( m_showCheckBox, 0 );
    layout->addWidget( m_organChoice, 1 );

    container->setLayout( layout );

    QObject::connect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    QObject::connect(m_organChoice, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(onOrganChoiceVisibility(QListWidgetItem *)));
    QObject::connect(m_organChoice, SIGNAL(currentItemChanged ( QListWidgetItem *, QListWidgetItem * )), this, SLOT(onCurrentItemChanged ( QListWidgetItem * , QListWidgetItem * )));
    this->updating();
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    QObject::disconnect(m_organChoice, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(onOrganChoiceVisibility(QListWidgetItem *)));
    QObject::disconnect(m_organChoice, SIGNAL(currentItemChanged ( QListWidgetItem *, QListWidgetItem * )), this, SLOT(onCurrentItemChanged ( QListWidgetItem * , QListWidgetItem * )));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::updating() throw(::fwTools::Failed)
{
    this->updateReconstructions();
    this->refreshVisibility();
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void SOrganSeriesListEditor::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::AcquisitionMsg::csptr acquisitionMsg = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg ) ;
    if ( acquisitionMsg )
    {
        if ( acquisitionMsg->hasEvent(::fwComEd::AcquisitionMsg::SHOW_RECONSTRUCTIONS) )
        {
            this->updating();
        }
        else if ( acquisitionMsg->hasEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION) )
        {
            this->updating();
        }
        else if ( acquisitionMsg->hasEvent(::fwComEd::AcquisitionMsg::REMOVED_RECONSTRUCTIONS) )
        {
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::updateReconstructions()
{
    m_organChoice->blockSignals(true);
    m_organChoice->clear();
    m_map.clear();
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();

    SLM_ASSERT("container not instanced", container);
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    bool hasReconstructions = !modelSeries->getReconstructionDB().empty();
    container->setEnabled( hasReconstructions );

    if(hasReconstructions)
    {
        BOOST_FOREACH(::fwData::Reconstruction::sptr rec, modelSeries->getReconstructionDB())
        {
            m_map[ rec->getOrganName() ] = rec;
        }

        for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter )
        {
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString((*iter).first), m_organChoice);
            item->setCheckState(Qt::Unchecked);
            m_organChoice->addItem(item);
        }

        const bool showAllRec
            = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

        m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked );
        m_organChoice->setEnabled(m_showCheckBox->checkState() == Qt::Unchecked);
    }

    m_organChoice->blockSignals(false);
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::onCurrentItemChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
    SLM_ASSERT( "Current selected item is null", current );

    QListWidgetItem * currentItem = m_organChoice->currentItem();
    std::string organSelected = currentItem->text().toStdString();

    if( m_map.find(organSelected) != m_map.end() )
    {
        ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
        ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;

        ::fwComEd::AcquisitionMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::AcquisitionMsg::NEW_RECONSTRUCTION_SELECTED, ::fwData::String::New( rec->getID() ) ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);
    }
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::onOrganChoiceVisibility(QListWidgetItem * item )
{
    std::string organSelected = item->text().toStdString();
    ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;
    SLM_ASSERT("rec not instanced", rec);

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

void SOrganSeriesListEditor::onShowReconstructions(int state )
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
    modelSeries->setField("ShowReconstructions",  ::fwData::Boolean::NewSptr(state == Qt::Unchecked) );

    ::fwComEd::AcquisitionMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::AcquisitionMsg::SHOW_RECONSTRUCTIONS );
    ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);

    m_organChoice->setEnabled(state == Qt::Unchecked);
}

//------------------------------------------------------------------------------

void SOrganSeriesListEditor::refreshVisibility()
{
    int item = 0;
    for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter, ++item )
    {
        QListWidgetItem *  currentItem = m_organChoice->item (item);
        currentItem->setCheckState( iter->second->getIsVisible() ? Qt::Checked : Qt::Unchecked );
    }
}

}

