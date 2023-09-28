/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "data/ImageSeries.hpp"
#include "data/Landmarks.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/FiducialsSeries.hpp>

#include <geometry/data/Matrix4.hpp>

#include <ui/qt/container/widget.hpp>

#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include <cfloat>
#include <cmath>
#include <random>

namespace sight::module::ui::qt::metrics
{

static_assert(
    std::is_same_v<data::Landmarks::GroupModifiedSignalType, data::has_fiducials::signals::GroupModified>,
    "'groupModified' signal from data::Landmarks and data::has_fiducials must have the same signature"
);

namespace
{

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<std::string> getGroupNames(LandmarksOrImageSeriesConstPtr li)
{
    if(li.landmarks != nullptr)
    {
        return li.landmarks->getGroupNames();
    }

    if(li.imageSeries != nullptr)
    {
        return li.imageSeries->getFiducials()->getPointFiducialsGroupNames();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::size_t> numPoints(LandmarksOrImageSeriesConstPtr li, const std::string& groupName)
{
    if(li.landmarks != nullptr)
    {
        if(!li.landmarks->hasGroup(groupName))
        {
            return std::nullopt;
        }

        return li.landmarks->numPoints(groupName);
    }

    if(li.imageSeries != nullptr)
    {
        return li.imageSeries->getFiducials()->getNumberOfPointsInGroup(groupName);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::Landmarks::LandmarksGroup> getGroup(
    LandmarksOrImageSeriesConstPtr li,
    const std::string& groupName
)
{
    if(li.landmarks != nullptr)
    {
        if(!li.landmarks->hasGroup(groupName))
        {
            return std::nullopt;
        }

        return li.landmarks->getGroup(groupName);
    }

    if(li.imageSeries != nullptr)
    {
        return li.imageSeries->getFiducials()->getGroup(groupName);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::Landmarks::PointType> getPoint(
    LandmarksOrImageSeriesConstPtr li,
    const std::string& groupName,
    std::size_t index
)
{
    if(li.landmarks != nullptr)
    {
        if(!li.landmarks->hasGroup(groupName))
        {
            return std::nullopt;
        }

        return li.landmarks->getPoint(groupName, index);
    }

    if(li.imageSeries != nullptr)
    {
        return li.imageSeries->getFiducials()->getPoint(groupName, index);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

} // namespace

//------------------------------------------------------------------------------

static const char* s_GROUP_PROPERTY_NAME = "group";
static const int s_GROUP_NAME_ROLE       = Qt::UserRole + 1;

static const core::com::slots::key_t ADD_POINT_SLOT      = "addPoint";
static const core::com::slots::key_t MODIFY_POINT_SLOT   = "modifyPoint";
static const core::com::slots::key_t SELECT_POINT_SLOT   = "selectPoint";
static const core::com::slots::key_t DESELECT_POINT_SLOT = "deselectPoint";
static const core::com::slots::key_t REMOVE_POINT_SLOT   = "removePoint";
static const core::com::slots::key_t ADD_GROUP_SLOT      = "addGroup";
static const core::com::slots::key_t REMOVE_GROUP_SLOT   = "removeGroup";
static const core::com::slots::key_t MODIFY_GROUP_SLOT   = "modifyGroup";
static const core::com::slots::key_t RENAME_GROUP_SLOT   = "renameGroup";

static const std::string s_SIZE_CONFIG     = "size";
static const std::string s_OPACITY_CONFIG  = "opacity";
static const std::string s_ADVANCED_CONFIG = "advanced";
static const std::string s_TEXT_CONFIG     = "text";

const core::com::signals::key_t SLandmarks::SEND_WORLD_COORD = "sendWorldCoord";
const core::com::signals::key_t SLandmarks::GROUP_SELECTED   = "groupSelected";

//------------------------------------------------------------------------------

SLandmarks::SLandmarks() noexcept
{
    new_slot(ADD_POINT_SLOT, &SLandmarks::addPoint, this);
    new_slot(MODIFY_POINT_SLOT, &SLandmarks::modifyPoint, this);
    new_slot(SELECT_POINT_SLOT, &SLandmarks::selectPoint, this);
    new_slot(DESELECT_POINT_SLOT, &SLandmarks::deselectPoint, this);
    new_slot(ADD_GROUP_SLOT, &SLandmarks::addGroup, this);
    new_slot(REMOVE_POINT_SLOT, &SLandmarks::removePoint, this);
    new_slot(REMOVE_GROUP_SLOT, &SLandmarks::removeGroup, this);
    new_slot(MODIFY_GROUP_SLOT, &SLandmarks::modifyGroup, this);
    new_slot(RENAME_GROUP_SLOT, &SLandmarks::renameGroup, this);

    new_signal<world_coordinates_signal_t>(SEND_WORLD_COORD);
    new_signal<GroupSelectedSignal>(GROUP_SELECTED);
}

//------------------------------------------------------------------------------

SLandmarks::~SLandmarks() noexcept =
    default;

//------------------------------------------------------------------------------

void SLandmarks::configuring()
{
    this->sight::ui::service::initialize();

    const service::config_t config = this->getConfiguration();

    m_defaultLandmarkSize = config.get<float>(s_SIZE_CONFIG, m_defaultLandmarkSize);
    SIGHT_FATAL_IF(
        "'size' value must be a positive number greater than 0 (current value: " << m_defaultLandmarkSize << ")",
        m_defaultLandmarkSize <= 0.F
    );

    m_defaultLandmarkOpacity = config.get<float>(s_OPACITY_CONFIG, m_defaultLandmarkOpacity);
    SIGHT_FATAL_IF(
        "'opacity' value must be a number between 0.0 and 1.0 (current value: " << m_defaultLandmarkOpacity << ")",
        m_defaultLandmarkOpacity<0.F || m_defaultLandmarkOpacity>1.F
    );

    m_advancedMode = config.get<bool>(s_ADVANCED_CONFIG, false);

    m_text = config.get<std::string>(s_TEXT_CONFIG, m_text);
}

//------------------------------------------------------------------------------

void SLandmarks::starting()
{
    this->sight::ui::service::create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    const auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    auto* const layout     = new QVBoxLayout();
    auto* const gridLayout = new QGridLayout();

    m_visibilityCheckbox = new QCheckBox();
    auto* const visibilityLabel = new QLabel(QString("Visibility"));
    m_visibilityCheckbox->setObjectName(serviceID + "/" + visibilityLabel->text());
    m_sizeSlider = new QSlider(Qt::Horizontal);
    m_sizeSlider->setMinimum(1);
    m_sizeSlider->setMaximum(100);
    auto* const sizeLabel = new QLabel(QString("Size"));
    m_sizeSlider->setObjectName(serviceID + "/" + sizeLabel->text());
    m_opacitySlider = new QSlider(Qt::Horizontal);
    auto* const opacityLabel = new QLabel("Opacity");
    m_opacitySlider->setObjectName(serviceID + "/" + opacityLabel->text());
    m_shapeSelector = new QComboBox();
    m_shapeSelector->addItem(QString("Cube"));
    m_shapeSelector->addItem(QString("Sphere"));
    auto* const shapeLabel = new QLabel("Shape");
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
    m_treeWidget->setObjectName(serviceID + "/treeWidget");
    if(!m_text.empty())
    {
        auto* helperTextLabel = new QLabel(QString::fromStdString(m_text));
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

    SIGHT_ASSERT(
        "Either 'landmarks' or 'imageSeries' parameter must be set.",
        (m_landmarks.const_lock() != nullptr) + (m_imageSeries.const_lock() != nullptr) == 1
    );
    if(auto imageSeries = m_imageSeries.lock())
    {
        imageSeries->getFiducials()->setGroupNamesForPointFiducials();
    }
}

//------------------------------------------------------------------------------

service::connections_t SLandmarks::getAutoConnections() const
{
    connections_t connections;

    connections.push(s_LANDMARKS_INOUT, data::Landmarks::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::POINT_ADDED_SIG, ADD_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::POINT_MODIFIED_SIG, MODIFY_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::POINT_SELECTED_SIG, SELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::POINT_DESELECTED_SIG, DESELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::GROUP_ADDED_SIG, ADD_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::GROUP_REMOVED_SIG, REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::POINT_REMOVED_SIG, REMOVE_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::GROUP_MODIFIED_SIG, MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::GROUP_RENAMED_SIG, RENAME_GROUP_SLOT);

    connections.push(s_IMAGE_SERIES_INOUT, data::ImageSeries::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_ADDED, ADD_POINT_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_MODIFIED, MODIFY_POINT_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_SELECTED, SELECT_POINT_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_DESELECTED, DESELECT_POINT_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_ADDED, ADD_GROUP_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_REMOVED, REMOVE_GROUP_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_REMOVED, REMOVE_POINT_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_MODIFIED, MODIFY_GROUP_SLOT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_RENAMED, RENAME_GROUP_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SLandmarks::updating()
{
    m_treeWidget->blockSignals(true);

    {
        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        m_treeWidget->clear();

        for(const auto& name : getGroupNames(liLock))
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
    auto qtContainer         = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    const auto oldColor  = sender->property("color").value<QColor>();
    const QColor colorQt = QColorDialog::getColor(oldColor, container, "Select Color", QColorDialog::ShowAlphaChannel);
    if(colorQt.isValid())
    {
        auto* const colorButton = dynamic_cast<QPushButton*>(sender);
        colorButton->setProperty("color", colorQt);

        setColorButtonIcon(colorButton, colorQt);

        const std::string groupName = colorButton->property(s_GROUP_PROPERTY_NAME).value<QString>().toStdString();

        data::Landmarks::ColorType color = {{
            float(colorQt.red()) / 255.F, float(colorQt.green()) / 255.F,
            float(colorQt.blue()) / 255.F, float(colorQt.alpha()) / 255.F
        }
        };

        data::Landmarks::GroupModifiedSignalType::sptr sig;
        {
            LandmarksOrImageSeriesLock liLock = lock();
            SIGHT_ASSERT(
                "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
                liLock.landmarks != nullptr || liLock.imageSeries != nullptr
            );
            if(liLock.landmarks != nullptr)
            {
                auto& group = liLock.landmarks->getGroup(groupName);
                group.m_color = color;

                sig = liLock.landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
                    data::Landmarks::GROUP_MODIFIED_SIG
                );
            }
            else if(liLock.imageSeries != nullptr)
            {
                std::optional<std::pair<data::FiducialsSeries::FiducialSet, std::size_t> > fiducialSet =
                    liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
                if(!fiducialSet.has_value())
                {
                    return;
                }

                liLock.imageSeries->getFiducials()->setColor(fiducialSet->second, color);
                sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                    data::has_fiducials::signals::GROUP_MODIFIED
                );
            }
        }

        m_opacitySlider->setValue(static_cast<int>(color[3] * float(m_opacitySlider->maximum())));

        {
            core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));
            sig->async_emit(groupName);
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
                    LandmarksOrImageSeriesLock liLock = lock();
                    SIGHT_ASSERT(
                        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
                        liLock.landmarks != nullptr || liLock.imageSeries != nullptr
                    );

                    if(liLock.landmarks != nullptr)
                    {
                        liLock.landmarks->renameGroup(oldGroupName.toStdString(), newGroupName.toStdString());

                        const auto sig = liLock.landmarks->signal<data::Landmarks::GroupRenamedSignalType>(
                            data::Landmarks::GROUP_RENAMED_SIG
                        );

                        {
                            core::com::connection::blocker block(sig->get_connection(this->slot(RENAME_GROUP_SLOT)));
                            sig->async_emit(oldGroupName.toStdString(), newGroupName.toStdString());
                        }
                    }
                    else if(liLock.imageSeries != nullptr)
                    {
                        std::optional<std::pair<data::FiducialsSeries::FiducialSet, std::size_t> > fiducialSet =
                            liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(oldGroupName.toStdString());
                        if(!fiducialSet.has_value())
                        {
                            throw data::Exception("'" + oldGroupName.toStdString() + "' group doesn't exist");
                        }

                        liLock.imageSeries->getFiducials()->setGroupName(
                            fiducialSet->second,
                            newGroupName.toStdString()
                        );

                        const auto sig =
                            liLock.imageSeries->signal<data::has_fiducials::signals::GroupRenamed>(
                                data::has_fiducials::signals::GROUP_RENAMED
                            );

                        core::com::connection::blocker block(sig->get_connection(this->slot(RENAME_GROUP_SLOT)));
                        sig->async_emit(oldGroupName.toStdString(), newGroupName.toStdString());
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
    if(_previous != nullptr)
    {
        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::Landmarks::PointDeselectedSignalType::sptr deselectSig;
        static_assert(
            std::is_same_v<data::Landmarks::PointDeselectedSignalType,
                           data::has_fiducials::signals::PointDeselected>,
            "'pointDeselected' signal from data::Landmarks and data::has_fiducials must have the same signature"
        );
        if(liLock.landmarks != nullptr)
        {
            deselectSig = liLock.landmarks->signal<data::Landmarks::PointDeselectedSignalType>(
                data::Landmarks::POINT_DESELECTED_SIG
            );
        }
        else if(liLock.imageSeries != nullptr)
        {
            deselectSig = liLock.imageSeries->signal<data::has_fiducials::signals::PointDeselected>(
                data::has_fiducials::signals::POINT_DESELECTED
            );
        }

        const core::com::connection::blocker block(deselectSig->get_connection(this->slot(DESELECT_POINT_SLOT)));

        if(m_advancedMode)
        {
            const QTreeWidgetItem* const previousParent = _previous->parent();

            if(previousParent != nullptr)
            {
                const std::string& groupName = previousParent->text(0).toStdString();

                const auto index = static_cast<std::size_t>(previousParent->indexOfChild(_previous));

                SIGHT_ASSERT(
                    "index must be inferior to the number of points in '" + groupName + "'.",
                    index < numPoints(liLock, groupName)
                );

                deselectSig->async_emit(groupName, index);
            }
        }
        else
        {
            const std::string& groupName = _previous->text(0).toStdString();

            deselectSig->async_emit(groupName, 0);
        }
    }

    if(_current != nullptr)
    {
        int size     = 0;
        bool visible = false;
        QString shapeText;
        float opacity = NAN;
        {
            LandmarksOrImageSeriesConstLock liLock = constLock();
            SIGHT_ASSERT(
                "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
                liLock.landmarks != nullptr || liLock.imageSeries != nullptr
            );

            data::Landmarks::PointSelectedSignalType::sptr selectSig;
            static_assert(
                std::is_same_v<data::Landmarks::PointSelectedSignalType,
                               data::has_fiducials::signals::PointSelected>,
                "'pointSelected' signal from data::Landmarks and data::has_fiducials must have the same signature"
            );
            if(liLock.landmarks != nullptr)
            {
                selectSig = liLock.landmarks->signal<data::Landmarks::PointSelectedSignalType>(
                    data::Landmarks::POINT_SELECTED_SIG
                );
            }
            else if(liLock.imageSeries != nullptr)
            {
                selectSig = liLock.imageSeries->signal<data::has_fiducials::signals::PointSelected>(
                    data::has_fiducials::signals::POINT_SELECTED
                );
            }

            std::string groupName;

            if(m_advancedMode)
            {
                const QTreeWidgetItem* const currentParent = _current->parent();

                if(currentParent != nullptr)
                {
                    groupName = currentParent->text(0).toStdString();

                    const auto index = static_cast<std::size_t>(currentParent->indexOfChild(_current));

                    SIGHT_ASSERT(
                        "index must be inferior to the number of points in '" + groupName + "'.",
                        index < numPoints(liLock, groupName)
                    );

                    const core::com::connection::blocker block(selectSig->get_connection(this->slot(SELECT_POINT_SLOT)));
                    selectSig->async_emit(groupName, index);
                }
                else
                {
                    groupName = _current->text(0).toStdString();
                }
            }
            else
            {
                groupName = _current->text(0).toStdString();

                core::com::connection::blocker block(selectSig->get_connection(this->slot(SELECT_POINT_SLOT)));
                selectSig->async_emit(groupName, 0);
            }

            std::optional<data::Landmarks::LandmarksGroup> group = getGroup(liLock, groupName);
            if(!group.has_value())
            {
                return;
            }

            size      = static_cast<int>(group->m_size);
            visible   = group->m_visibility;
            shapeText = group->m_shape == data::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
            opacity   = group->m_color[3];

            signal<GroupSelectedSignal>(GROUP_SELECTED)->async_emit(groupName);
        }

        // Set widget values
        m_sizeSlider->setValue(size);
        m_visibilityCheckbox->setChecked(visible);
        m_shapeSelector->setCurrentText(shapeText);
        m_opacitySlider->setValue(static_cast<int>(opacity * float(m_opacitySlider->maximum())));
    }
    else
    {
        signal<GroupSelectedSignal>(GROUP_SELECTED)->async_emit("");
    }

    m_groupEditorWidget->setDisabled(_current == nullptr);
}

//------------------------------------------------------------------------------

void SLandmarks::onLandmarkDoubleClicked(QTreeWidgetItem* _item, int /*unused*/) const
{
    if(_item != nullptr)
    {
        // Exclude top level item
        if(_item->childCount() != 0)
        {
            return;
        }

        std::array<QString, 3> index {
            _item->text(0),
            _item->text(1),
            _item->text(2)
        };

        if(index[0].isEmpty() || index[1].isEmpty() || index[2].isEmpty())
        {
            // Do nothing if a value is missing.
            return;
        }

        // Convert to double
        std::array check = {false, false, false};
        data::Landmarks::PointType world_coord {
            index[0].toDouble(check.data()),
            index[1].toDouble(&check[1]),
            index[2].toDouble(&check[2])
        };

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

        updateCurrentLandmark(world_coord);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::updateCurrentLandmark(data::Landmarks::PointType& world_coord) const
{
    // Send signal with world coordinates of the landmarks

    SIGHT_DEBUG(
        " Send world coordinates [" << world_coord[0] << ", " << world_coord[1]
        << ", " << world_coord[2] << " ]"
    );
    this->signal<world_coordinates_signal_t>(SEND_WORLD_COORD)->async_emit(
        world_coord[0],
        world_coord[1],
        world_coord[2]
    );

    //update the data with the current point
    auto currentLandmark = m_currentLandmark.lock();
    if(currentLandmark)
    {
        currentLandmark->setCoord(world_coord);
        auto currentLandmarkModifiedSig =
            currentLandmark->signal<sight::data::Object::ModifiedSignalType>(
                sight::data::Object::MODIFIED_SIG
            );
        currentLandmarkModifiedSig->async_emit();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onSizeChanged(int _newSize)
{
    const auto realSize = static_cast<data::Landmarks::SizeType>(_newSize);

    std::string groupName;
    if(currentSelection(groupName))
    {
        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::Landmarks::GroupModifiedSignalType::sptr sig;
        if(liLock.landmarks != nullptr)
        {
            sig = liLock.landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
                data::Landmarks::GROUP_MODIFIED_SIG
            );
            liLock.landmarks->setGroupSize(groupName, realSize);
        }
        else if(liLock.imageSeries != nullptr)
        {
            sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::FiducialsSeries::FiducialSet,
                                    std::size_t> > fiducialSet =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fiducialSet.has_value())
            {
                SIGHT_WARN("Couldn't change size of size of group '" << groupName << "', it doesn't exist");
                return;
            }

            liLock.imageSeries->getFiducials()->setSize(fiducialSet->second, realSize);
        }

        const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onOpacityChanged(int _newOpacity)
{
    const auto sliderSize = static_cast<float>(m_opacitySlider->maximum() - m_opacitySlider->minimum());

    const float realOpacity = static_cast<float>(_newOpacity) / sliderSize;

    std::string groupName;
    if(currentSelection(groupName))
    {
        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::Landmarks::GroupModifiedSignalType::sptr sig;

        if(liLock.landmarks != nullptr)
        {
            sig = liLock.landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
                data::Landmarks::GROUP_MODIFIED_SIG
            );

            const auto groupColor = liLock.landmarks->getGroup(groupName).m_color;

            const data::Landmarks::ColorType newGroupColor =
            {{groupColor[0], groupColor[1], groupColor[2], realOpacity}};

            liLock.landmarks->setGroupColor(groupName, newGroupColor);
        }
        else if(liLock.imageSeries != nullptr)
        {
            sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::FiducialsSeries::FiducialSet,
                                    std::size_t> > fs =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fs.has_value())
            {
                SIGHT_WARN("Couldn't modify the color of group '" << groupName << "', it doesn't exist");
                return;
            }

            std::optional<data::Landmarks::ColorType> color = liLock.imageSeries->getFiducials()->getColor(fs->second);
            SIGHT_ASSERT("The group must have a color", color.has_value());
            (*color)[3] = realOpacity;
            liLock.imageSeries->getFiducials()->setColor(fs->second, *color);
        }

        QTreeWidgetItem* const item = getGroupItem(groupName);
        auto* const colorButton     = dynamic_cast<QPushButton*>(m_treeWidget->itemWidget(item, 1));

        auto currentColor = colorButton->property("color").value<QColor>();
        currentColor.setAlphaF(realOpacity);
        colorButton->setProperty("color", currentColor);

        setColorButtonIcon(colorButton, currentColor);

        core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onVisibilityChanged(int _visibility)
{
    std::string groupName;
    if(currentSelection(groupName))
    {
        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        if(liLock.landmarks != nullptr)
        {
            liLock.landmarks->setGroupVisibility(groupName, static_cast<bool>(_visibility));

            const auto sig = liLock.landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
                data::Landmarks::GROUP_MODIFIED_SIG
            );

            const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

            sig->async_emit(groupName);
        }
        else if(liLock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::FiducialsSeries::FiducialSet, std::size_t> > fiducialSet =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fiducialSet.has_value())
            {
                SIGHT_WARN("Couldn't change visibility of group '" << groupName << "', it doesn't exist");
                return;
            }

            liLock.imageSeries->getFiducials()->setVisibility(fiducialSet->second, static_cast<bool>(_visibility));

            const auto sig = liLock.imageSeries->signal<data::ImageSeries::signals::GroupModified>(
                data::ImageSeries::signals::GROUP_MODIFIED
            );
            const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));
            sig->async_emit(groupName);
        }
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

        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::Landmarks::GroupModifiedSignalType::sptr sig;

        if(liLock.landmarks != nullptr)
        {
            sig = liLock.landmarks->signal<data::Landmarks::GroupModifiedSignalType>(
                data::Landmarks::GROUP_MODIFIED_SIG
            );
            liLock.landmarks->setGroupShape(groupName, s);
        }
        else if(liLock.imageSeries != nullptr)
        {
            sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::FiducialsSeries::FiducialSet,
                                    std::size_t> > fiducialSet =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fiducialSet.has_value())
            {
                SIGHT_WARN("Couldn't change shape of group '" << groupName << "', it doesn't exist");
                return;
            }

            liLock.imageSeries->getFiducials()->setShape(
                fiducialSet->second,
                s == data::Landmarks::Shape::CUBE ? data::FiducialsSeries::PrivateShape::CUBE
                                                  : data::FiducialsSeries::PrivateShape::SPHERE
            );
        }

        const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(groupName);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::onAddNewGroup()
{
    const std::string groupName = this->generateNewGroupName();

    LandmarksOrImageSeriesLock liLock = lock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        liLock.landmarks != nullptr || liLock.imageSeries != nullptr
    );
    if(liLock.landmarks != nullptr)
    {
        liLock.landmarks->addGroup(groupName, this->generateNewColor(), m_defaultLandmarkSize);
        liLock.landmarks->signal<data::Landmarks::GroupAddedSignalType>(data::Landmarks::GROUP_ADDED_SIG)->async_emit(
            groupName
        );
    }
    else if(liLock.imageSeries != nullptr)
    {
        liLock.imageSeries->getFiducials()->addGroup(groupName, this->generateNewColor(), m_defaultLandmarkSize);
        liLock.imageSeries->signal<data::has_fiducials::signals::GroupAdded>(
            data::has_fiducials::signals::GROUP_ADDED
        )->async_emit(groupName);
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

        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        if(m_advancedMode && topLevelIndex == -1) // Delete point
        {
            QTreeWidgetItem* const itemParent = item->parent();

            const auto index             = static_cast<std::size_t>(itemParent->indexOfChild(item));
            const std::string& groupName = itemParent->text(0).toStdString();

            data::Landmarks::PointRemovedSignalType::sptr sig;
            static_assert(
                std::is_same_v<data::Landmarks::PointRemovedSignalType,
                               data::has_fiducials::signals::PointRemoved>,
                "'pointRemoved' signal from data::Landmarks and data::has_fiducials must have the same signature"
            );

            if(liLock.landmarks != nullptr)
            {
                sig = liLock.landmarks->signal<data::Landmarks::PointRemovedSignalType>(
                    data::Landmarks::POINT_REMOVED_SIG
                );
                liLock.landmarks->removePoint(groupName, index);
            }
            else if(liLock.imageSeries != nullptr)
            {
                sig = liLock.imageSeries->signal<data::has_fiducials::signals::PointRemoved>(
                    data::has_fiducials::signals::POINT_REMOVED
                );
                liLock.imageSeries->getFiducials()->removePoint(groupName, index);
            }

            itemParent->removeChild(item);

            {
                const core::com::connection::blocker block(sig->get_connection(this->slot(REMOVE_POINT_SLOT)));
                sig->async_emit(groupName, index);
            }
        }
        else
        {
            const std::string& groupName = item->text(0).toStdString();

            data::Landmarks::GroupRemovedSignalType::sptr sig;
            static_assert(
                std::is_same_v<data::Landmarks::GroupRemovedSignalType,
                               data::has_fiducials::signals::GroupRemoved>,
                "'groupRemoved' signal from data::Landmarks and data::has_fiducials must have the same signature"
            );

            if(liLock.landmarks != nullptr)
            {
                sig = liLock.landmarks->signal<data::Landmarks::GroupRemovedSignalType>(
                    data::Landmarks::GROUP_REMOVED_SIG
                );
                liLock.landmarks->removeGroup(groupName);
            }
            else if(liLock.imageSeries != nullptr)
            {
                sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupRemoved>(
                    data::has_fiducials::signals::GROUP_REMOVED
                );
                liLock.imageSeries->getFiducials()->removeGroup(groupName);
            }

            delete m_treeWidget->takeTopLevelItem(topLevelIndex);

            {
                const core::com::connection::blocker block(sig->get_connection(this->slot(REMOVE_GROUP_SLOT)));
                sig->async_emit(groupName);
            }
        }

        m_treeWidget->setCurrentItem(nullptr);
        m_groupEditorWidget->setDisabled(true);
    }

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string _groupName) const
{
    if(m_advancedMode)
    {
        m_treeWidget->blockSignals(true);

        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        QTreeWidgetItem* const item = getGroupItem(_groupName);

        const auto nbChilds                      = static_cast<std::size_t>(item->childCount());
        std::optional<std::size_t> maybeNbPoints = numPoints(liLock, _groupName);
        if(!maybeNbPoints.has_value())
        {
            return;
        }

        std::size_t nbPoints = *maybeNbPoints;
        for(std::size_t idx = nbChilds ; idx < nbPoints ; ++idx)
        {
            std::optional<data::Landmarks::PointType> maybeNewPoint = getPoint(liLock, _groupName, idx);
            if(!maybeNewPoint.has_value())
            {
                continue;
            }

            data::Landmarks::PointType newPoint = *maybeNewPoint;

            auto* const pt = new QTreeWidgetItem();
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

void SLandmarks::addGroup(std::string _name) const
{
    QColor color;
    {
        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        std::optional<data::Landmarks::LandmarksGroup> group = getGroup(liLock, _name);
        if(!group.has_value())
        {
            return;
        }

        color = convertToQColor(group->m_color);
    }

    auto* const item = new QTreeWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setData(0, s_GROUP_NAME_ROLE, QString::fromStdString(_name));

    m_treeWidget->addTopLevelItem(item);

    item->setText(0, QString::fromStdString(_name));
    auto* const button = new QPushButton();

    setColorButtonIcon(button, color);
    button->setProperty("color", color);
    button->setProperty(s_GROUP_PROPERTY_NAME, QString::fromStdString(_name));

    QObject::connect(button, &QPushButton::clicked, this, &SLandmarks::onColorButton);

    m_treeWidget->setItemWidget(item, 1, button);
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string _name) const
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

void SLandmarks::removePoint(std::string _groupName, std::size_t _index) const
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* const item = getGroupItem(_groupName);

    SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

    QTreeWidgetItem* const pointItem = item->child(static_cast<int>(_index));
    item->removeChild(pointItem);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::renameGroup(std::string _oldName, std::string _newName) const
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

void SLandmarks::modifyGroup(std::string _name) const
{
    QTreeWidgetItem* const item = getGroupItem(_name);

    item->setText(0, _name.c_str());

    LandmarksOrImageSeriesConstLock liLock = constLock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        liLock.landmarks != nullptr || liLock.imageSeries != nullptr
    );

    std::optional<data::Landmarks::LandmarksGroup> maybeGroup = getGroup(liLock, _name);
    if(!maybeGroup.has_value())
    {
        return;
    }

    data::Landmarks::LandmarksGroup group = *maybeGroup;

    auto* const colorButton = dynamic_cast<QPushButton*>(m_treeWidget->itemWidget(item, 1));

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
        m_opacitySlider->setValue(static_cast<int>(opacity * float(m_opacitySlider->maximum())));
    }
}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string _groupName, std::size_t _index) const
{
    if(m_advancedMode)
    {
        auto const itemList = m_treeWidget->findItems(QString::fromStdString(_groupName), Qt::MatchExactly);

        SIGHT_ASSERT("Only a single item can be named '" + _groupName + "'", itemList.size() == 1);

        QTreeWidgetItem* const item = itemList.at(0);

        SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

        QTreeWidgetItem* const pointItem = item->child(static_cast<int>(_index));

        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        std::optional<data::Landmarks::PointType> maybePoint = getPoint(liLock, _groupName, _index);
        if(!maybePoint.has_value())
        {
            return;
        }

        data::Landmarks::PointType point = *maybePoint;

        m_treeWidget->blockSignals(true);
        for(int i = 0 ; i < 3 ; ++i)
        {
            pointItem->setText(i, QString("%1").arg(point[static_cast<std::size_t>(i)]));
        }

        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string _groupName, std::size_t _index) const
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

    int size     = 0;
    bool visible = false;
    QString shapeText;
    float opacity = NAN;
    {
        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        std::optional<data::Landmarks::LandmarksGroup> maybeGroup = getGroup(liLock, _groupName);
        if(!maybeGroup.has_value())
        {
            return;
        }

        data::Landmarks::LandmarksGroup group = *maybeGroup;

        size      = static_cast<int>(group.m_size);
        visible   = group.m_visibility;
        shapeText = group.m_shape == data::Landmarks::Shape::CUBE ? "Cube" : "Sphere";
        opacity   = group.m_color[3];
    }

    // Set widget values
    m_sizeSlider->setValue(size);
    m_visibilityCheckbox->setChecked(visible);
    m_shapeSelector->setCurrentText(shapeText);
    m_opacitySlider->setValue(static_cast<int>(opacity * float(m_opacitySlider->maximum())));

    m_groupEditorWidget->setEnabled(true);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string /*unused*/, std::size_t /*unused*/) const
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

    // const auto landmarks = m_landmarks.lock();
    LandmarksOrImageSeriesConstLock liLock = constLock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        liLock.landmarks != nullptr || liLock.imageSeries != nullptr
    );

    const data::Landmarks::GroupNameContainer groupNames = getGroupNames(liLock);

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
    return {
        static_cast<int>(_color[0] * 255),
        static_cast<int>(_color[1] * 255),
        static_cast<int>(_color[2] * 255),
        static_cast<int>(_color[3] * 255)
    };
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

//------------------------------------------------------------------------------

LandmarksOrImageSeriesLock SLandmarks::lock()
{
    return {.landmarks = m_landmarks.lock(), .imageSeries = m_imageSeries.lock()};
}

//------------------------------------------------------------------------------

LandmarksOrImageSeriesConstLock SLandmarks::constLock() const
{
    return {.landmarks = m_landmarks.const_lock(), .imageSeries = m_imageSeries.const_lock()};
}

} // namespace sight::module::ui::qt::metrics
