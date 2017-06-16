/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurementQt/editor/SLandmarks.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Exception.hpp>
#include <fwData/Landmarks.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/NumericRoundCast.hxx>

#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace uiMeasurementQt
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiMeasurementQt::editor::SLandmarks );

static const ::fwServices::IService::KeyType s_LANDMARKS_INOUT = "landmarks";
static const char* s_GROUP_PROPERTY_NAME                       = "group";
static const int s_GROUP_NAME_ROLE                             = ::Qt::UserRole + 1;
static const float s_DEFAULT_POINT_SIZE                        = 10.;

static const ::fwCom::Slots::SlotKeyType s_ADD_PICKED_POINT_SLOT = "addPickedPoint";
static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT        = "addPoint";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_POINT_SLOT     = "modifyPoint";
static const ::fwCom::Slots::SlotKeyType s_SELECT_POINT_SLOT     = "selectPoint";
static const ::fwCom::Slots::SlotKeyType s_DESELECT_POINT_SLOT   = "deselectPoint";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT     = "removePoint";
static const ::fwCom::Slots::SlotKeyType s_ADD_GROUP_SLOT        = "addGroup";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_GROUP_SLOT     = "removeGroup";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_GROUP_SLOT     = "modifyGroup";
static const ::fwCom::Slots::SlotKeyType s_RENAME_GROUP_SLOT     = "renameGroup";

