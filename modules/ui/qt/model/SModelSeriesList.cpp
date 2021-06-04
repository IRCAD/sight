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

#include "SModelSeriesList.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/operations.hpp>
#include <core/tools/fwID.hpp>

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/helper/Field.hpp>
#include <data/Integer.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <ui/qt/container/QtContainer.hpp>

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

namespace sight::module::ui::qt
{

namespace model
{

class ValueView
{
public:

    ValueView()          = default;
    virtual ~ValueView() = default;

    //------------------------------------------------------------------------------

    virtual std::string apply(data::Object::sptr _obj)
    {
        if(_obj->isA("sight::data::String"))
        {
            data::String::sptr fwValue = data::String::dynamicCast(_obj);
            return fwValue->getValue();
        }
        else if(_obj->isA("sight::data::Integer"))
        {
            data::Integer::sptr fwValue = data::Integer::dynamicCast(_obj);
            return ::boost::lexical_cast<std::string>(fwValue->getValue());
        }
        else if(_obj->isA("sight::data::Float"))
        {
            data::Float::sptr fwValue = data::Float::dynamicCast(_obj);
            return ::boost::lexical_cast<std::string>(fwValue->getValue());
        }
        else
        {
            SIGHT_WARN(_obj->getClassname() + " is not a printable object  : ");
            return "";
        }
    }
};

//------------------------------------------------------------------------------

class PositiveView : public ValueView
{
public:

    //------------------------------------------------------------------------------

    virtual std::string apply(data::Object::sptr _obj)
    {
        if(_obj->isA("sight::data::Integer"))
        {
            data::Integer::sptr fwIntValue = data::Integer::dynamicCast(_obj);

            if(fwIntValue->getValue() > 0)
            {
                std::stringstream ss;
                data::Float::sptr fwValue = data::Float::dynamicCast(_obj);
                ss << ::boost::format("%11.2f") % (fwValue->getValue());
                return ss.str();
            }

            return "Unknown";
        }
        else if(_obj->isA("sight::data::Float"))
        {
            data::Float::sptr fwValue = data::Float::dynamicCast(_obj);
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
            SIGHT_WARN(_obj->getClassname() + " is not a printable object  : ");
            return "";
        }
    }
};

//------------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType s_RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
static const core::com::Signals::SignalKeyType s_EMPTIED_SELECTION_SIG       = "emptiedSelection";
static const core::com::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT       = "showReconstructions";

static const service::IService::KeyType s_MODEL_SERIES_INOUT = "modelSeries";

//------------------------------------------------------------------------------

SModelSeriesList::SModelSeriesList() noexcept
{
    m_sigReconstructionSelected = newSignal<ReconstructionSelectedSignalType>(s_RECONSTRUCTION_SELECTED_SIG);
    m_sigEmptiedSelection       = newSignal<EmptiedSelectionSignalType>(s_EMPTIED_SELECTION_SIG);

    newSlot(s_SHOW_RECONSTRUCTIONS_SLOT, &SModelSeriesList::showReconstructions, this);

    this->registerObject("modelSeries", service::IService::AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() noexcept
{
    for(auto cIt : m_displayedInfo)
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

    const core::runtime::ConfigurationElement::sptr& columns = m_configuration->findConfigurationElement("columns");
    if(columns)
    {
        core::runtime::ConfigurationElement::Container::const_iterator cIt = columns->begin();
        m_headers.clear();
        for( ; cIt != columns->end() ; cIt++)
        {
            ValueView* view;
            core::runtime::ConfigurationElement::AttributePair configView = (*cIt)->getSafeAttributeValue("view");
            if(!configView.first)
            {
                view = new ValueView();
            }
            else if(configView.second == ("positive"))
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
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    QVBoxLayout* layout       = new QVBoxLayout;
    QHBoxLayout* layoutButton = new QHBoxLayout;
    layout->addLayout(layoutButton);

    m_tree = new QTreeWidget();

    m_tree->setColumnCount(m_headers.size());
    m_tree->setHeaderLabels(m_headers);

    if(m_enableHideAll)
    {
        // check box "show"
        m_showCheckBox = new QCheckBox(tr("Hide all organs"));
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layoutButton->addWidget(m_showCheckBox, 0);
        QObject::connect(m_showCheckBox, &QCheckBox::stateChanged, this, &SModelSeriesList::onShowReconstructions);

        m_checkAllButton = new QPushButton(tr("Check all"));
        layoutButton->addWidget(m_checkAllButton, 0);
        QObject::connect(m_checkAllButton, &QPushButton::clicked, this, &SModelSeriesList::onCheckAllCheckBox);

        m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
        layoutButton->addWidget(m_unCheckAllButton, 0);
        QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this, &SModelSeriesList::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        m_deleteAllButton = new QPushButton(tr("Delete all"));
        layoutButton->addWidget(m_deleteAllButton, 0);
        QObject::connect(m_deleteAllButton, &QPushButton::clicked, this, &SModelSeriesList::onDeleteAllCheckBox);
    }

    layout->addWidget(m_tree, 1);

    qtContainer->setLayout(layout);

    QObject::connect(
        m_tree,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*))
    );

    this->updating();

    QObject::connect(
        m_tree,
        SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,int))
    );

    if(m_enableDelete)
    {
        m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &SModelSeriesList::onCustomContextMenuRequested
        );
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SModelSeriesList::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);

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
        QObject::disconnect(m_showCheckBox, &QCheckBox::stateChanged, this, &SModelSeriesList::onShowReconstructions);
        QObject::disconnect(m_checkAllButton, &QPushButton::clicked, this, &SModelSeriesList::onCheckAllCheckBox);
        QObject::disconnect(m_unCheckAllButton, &QPushButton::clicked, this, &SModelSeriesList::onUnCheckAllCheckBox);
    }

