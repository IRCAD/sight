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

#include "data/landmarks.hpp"

#include "modules/ui/qt/metrics/landmarks.hpp"

#include "data/image_series.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/fiducials_series.hpp>

#include <geometry/data/matrix4.hpp>

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
    std::is_same_v<data::landmarks::GroupModifiedSignalType, data::has_fiducials::signals::GroupModified>,
    "'groupModified' signal from data::landmarks and data::has_fiducials must have the same signature"
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

[[nodiscard]] std::optional<data::landmarks::LandmarksGroup> getGroup(
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

[[nodiscard]] std::optional<data::landmarks::PointType> getPoint(
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

const core::com::signals::key_t landmarks::SEND_WORLD_COORD = "sendWorldCoord";
const core::com::signals::key_t landmarks::GROUP_SELECTED   = "groupSelected";

//------------------------------------------------------------------------------

landmarks::landmarks() noexcept
{
    new_slot(ADD_POINT_SLOT, &landmarks::addPoint, this);
    new_slot(MODIFY_POINT_SLOT, &landmarks::modifyPoint, this);
    new_slot(SELECT_POINT_SLOT, &landmarks::selectPoint, this);
    new_slot(DESELECT_POINT_SLOT, &landmarks::deselectPoint, this);
    new_slot(ADD_GROUP_SLOT, &landmarks::addGroup, this);
    new_slot(REMOVE_POINT_SLOT, &landmarks::removePoint, this);
    new_slot(REMOVE_GROUP_SLOT, &landmarks::removeGroup, this);
    new_slot(MODIFY_GROUP_SLOT, &landmarks::modifyGroup, this);
    new_slot(RENAME_GROUP_SLOT, &landmarks::renameGroup, this);

    new_signal<world_coordinates_signal_t>(SEND_WORLD_COORD);
    new_signal<GroupSelectedSignal>(GROUP_SELECTED);
}

//------------------------------------------------------------------------------

landmarks::~landmarks() noexcept =
    default;

//------------------------------------------------------------------------------

void landmarks::configuring()
{
    this->sight::ui::service::initialize();

    const service::config_t config = this->get_config();

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

void landmarks::starting()
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

service::connections_t landmarks::auto_connections() const
{
    connections_t connections;

    connections.push(s_LANDMARKS_INOUT, data::landmarks::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_ADDED_SIG, ADD_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_MODIFIED_SIG, MODIFY_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_SELECTED_SIG, SELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_DESELECTED_SIG, DESELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_ADDED_SIG, ADD_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_REMOVED_SIG, REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_REMOVED_SIG, REMOVE_POINT_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_MODIFIED_SIG, MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_RENAMED_SIG, RENAME_GROUP_SLOT);

    connections.push(s_IMAGE_SERIES_INOUT, data::image_series::MODIFIED_SIG, service::slots::UPDATE);
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

void landmarks::updating()
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

    QObject::connect(m_treeWidget.data(), &QTreeWidget::itemChanged, this, &landmarks::onGroupNameEdited);
    QObject::connect(m_treeWidget.data(), &QTreeWidget::currentItemChanged, this, &landmarks::onSelectionChanged);
    QObject::connect(m_treeWidget.data(), &QTreeWidget::itemDoubleClicked, this, &landmarks::onLandmarkDoubleClicked);
    QObject::connect(m_sizeSlider.data(), &QSlider::valueChanged, this, &landmarks::onSizeChanged);
    QObject::connect(m_opacitySlider.data(), &QSlider::valueChanged, this, &landmarks::onOpacityChanged);
    QObject::connect(m_visibilityCheckbox.data(), &QCheckBox::stateChanged, this, &landmarks::onVisibilityChanged);
    QObject::connect(m_shapeSelector.data(), &QComboBox::currentTextChanged, this, &landmarks::onShapeChanged);
    QObject::connect(m_removeButton.data(), &QPushButton::clicked, this, &landmarks::onRemoveSelection);
    QObject::connect(m_newGroupButton.data(), &QPushButton::clicked, this, &landmarks::onAddNewGroup);

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void landmarks::onColorButton()
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

        data::landmarks::ColorType color = {{
            float(colorQt.red()) / 255.F, float(colorQt.green()) / 255.F,
            float(colorQt.blue()) / 255.F, float(colorQt.alpha()) / 255.F
        }
        };

        data::landmarks::GroupModifiedSignalType::sptr sig;
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

                sig = liLock.landmarks->signal<data::landmarks::GroupModifiedSignalType>(
                    data::landmarks::GROUP_MODIFIED_SIG
                );
            }
            else if(liLock.imageSeries != nullptr)
            {
                std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducialSet =
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

void landmarks::onGroupNameEdited(QTreeWidgetItem* _item, int _column)
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

                        const auto sig = liLock.landmarks->signal<data::landmarks::GroupRenamedSignalType>(
                            data::landmarks::GROUP_RENAMED_SIG
                        );

                        {
                            core::com::connection::blocker block(sig->get_connection(this->slot(RENAME_GROUP_SLOT)));
                            sig->async_emit(oldGroupName.toStdString(), newGroupName.toStdString());
                        }
                    }
                    else if(liLock.imageSeries != nullptr)
                    {
                        std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducialSet =
                            liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(oldGroupName.toStdString());
                        if(!fiducialSet.has_value())
                        {
                            throw data::exception("'" + oldGroupName.toStdString() + "' group doesn't exist");
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
            catch(data::exception& e)
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

void landmarks::onSelectionChanged(QTreeWidgetItem* _current, QTreeWidgetItem* _previous)
{
    if(_previous != nullptr)
    {
        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::landmarks::PointDeselectedSignalType::sptr deselectSig;
        static_assert(
            std::is_same_v<data::landmarks::PointDeselectedSignalType,
                           data::has_fiducials::signals::PointDeselected>,
            "'pointDeselected' signal from data::landmarks and data::has_fiducials must have the same signature"
        );
        if(liLock.landmarks != nullptr)
        {
            deselectSig = liLock.landmarks->signal<data::landmarks::PointDeselectedSignalType>(
                data::landmarks::POINT_DESELECTED_SIG
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

            data::landmarks::PointSelectedSignalType::sptr selectSig;
            static_assert(
                std::is_same_v<data::landmarks::PointSelectedSignalType,
                               data::has_fiducials::signals::PointSelected>,
                "'pointSelected' signal from data::landmarks and data::has_fiducials must have the same signature"
            );
            if(liLock.landmarks != nullptr)
            {
                selectSig = liLock.landmarks->signal<data::landmarks::PointSelectedSignalType>(
                    data::landmarks::POINT_SELECTED_SIG
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

            std::optional<data::landmarks::LandmarksGroup> group = getGroup(liLock, groupName);
            if(!group.has_value())
            {
                return;
            }

            size      = static_cast<int>(group->m_size);
            visible   = group->m_visibility;
            shapeText = group->m_shape == data::landmarks::Shape::CUBE ? "Cube" : "Sphere";
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

void landmarks::onLandmarkDoubleClicked(QTreeWidgetItem* _item, int /*unused*/) const
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
        data::landmarks::PointType world_coord {
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

void landmarks::updateCurrentLandmark(data::landmarks::PointType& world_coord) const
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
            currentLandmark->signal<sight::data::object::ModifiedSignalType>(
                sight::data::object::MODIFIED_SIG
            );
        currentLandmarkModifiedSig->async_emit();
    }
}

//------------------------------------------------------------------------------

void landmarks::onSizeChanged(int _newSize)
{
    const auto realSize = static_cast<data::landmarks::SizeType>(_newSize);

    std::string groupName;
    if(currentSelection(groupName))
    {
        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::landmarks::GroupModifiedSignalType::sptr sig;
        if(liLock.landmarks != nullptr)
        {
            sig = liLock.landmarks->signal<data::landmarks::GroupModifiedSignalType>(
                data::landmarks::GROUP_MODIFIED_SIG
            );
            liLock.landmarks->setGroupSize(groupName, realSize);
        }
        else if(liLock.imageSeries != nullptr)
        {
            sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::fiducials_series::FiducialSet,
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

void landmarks::onOpacityChanged(int _newOpacity)
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

        data::landmarks::GroupModifiedSignalType::sptr sig;

        if(liLock.landmarks != nullptr)
        {
            sig = liLock.landmarks->signal<data::landmarks::GroupModifiedSignalType>(
                data::landmarks::GROUP_MODIFIED_SIG
            );

            const auto groupColor = liLock.landmarks->getGroup(groupName).m_color;

            const data::landmarks::ColorType newGroupColor =
            {{groupColor[0], groupColor[1], groupColor[2], realOpacity}};

            liLock.landmarks->setGroupColor(groupName, newGroupColor);
        }
        else if(liLock.imageSeries != nullptr)
        {
            sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::fiducials_series::FiducialSet,
                                    std::size_t> > fs =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fs.has_value())
            {
                SIGHT_WARN("Couldn't modify the color of group '" << groupName << "', it doesn't exist");
                return;
            }

            std::optional<data::landmarks::ColorType> color = liLock.imageSeries->getFiducials()->getColor(fs->second);
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

void landmarks::onVisibilityChanged(int _visibility)
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

            const auto sig = liLock.landmarks->signal<data::landmarks::GroupModifiedSignalType>(
                data::landmarks::GROUP_MODIFIED_SIG
            );

            const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

            sig->async_emit(groupName);
        }
        else if(liLock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducialSet =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fiducialSet.has_value())
            {
                SIGHT_WARN("Couldn't change visibility of group '" << groupName << "', it doesn't exist");
                return;
            }

            liLock.imageSeries->getFiducials()->setVisibility(fiducialSet->second, static_cast<bool>(_visibility));

            const auto sig = liLock.imageSeries->signal<data::image_series::signals::GroupModified>(
                data::image_series::signals::GROUP_MODIFIED
            );
            const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));
            sig->async_emit(groupName);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::onShapeChanged(const QString& _shape)
{
    std::string groupName;
    if(currentSelection(groupName))
    {
        SIGHT_ASSERT("Shape must be 'Cube' or 'Sphere'.", _shape == "Cube" || _shape == "Sphere");
        const data::landmarks::Shape s =
            (_shape == "Cube") ? data::landmarks::Shape::CUBE : data::landmarks::Shape::SPHERE;

        LandmarksOrImageSeriesLock liLock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        data::landmarks::GroupModifiedSignalType::sptr sig;

        if(liLock.landmarks != nullptr)
        {
            sig = liLock.landmarks->signal<data::landmarks::GroupModifiedSignalType>(
                data::landmarks::GROUP_MODIFIED_SIG
            );
            liLock.landmarks->setGroupShape(groupName, s);
        }
        else if(liLock.imageSeries != nullptr)
        {
            sig = liLock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::fiducials_series::FiducialSet,
                                    std::size_t> > fiducialSet =
                liLock.imageSeries->getFiducials()->getFiducialSetAndIndex(groupName);
            if(!fiducialSet.has_value())
            {
                SIGHT_WARN("Couldn't change shape of group '" << groupName << "', it doesn't exist");
                return;
            }

            liLock.imageSeries->getFiducials()->setShape(
                fiducialSet->second,
                s == data::landmarks::Shape::CUBE ? data::fiducials_series::PrivateShape::CUBE
                                                  : data::fiducials_series::PrivateShape::SPHERE
            );
        }

        const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(groupName);
    }
}

//------------------------------------------------------------------------------

void landmarks::onAddNewGroup()
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
        liLock.landmarks->signal<data::landmarks::GroupAddedSignalType>(data::landmarks::GROUP_ADDED_SIG)->async_emit(
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

void landmarks::onRemoveSelection()
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

            data::landmarks::PointRemovedSignalType::sptr sig;
            static_assert(
                std::is_same_v<data::landmarks::PointRemovedSignalType,
                               data::has_fiducials::signals::PointRemoved>,
                "'pointRemoved' signal from data::landmarks and data::has_fiducials must have the same signature"
            );

            if(liLock.landmarks != nullptr)
            {
                sig = liLock.landmarks->signal<data::landmarks::PointRemovedSignalType>(
                    data::landmarks::POINT_REMOVED_SIG
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

            data::landmarks::GroupRemovedSignalType::sptr sig;
            static_assert(
                std::is_same_v<data::landmarks::GroupRemovedSignalType,
                               data::has_fiducials::signals::GroupRemoved>,
                "'groupRemoved' signal from data::landmarks and data::has_fiducials must have the same signature"
            );

            if(liLock.landmarks != nullptr)
            {
                sig = liLock.landmarks->signal<data::landmarks::GroupRemovedSignalType>(
                    data::landmarks::GROUP_REMOVED_SIG
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

void landmarks::addPoint(std::string _groupName) const
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
            std::optional<data::landmarks::PointType> maybeNewPoint = getPoint(liLock, _groupName, idx);
            if(!maybeNewPoint.has_value())
            {
                continue;
            }

            data::landmarks::PointType newPoint = *maybeNewPoint;

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

void landmarks::addGroup(std::string _name) const
{
    QColor color;
    {
        LandmarksOrImageSeriesConstLock liLock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            liLock.landmarks != nullptr || liLock.imageSeries != nullptr
        );

        std::optional<data::landmarks::LandmarksGroup> group = getGroup(liLock, _name);
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

    QObject::connect(button, &QPushButton::clicked, this, &landmarks::onColorButton);

    m_treeWidget->setItemWidget(item, 1, button);
}

//------------------------------------------------------------------------------

void landmarks::removeGroup(std::string _name) const
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

void landmarks::removePoint(std::string _groupName, std::size_t _index) const
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* const item = getGroupItem(_groupName);

    SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

    QTreeWidgetItem* const pointItem = item->child(static_cast<int>(_index));
    item->removeChild(pointItem);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::renameGroup(std::string _oldName, std::string _newName) const
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

void landmarks::modifyGroup(std::string _name) const
{
    QTreeWidgetItem* const item = getGroupItem(_name);

    item->setText(0, _name.c_str());

    LandmarksOrImageSeriesConstLock liLock = constLock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        liLock.landmarks != nullptr || liLock.imageSeries != nullptr
    );

    std::optional<data::landmarks::LandmarksGroup> maybeGroup = getGroup(liLock, _name);
    if(!maybeGroup.has_value())
    {
        return;
    }

    data::landmarks::LandmarksGroup group = *maybeGroup;

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

        const QString shapeText = group.m_shape == data::landmarks::Shape::CUBE ? "Cube" : "Sphere";
        m_shapeSelector->setCurrentText(shapeText);

        const float opacity = group.m_color[3];
        m_opacitySlider->setValue(static_cast<int>(opacity * float(m_opacitySlider->maximum())));
    }
}

//------------------------------------------------------------------------------

void landmarks::modifyPoint(std::string _groupName, std::size_t _index) const
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

        std::optional<data::landmarks::PointType> maybePoint = getPoint(liLock, _groupName, _index);
        if(!maybePoint.has_value())
        {
            return;
        }

        data::landmarks::PointType point = *maybePoint;

        m_treeWidget->blockSignals(true);
        for(int i = 0 ; i < 3 ; ++i)
        {
            pointItem->setText(i, QString("%1").arg(point[static_cast<std::size_t>(i)]));
        }

        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void landmarks::selectPoint(std::string _groupName, std::size_t _index) const
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

        std::optional<data::landmarks::LandmarksGroup> maybeGroup = getGroup(liLock, _groupName);
        if(!maybeGroup.has_value())
        {
            return;
        }

        data::landmarks::LandmarksGroup group = *maybeGroup;

        size      = static_cast<int>(group.m_size);
        visible   = group.m_visibility;
        shapeText = group.m_shape == data::landmarks::Shape::CUBE ? "Cube" : "Sphere";
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

void landmarks::deselectPoint(std::string /*unused*/, std::size_t /*unused*/) const
{
    m_treeWidget->blockSignals(true);
    m_treeWidget->setCurrentItem(nullptr);
    m_groupEditorWidget->setDisabled(true);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

std::string landmarks::generateNewGroupName() const
{
    static std::size_t groupCount = 0;

    // const auto landmarks = m_landmarks.lock();
    LandmarksOrImageSeriesConstLock liLock = constLock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        liLock.landmarks != nullptr || liLock.imageSeries != nullptr
    );

    const data::landmarks::GroupNameContainer groupNames = getGroupNames(liLock);

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

std::array<float, 4> landmarks::generateNewColor()
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

bool landmarks::currentSelection(std::string& _selection) const
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

QColor landmarks::convertToQColor(const data::landmarks::ColorType& _color)
{
    return {
        static_cast<int>(_color[0] * 255),
        static_cast<int>(_color[1] * 255),
        static_cast<int>(_color[2] * 255),
        static_cast<int>(_color[3] * 255)
    };
}

//------------------------------------------------------------------------------

void landmarks::setColorButtonIcon(QPushButton* _button, const QColor& _color)
{
    const int iconSize = _button->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);
    pix.fill(_color);

    _button->setIcon(QIcon(pix));
}

//------------------------------------------------------------------------------

QTreeWidgetItem* landmarks::getGroupItem(const std::string& _groupName) const
{
    const auto itemList = m_treeWidget->findItems(QString::fromStdString(_groupName), Qt::MatchExactly);

    SIGHT_ASSERT("Only a single item can be named '" + _groupName + "'", itemList.size() == 1);

    return itemList.at(0);
}

//------------------------------------------------------------------------------

LandmarksOrImageSeriesLock landmarks::lock()
{
    return {.landmarks = m_landmarks.lock(), .imageSeries = m_imageSeries.lock()};
}

//------------------------------------------------------------------------------

LandmarksOrImageSeriesConstLock landmarks::constLock() const
{
    return {.landmarks = m_landmarks.const_lock(), .imageSeries = m_imageSeries.const_lock()};
}

} // namespace sight::module::ui::qt::metrics