SLandmarks::SLandmarks() throw() :
    m_advancedMode(false)
{
    newSlot(s_ADD_PICKED_POINT_SLOT, &SLandmarks::addPickedPoint, this);
    newSlot(s_ADD_POINT_SLOT, &SLandmarks::addPoint, this);
    newSlot(s_MODIFY_POINT_SLOT, &SLandmarks::modifyPoint, this);
    newSlot(s_SELECT_POINT_SLOT, &SLandmarks::selectPoint, this);
    newSlot(s_DESELECT_POINT_SLOT, &SLandmarks::deselectPoint, this);
    newSlot(s_ADD_GROUP_SLOT, &SLandmarks::addGroup, this);
    newSlot(s_REMOVE_POINT_SLOT, &SLandmarks::removePoint, this);
    newSlot(s_REMOVE_GROUP_SLOT, &SLandmarks::removeGroup, this);
    newSlot(s_MODIFY_GROUP_SLOT, &SLandmarks::modifyGroup, this);
    newSlot(s_RENAME_GROUP_SLOT, &SLandmarks::renameGroup, this);

    std::srand(::fwTools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

SLandmarks::~SLandmarks() throw()
{
}

//------------------------------------------------------------------------------

void SLandmarks::configuring() throw(fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();

    const ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("service");

    const std::string advancedMode = config.get_optional<std::string>("advanced").get_value_or("no");

    SLM_FATAL_IF("'advanced' value must be 'yes' or 'no', here : '" + advancedMode + "'.",
                 advancedMode != "yes" && advancedMode != "no");

    m_advancedMode = (advancedMode == "yes");
}

//------------------------------------------------------------------------------

void SLandmarks::starting() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout     = new QVBoxLayout();
    QGridLayout* gridLayout = new QGridLayout();

    m_visibilityCheckbox = new QCheckBox();
    QLabel* visibilityLabel = new QLabel(QString("Visibility"));
    m_sizeSlider = new QSlider(Qt::Horizontal);
    m_sizeSlider->setMinimum(1);
    m_sizeSlider->setMaximum(100);
    QLabel* sizeLabel = new QLabel(QString("Size"));
    m_transparencySlider = new QSlider(Qt::Horizontal);
    QLabel* transparencyLabel = new QLabel("Opacity");
    m_shapeSelector = new QComboBox();
    m_shapeSelector->addItem(QString("Cube"));
    m_shapeSelector->addItem(QString("Sphere"));
    QLabel* shapeLabel = new QLabel("Shape");

    m_newGroupButton = m_advancedMode ? new QPushButton("New Group") : nullptr;

    m_removeButton = new QPushButton("Delete");
    m_removeButton->setShortcut(QKeySequence::Delete);

    gridLayout->addWidget(visibilityLabel, 0, 0);
    gridLayout->addWidget(m_visibilityCheckbox, 0, 1);
    gridLayout->addWidget(sizeLabel, 1, 0);
    gridLayout->addWidget(m_sizeSlider, 1, 1);
    gridLayout->addWidget(transparencyLabel, 2, 0);
    gridLayout->addWidget(m_transparencySlider, 2, 1);
    gridLayout->addWidget(shapeLabel, 3, 0);
    gridLayout->addWidget(m_shapeSelector, 3, 1);
    gridLayout->addWidget(m_removeButton, 4, 1);

    m_groupEditorWidget = new QWidget();
    m_groupEditorWidget->setLayout(gridLayout);

    m_treeWidget = new QTreeWidget();
    QLabel* helperTextLabel = new QLabel("Use 'Ctrl+Left Click' to add new landmarks");
    layout->addWidget(helperTextLabel);

    if(m_advancedMode)
    {
        layout->addWidget(m_newGroupButton);
    }

    layout->addWidget(m_treeWidget);
    layout->addWidget(m_groupEditorWidget);
    m_groupEditorWidget->hide();

    QStringList headers;
    headers << "Group" << "Color";

    if(m_advancedMode)
    {
        // Add empty third column to display and edit point coordinates.
        headers << "";
    }

    m_treeWidget->setHeaderLabels(headers);

    qtContainer->setLayout( layout );

    this->updating();
}

//------------------------------------------------------------------------------

void SLandmarks::updating() throw(::fwTools::Failed)
{
    m_treeWidget->blockSignals(true);

    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    m_treeWidget->clear();

    for (const auto& name : landmarks->getGroupNames())
    {
        this->addGroup(name);
        this->addPoint(name);
    }

    QObject::connect(m_treeWidget.data(), &QTreeWidget::itemChanged, this, &SLandmarks::onGroupNameEdited);
    QObject::connect(m_treeWidget.data(), &QTreeWidget::currentItemChanged, this, &SLandmarks::onSelectionChanged);
    QObject::connect(m_sizeSlider.data(), &QSlider::valueChanged, this, &SLandmarks::onSizeChanged);
    QObject::connect(m_transparencySlider.data(), &QSlider::valueChanged, this, &SLandmarks::onTransparencyChanged);
    QObject::connect(m_visibilityCheckbox.data(), &QCheckBox::stateChanged, this, &SLandmarks::onVisibilityChanged);
    QObject::connect(m_shapeSelector.data(), &QComboBox::currentTextChanged, this, &SLandmarks::onShapeChanged);
    QObject::connect(m_removeButton.data(), &QPushButton::clicked, this, &SLandmarks::onRemoveSelection);
    QObject::connect(m_newGroupButton.data(), &QPushButton::clicked, this, &SLandmarks::onAddNewGroup);

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::stopping() throw(::fwTools::Failed)
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SLandmarks::onColorButton()
{
    QObject* sender = this->sender();

    // Create Color choice dialog.
    auto qtContainer         = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    const QColor oldColor = sender->property("color").value<QColor>();
    const QColor colorQt  = QColorDialog::getColor(oldColor, container);
    if(colorQt.isValid())
    {
        const QString key = sender->property("key").toString();

        QPushButton* colorButton = dynamic_cast<QPushButton*>(sender);
        colorButton->setProperty("color", colorQt);

        setColorButtonIcon(colorButton, colorQt);

        const std::string groupName = colorButton->property(s_GROUP_PROPERTY_NAME).value<QString>().toStdString();

        ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

        auto& group = landmarks->getGroup(groupName);
        ::fwData::Landmarks::ColorType color = {{colorQt.red()/255.f, colorQt.green()/255.f, colorQt.blue()/255.f,
                                                                      colorQt.alpha()/255.f}};

        group.m_color = color;

        auto sig = landmarks->signal< ::fwData::Landmarks::GroupModifiedSignalType >(
            ::fwData::Landmarks::s_GROUP_MODIFIED_SIG);

        {
            ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));
            sig->asyncEmit(groupName);
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onGroupNameEdited(QTreeWidgetItem* item, int column)
{
    OSLM_ERROR_IF("A column different from the group's name is being edited", column != 0);
    m_treeWidget->blockSignals(true);

    if(column == 0)
    {
        ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

        QString oldGroupName = item->data(0, s_GROUP_NAME_ROLE).toString();
        QString newGroupName = item->text(0);

        if(newGroupName.isEmpty())
        {
            QString msg = "The new group name for '" + oldGroupName +
                          "' is empty. Please enter a valid name and try again";
            QMessageBox msgBox(QMessageBox::Warning, "No group name", msg, QMessageBox::Ok);
            msgBox.exec();

            item->setText(0, oldGroupName);
        }
        else if(oldGroupName != newGroupName)
        {
            try
            {
                landmarks->renameGroup(oldGroupName.toStdString(), newGroupName.toStdString());

                auto sig = landmarks->signal< ::fwData::Landmarks::GroupRenamedSignalType >(
                    ::fwData::Landmarks::s_GROUP_RENAMED_SIG);

                {
                    ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_RENAME_GROUP_SLOT)));
                    sig->asyncEmit(oldGroupName.toStdString(), newGroupName.toStdString());
                }

                item->setData(0, s_GROUP_NAME_ROLE, newGroupName);
                QWidget* widget = m_treeWidget->itemWidget(item, 1);

                widget->setProperty(s_GROUP_PROPERTY_NAME, newGroupName);
            }
            catch(::fwData::Exception& e)
            {
                QString msg = "Can't rename '" + oldGroupName +"' as '" + newGroupName + ".\n" +
                              QString(e.what());
                QMessageBox msgBox(QMessageBox::Warning, "Can't rename" + oldGroupName, msg, QMessageBox::Ok);
                msgBox.exec();

                item->setText(0, oldGroupName);
            }
        }
    }
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::onSelectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    if(previous)
    {
        auto deselectSig = landmarks->signal< ::fwData::Landmarks::PointDeselectedSignalType >(
            ::fwData::Landmarks::s_POINT_DESELECTED_SIG);

        ::fwCom::Connection::Blocker block(deselectSig->getConnection(this->slot(s_DESELECT_POINT_SLOT)));

        if(m_advancedMode)
        {
            QTreeWidgetItem* previousParent = previous->parent();

            if(previousParent != nullptr)
            {
                const std::string& groupName = previousParent->text(0).toStdString();

                size_t index = static_cast<size_t>(previousParent->indexOfChild(previous));

                SLM_ASSERT("index must be inferior to the number of points in '" + groupName +"'.",
                           index < landmarks->getNumberOfPoints(groupName));

                deselectSig->asyncEmit(groupName, index);
            }
        }
        else
        {
            const std::string& groupName = previous->text(0).toStdString();

            deselectSig->asyncEmit(groupName, 0);
        }
    }

    if(current)
    {
        auto selectSig = landmarks->signal< ::fwData::Landmarks::PointSelectedSignalType >(
            ::fwData::Landmarks::s_POINT_SELECTED_SIG);

        std::string groupName;
        if(m_advancedMode)
        {
            QTreeWidgetItem* currentParent = current->parent();

            if(currentParent != nullptr)
            {
                groupName = currentParent->text(0).toStdString();

                size_t index = static_cast<size_t>(currentParent->indexOfChild(current));

                SLM_ASSERT("index must be inferior to the number of points in '" + groupName +"'.",
                           index < landmarks->getNumberOfPoints(groupName));

                ::fwCom::Connection::Blocker block(selectSig->getConnection(this->slot(s_SELECT_POINT_SLOT)));
                selectSig->asyncEmit(groupName, index);
            }
            else
            {
                groupName = current->text(0).toStdString();
            }
        }
        else
        {
            groupName = current->text(0).toStdString();

            ::fwCom::Connection::Blocker block(selectSig->getConnection(this->slot(s_SELECT_POINT_SLOT)));
            selectSig->asyncEmit(groupName, 0);
        }

        const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);

        // Set widget values
        m_sizeSlider->setValue(static_cast<int>(group.m_size));
        m_visibilityCheckbox->setChecked(group.m_visibility);

        const QString shapeText = group.m_shape == ::fwData::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
        m_shapeSelector->setCurrentText(shapeText);

        float transparency = group.m_color[3];
        m_transparencySlider->setValue(static_cast<int>(transparency * m_transparencySlider->maximum()));

    }

    m_groupEditorWidget->setHidden(current == nullptr);
}

//------------------------------------------------------------------------------

void SLandmarks::onSizeChanged(int newSize)
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    const ::fwData::Landmarks::SizeType realSize = static_cast< ::fwData::Landmarks::SizeType >(newSize);

    std::string groupName;
    if(currentSelection(groupName))
    {
        landmarks->setGroupSize(groupName, realSize);

        auto sig = landmarks->signal< ::fwData::Landmarks::GroupModifiedSignalType >(
            ::fwData::Landmarks::s_GROUP_MODIFIED_SIG);

        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onTransparencyChanged(int newTransparency)
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    const float sliderSize = static_cast<float>( m_transparencySlider->maximum() - m_transparencySlider->minimum());

    const float realTransparency = static_cast<float>(newTransparency) / sliderSize;

    std::string groupName;
    if(currentSelection(groupName))
    {
        const auto groupColor = landmarks->getGroup(groupName).m_color;

        ::fwData::Landmarks::ColorType newGroupColor
            = {{groupColor[0], groupColor[1], groupColor[2], realTransparency}};

        landmarks->setGroupColor(groupName, newGroupColor);

        auto sig = landmarks->signal< ::fwData::Landmarks::GroupModifiedSignalType >(
            ::fwData::Landmarks::s_GROUP_MODIFIED_SIG);

        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onVisibilityChanged(int visibility)
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    std::string groupName;
    if(currentSelection(groupName))
    {
        landmarks->setGroupVisibility(groupName, static_cast<bool>(visibility));

        auto sig = landmarks->signal< ::fwData::Landmarks::GroupModifiedSignalType >(
            ::fwData::Landmarks::s_GROUP_MODIFIED_SIG);

        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onShapeChanged(const QString& shape)
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    std::string groupName;
    if(currentSelection(groupName))
    {
        SLM_ASSERT("Shape must be 'Cube' or 'Sphere'.", shape == "Cube" || shape == "Sphere");
        ::fwData::Landmarks::Shape s
            = (shape == "Cube") ? ::fwData::Landmarks::Shape::CUBE : ::fwData::Landmarks::Shape::SPHERE;

        landmarks->setGroupShape(groupName, s);

        auto sig = landmarks->signal< ::fwData::Landmarks::GroupModifiedSignalType >(
            ::fwData::Landmarks::s_GROUP_MODIFIED_SIG);

        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_MODIFY_GROUP_SLOT)));

        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onAddNewGroup()
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    const std::string groupName = this->generateNewGroupName();
    landmarks->addGroup(groupName, this->generateNewColor(), s_DEFAULT_POINT_SIZE);

    this->addGroup(groupName);

    auto sig = landmarks->signal< ::fwData::Landmarks::GroupAddedSignalType >(
        ::fwData::Landmarks::s_GROUP_ADDED_SIG);

    {
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_GROUP_SLOT)));
        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onRemoveSelection()
{
    m_treeWidget->blockSignals(true);

    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);
    QTreeWidgetItem* item = m_treeWidget->currentItem();

    if(item != nullptr)
    {
        const int topLevelIndex = m_treeWidget->indexOfTopLevelItem(item);

        if(m_advancedMode && topLevelIndex == -1) // Delete point
        {
            QTreeWidgetItem* itemParent = item->parent();

            const size_t index           = static_cast<size_t>(itemParent->indexOfChild(item));
            const std::string& groupName = itemParent->text(0).toStdString();

            landmarks->removePoint(groupName, index);
            itemParent->removeChild(item);

            auto sig = landmarks->signal< ::fwData::Landmarks::PointRemovedSignalType >(
                ::fwData::Landmarks::s_POINT_REMOVED_SIG);

            {
                ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_POINT_SLOT)));
                sig->asyncEmit(groupName, index);
            }
        }
        else
        {
            const std::string& groupName = item->text(0).toStdString();

            landmarks->removeGroup(groupName);
            delete m_treeWidget->takeTopLevelItem(topLevelIndex);

            auto sig = landmarks->signal< ::fwData::Landmarks::GroupRemovedSignalType >(
                ::fwData::Landmarks::s_GROUP_REMOVED_SIG);

            {
                ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_GROUP_SLOT)));
                sig->asyncEmit(groupName);
            }
        }

        m_treeWidget->setCurrentItem(nullptr);
        m_groupEditorWidget->setHidden(true);
    }

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::addPickedPoint(::fwDataTools::PickingInfo pickingInfo)
{
    if(pickingInfo.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP &&
       pickingInfo.m_modifierMask & ::fwDataTools::PickingInfo::CTRL)
    {
        double* pickedPos = pickingInfo.m_worldPos;
        ::fwData::Landmarks::PointType newPoint = {{ pickedPos[0], pickedPos[1], pickedPos[2] }};

        ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

        std::string groupName;
        QTreeWidgetItem* item = m_treeWidget->currentItem();

        if(item == nullptr || !m_advancedMode) // No selection or simple mode, create a new group.
        {
            groupName = this->generateNewGroupName();
            landmarks->addGroup(groupName, this->generateNewColor(), s_DEFAULT_POINT_SIZE);

            this->addGroup(groupName);

            auto sig = landmarks->signal< ::fwData::Landmarks::GroupAddedSignalType >(
                ::fwData::Landmarks::s_GROUP_ADDED_SIG);

            {
                ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_GROUP_SLOT)));
                sig->asyncEmit(groupName);
            }
        }
        else // Advanced mode and a point or a group is selected.
        {
            const int topLevelIndex = m_treeWidget->indexOfTopLevelItem(item);

            if(topLevelIndex == -1) // Point selected, add new point to parent group.
            {
                item = item->parent();
            }
            groupName = item->text(0).toStdString();
        }

        landmarks->addPoint(groupName, newPoint);
        this->addPoint(groupName);

        auto sig =
            landmarks->signal< ::fwData::Landmarks::PointAddedSignalType >(::fwData::Landmarks::s_POINT_ADDED_SIG);

        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_POINT_SLOT)));
        sig->asyncEmit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string groupName)
{
    if(m_advancedMode)
    {
        m_treeWidget->blockSignals(true);

        ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

        QTreeWidgetItem* item = getGroupItem(groupName);

        const size_t nbChilds = static_cast<size_t>(item->childCount());
        const size_t nbPoints = landmarks->getNumberOfPoints(groupName);
        for(size_t idx = nbChilds; idx < nbPoints; ++idx)
        {
            const ::fwData::Landmarks::PointType& newPoint = landmarks->getPoint(groupName, idx);

            QTreeWidgetItem* pt = new QTreeWidgetItem();
            for(int i = 0; i < 3; ++i)
            {
                pt->setText(i, QString::fromStdString(std::to_string(newPoint[static_cast<size_t>(i)])));
            }
            item->addChild(pt);
        }

        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::addGroup(std::string name)
{
    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    const auto& group = landmarks->getGroup(name);

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setData(0, s_GROUP_NAME_ROLE, QString::fromStdString(name));

    m_treeWidget->addTopLevelItem(item);

    item->setText(0, QString::fromStdString(name));
    QPushButton* button = new QPushButton();

    const QColor color = convertToQColor(group.m_color);

    setColorButtonIcon(button, color);
    button->setProperty("color", color);
    button->setProperty(s_GROUP_PROPERTY_NAME, QString::fromStdString(name));

    QObject::connect(button, &QPushButton::clicked, this, &SLandmarks::onColorButton);

    m_treeWidget->setItemWidget(item, 1, button);
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string name)
{
    QTreeWidgetItem* item = getGroupItem(name);

    while(item->childCount() != 0)
    {
        item->removeChild(item->child(0));
    }
    const int index          = m_treeWidget->indexOfTopLevelItem(item);
    QTreeWidgetItem* topItem = m_treeWidget->takeTopLevelItem(index);
    delete topItem;
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string groupName, size_t index)
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* item = getGroupItem(groupName);

    OSLM_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(index) < item->childCount());

    QTreeWidgetItem* pointItem = item->child(static_cast<int>(index));
    item->removeChild(pointItem);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::renameGroup(std::string oldName, std::string newName)
{
    m_treeWidget->blockSignals(true);

    QString qtNewName     = QString::fromStdString(newName);
    QTreeWidgetItem* item = getGroupItem(oldName);
    item->setData(0, s_GROUP_NAME_ROLE, qtNewName);
    QWidget* widget = m_treeWidget->itemWidget(item, 1);
    widget->setProperty(s_GROUP_PROPERTY_NAME, qtNewName);

    item->setText(0, qtNewName);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::modifyGroup(std::string name)
{
    QTreeWidgetItem* item = getGroupItem(name);

    item->setText(0, name.c_str());

    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(name);

    QPushButton* colorButton = dynamic_cast< QPushButton* >(m_treeWidget->itemWidget(item, 1));

    const QColor color = convertToQColor(group.m_color);

    setColorButtonIcon(colorButton, color);

    bool groupSelected = item->isSelected();

    if(m_advancedMode) // Check if a child is selected.
    {
        for(int i = 0; i < item->childCount() && !groupSelected; ++i)
        {
            groupSelected = item->child(i)->isSelected();
        }
    }

    if(groupSelected)
    {
        const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(name);

        // Set widget values
        m_sizeSlider->setValue(static_cast<int>(group.m_size));
        m_visibilityCheckbox->setChecked(group.m_visibility);

        QString shapeText = group.m_shape == ::fwData::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
        m_shapeSelector->setCurrentText(shapeText);

        float transparency = group.m_color[3];
        m_transparencySlider->setValue(static_cast<int>(transparency * m_transparencySlider->maximum()));
    }

}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string groupName, size_t index)
{
    if( m_advancedMode )
    {
        ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

        auto itemList = m_treeWidget->findItems(QString::fromStdString(groupName), Qt::MatchExactly);

        SLM_ASSERT("Only a single item can be named '" + groupName + "'", itemList.size() == 1);

        QTreeWidgetItem* item = itemList.at(0);

        OSLM_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(index) < item->childCount());

        QTreeWidgetItem* pointItem                  = item->child(static_cast<int>(index));
        const ::fwData::Landmarks::PointType& point = landmarks->getPoint(groupName, index);

        m_treeWidget->blockSignals(true);
        for(int i = 0; i < 3; ++i)
        {
            pointItem->setText(i, QString("%1").arg(point[static_cast<size_t>(i)]));
        }
        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string groupName, size_t index)
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* currentItem = getGroupItem(groupName);

    if(m_advancedMode)
    {
        OSLM_ASSERT(
            "Index must be less than " << currentItem->childCount(),
            static_cast<int>(index) < currentItem->childCount());

        currentItem = currentItem->child(static_cast<int>(index));
    }
    m_treeWidget->setCurrentItem(currentItem);

    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);

    // Set widget values
    m_sizeSlider->setValue(static_cast<int>(group.m_size));
    m_visibilityCheckbox->setChecked(group.m_visibility);

    const QString shapeText = group.m_shape == ::fwData::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
    m_shapeSelector->setCurrentText(shapeText);

    float transparency = group.m_color[3];
    m_transparencySlider->setValue(static_cast<int>(transparency * m_transparencySlider->maximum()));

    m_groupEditorWidget->show();
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string, size_t)
{
    m_treeWidget->blockSignals(true);
    m_treeWidget->setCurrentItem(nullptr);
    m_groupEditorWidget->hide();
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

std::string SLandmarks::generateNewGroupName() const
{
    static size_t groupCount = 0;

    ::fwData::Landmarks::sptr landmarks = this->getInOut< ::fwData::Landmarks >(s_LANDMARKS_INOUT);

    ::fwData::Landmarks::GroupNameContainer groupNames = landmarks->getGroupNames();

    const std::string newGroupNamePrefix = m_advancedMode ? "Group_" : "Point_";

    while(std::find(groupNames.begin(), groupNames.end(),
                    newGroupNamePrefix + std::to_string(groupCount)) != groupNames.end())
    {
        ++groupCount;
    }

    return newGroupNamePrefix + std::to_string(groupCount);
}

//------------------------------------------------------------------------------

std::array<float, 4> SLandmarks::generateNewColor()
{
    std::array<float, 4> color = {{rand()%255/255.f, rand()%255/255.f, rand()%255/255.f, 1.f}};
    return color;
}

//------------------------------------------------------------------------------

bool SLandmarks::currentSelection(std::string& selection) const
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

        selection = item->text(0).toStdString();
    }

    return selectedGroup;
}

//------------------------------------------------------------------------------

QColor SLandmarks::convertToQColor(const ::fwData::Landmarks::ColorType& color)
{
    return QColor(
        static_cast<int>(color[0]*255),
        static_cast<int>(color[1]*255),
        static_cast<int>(color[2]*255),
        static_cast<int>(color[3]*255)
        );
}

//------------------------------------------------------------------------------

void SLandmarks::setColorButtonIcon(QPushButton* button, const QColor& color)
{
    const int iconSize = button->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);
    pix.fill(color);

    button->setIcon(QIcon(pix));
}

//------------------------------------------------------------------------------

QTreeWidgetItem* SLandmarks::getGroupItem(const std::string& groupName) const
{
    const auto itemList = m_treeWidget->findItems(QString::fromStdString(groupName), Qt::MatchExactly);

    SLM_ASSERT("Only a single item can be named '" + groupName + "'", itemList.size() == 1);

    return itemList.at(0);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_POINT_MODIFIED_SIG, s_MODIFY_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_POINT_SELECTED_SIG, s_SELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_POINT_DESELECTED_SIG, s_DESELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_GROUP_ADDED_SIG, s_ADD_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_GROUP_REMOVED_SIG, s_REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_GROUP_MODIFIED_SIG, s_MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, ::fwData::Landmarks::s_GROUP_RENAMED_SIG, s_RENAME_GROUP_SLOT);

    return connections;
}

} // namespace editor
} // namespace uiMeasurementQt