    if(m_enableDelete)
    {
        QObject::disconnect(m_deleteAllButton, &QPushButton::clicked, this, &SModelSeriesList::onDeleteAllCheckBox);
        QObject::disconnect(
            m_tree,
            &QTreeWidget::customContextMenuRequested,
            this,
            &SModelSeriesList::onCustomContextMenuRequested
        );
    }

    QObject::disconnect(
        m_tree,
        SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,int))
    );

    QObject::disconnect(
        m_tree,
        SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this,
        SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*))
    );

    this->destroy();
}

//------------------------------------------------------------------------------

void SModelSeriesList::updateReconstructions()
{
    auto qtContainer         = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    SIGHT_ASSERT("container not instanced", container);

    data::mt::locked_ptr<data::ModelSeries> modelSeries =
        this->getLockedInOut<data::ModelSeries>(s_MODEL_SERIES_INOUT);

    bool hasReconstructions = !modelSeries->getReconstructionDB().empty();
    container->setEnabled(hasReconstructions);

    this->fillTree(modelSeries);
    if(hasReconstructions)
    {
        if(m_showCheckBox)
        {
            m_showCheckBox->blockSignals(true);
            const bool showAllRec = modelSeries->getField("ShowReconstructions", data::Boolean::New(true))->value();
            m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked);
            m_showCheckBox->blockSignals(false);
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::fillTree(const data::mt::locked_ptr<data::ModelSeries>& _modelSeries)
{
    auto& reconstructions = _modelSeries->getReconstructionDB();

    if(!m_tree->selectedItems().empty())
    {
        m_sigEmptiedSelection->asyncEmit();
    }

    m_tree->clear();

    // Create items
    for(auto const& reconstruction : reconstructions)
    {
        QStringList info;
        for(auto const& cIt : m_displayedInfo)
        {
            data::Object::sptr obj = data::reflection::getObject(reconstruction, cIt.first);
            SIGHT_ASSERT("Invalid seshat path : '" << cIt.first << "'", obj);
            info << QString::fromStdString(cIt.second->apply(obj));
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(info);
        item->setCheckState(0, Qt::Unchecked);
        m_tree->addTopLevelItem(item);
        item->setData(0, Qt::UserRole, QString::fromStdString(reconstruction->getID()));
    }

    for(int i = 0 ; i < m_tree->topLevelItemCount() ; i++)
    {
        m_tree->resizeColumnToContents(i);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged(QTreeWidgetItem* _current, QTreeWidgetItem*)
{
    SIGHT_ASSERT("Current selected item is null", _current);
    std::string id = _current->data(0, Qt::UserRole).toString().toStdString();

    data::Reconstruction::sptr rec = data::Reconstruction::dynamicCast(core::tools::fwID::getObject(id));

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
    std::string id                 = _item->data(0, Qt::UserRole).toString().toStdString();
    data::Reconstruction::sptr rec = data::Reconstruction::dynamicCast(core::tools::fwID::getObject(id));
    SIGHT_ASSERT("rec not instanced", rec);

    const bool itemIsChecked = (_item->checkState(0) == Qt::Checked);

    if(rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(itemIsChecked);

        data::Reconstruction::VisibilityModifiedSignalType::sptr sig;
        sig = rec->signal<data::Reconstruction::VisibilityModifiedSignalType>(
            data::Reconstruction::s_VISIBILITY_MODIFIED_SIG
        );
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

    data::mt::locked_ptr<data::ModelSeries> modelSeries =
        this->getLockedInOut<data::ModelSeries>(s_MODEL_SERIES_INOUT);

    {
        data::helper::Field helper(modelSeries.get_shared());
        helper.addOrSwap("ShowReconstructions", data::Boolean::New(_state == Qt::Unchecked));
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::refreshVisibility()
{
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item          = m_tree->topLevelItem(i);
        std::string id                 = item->data(0, Qt::UserRole).toString().toStdString();
        data::Reconstruction::sptr rec = data::Reconstruction::dynamicCast(core::tools::fwID::getObject(id));
        item->setCheckState(0, rec->getIsVisible() ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::showReconstructions(bool _show)
{
    if(m_showCheckBox)
    {
        m_showCheckBox->setCheckState(_show ? Qt::Unchecked : Qt::Checked);
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
    for(int i = 0 ; i < m_tree->topLevelItemCount() ; ++i)
    {
        QTreeWidgetItem* item = m_tree->topLevelItem(i);
        item->setCheckState(0, _visible ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onDeleteAllCheckBox()
{
    data::mt::locked_ptr<data::ModelSeries> modelSeries =
        this->getLockedInOut<data::ModelSeries>(s_MODEL_SERIES_INOUT);

    // Remove all reconstructions.
    data::ModelSeries::ReconstructionVectorType reconstructions = modelSeries->getReconstructionDB();
    modelSeries->setReconstructionDB(data::ModelSeries::ReconstructionVectorType());

    // Send the signals.
    auto sig = modelSeries->signal<data::ModelSeries::ReconstructionsRemovedSignalType>(
        data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG
    );
    sig->asyncEmit(reconstructions);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCustomContextMenuRequested(const QPoint& _pos)
{
    QModelIndex index = m_tree->indexAt(_pos);
    if(index.isValid())
    {
        QAction* const deleteAction = new QAction("Delete");
        QObject::connect(
            deleteAction,
            &QAction::triggered,
            this,
            [ = ]()
                {
                    data::mt::locked_ptr<data::ModelSeries> modelSeries =
                        this->getLockedInOut<data::ModelSeries>(
                            s_MODEL_SERIES_INOUT
                        );

                    data::ModelSeries::ReconstructionVectorType deletedReconstructions;

                    // Remove reconstruction.
                    data::ModelSeries::ReconstructionVectorType reconstructions =
                        modelSeries->getReconstructionDB();
                    const data::ModelSeries::ReconstructionVectorType::iterator recIt =
                        reconstructions.begin() + index.row();
                    const data::Reconstruction::sptr reconstruction = *recIt;
                    reconstructions.erase(recIt);
                    modelSeries->setReconstructionDB(reconstructions);

                    // Send the signals.
                    deletedReconstructions.push_back(reconstruction);
                    auto sig = modelSeries->signal<data::ModelSeries::ReconstructionsRemovedSignalType>(
                        data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG
                    );
                    sig->asyncEmit(deletedReconstructions);
                });

        QMenu contextMenu;
        contextMenu.addAction(deleteAction);
        contextMenu.exec(QCursor::pos());
    }
}

//------------------------------------------------------------------------------

} // namespace model.

} // namespace sight::module::ui::qt.
