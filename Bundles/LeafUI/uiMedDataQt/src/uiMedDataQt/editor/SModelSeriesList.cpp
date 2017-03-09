/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/editor/SModelSeriesList.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/String.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

#include <boost/format.hpp>

#include <QCheckBox>
#include <QGroupBox>
#include <QListWidgetItem>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace uiMedData
{
namespace editor
{

class ValueView
{
public:
    //------------------------------------------------------------------------------

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
    //------------------------------------------------------------------------------

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

const ::fwCom::Signals::SignalKeyType SModelSeriesList::s_REC_DISPLAY_MODIFIED__SIG   = "recDisplayModified";
const ::fwCom::Signals::SignalKeyType SModelSeriesList::s_RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
const ::fwCom::Signals::SignalKeyType SModelSeriesList::s_EMPTIED_SELECTION_SIG       = "emptiedSelection";
const ::fwCom::Slots::SlotKeyType SModelSeriesList::s_SHOW_RECONSTRUCTIONS_SLOT       = "showReconstructions";

SModelSeriesList::SModelSeriesList() throw() :
    m_tree(new QTreeWidget()),
    m_enableHideAll(true)
{
    m_sigRecDisplayModified     = newSignal< RecDisplayModifiedSignalType >( s_REC_DISPLAY_MODIFIED__SIG );
    m_sigReconstructionSelected = newSignal< ReconstructionSelectedSignalType >( s_RECONSTRUCTION_SELECTED_SIG );
    m_sigEmptiedSelection       = newSignal< EmptiedSelectionSignalType >( s_EMPTIED_SELECTION_SIG );

    newSlot(s_SHOW_RECONSTRUCTIONS_SLOT, &SModelSeriesList::showReconstructions, this);
}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() throw()
{
    for(auto cIt :  m_displayedInfo)
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

    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* layoutButton = new QHBoxLayout;
    layout->addLayout(layoutButton);

    if (m_enableHideAll)
    {
        // check box "show"
        m_showCheckBox = new QCheckBox( tr("Hide all organs"));
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layoutButton->addWidget( m_showCheckBox, 0 );
        QObject::connect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));

        m_checkAllButton = new QPushButton(tr("Check all"));
        layoutButton->addWidget(m_checkAllButton, 0);
        QObject::connect( m_checkAllButton, SIGNAL(clicked()), this, SLOT(onCheckAllCheckBox()) );

        m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
        layoutButton->addWidget(m_unCheckAllButton, 0);
        QObject::connect( m_unCheckAllButton, SIGNAL(clicked()), this, SLOT(onUnCheckAllCheckBox()) );
    }

    layout->addWidget( m_tree, 1 );

    container->setLayout( layout );

    QObject::connect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    this->updating();

    QObject::connect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem*, int )),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, int )));
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if(m_showCheckBox)
    {
        QObject::disconnect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    }

    QObject::disconnect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem*, int )),
                        this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, int )));
    QObject::disconnect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                        this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

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
        m_tree->setColumnCount(static_cast<int>(columns->size()));
        QStringList header;
        for(; cIt != columns->end(); cIt++)
        {
            ValueView* view;
            ::fwRuntime::ConfigurationElement::AttributePair configView = (*cIt)->getSafeAttributeValue("view");
            if(!configView.first)
            {
                view = new ValueView();
            }
            else if (configView.second == ("positive"))
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
    m_tree->blockSignals(true);

    this->updateReconstructions();
    this->refreshVisibility();

    m_tree->blockSignals(false);

}

//------------------------------------------------------------------------------

void SModelSeriesList::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void SModelSeriesList::info( std::ostream& _sstream )
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

//------------------------------------------------------------------------------

void SModelSeriesList::fillTree()
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
    auto& reconstructions = modelSeries->getReconstructionDB();

    if(!m_tree->selectedItems().empty())
    {
        m_sigEmptiedSelection->asyncEmit();
    }

    m_tree->clear();

    // Create items
    for(auto const& reconstruction :  reconstructions)
    {
        QStringList info;
        for(auto const& cIt :  m_displayedInfo)
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

    for(int i = 0; i < m_tree->topLevelItemCount(); i++)
    {
        m_tree->resizeColumnToContents(i);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    SLM_ASSERT( "Current selected item is null", current );
    std::string id = current->data(0, Qt::UserRole).toString().toStdString();

    ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));

    m_sigReconstructionSelected->asyncEmit(rec);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged ( QTreeWidgetItem* current, int column )
{
    this->onOrganChoiceVisibility(current, column);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganChoiceVisibility(QTreeWidgetItem* item, int column)
{
    std::string id = item->data(0, Qt::UserRole).toString().toStdString();
    ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));
    SLM_ASSERT("rec not instanced", rec);

    const bool itemIsChecked = (item->checkState(0) == Qt::Checked);

    if (rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(itemIsChecked);

        ::fwData::Reconstruction::VisibilityModifiedSignalType::sptr sig;
        sig = rec->signal< ::fwData::Reconstruction::VisibilityModifiedSignalType >(
            ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG);
        sig->asyncEmit(itemIsChecked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onShowReconstructions(int state )
{
    const bool visible = static_cast<bool>(state);

    m_checkAllButton->setEnabled(!visible);
    m_unCheckAllButton->setEnabled(!visible);
    m_tree->setEnabled(!visible);

    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
    modelSeries->setField("ShowReconstructions",  ::fwData::Boolean::New(state == Qt::Unchecked) );

    ::fwCom::Connection::Blocker block(m_sigRecDisplayModified->getConnection(m_slotShowReconstuctions));
    m_sigRecDisplayModified->asyncEmit(state == Qt::Unchecked);
}

//------------------------------------------------------------------------------

void SModelSeriesList::refreshVisibility()
{
    for( int i = 0; i < m_tree->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem* item = m_tree->topLevelItem( i );
        std::string id        = item->data(0, Qt::UserRole).toString().toStdString();
        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));
        item->setCheckState(0, rec->getIsVisible() ? Qt::Checked : Qt::Unchecked );
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::showReconstructions(bool show)
{
    if(m_showCheckBox)
    {
        m_showCheckBox->setCheckState(show ? Qt::Unchecked : Qt::Checked );
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SModelSeriesList::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCheckAllCheckBox()
{
    this->onCheckAllBoxes(true);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onUnCheckAllCheckBox()
{
    this->onCheckAllBoxes(false);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCheckAllBoxes( bool visible )
{
    for( int i = 0; i < m_tree->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem* item = m_tree->topLevelItem( i );
        item->setCheckState(0, visible ? Qt::Checked : Qt::Unchecked );
    }
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedData
