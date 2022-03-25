/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/ui/qt/metrics/SLandmarks.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <geometry/data/Matrix4.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include <random>

namespace sight::module::ui::qt::metrics
{

//------------------------------------------------------------------------------

static const char* s_GROUP_PROPERTY_NAME = "group";
static const int s_GROUP_NAME_ROLE       = Qt::UserRole + 1;

static const core::com::Slots::SlotKeyType s_ADD_PICKED_POINT_SLOT = "addPickedPoint";
static const core::com::Slots::SlotKeyType s_PICK_SLOT             = "pick";
static const core::com::Slots::SlotKeyType s_ADD_POINT_SLOT        = "addPoint";
static const core::com::Slots::SlotKeyType s_MODIFY_POINT_SLOT     = "modifyPoint";
static const core::com::Slots::SlotKeyType s_SELECT_POINT_SLOT     = "selectPoint";
static const core::com::Slots::SlotKeyType s_DESELECT_POINT_SLOT   = "deselectPoint";
static const core::com::Slots::SlotKeyType s_REMOVE_POINT_SLOT     = "removePoint";
static const core::com::Slots::SlotKeyType s_ADD_GROUP_SLOT        = "addGroup";
static const core::com::Slots::SlotKeyType s_REMOVE_GROUP_SLOT     = "removeGroup";
static const core::com::Slots::SlotKeyType s_MODIFY_GROUP_SLOT     = "modifyGroup";
static const core::com::Slots::SlotKeyType s_RENAME_GROUP_SLOT     = "renameGroup";

static const std::string s_SIZE_CONFIG     = "size";
static const std::string s_OPACITY_CONFIG  = "opacity";
static const std::string s_ADVANCED_CONFIG = "advanced";
static const std::string s_TEXT_CONFIG     = "text";

const core::com::Signals::SignalKeyType SLandmarks::s_SEND_WORLD_COORD = "sendWorldCoord";

//------------------------------------------------------------------------------

SLandmarks::SLandmarks() noexcept
{
    newSlot(s_ADD_PICKED_POINT_SLOT, &SLandmarks::addPickedPoint, this);
    newSlot(s_PICK_SLOT, &SLandmarks::pick, this);
    newSlot(s_ADD_POINT_SLOT, &SLandmarks::addPoint, this);
    newSlot(s_MODIFY_POINT_SLOT, &SLandmarks::modifyPoint, this);
    newSlot(s_SELECT_POINT_SLOT, &SLandmarks::selectPoint, this);
    newSlot(s_DESELECT_POINT_SLOT, &SLandmarks::deselectPoint, this);
    newSlot(s_ADD_GROUP_SLOT, &SLandmarks::addGroup, this);
    newSlot(s_REMOVE_POINT_SLOT, &SLandmarks::removePoint, this);
    newSlot(s_REMOVE_GROUP_SLOT, &SLandmarks::removeGroup, this);
    newSlot(s_MODIFY_GROUP_SLOT, &SLandmarks::modifyGroup, this);
    newSlot(s_RENAME_GROUP_SLOT, &SLandmarks::renameGroup, this);

    newSignal<world_coordinates_signal_t>(s_SEND_WORLD_COORD);
}

//------------------------------------------------------------------------------

SLandmarks::~SLandmarks() noexcept
{
}

//------------------------------------------------------------------------------

void SLandmarks::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();

    const service::IService::ConfigType config = this->getConfigTree();

    m_defaultLandmarkSize = config.get<float>(s_SIZE_CONFIG, m_defaultLandmarkSize);
    SIGHT_FATAL_IF(
        "'size' value must be a positive number greater than 0 (current value: " << m_defaultLandmarkSize << ")",
        m_defaultLandmarkSize <= 0.f
    );

    m_defaultLandmarkOpacity = config.get<float>(s_OPACITY_CONFIG, m_defaultLandmarkOpacity);
    SIGHT_FATAL_IF(
        "'opacity' value must be a number between 0.0 and 1.0 (current value: " << m_defaultLandmarkOpacity << ")",
        m_defaultLandmarkOpacity<0.f || m_defaultLandmarkOpacity>1.f
    );

    m_advancedMode = config.get<bool>(s_ADVANCED_CONFIG, false);

    m_text = config.get<std::string>(s_TEXT_CONFIG, m_text);
}

//------------------------------------------------------------------------------

void SLandmarks::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    const auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    QVBoxLayout* const layout     = new QVBoxLayout();
    QGridLayout* const gridLayout = new QGridLayout();

    m_visibilityCheckbox = new QCheckBox();
    QLabel* const visibilityLabel = new QLabel(QString("Visibility"));
    m_visibilityCheckbox->setObjectName(serviceID + "/" + visibilityLabel->text());
    m_sizeSlider = new QSlider(Qt::Horizontal);
    m_sizeSlider->setMinimum(1);
    m_sizeSlider->setMaximum(100);
    QLabel* const sizeLabel = new QLabel(QString("Size"));
    m_sizeSlider->setObjectName(serviceID + "/" + sizeLabel->text());
    m_opacitySlider = new QSlider(Qt::Horizontal);
    QLabel* const opacityLabel = new QLabel("Opacity");
    m_opacitySlider->setObjectName(serviceID + "/" + opacityLabel->text());
    m_shapeSelector = new QComboBox();
    m_shapeSelector->addItem(QString("Cube"));
    m_shapeSelector->addItem(QString("Sphere"));
    QLabel* const shapeLabel = new QLabel("Shape");
    m_shapeSelector->setObjectName(serviceID + "/" + shapeLabel->text());

    if(m_advancedMode)
    {
        m_newGroupButton = new QPushButton("New Group");
        m_newGroupButton->setObjectName(serviceID + "/" + m_newGroupButton->text());
    }

    m_removeButton = new QPushButton("Delete");
    m_removeButton->setObjectName(serviceID + "/" + m_removeButton->text());
    m_removeButton->setShortcut(QKeySequence::Delete);

    gridLayout->addWidget(visibilityLabel, 0, 0);
    gridLayout->addWidget(m_visibilityCheckbox, 0, 1);
    gridLayout->addWidget(sizeLabel, 1, 0);
    gridLayout->addWidget(m_sizeSlider, 1, 1);
    gridLayout->addWidget(opacityLabel, 2, 0);
    gridLayout->addWidget(m_opacitySlider, 2, 1);
    gridLayout->addWidget(shapeLabel, 3, 0);
    gridLayout->addWidget(m_shapeSelector, 3, 1);
    gridLayout->addWidget(m_removeButton, 4, 1);

    m_groupEditorWidget = new QWidget();
    m_groupEditorWidget->setLayout(gridLayout);

    m_treeWidget = new QTreeWidget();
    if(!m_text.empty())
    {
        QLabel* helperTextLabel = new QLabel(QString::fromStdString(m_text));
        layout->addWidget(helperTextLabel);
    }

    if(m_advancedMode)
    {
        layout->addWidget(m_newGroupButton);
    }

    layout->addWidget(m_treeWidget);
    layout->addWidget(m_groupEditorWidget);
    m_groupEditorWidget->setDisabled(true);

    QStringList headers;
    headers << "Group" << "Color";

    if(m_advancedMode)
    {
        // Add empty third column to display and edit point coordinates.
        headers << "";
    }

    m_treeWidget->setHeaderLabels(headers);

    qtContainer->setLayout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_MODIFIED_SIG, s_MODIFY_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_SELECTED_SIG, s_SELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_DESELECTED_SIG, s_DESELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_ADDED_SIG, s_ADD_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_REMOVED_SIG, s_REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_MODIFIED_SIG, s_MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_RENAMED_SIG, s_RENAME_GROUP_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SLandmarks::updating()
{
    m_treeWidget->blockSignals(true);

    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        m_treeWidget->clear();

        for(const auto& name : landmarks->getGroupNames())
        {
            this->addGroup(name);
            this->addPoint(name);
        }
    }

    QObject::connect(m_treeWidget.data(), &QTreeWidget::itemChanged, this, &SLandmarks::onGroupNameEdited);
    QObject::connect(m_treeWidget.data(), &QTreeWidget::currentItemChanged, this, &SLandmarks::onSelectionChanged);
    QObject::connect(m_treeWidget.data(), &QTreeWidget::itemDoubleClicked, this, &SLandmarks::onLandmarkDoubleClicked);
    QObject::connect(m_sizeSlider.data(), &QSlider::valueChanged, this, &SLandmarks::onSizeChanged);
    QObject::connect(m_opacitySlider.data(), &QSlider::valueChanged, this, &SLandmarks::onOpacityChanged);
    QObject::connect(m_visibilityCheckbox.data(), &QCheckBox::stateChanged, this, &SLandmarks::onVisibilityChanged);
    QObject::connect(m_shapeSelector.data(), &QComboBox::currentTextChanged, this, &SLandmarks::onShapeChanged);
    QObject::connect(m_removeButton.data(), &QPushButton::clicked, this, &SLandmarks::onRemoveSelection);
    QObject::connect(m_newGroupButton.data(), &QPushButton::clicked, this, &SLandmarks::onAddNewGroup);

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SLandmarks::onColorButton()
{
    QObject* const sender = this->sender();

    // Create Color choice dialog.
    auto qtContainer         = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    const QColor oldColor = sender->property("color").value<QColor>();
    const QColor colorQt  = QColorDialog::getColor(oldColor, container, "Select Color", QColorDialog::ShowAlphaChannel);
    if(colorQt.isValid())
    {
        QPushButton* const colorButton = dynamic_cast<QPushButton*>(sender);
        colorButton->setProperty("color", colorQt);

        setColorButtonIcon(colorButton, colorQt);

        const std::string groupName = colorButton->property(s_GROUP_PROPERTY_NAME).value<QString>().toStdString();

        data::Landmarks::ColorType color = {{
            colorQt.red() / 255.f, colorQt.green() / 255.f,
            colorQt.blue() / 255.f, colorQt.alpha() / 255.f
        }
        };

        data::Landmarks::GroupModifiedSignalType::sptr sig;
        {
            const auto landmarks = m_landmarks.lock();
            SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

            auto& group = landmarks->getGroup(groupName);
            group.m_color = color;

            sig = landmarks->signal<data::Landmarks::GroupModifiedSignalType>(data::Landmarks::s_GROUP_MODIFIED_SIG);
        }

        m_opacitySlider->setValue(static_cast<int>(color[3] * m_opacitySlider->maximum()));

        {
            core::com::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));
            sig->asyncEmit(groupName);
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onGroupNameEdited(QTreeWidgetItem* _item, int _column)
{
    SIGHT_ERROR_IF("A column different from the group's name is being edited", _column != 0);
    m_treeWidget->blockSignals(true);

    if(_column == 0)
    {
        const QString oldGroupName = _item->data(0, s_GROUP_NAME_ROLE).toString();
        const QString newGroupName = _item->text(0);

        if(newGroupName.isEmpty())
        {
            const QString msg = "The new group name for '" + oldGroupName
                                + "' is empty. Please enter a valid name and try again";
            QMessageBox msgBox(QMessageBox::Warning, "No group name", msg, QMessageBox::Ok);
            msgBox.exec();

            _item->setText(0, oldGroupName);
        }
        else if(oldGroupName != newGroupName)
        {
            try
            {
                {
                    const auto landmarks = m_landmarks.lock();
                    SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

                    landmarks->renameGroup(oldGroupName.toStdString(), newGroupName.toStdString());

                    const auto sig = landmarks->signal<data::Landmarks::GroupRenamedSignalType>(
                        data::Landmarks::s_GROUP_RENAMED_SIG
                    );

                    {
                        core::com::Connection::Blocker block(sig->getConnection(this->slot(s_RENAME_GROUP_SLOT)));
                        sig->asyncEmit(oldGroupName.toStdString(), newGroupName.toStdString());
                    }
                }

                _item->setData(0, s_GROUP_NAME_ROLE, newGroupName);
                QWidget* const widget = m_treeWidget->itemWidget(_item, 1);

                widget->setProperty(s_GROUP_PROPERTY_NAME, newGroupName);
            }
            catch(data::Exception& e)
            {
                const QString msg = "Can't rename '" + oldGroupName + "' as '" + newGroupName + ".\n"
                                    + QString(e.what());
                QMessageBox msgBox(QMessageBox::Warning, "Can't rename" + oldGroupName, msg, QMessageBox::Ok);
                msgBox.exec();

                _item->setText(0, oldGroupName);
            }
        }
    }

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::onSelectionChanged(QTreeWidgetItem* _current, QTreeWidgetItem* _previous)
{
    if(_previous)
    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        const auto deselectSig = landmarks->signal<data::Landmarks::PointDeselectedSignalType>(
            data::Landmarks::s_POINT_DESELECTED_SIG
        );

        const core::com::Connection::Blocker block(deselectSig->getConnection(this->slot(s_DESELECT_POINT_SLOT)));

        if(m_advancedMode)
        {
            const QTreeWidgetItem* const previousParent = _previous->parent();

            if(previousParent != nullptr)
            {
                const std::string& groupName = previousParent->text(0).toStdString();

                const std::size_t index = static_cast<std::size_t>(previousParent->indexOfChild(_previous));

                SIGHT_ASSERT(
                    "index must be inferior to the number of points in '" + groupName + "'.",
                    index < landmarks->numPoints(groupName)
                );

                deselectSig->asyncEmit(groupName, index);
            }
        }
        else
        {
            const std::string& groupName = _previous->text(0).toStdString();

            deselectSig->asyncEmit(groupName, 0);
        }
    }

    if(_current)
    {
        int size;
        bool visible;
        QString shapeText;
        float opacity;
        {
            const auto landmarks = m_landmarks.lock();
            SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

            const auto selectSig = landmarks->signal<data::Landmarks::PointSelectedSignalType>(
                data::Landmarks::s_POINT_SELECTED_SIG
            );

            std::string groupName;

            if(m_advancedMode)
            {
                const QTreeWidgetItem* const currentParent = _current->parent();

                if(currentParent != nullptr)
                {
                    groupName = currentParent->text(0).toStdString();

                    const std::size_t index = static_cast<std::size_t>(currentParent->indexOfChild(_current));

                    SIGHT_ASSERT(
                        "index must be inferior to the number of points in '" + groupName + "'.",
                        index < landmarks->numPoints(groupName)
                    );

                    const core::com::Connection::Blocker block(selectSig->getConnection(this->slot(s_SELECT_POINT_SLOT)));
                    selectSig->asyncEmit(groupName, index);
                }
                else
                {
                    groupName = _current->text(0).toStdString();
                }
            }
            else
            {
                groupName = _current->text(0).toStdString();

                core::com::Connection::Blocker block(selectSig->getConnection(this->slot(s_SELECT_POINT_SLOT)));
                selectSig->asyncEmit(groupName, 0);
            }

            const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);

            size      = static_cast<int>(group.m_size);
            visible   = group.m_visibility;
            shapeText = group.m_shape == data::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
            opacity   = group.m_color[3];
        }

        // Set widget values
        m_sizeSlider->setValue(size);
        m_visibilityCheckbox->setChecked(visible);
        m_shapeSelector->setCurrentText(shapeText);
        m_opacitySlider->setValue(static_cast<int>(opacity * m_opacitySlider->maximum()));
    }

    m_groupEditorWidget->setDisabled(_current == nullptr);
}

//------------------------------------------------------------------------------

void SLandmarks::onLandmarkDoubleClicked(QTreeWidgetItem* _item, int) const
{
    if(_item)
    {
        // Exclude top level item
        if(_item->childCount() != 0)
        {
            return;
        }

        QString index[3];
        index[0] = _item->text(0);
        index[1] = _item->text(1);
        index[2] = _item->text(2);

        if(index[0].isEmpty() || index[1].isEmpty() || index[2].isEmpty())
        {
            // Do nothing if a value is missing.
            return;
        }

        // Convert to double
        double world_coord[3];
        bool check[3] = {false, false, false};
        world_coord[0] = index[0].toDouble(&check[0]);
        world_coord[1] = index[1].toDouble(&check[1]);
        world_coord[2] = index[2].toDouble(&check[2]);

        // Check that conversion to double performed well.
        if(!check[0] || !check[1] || !check[2])
        {
            SIGHT_ERROR(
                "Cannot convert landmark position to double ("
                + index[0].toStdString()
                + ", " + index[1].toStdString()
                + ", " + index[2].toStdString() + ")"
            );
            return;
        }

        // Log that we double clicked on a landmark
        SIGHT_DEBUG(
            "Double clicked on landmark [" + index[0].toStdString()
            + ", " + index[1].toStdString()
            + ", " + index[2].toStdString() + "]"
        );

        // Send signal with world coordinates of the landmarks
        this->signal<world_coordinates_signal_t>(s_SEND_WORLD_COORD)->asyncEmit(
            world_coord[0],
            world_coord[1],
            world_coord[2]
        );
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onSizeChanged(int _newSize)
{
    const data::Landmarks::SizeType realSize = static_cast<data::Landmarks::SizeType>(_newSize);

    std::string groupName;
    if(currentSelection(groupName))
    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        landmarks->setGroupSize(groupName, realSize);

        const auto sig = landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
            data::Landmarks::s_GROUP_MODIFIED_SIG
        );

        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onOpacityChanged(int _newOpacity)
{
    const float sliderSize = static_cast<float>(m_opacitySlider->maximum() - m_opacitySlider->minimum());

    const float realOpacity = static_cast<float>(_newOpacity) / sliderSize;

    std::string groupName;
    if(currentSelection(groupName))
    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        const auto groupColor = landmarks->getGroup(groupName).m_color;

        const data::Landmarks::ColorType newGroupColor =
        {{groupColor[0], groupColor[1], groupColor[2], realOpacity}};

        landmarks->setGroupColor(groupName, newGroupColor);

        QTreeWidgetItem* const item    = getGroupItem(groupName);
        QPushButton* const colorButton = dynamic_cast<QPushButton*>(m_treeWidget->itemWidget(item, 1));

        QColor currentColor = colorButton->property("color").value<QColor>();
        currentColor.setAlphaF(realOpacity);
        colorButton->setProperty("color", currentColor);

        setColorButtonIcon(colorButton, currentColor);

        auto sig = landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
            data::Landmarks::s_GROUP_MODIFIED_SIG
        );

        core::com::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onVisibilityChanged(int _visibility)
{
    std::string groupName;
    if(currentSelection(groupName))
    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        landmarks->setGroupVisibility(groupName, static_cast<bool>(_visibility));

        const auto sig = landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
            data::Landmarks::s_GROUP_MODIFIED_SIG
        );

        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onShapeChanged(const QString& _shape)
{
    std::string groupName;
    if(currentSelection(groupName))
    {
        SIGHT_ASSERT("Shape must be 'Cube' or 'Sphere'.", _shape == "Cube" || _shape == "Sphere");
        const data::Landmarks::Shape s =
            (_shape == "Cube") ? data::Landmarks::Shape::CUBE : data::Landmarks::Shape::SPHERE;

        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        landmarks->setGroupShape(groupName, s);

        auto sig = landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
            data::Landmarks::s_GROUP_MODIFIED_SIG
        );

        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onAddNewGroup()
{
    data::Landmarks::GroupAddedSignalType::sptr sig;
    const std::string groupName = this->generateNewGroupName();

    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        landmarks->addGroup(groupName, this->generateNewColor(), m_defaultLandmarkSize);
        sig = landmarks->signal<data::Landmarks::GroupAddedSignalType>(data::Landmarks::s_GROUP_ADDED_SIG);
    }
    this->addGroup(groupName);
    {
        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_GROUP_SLOT)));
        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onRemoveSelection()
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* const item = m_treeWidget->currentItem();

    if(item != nullptr)
    {
        const int topLevelIndex = m_treeWidget->indexOfTopLevelItem(item);

        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        if(m_advancedMode && topLevelIndex == -1) // Delete point
        {
            QTreeWidgetItem* const itemParent = item->parent();

            const std::size_t index      = static_cast<std::size_t>(itemParent->indexOfChild(item));
            const std::string& groupName = itemParent->text(0).toStdString();

            landmarks->removePoint(groupName, index);
            itemParent->removeChild(item);

            auto sig = landmarks->signal<data::Landmarks::PointRemovedSignalType>(
                data::Landmarks::s_POINT_REMOVED_SIG
            );

            {
                const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_POINT_SLOT)));
                sig->asyncEmit(groupName, index);
            }
        }
        else
        {
            const std::string& groupName = item->text(0).toStdString();

            landmarks->removeGroup(groupName);
            delete m_treeWidget->takeTopLevelItem(topLevelIndex);

            const auto sig = landmarks->signal<data::Landmarks::GroupRemovedSignalType>(
                data::Landmarks::s_GROUP_REMOVED_SIG
            );

            {
                const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_GROUP_SLOT)));
                sig->asyncEmit(groupName);
            }
        }

        m_treeWidget->setCurrentItem(nullptr);
        m_groupEditorWidget->setDisabled(true);
    }

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::addPickedPoint(data::tools::PickingInfo _pickingInfo)
{
    FW_DEPRECATED_MSG(
        "sight::module::ui::qt::metrics::addPickedPoint is no longer supported, the methods have been moved to module::ui::qt::metrics::pick",
        "22.0"
    )
    this->pick(_pickingInfo);
}

