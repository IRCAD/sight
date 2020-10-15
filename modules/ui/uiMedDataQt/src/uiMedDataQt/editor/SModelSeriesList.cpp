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

#include <fwDataTools/helper/Field.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

#include <boost/format.hpp>

#include <QAction>
#include <QCheckBox>
#include <QGroupBox>
#include <QListWidgetItem>
#include <QMenu>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace uiMedDataQt
{
namespace editor
{

class ValueView
{

public:

    //------------------------------------------------------------------------------

    virtual std::string apply(::fwData::Object::sptr _obj)
    {
        if(_obj->isA("::fwData::String"))
        {
            ::fwData::String::sptr fwValue = ::fwData::String::dynamicCast(_obj);
            return fwValue->getValue();
        }
        else if(_obj->isA("::fwData::Integer"))
        {
            ::fwData::Integer::sptr fwValue = ::fwData::Integer::dynamicCast(_obj);
            return ::boost::lexical_cast<std::string>(fwValue->getValue());
        }
        else if(_obj->isA("::fwData::Float"))
        {
            ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(_obj);
            return ::boost::lexical_cast<std::string>(fwValue->getValue());
        }
        else
        {
            SLM_WARN(_obj->getClassname() +  " is not a printable object  : ");
            return "";
        }
    }
};

//------------------------------------------------------------------------------

class PositiveView : public ValueView
{

public:

    //------------------------------------------------------------------------------

    virtual std::string apply(::fwData::Object::sptr _obj)
    {
        if(_obj->isA("::fwData::Integer"))
        {
            ::fwData::Integer::sptr fwIntValue = ::fwData::Integer::dynamicCast(_obj);

            if(fwIntValue->getValue() > 0)
            {
                std::stringstream ss;
                ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(_obj);
                ss << ::boost::format("%11.2f") % (fwValue->getValue());
                return ss.str();
            }
            return "Unknown";
        }
        else if(_obj->isA("::fwData::Float"))
        {
            ::fwData::Float::sptr fwValue = ::fwData::Float::dynamicCast(_obj);
            if(fwValue->getValue() > 0)
            {
                std::stringstream ss;
                ss << ::boost::format("%11.2f") % (fwValue->getValue());
                return ss.str();
            }
            return "Unknown";
        }
        else
        {
            SLM_WARN(_obj->getClassname() +  " is not a printable object  : ");
            return "";
        }
    }
};

//------------------------------------------------------------------------------

fwServicesRegisterMacro(::fwGui::editor::IEditor, ::uiMedDataQt::editor::SModelSeriesList, ::fwMedData::ModelSeries)

static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
static const ::fwCom::Signals::SignalKeyType s_EMPTIED_SELECTION_SIG = "emptiedSelection";
static const ::fwCom::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT = "showReconstructions";

static const ::fwServices::IService::KeyType s_MODEL_SERIES_INOUT = "modelSeries";

//------------------------------------------------------------------------------

SModelSeriesList::SModelSeriesList() noexcept
{
    m_sigReconstructionSelected = newSignal< ReconstructionSelectedSignalType >( s_RECONSTRUCTION_SELECTED_SIG );
    m_sigEmptiedSelection       = newSignal< EmptiedSelectionSignalType >( s_EMPTIED_SELECTION_SIG );

    newSlot(s_SHOW_RECONSTRUCTIONS_SLOT, &SModelSeriesList::showReconstructions, this);

    this->registerObject("modelSeries", ::fwServices::IService::AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() noexcept
{
    for(auto cIt :  m_displayedInfo)
    {
        delete cIt.second;
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::configuring()
{
    this->initialize();

    const ConfigType configType = this->getConfigTree();
    const auto config           = configType.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_enableHideAll = config->get<bool>("enable_hide_all", m_enableHideAll);
        m_enableDelete  = config->get<bool>("enableDelete", m_enableDelete);
    }

    const ::fwRuntime::ConfigurationElement::sptr& columns = m_configuration->findConfigurationElement( "columns" );
    if(columns)
    {
        ::fwRuntime::ConfigurationElement::Container::const_iterator cIt = columns->begin();
        m_headers.clear();
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
            m_headers << QString::fromStdString((*cIt)->getName());
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::starting()
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    QVBoxLayout* layout       = new QVBoxLayout;
    QHBoxLayout* layoutButton = new QHBoxLayout;
    layout->addLayout(layoutButton);

    m_tree = new QTreeWidget();

    m_tree->setColumnCount(m_headers.size());
    m_tree->setHeaderLabels(m_headers);

    if(m_enableHideAll)
    {
        // check box "show"
        m_showCheckBox = new QCheckBox( tr("Hide all organs"));
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layoutButton->addWidget( m_showCheckBox, 0 );
        QObject::connect(m_showCheckBox, &QCheckBox::stateChanged, this,
                         &::uiMedDataQt::editor::SModelSeriesList::onShowReconstructions);

        m_checkAllButton = new QPushButton(tr("Check all"));
        layoutButton->addWidget(m_checkAllButton, 0);
        QObject::connect(m_checkAllButton, &QPushButton::clicked, this,
                         &::uiMedDataQt::editor::SModelSeriesList::onCheckAllCheckBox);

        m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
        layoutButton->addWidget(m_unCheckAllButton, 0);
        QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this,
                         &::uiMedDataQt::editor::SModelSeriesList::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        m_deleteAllButton = new QPushButton(tr("Delete all"));
        layoutButton->addWidget(m_deleteAllButton, 0);
        QObject::connect(m_deleteAllButton, &QPushButton::clicked, this,
                         &::uiMedDataQt::editor::SModelSeriesList::onDeleteAllCheckBox );
    }

    layout->addWidget( m_tree, 1 );

    qtContainer->setLayout( layout );

    QObject::connect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    this->updating();

    QObject::connect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
                     this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,int)));

    if(m_enableDelete)
    {
        m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(m_tree, &QTreeWidget::customContextMenuRequested, this,
                         &::uiMedDataQt::editor::SModelSeriesList::onCustomContextMenuRequested);
    }

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SModelSeriesList::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_MODEL_SERIES_INOUT, ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES_INOUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES_INOUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SModelSeriesList::updating()
{
    m_tree->blockSignals(true);

    this->updateReconstructions();
    this->refreshVisibility();

    m_tree->blockSignals(false);
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping()
{

    if(m_enableHideAll)
    {
        QObject::disconnect(m_showCheckBox, &QCheckBox::stateChanged, this,
                            &::uiMedDataQt::editor::SModelSeriesList::onShowReconstructions);

        QObject::disconnect(m_checkAllButton, &QPushButton::clicked, this,
                            &::uiMedDataQt::editor::SModelSeriesList::onCheckAllCheckBox);

        QObject::disconnect(m_unCheckAllButton, &QPushButton::clicked, this,
                            &::uiMedDataQt::editor::SModelSeriesList::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        QObject::disconnect(m_deleteAllButton, &QPushButton::clicked, this,
                            &::uiMedDataQt::editor::SModelSeriesList::onDeleteAllCheckBox );

        QObject::disconnect(m_tree, &QTreeWidget::customContextMenuRequested, this,
                            &::uiMedDataQt::editor::SModelSeriesList::onCustomContextMenuRequested);
    }

    QObject::disconnect(m_tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
                        this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,int)));

    QObject::disconnect(m_tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                        this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    this->destroy();
}

//------------------------------------------------------------------------------

void SModelSeriesList::updateReconstructions()
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();

    SLM_ASSERT("container not instanced", container);

    ::fwData::mt::locked_ptr< ::fwMedData::ModelSeries > modelSeries =
        this->getLockedInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);

    bool hasReconstructions = !modelSeries->getReconstructionDB().empty();
    container->setEnabled( hasReconstructions );

    this->fillTree(modelSeries);
    if(hasReconstructions)
    {
        if(m_showCheckBox)
        {
            m_showCheckBox->blockSignals(true);
            const bool showAllRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();
            m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked );
            m_showCheckBox->blockSignals(false);
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::fillTree(const ::fwData::mt::locked_ptr< ::fwMedData::ModelSeries >& _modelSeries)
{

    auto& reconstructions = _modelSeries->getReconstructionDB();

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
            SLM_ASSERT("Invalid seshat path : '"<< cIt.first <<"'", obj);
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

void SModelSeriesList::onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem*)
{
    SLM_ASSERT("Current selected item is null", _current);
    std::string id = _current->data(0, Qt::UserRole).toString().toStdString();

    ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));

    m_sigReconstructionSelected->asyncEmit(rec);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged(QTreeWidgetItem* _current, int _column)
{
    this->onOrganChoiceVisibility(_current, _column);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganChoiceVisibility(QTreeWidgetItem* _item, int)
{
    std::string id = _item->data(0, Qt::UserRole).toString().toStdString();
    ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));
    SLM_ASSERT("rec not instanced", rec);

    const bool itemIsChecked = (_item->checkState(0) == Qt::Checked);

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

void SModelSeriesList::onShowReconstructions(int _state)
{
    const bool visible = static_cast<bool>(_state);

    m_checkAllButton->setEnabled(!visible);
    m_unCheckAllButton->setEnabled(!visible);
    m_tree->setEnabled(!visible);

    ::fwData::mt::locked_ptr< ::fwMedData::ModelSeries > modelSeries =
        this->getLockedInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);

    {
        ::fwDataTools::helper::Field helper( modelSeries.get_shared() );
        helper.addOrSwap("ShowReconstructions", ::fwData::Boolean::New(_state == Qt::Unchecked));
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::refreshVisibility()
{
    for( int i = 0; i < m_tree->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = m_tree->topLevelItem(i);
        std::string id        = item->data(0, Qt::UserRole).toString().toStdString();
        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(::fwTools::fwID::getObject(id));
        item->setCheckState(0, rec->getIsVisible() ? Qt::Checked : Qt::Unchecked );
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::showReconstructions(bool _show)
{
    if(m_showCheckBox)
    {
        m_showCheckBox->setCheckState(_show ? Qt::Unchecked : Qt::Checked );
    }
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

void SModelSeriesList::onCheckAllBoxes(bool _visible)
{
    for( int i = 0; i < m_tree->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem* item = m_tree->topLevelItem( i );
        item->setCheckState(0, _visible ? Qt::Checked : Qt::Unchecked );
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onDeleteAllCheckBox()
{
    ::fwData::mt::locked_ptr< ::fwMedData::ModelSeries > modelSeries =
        this->getLockedInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);

    // Remove all reconstructions.
    ::fwMedData::ModelSeries::ReconstructionVectorType reconstructions = modelSeries->getReconstructionDB();
    modelSeries->setReconstructionDB(::fwMedData::ModelSeries::ReconstructionVectorType());

    // Send the signals.
    auto sig = modelSeries->signal< ::fwMedData::ModelSeries::ReconstructionsRemovedSignalType >(
        ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG);
    sig->asyncEmit(reconstructions);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCustomContextMenuRequested(const QPoint& _pos)
{
    QModelIndex index = m_tree->indexAt(_pos);
    if (index.isValid())
    {
        QAction* const deleteAction = new QAction("Delete");
        QObject::connect(deleteAction, &QAction::triggered, this, [ = ]()
                {
                    ::fwData::mt::locked_ptr< ::fwMedData::ModelSeries > modelSeries
                        = this->getLockedInOut< ::fwMedData::ModelSeries >(
                              s_MODEL_SERIES_INOUT);

                    ::fwMedData::ModelSeries::ReconstructionVectorType deletedReconstructions;

                    // Remove reconstruction.
                    ::fwMedData::ModelSeries::ReconstructionVectorType reconstructions
                        = modelSeries->getReconstructionDB();
                    const ::fwMedData::ModelSeries::ReconstructionVectorType::iterator recIt
                        = reconstructions.begin() + index.row();
                    const ::fwData::Reconstruction::sptr reconstruction = *recIt;
                    reconstructions.erase(recIt);
                    modelSeries->setReconstructionDB(reconstructions);

                    // Send the signals.
                    deletedReconstructions.push_back(reconstruction);
                    auto sig = modelSeries->signal< ::fwMedData::ModelSeries::ReconstructionsRemovedSignalType >(
                        ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG);
                    sig->asyncEmit(deletedReconstructions);
                });

        QMenu contextMenu;
        contextMenu.addAction(deleteAction);
        contextMenu.exec(QCursor::pos());

    }
}

//------------------------------------------------------------------------------

} // namespace editor.
} // namespace uiMedDataQt.