//------------------------------------------------------------------------------

void SLandmarks::pick(data::tools::PickingInfo _info)
{
    if(_info.m_modifierMask & data::tools::PickingInfo::CTRL)
    {
        // Adds a new landmark.
        if(_info.m_eventId == data::tools::PickingInfo::Event::MOUSE_LEFT_UP)
        {
            const double* const pickedPos       = _info.m_worldPos;
            data::Landmarks::PointType newPoint = {{pickedPos[0], pickedPos[1], pickedPos[2]}};

            {
                const auto matrix = m_matrix.lock();
                if(matrix)
                {
                    const auto pickedPoint = glm::dvec4 {pickedPos[0], pickedPos[1], pickedPos[2], 1.0};
                    const auto mat         = geometry::data::getMatrixFromTF3D(*matrix);

                    const auto modifiedPoint = mat * pickedPoint;
                    for(uint8_t i = 0 ; i < 3 ; ++i)
                    {
                        newPoint[i] = modifiedPoint[i];
                    }
                }
            }

            std::string groupName = this->generateNewGroupName();
            QTreeWidgetItem* item = m_treeWidget->currentItem();

            // No selection or simple mode, create a new group.
            if(item == nullptr || !m_advancedMode)
            {
                data::Landmarks::GroupAddedSignalType::sptr sig;
                {
                    auto landmarks = m_landmarks.lock();
                    SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

                    landmarks->addGroup(groupName, this->generateNewColor(), m_defaultLandmarkSize);

                    sig = landmarks->signal<data::Landmarks::GroupAddedSignalType>(data::Landmarks::s_GROUP_ADDED_SIG);
                }

                this->addGroup(groupName);

                {
                    const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_GROUP_SLOT)));
                    sig->asyncEmit(groupName);
                }
            }
            // Advanced mode and a point or a group is selected.
            else
            {
                const int topLevelIndex = m_treeWidget->indexOfTopLevelItem(item);

                // Point selected, add new point to parent group.
                if(topLevelIndex == -1)
                {
                    item = item->parent();
                }

                groupName = item->text(0).toStdString();
            }

            data::Landmarks::PointAddedSignalType::sptr sig;
            {
                auto landmarks = m_landmarks.lock();
                SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);
                landmarks->addPoint(groupName, newPoint);
                sig = landmarks->signal<data::Landmarks::PointAddedSignalType>(data::Landmarks::s_POINT_ADDED_SIG);
            }

            this->addPoint(groupName);

            {
                const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_POINT_SLOT)));
                sig->asyncEmit(groupName);
            }
        }
        else if(_info.m_eventId == data::tools::PickingInfo::Event::MOUSE_RIGHT_UP)
        {
            const double* const pickedPos = _info.m_worldPos;

            auto landmarks = m_landmarks.lock();
            SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

            // Find closest landmarks.
            double closest             = std::numeric_limits<double>::max();
            std::size_t foundIndex     = 0;
            std::string foundGroupname = "";

            for(const std::string& groupName : landmarks->getGroupNames())
            {
                for(std::size_t index = 0 ; index < landmarks->numPoints(groupName) ; ++index)
                {
                    const data::Landmarks::PointType landmark = landmarks->getPoint(groupName, index);

                    const double tmpClosest = std::sqrt(
                        std::pow(pickedPos[0] - landmark[0], 2)
                        + std::pow(pickedPos[1] - landmark[1], 2)
                        + std::pow(pickedPos[2] - landmark[2], 2)
                    );

                    if(tmpClosest < closest)
                    {
                        closest        = tmpClosest;
                        foundGroupname = groupName;
                        foundIndex     = index;
                    }
                }
            }

            // 10.0 is an acceptable delta to remove a landmark.
            if(!foundGroupname.empty() && closest < 10.)
            {
                // If the group contains only one point, we remove it.
                if(landmarks->numPoints(foundGroupname) == 1)
                {
                    this->removeGroup(foundGroupname);

                    landmarks->removeGroup(foundGroupname);

                    auto sig = landmarks->signal<data::Landmarks::GroupRemovedSignalType>(
                        data::Landmarks::s_GROUP_REMOVED_SIG
                    );
                    {
                        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_GROUP_SLOT)));
                        sig->asyncEmit(foundGroupname);
                    }
                }
                else
                {
                    landmarks->removePoint(foundGroupname, foundIndex);

                    this->removePoint(foundGroupname, foundIndex);

                    const auto sig = landmarks->signal<data::Landmarks::PointRemovedSignalType>(
                        data::Landmarks::s_POINT_REMOVED_SIG
                    );

                    {
                        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_POINT_SLOT)));
                        sig->asyncEmit(foundGroupname, foundIndex);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string _groupName)
{
    if(m_advancedMode)
    {
        m_treeWidget->blockSignals(true);

        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        QTreeWidgetItem* const item = getGroupItem(_groupName);

        const std::size_t nbChilds = static_cast<std::size_t>(item->childCount());
        const std::size_t nbPoints = landmarks->numPoints(_groupName);
        for(std::size_t idx = nbChilds ; idx < nbPoints ; ++idx)
        {
            const data::Landmarks::PointType& newPoint = landmarks->getPoint(_groupName, idx);

            QTreeWidgetItem* const pt = new QTreeWidgetItem();
            for(int i = 0 ; i < 3 ; ++i)
            {
                pt->setText(i, QString::fromStdString(std::to_string(newPoint[static_cast<std::size_t>(i)])));
            }

            item->addChild(pt);
        }

        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::addGroup(std::string _name)
{
    QColor color;
    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        const auto& group = landmarks->getGroup(_name);

        color = convertToQColor(group.m_color);
    }

    QTreeWidgetItem* const item = new QTreeWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setData(0, s_GROUP_NAME_ROLE, QString::fromStdString(_name));

    m_treeWidget->addTopLevelItem(item);

    item->setText(0, QString::fromStdString(_name));
    QPushButton* const button = new QPushButton();

    setColorButtonIcon(button, color);
    button->setProperty("color", color);
    button->setProperty(s_GROUP_PROPERTY_NAME, QString::fromStdString(_name));

    QObject::connect(button, &QPushButton::clicked, this, &SLandmarks::onColorButton);

    m_treeWidget->setItemWidget(item, 1, button);
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string _name)
{
    try
    {
        QTreeWidgetItem* const item = getGroupItem(_name);

        while(item->childCount() != 0)
        {
            QTreeWidgetItem* const child = item->child(0);
            item->removeChild(child);
        }

        const int index                = m_treeWidget->indexOfTopLevelItem(item);
        QTreeWidgetItem* const topItem = m_treeWidget->takeTopLevelItem(index);
        delete topItem;
    }
    catch(const std::exception& _e)
    {
        std::cout << _e.what() << std::endl;
    }
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string _groupName, std::size_t _index)
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* const item = getGroupItem(_groupName);

    SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

    QTreeWidgetItem* const pointItem = item->child(static_cast<int>(_index));
    item->removeChild(pointItem);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::renameGroup(std::string _oldName, std::string _newName)
{
    m_treeWidget->blockSignals(true);

    const QString qtNewName     = QString::fromStdString(_newName);
    QTreeWidgetItem* const item = getGroupItem(_oldName);
    item->setData(0, s_GROUP_NAME_ROLE, qtNewName);
    QWidget* const widget = m_treeWidget->itemWidget(item, 1);
    widget->setProperty(s_GROUP_PROPERTY_NAME, qtNewName);

    item->setText(0, qtNewName);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::modifyGroup(std::string _name)
{
    QTreeWidgetItem* const item = getGroupItem(_name);

    item->setText(0, _name.c_str());

    const auto landmarks = m_landmarks.lock();
    SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

    const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(_name);

    QPushButton* const colorButton = dynamic_cast<QPushButton*>(m_treeWidget->itemWidget(item, 1));

    const QColor color = convertToQColor(group.m_color);

    setColorButtonIcon(colorButton, color);

    bool groupSelected = item->isSelected();

    if(m_advancedMode) // Check if a child is selected.
    {
        for(int i = 0 ; i < item->childCount() && !groupSelected ; ++i)
        {
            groupSelected = item->child(i)->isSelected();
        }
    }

    if(groupSelected)
    {
        // Set widget values
        m_sizeSlider->setValue(static_cast<int>(group.m_size));
        m_visibilityCheckbox->setChecked(group.m_visibility);

        const QString shapeText = group.m_shape == data::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
        m_shapeSelector->setCurrentText(shapeText);

        const float opacity = group.m_color[3];
        m_opacitySlider->setValue(static_cast<int>(opacity * m_opacitySlider->maximum()));
    }
}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string _groupName, std::size_t _index)
{
    if(m_advancedMode)
    {
        auto const itemList = m_treeWidget->findItems(QString::fromStdString(_groupName), Qt::MatchExactly);

        SIGHT_ASSERT("Only a single item can be named '" + _groupName + "'", itemList.size() == 1);

        QTreeWidgetItem* const item = itemList.at(0);

        SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

        QTreeWidgetItem* const pointItem = item->child(static_cast<int>(_index));

        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        const data::Landmarks::PointType& point = landmarks->getPoint(_groupName, _index);

        m_treeWidget->blockSignals(true);
        for(int i = 0 ; i < 3 ; ++i)
        {
            pointItem->setText(i, QString("%1").arg(point[static_cast<std::size_t>(i)]));
        }

        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string _groupName, std::size_t _index)
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* currentItem = getGroupItem(_groupName);

    if(m_advancedMode)
    {
        SIGHT_ASSERT(
            "Index must be less than " << currentItem->childCount(),
            static_cast<int>(_index) < currentItem->childCount()
        );

        currentItem = currentItem->child(static_cast<int>(_index));
    }

    m_treeWidget->setCurrentItem(currentItem);

    int size;
    bool visible;
    QString shapeText;
    float opacity;
    {
        const auto landmarks = m_landmarks.lock();
        SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

        const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(_groupName);

        size      = static_cast<int>(group.m_size);
        visible   = group.m_visibility;
        shapeText = group.m_shape == data::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
        opacity   = group.m_color[3];
    }

    // Set widget values
    m_sizeSlider->setValue(size);
    m_visibilityCheckbox->setChecked(visible);
    m_shapeSelector->setCurrentText(shapeText);
    m_opacitySlider->setValue(static_cast<int>(opacity * m_opacitySlider->maximum()));

    m_groupEditorWidget->setEnabled(true);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string, std::size_t)
{
    m_treeWidget->blockSignals(true);
    m_treeWidget->setCurrentItem(nullptr);
    m_groupEditorWidget->setDisabled(true);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

std::string SLandmarks::generateNewGroupName() const
{
    static std::size_t groupCount = 0;

    const auto landmarks = m_landmarks.lock();
    SIGHT_ASSERT("inout '" << s_LANDMARKS_INOUT << "' does not exist.", landmarks);

    const data::Landmarks::GroupNameContainer groupNames = landmarks->getGroupNames();

    const std::string newGroupNamePrefix = m_advancedMode ? "Group_" : "Point_";

    while(std::find(
              groupNames.begin(),
              groupNames.end(),
              newGroupNamePrefix + std::to_string(groupCount)
          ) != groupNames.end())
    {
        ++groupCount;
    }

    return newGroupNamePrefix + std::to_string(groupCount);
}

//------------------------------------------------------------------------------

std::array<float, 4> SLandmarks::generateNewColor()
{
    const std::array<float, 4> color = {
        m_distributor(m_generator),
        m_distributor(m_generator),
        m_distributor(m_generator),
        m_defaultLandmarkOpacity
    };

    return color;
}

//------------------------------------------------------------------------------

bool SLandmarks::currentSelection(std::string& _selection) const
{
    QTreeWidgetItem* item = m_treeWidget->currentItem();

    const bool selectedGroup = (item != nullptr);

    if(selectedGroup)
    {
        const int topLevelIndex = m_treeWidget->indexOfTopLevelItem(item);

        if(m_advancedMode && topLevelIndex == -1)
        {
            item = item->parent();
        }

        _selection = item->text(0).toStdString();
    }

    return selectedGroup;
}

//------------------------------------------------------------------------------

QColor SLandmarks::convertToQColor(const data::Landmarks::ColorType& _color)
{
    return QColor(
        static_cast<int>(_color[0] * 255),
        static_cast<int>(_color[1] * 255),
        static_cast<int>(_color[2] * 255),
        static_cast<int>(_color[3] * 255)
    );
}

//------------------------------------------------------------------------------

void SLandmarks::setColorButtonIcon(QPushButton* _button, const QColor& _color)
{
    const int iconSize = _button->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);
    pix.fill(_color);

    _button->setIcon(QIcon(pix));
}

//------------------------------------------------------------------------------

QTreeWidgetItem* SLandmarks::getGroupItem(const std::string& _groupName) const
{
    const auto itemList = m_treeWidget->findItems(QString::fromStdString(_groupName), Qt::MatchExactly);

    SIGHT_ASSERT("Only a single item can be named '" + _groupName + "'", itemList.size() == 1);

    return itemList.at(0);
}

} // namespace sight::module::ui::qt::metrics
