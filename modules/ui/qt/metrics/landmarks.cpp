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
    std::is_same_v<data::landmarks::group_modified_signal_t, data::has_fiducials::signals::GroupModified>,
    "'groupModified' signal from data::landmarks and data::has_fiducials must have the same signature"
);

namespace
{

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<std::string> get_group_names(LandmarksOrImageSeriesConstPtr _li)
{
    if(_li.landmarks != nullptr)
    {
        return _li.landmarks->getGroupNames();
    }

    if(_li.imageSeries != nullptr)
    {
        return _li.imageSeries->getFiducials()->getPointFiducialsGroupNames();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::size_t> num_points(LandmarksOrImageSeriesConstPtr _li, const std::string& _group_name)
{
    if(_li.landmarks != nullptr)
    {
        if(!_li.landmarks->hasGroup(_group_name))
        {
            return std::nullopt;
        }

        return _li.landmarks->numPoints(_group_name);
    }

    if(_li.imageSeries != nullptr)
    {
        return _li.imageSeries->getFiducials()->getNumberOfPointsInGroup(_group_name);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::landmarks::LandmarksGroup> get_group(
    LandmarksOrImageSeriesConstPtr _li,
    const std::string& _group_name
)
{
    if(_li.landmarks != nullptr)
    {
        if(!_li.landmarks->hasGroup(_group_name))
        {
            return std::nullopt;
        }

        return _li.landmarks->getGroup(_group_name);
    }

    if(_li.imageSeries != nullptr)
    {
        return _li.imageSeries->getFiducials()->getGroup(_group_name);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::landmarks::point_t> get_point(
    LandmarksOrImageSeriesConstPtr _li,
    const std::string& _group_name,
    std::size_t _index
)
{
    if(_li.landmarks != nullptr)
    {
        if(!_li.landmarks->hasGroup(_group_name))
        {
            return std::nullopt;
        }

        return _li.landmarks->getPoint(_group_name, _index);
    }

    if(_li.imageSeries != nullptr)
    {
        return _li.imageSeries->getFiducials()->getPoint(_group_name, _index);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

} // namespace

//------------------------------------------------------------------------------

static const char* s_group_property_name = "group";
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

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qt_container->getQtContainer()->setObjectName(service_id);

    auto* const layout      = new QVBoxLayout();
    auto* const grid_layout = new QGridLayout();

    m_visibilityCheckbox = new QCheckBox();
    auto* const visibility_label = new QLabel(QString("Visibility"));
    m_visibilityCheckbox->setObjectName(service_id + "/" + visibility_label->text());
    m_sizeSlider = new QSlider(Qt::Horizontal);
    m_sizeSlider->setMinimum(1);
    m_sizeSlider->setMaximum(100);
    auto* const size_label = new QLabel(QString("Size"));
    m_sizeSlider->setObjectName(service_id + "/" + size_label->text());
    m_opacitySlider = new QSlider(Qt::Horizontal);
    auto* const opacity_label = new QLabel("Opacity");
    m_opacitySlider->setObjectName(service_id + "/" + opacity_label->text());
    m_shapeSelector = new QComboBox();
    m_shapeSelector->addItem(QString("Cube"));
    m_shapeSelector->addItem(QString("Sphere"));
    auto* const shape_label = new QLabel("Shape");
    m_shapeSelector->setObjectName(service_id + "/" + shape_label->text());

    if(m_advancedMode)
    {
        m_newGroupButton = new QPushButton("New Group");
        m_newGroupButton->setObjectName(service_id + "/" + m_newGroupButton->text());
    }

    m_removeButton = new QPushButton("Delete");
    m_removeButton->setObjectName(service_id + "/" + m_removeButton->text());
    m_removeButton->setShortcut(QKeySequence::Delete);

    grid_layout->addWidget(visibility_label, 0, 0);
    grid_layout->addWidget(m_visibilityCheckbox, 0, 1);
    grid_layout->addWidget(size_label, 1, 0);
    grid_layout->addWidget(m_sizeSlider, 1, 1);
    grid_layout->addWidget(opacity_label, 2, 0);
    grid_layout->addWidget(m_opacitySlider, 2, 1);
    grid_layout->addWidget(shape_label, 3, 0);
    grid_layout->addWidget(m_shapeSelector, 3, 1);
    grid_layout->addWidget(m_removeButton, 4, 1);

    m_groupEditorWidget = new QWidget();
    m_groupEditorWidget->setLayout(grid_layout);

    m_treeWidget = new QTreeWidget();
    m_treeWidget->setObjectName(service_id + "/treeWidget");
    if(!m_text.empty())
    {
        auto* helper_text_label = new QLabel(QString::fromStdString(m_text));
        layout->addWidget(helper_text_label);
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

    qt_container->setLayout(layout);

    this->updating();

    SIGHT_ASSERT(
        "Either 'landmarks' or 'imageSeries' parameter must be set.",
        (m_landmarks.const_lock() != nullptr) + (m_imageSeries.const_lock() != nullptr) == 1
    );
    if(auto image_series = m_imageSeries.lock())
    {
        image_series->getFiducials()->setGroupNamesForPointFiducials();
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
        LandmarksOrImageSeriesConstLock li_lock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        m_treeWidget->clear();

        for(const auto& name : get_group_names(li_lock))
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
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QWidget* const container = qt_container->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    const auto old_color  = sender->property("color").value<QColor>();
    const QColor color_qt =
        QColorDialog::getColor(old_color, container, "Select Color", QColorDialog::ShowAlphaChannel);
    if(color_qt.isValid())
    {
        auto* const color_button = dynamic_cast<QPushButton*>(sender);
        color_button->setProperty("color", color_qt);

        setColorButtonIcon(color_button, color_qt);

        const std::string group_name = color_button->property(s_group_property_name).value<QString>().toStdString();

        data::landmarks::color_t color = {{
            float(color_qt.red()) / 255.F, float(color_qt.green()) / 255.F,
            float(color_qt.blue()) / 255.F, float(color_qt.alpha()) / 255.F
        }
        };

        data::landmarks::group_modified_signal_t::sptr sig;
        {
            LandmarksOrImageSeriesLock li_lock = lock();
            SIGHT_ASSERT(
                "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
                li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
            );
            if(li_lock.landmarks != nullptr)
            {
                auto& group = li_lock.landmarks->getGroup(group_name);
                group.m_color = color;

                sig = li_lock.landmarks->signal<data::landmarks::group_modified_signal_t>(
                    data::landmarks::GROUP_MODIFIED_SIG
                );
            }
            else if(li_lock.imageSeries != nullptr)
            {
                std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducial_set =
                    li_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(group_name);
                if(!fiducial_set.has_value())
                {
                    return;
                }

                li_lock.imageSeries->getFiducials()->setColor(fiducial_set->second, color);
                sig = li_lock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                    data::has_fiducials::signals::GROUP_MODIFIED
                );
            }
        }

        m_opacitySlider->setValue(static_cast<int>(color[3] * float(m_opacitySlider->maximum())));

        {
            core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));
            sig->async_emit(group_name);
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
        const QString old_group_name = _item->data(0, s_GROUP_NAME_ROLE).toString();
        const QString new_group_name = _item->text(0);

        if(new_group_name.isEmpty())
        {
            const QString msg = "The new group name for '" + old_group_name
                                + "' is empty. Please enter a valid name and try again";
            QMessageBox msg_box(QMessageBox::Warning, "No group name", msg, QMessageBox::Ok);
            msg_box.exec();

            _item->setText(0, old_group_name);
        }
        else if(old_group_name != new_group_name)
        {
            try
            {
                {
                    LandmarksOrImageSeriesLock li_lock = lock();
                    SIGHT_ASSERT(
                        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
                        li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
                    );

                    if(li_lock.landmarks != nullptr)
                    {
                        li_lock.landmarks->renameGroup(old_group_name.toStdString(), new_group_name.toStdString());

                        const auto sig = li_lock.landmarks->signal<data::landmarks::group_renamed_signal_t>(
                            data::landmarks::GROUP_RENAMED_SIG
                        );

                        {
                            core::com::connection::blocker block(sig->get_connection(this->slot(RENAME_GROUP_SLOT)));
                            sig->async_emit(old_group_name.toStdString(), new_group_name.toStdString());
                        }
                    }
                    else if(li_lock.imageSeries != nullptr)
                    {
                        std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducial_set =
                            li_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(old_group_name.toStdString());
                        if(!fiducial_set.has_value())
                        {
                            throw data::exception("'" + old_group_name.toStdString() + "' group doesn't exist");
                        }

                        li_lock.imageSeries->getFiducials()->setGroupName(
                            fiducial_set->second,
                            new_group_name.toStdString()
                        );

                        const auto sig =
                            li_lock.imageSeries->signal<data::has_fiducials::signals::GroupRenamed>(
                                data::has_fiducials::signals::GROUP_RENAMED
                            );

                        core::com::connection::blocker block(sig->get_connection(this->slot(RENAME_GROUP_SLOT)));
                        sig->async_emit(old_group_name.toStdString(), new_group_name.toStdString());
                    }
                }

                _item->setData(0, s_GROUP_NAME_ROLE, new_group_name);
                QWidget* const widget = m_treeWidget->itemWidget(_item, 1);

                widget->setProperty(s_group_property_name, new_group_name);
            }
            catch(data::exception& e)
            {
                const QString msg = "Can't rename '" + old_group_name + "' as '" + new_group_name + ".\n"
                                    + QString(e.what());
                QMessageBox msg_box(QMessageBox::Warning, "Can't rename" + old_group_name, msg, QMessageBox::Ok);
                msg_box.exec();

                _item->setText(0, old_group_name);
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
        LandmarksOrImageSeriesConstLock li_lock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        data::landmarks::point_deselected_signal_t::sptr deselect_sig;
        static_assert(
            std::is_same_v<data::landmarks::point_deselected_signal_t,
                           data::has_fiducials::signals::PointDeselected>,
            "'pointDeselected' signal from data::landmarks and data::has_fiducials must have the same signature"
        );
        if(li_lock.landmarks != nullptr)
        {
            deselect_sig = li_lock.landmarks->signal<data::landmarks::point_deselected_signal_t>(
                data::landmarks::POINT_DESELECTED_SIG
            );
        }
        else if(li_lock.imageSeries != nullptr)
        {
            deselect_sig = li_lock.imageSeries->signal<data::has_fiducials::signals::PointDeselected>(
                data::has_fiducials::signals::POINT_DESELECTED
            );
        }

        const core::com::connection::blocker block(deselect_sig->get_connection(this->slot(DESELECT_POINT_SLOT)));

        if(m_advancedMode)
        {
            const QTreeWidgetItem* const previous_parent = _previous->parent();

            if(previous_parent != nullptr)
            {
                const std::string& group_name = previous_parent->text(0).toStdString();

                const auto index = static_cast<std::size_t>(previous_parent->indexOfChild(_previous));

                SIGHT_ASSERT(
                    "index must be inferior to the number of points in '" + group_name + "'.",
                    index < num_points(li_lock, group_name)
                );

                deselect_sig->async_emit(group_name, index);
            }
        }
        else
        {
            const std::string& group_name = _previous->text(0).toStdString();

            deselect_sig->async_emit(group_name, 0);
        }
    }

    if(_current != nullptr)
    {
        int size     = 0;
        bool visible = false;
        QString shape_text;
        float opacity = NAN;
        {
            LandmarksOrImageSeriesConstLock li_lock = constLock();
            SIGHT_ASSERT(
                "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
                li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
            );

            data::landmarks::point_selected_signal_t::sptr select_sig;
            static_assert(
                std::is_same_v<data::landmarks::point_selected_signal_t,
                               data::has_fiducials::signals::PointSelected>,
                "'pointSelected' signal from data::landmarks and data::has_fiducials must have the same signature"
            );
            if(li_lock.landmarks != nullptr)
            {
                select_sig = li_lock.landmarks->signal<data::landmarks::point_selected_signal_t>(
                    data::landmarks::POINT_SELECTED_SIG
                );
            }
            else if(li_lock.imageSeries != nullptr)
            {
                select_sig = li_lock.imageSeries->signal<data::has_fiducials::signals::PointSelected>(
                    data::has_fiducials::signals::POINT_SELECTED
                );
            }

            std::string group_name;

            if(m_advancedMode)
            {
                const QTreeWidgetItem* const current_parent = _current->parent();

                if(current_parent != nullptr)
                {
                    group_name = current_parent->text(0).toStdString();

                    const auto index = static_cast<std::size_t>(current_parent->indexOfChild(_current));

                    SIGHT_ASSERT(
                        "index must be inferior to the number of points in '" + group_name + "'.",
                        index < num_points(li_lock, group_name)
                    );

                    const core::com::connection::blocker block(select_sig->get_connection(this->slot(SELECT_POINT_SLOT)));
                    select_sig->async_emit(group_name, index);
                }
                else
                {
                    group_name = _current->text(0).toStdString();
                }
            }
            else
            {
                group_name = _current->text(0).toStdString();

                core::com::connection::blocker block(select_sig->get_connection(this->slot(SELECT_POINT_SLOT)));
                select_sig->async_emit(group_name, 0);
            }

            std::optional<data::landmarks::LandmarksGroup> group = get_group(li_lock, group_name);
            if(!group.has_value())
            {
                return;
            }

            size       = static_cast<int>(group->m_size);
            visible    = group->m_visibility;
            shape_text = group->m_shape == data::landmarks::Shape::CUBE ? "Cube" : "Sphere";
            opacity    = group->m_color[3];

            signal<GroupSelectedSignal>(GROUP_SELECTED)->async_emit(group_name);
        }

        // Set widget values
        m_sizeSlider->setValue(size);
        m_visibilityCheckbox->setChecked(visible);
        m_shapeSelector->setCurrentText(shape_text);
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
        data::landmarks::point_t world_coord {
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

void landmarks::updateCurrentLandmark(data::landmarks::point_t& _world_coord) const
{
    // Send signal with world coordinates of the landmarks

    SIGHT_DEBUG(
        " Send world coordinates [" << _world_coord[0] << ", " << _world_coord[1]
        << ", " << _world_coord[2] << " ]"
    );
    this->signal<world_coordinates_signal_t>(SEND_WORLD_COORD)->async_emit(
        _world_coord[0],
        _world_coord[1],
        _world_coord[2]
    );

    //update the data with the current point
    auto current_landmark = m_currentLandmark.lock();
    if(current_landmark)
    {
        current_landmark->setCoord(_world_coord);
        auto current_landmark_modified_sig =
            current_landmark->signal<sight::data::object::modified_signal_t>(
                sight::data::object::MODIFIED_SIG
            );
        current_landmark_modified_sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void landmarks::onSizeChanged(int _new_size)
{
    const auto real_size = static_cast<data::landmarks::size_t>(_new_size);

    std::string group_name;
    if(currentSelection(group_name))
    {
        LandmarksOrImageSeriesLock li_lock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        data::landmarks::group_modified_signal_t::sptr sig;
        if(li_lock.landmarks != nullptr)
        {
            sig = li_lock.landmarks->signal<data::landmarks::group_modified_signal_t>(
                data::landmarks::GROUP_MODIFIED_SIG
            );
            li_lock.landmarks->setGroupSize(group_name, real_size);
        }
        else if(li_lock.imageSeries != nullptr)
        {
            sig = li_lock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::fiducials_series::FiducialSet,
                                    std::size_t> > fiducial_set =
                li_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(group_name);
            if(!fiducial_set.has_value())
            {
                SIGHT_WARN("Couldn't change size of size of group '" << group_name << "', it doesn't exist");
                return;
            }

            li_lock.imageSeries->getFiducials()->setSize(fiducial_set->second, real_size);
        }

        const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::onOpacityChanged(int _new_opacity)
{
    const auto slider_size = static_cast<float>(m_opacitySlider->maximum() - m_opacitySlider->minimum());

    const float real_opacity = static_cast<float>(_new_opacity) / slider_size;

    std::string group_name;
    if(currentSelection(group_name))
    {
        LandmarksOrImageSeriesLock li_lock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        data::landmarks::group_modified_signal_t::sptr sig;

        if(li_lock.landmarks != nullptr)
        {
            sig = li_lock.landmarks->signal<data::landmarks::group_modified_signal_t>(
                data::landmarks::GROUP_MODIFIED_SIG
            );

            const auto group_color = li_lock.landmarks->getGroup(group_name).m_color;

            const data::landmarks::color_t new_group_color =
            {{group_color[0], group_color[1], group_color[2], real_opacity}};

            li_lock.landmarks->setGroupColor(group_name, new_group_color);
        }
        else if(li_lock.imageSeries != nullptr)
        {
            sig = li_lock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::fiducials_series::FiducialSet,
                                    std::size_t> > fs =
                li_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(group_name);
            if(!fs.has_value())
            {
                SIGHT_WARN("Couldn't modify the color of group '" << group_name << "', it doesn't exist");
                return;
            }

            std::optional<data::landmarks::color_t> color = li_lock.imageSeries->getFiducials()->getColor(fs->second);
            SIGHT_ASSERT("The group must have a color", color.has_value());
            (*color)[3] = real_opacity;
            li_lock.imageSeries->getFiducials()->setColor(fs->second, *color);
        }

        QTreeWidgetItem* const item = getGroupItem(group_name);
        auto* const color_button    = dynamic_cast<QPushButton*>(m_treeWidget->itemWidget(item, 1));

        auto current_color = color_button->property("color").value<QColor>();
        current_color.setAlphaF(real_opacity);
        color_button->setProperty("color", current_color);

        setColorButtonIcon(color_button, current_color);

        core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::onVisibilityChanged(int _visibility)
{
    std::string group_name;
    if(currentSelection(group_name))
    {
        LandmarksOrImageSeriesLock li_lock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        if(li_lock.landmarks != nullptr)
        {
            li_lock.landmarks->setGroupVisibility(group_name, static_cast<bool>(_visibility));

            const auto sig = li_lock.landmarks->signal<data::landmarks::group_modified_signal_t>(
                data::landmarks::GROUP_MODIFIED_SIG
            );

            const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

            sig->async_emit(group_name);
        }
        else if(li_lock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducial_set =
                li_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(group_name);
            if(!fiducial_set.has_value())
            {
                SIGHT_WARN("Couldn't change visibility of group '" << group_name << "', it doesn't exist");
                return;
            }

            li_lock.imageSeries->getFiducials()->setVisibility(fiducial_set->second, static_cast<bool>(_visibility));

            const auto sig = li_lock.imageSeries->signal<data::image_series::signals::GroupModified>(
                data::image_series::signals::GROUP_MODIFIED
            );
            const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));
            sig->async_emit(group_name);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::onShapeChanged(const QString& _shape)
{
    std::string group_name;
    if(currentSelection(group_name))
    {
        SIGHT_ASSERT("Shape must be 'Cube' or 'Sphere'.", _shape == "Cube" || _shape == "Sphere");
        const data::landmarks::Shape s =
            (_shape == "Cube") ? data::landmarks::Shape::CUBE : data::landmarks::Shape::SPHERE;

        LandmarksOrImageSeriesLock li_lock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        data::landmarks::group_modified_signal_t::sptr sig;

        if(li_lock.landmarks != nullptr)
        {
            sig = li_lock.landmarks->signal<data::landmarks::group_modified_signal_t>(
                data::landmarks::GROUP_MODIFIED_SIG
            );
            li_lock.landmarks->setGroupShape(group_name, s);
        }
        else if(li_lock.imageSeries != nullptr)
        {
            sig = li_lock.imageSeries->signal<data::has_fiducials::signals::GroupModified>(
                data::has_fiducials::signals::GROUP_MODIFIED
            );
            std::optional<std::pair<data::fiducials_series::FiducialSet,
                                    std::size_t> > fiducial_set =
                li_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(group_name);
            if(!fiducial_set.has_value())
            {
                SIGHT_WARN("Couldn't change shape of group '" << group_name << "', it doesn't exist");
                return;
            }

            li_lock.imageSeries->getFiducials()->setShape(
                fiducial_set->second,
                s == data::landmarks::Shape::CUBE ? data::fiducials_series::PrivateShape::CUBE
                                                  : data::fiducials_series::PrivateShape::SPHERE
            );
        }

        const core::com::connection::blocker block(sig->get_connection(this->slot(MODIFY_GROUP_SLOT)));

        sig->async_emit(group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::onAddNewGroup()
{
    const std::string group_name = this->generateNewGroupName();

    LandmarksOrImageSeriesLock li_lock = lock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
    );
    if(li_lock.landmarks != nullptr)
    {
        li_lock.landmarks->addGroup(group_name, this->generateNewColor(), m_defaultLandmarkSize);
        li_lock.landmarks->signal<data::landmarks::group_added_signal_t>(data::landmarks::GROUP_ADDED_SIG)->async_emit(
            group_name
        );
    }
    else if(li_lock.imageSeries != nullptr)
    {
        li_lock.imageSeries->getFiducials()->addGroup(group_name, this->generateNewColor(), m_defaultLandmarkSize);
        li_lock.imageSeries->signal<data::has_fiducials::signals::GroupAdded>(
            data::has_fiducials::signals::GROUP_ADDED
        )->async_emit(group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::onRemoveSelection()
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* const item = m_treeWidget->currentItem();

    if(item != nullptr)
    {
        const int top_level_index = m_treeWidget->indexOfTopLevelItem(item);

        LandmarksOrImageSeriesLock li_lock = lock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        if(m_advancedMode && top_level_index == -1) // Delete point
        {
            QTreeWidgetItem* const item_parent = item->parent();

            const auto index              = static_cast<std::size_t>(item_parent->indexOfChild(item));
            const std::string& group_name = item_parent->text(0).toStdString();

            data::landmarks::point_removed_signal_t::sptr sig;
            static_assert(
                std::is_same_v<data::landmarks::point_removed_signal_t,
                               data::has_fiducials::signals::PointRemoved>,
                "'pointRemoved' signal from data::landmarks and data::has_fiducials must have the same signature"
            );

            if(li_lock.landmarks != nullptr)
            {
                sig = li_lock.landmarks->signal<data::landmarks::point_removed_signal_t>(
                    data::landmarks::POINT_REMOVED_SIG
                );
                li_lock.landmarks->removePoint(group_name, index);
            }
            else if(li_lock.imageSeries != nullptr)
            {
                sig = li_lock.imageSeries->signal<data::has_fiducials::signals::PointRemoved>(
                    data::has_fiducials::signals::POINT_REMOVED
                );
                li_lock.imageSeries->getFiducials()->removePoint(group_name, index);
            }

            item_parent->removeChild(item);

            {
                const core::com::connection::blocker block(sig->get_connection(this->slot(REMOVE_POINT_SLOT)));
                sig->async_emit(group_name, index);
            }
        }
        else
        {
            const std::string& group_name = item->text(0).toStdString();

            data::landmarks::group_removed_signal_t::sptr sig;
            static_assert(
                std::is_same_v<data::landmarks::group_removed_signal_t,
                               data::has_fiducials::signals::GroupRemoved>,
                "'groupRemoved' signal from data::landmarks and data::has_fiducials must have the same signature"
            );

            if(li_lock.landmarks != nullptr)
            {
                sig = li_lock.landmarks->signal<data::landmarks::group_removed_signal_t>(
                    data::landmarks::GROUP_REMOVED_SIG
                );
                li_lock.landmarks->removeGroup(group_name);
            }
            else if(li_lock.imageSeries != nullptr)
            {
                sig = li_lock.imageSeries->signal<data::has_fiducials::signals::GroupRemoved>(
                    data::has_fiducials::signals::GROUP_REMOVED
                );
                li_lock.imageSeries->getFiducials()->removeGroup(group_name);
            }

            delete m_treeWidget->takeTopLevelItem(top_level_index);

            {
                const core::com::connection::blocker block(sig->get_connection(this->slot(REMOVE_GROUP_SLOT)));
                sig->async_emit(group_name);
            }
        }

        m_treeWidget->setCurrentItem(nullptr);
        m_groupEditorWidget->setDisabled(true);
    }

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::addPoint(std::string _group_name) const
{
    if(m_advancedMode)
    {
        m_treeWidget->blockSignals(true);

        LandmarksOrImageSeriesConstLock li_lock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        QTreeWidgetItem* const item = getGroupItem(_group_name);

        const auto nb_childs                       = static_cast<std::size_t>(item->childCount());
        std::optional<std::size_t> maybe_nb_points = num_points(li_lock, _group_name);
        if(!maybe_nb_points.has_value())
        {
            return;
        }

        std::size_t nb_points = *maybe_nb_points;
        for(std::size_t idx = nb_childs ; idx < nb_points ; ++idx)
        {
            std::optional<data::landmarks::point_t> maybe_new_point = get_point(li_lock, _group_name, idx);
            if(!maybe_new_point.has_value())
            {
                continue;
            }

            data::landmarks::point_t new_point = *maybe_new_point;

            auto* const pt = new QTreeWidgetItem();
            for(int i = 0 ; i < 3 ; ++i)
            {
                pt->setText(i, QString::fromStdString(std::to_string(new_point[static_cast<std::size_t>(i)])));
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
        LandmarksOrImageSeriesConstLock li_lock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        std::optional<data::landmarks::LandmarksGroup> group = get_group(li_lock, _name);
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
    button->setProperty(s_group_property_name, QString::fromStdString(_name));

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

        const int index                 = m_treeWidget->indexOfTopLevelItem(item);
        QTreeWidgetItem* const top_item = m_treeWidget->takeTopLevelItem(index);
        delete top_item;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

//------------------------------------------------------------------------------

void landmarks::removePoint(std::string _group_name, std::size_t _index) const
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* const item = getGroupItem(_group_name);

    SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

    QTreeWidgetItem* const point_item = item->child(static_cast<int>(_index));
    item->removeChild(point_item);
    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::renameGroup(std::string _old_name, std::string _new_name) const
{
    m_treeWidget->blockSignals(true);

    const QString qt_new_name   = QString::fromStdString(_new_name);
    QTreeWidgetItem* const item = getGroupItem(_old_name);
    item->setData(0, s_GROUP_NAME_ROLE, qt_new_name);
    QWidget* const widget = m_treeWidget->itemWidget(item, 1);
    widget->setProperty(s_group_property_name, qt_new_name);

    item->setText(0, qt_new_name);

    m_treeWidget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::modifyGroup(std::string _name) const
{
    QTreeWidgetItem* const item = getGroupItem(_name);

    item->setText(0, _name.c_str());

    LandmarksOrImageSeriesConstLock li_lock = constLock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
    );

    std::optional<data::landmarks::LandmarksGroup> maybe_group = get_group(li_lock, _name);
    if(!maybe_group.has_value())
    {
        return;
    }

    data::landmarks::LandmarksGroup group = *maybe_group;

    auto* const color_button = dynamic_cast<QPushButton*>(m_treeWidget->itemWidget(item, 1));

    const QColor color = convertToQColor(group.m_color);

    setColorButtonIcon(color_button, color);

    bool group_selected = item->isSelected();

    if(m_advancedMode) // Check if a child is selected.
    {
        for(int i = 0 ; i < item->childCount() && !group_selected ; ++i)
        {
            group_selected = item->child(i)->isSelected();
        }
    }

    if(group_selected)
    {
        // Set widget values
        m_sizeSlider->setValue(static_cast<int>(group.m_size));
        m_visibilityCheckbox->setChecked(group.m_visibility);

        const QString shape_text = group.m_shape == data::landmarks::Shape::CUBE ? "Cube" : "Sphere";
        m_shapeSelector->setCurrentText(shape_text);

        const float opacity = group.m_color[3];
        m_opacitySlider->setValue(static_cast<int>(opacity * float(m_opacitySlider->maximum())));
    }
}

//------------------------------------------------------------------------------

void landmarks::modifyPoint(std::string _group_name, std::size_t _index) const
{
    if(m_advancedMode)
    {
        auto const item_list = m_treeWidget->findItems(QString::fromStdString(_group_name), Qt::MatchExactly);

        SIGHT_ASSERT("Only a single item can be named '" + _group_name + "'", item_list.size() == 1);

        QTreeWidgetItem* const item = item_list.at(0);

        SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

        QTreeWidgetItem* const point_item = item->child(static_cast<int>(_index));

        LandmarksOrImageSeriesConstLock li_lock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        std::optional<data::landmarks::point_t> maybe_point = get_point(li_lock, _group_name, _index);
        if(!maybe_point.has_value())
        {
            return;
        }

        data::landmarks::point_t point = *maybe_point;

        m_treeWidget->blockSignals(true);
        for(int i = 0 ; i < 3 ; ++i)
        {
            point_item->setText(i, QString("%1").arg(point[static_cast<std::size_t>(i)]));
        }

        m_treeWidget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void landmarks::selectPoint(std::string _group_name, std::size_t _index) const
{
    m_treeWidget->blockSignals(true);

    QTreeWidgetItem* current_item = getGroupItem(_group_name);

    if(m_advancedMode)
    {
        SIGHT_ASSERT(
            "Index must be less than " << current_item->childCount(),
            static_cast<int>(_index) < current_item->childCount()
        );

        current_item = current_item->child(static_cast<int>(_index));
    }

    m_treeWidget->setCurrentItem(current_item);

    int size     = 0;
    bool visible = false;
    QString shape_text;
    float opacity = NAN;
    {
        LandmarksOrImageSeriesConstLock li_lock = constLock();
        SIGHT_ASSERT(
            "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
            li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
        );

        std::optional<data::landmarks::LandmarksGroup> maybe_group = get_group(li_lock, _group_name);
        if(!maybe_group.has_value())
        {
            return;
        }

        data::landmarks::LandmarksGroup group = *maybe_group;

        size       = static_cast<int>(group.m_size);
        visible    = group.m_visibility;
        shape_text = group.m_shape == data::landmarks::Shape::CUBE ? "Cube" : "Sphere";
        opacity    = group.m_color[3];
    }

    // Set widget values
    m_sizeSlider->setValue(size);
    m_visibilityCheckbox->setChecked(visible);
    m_shapeSelector->setCurrentText(shape_text);
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
    static std::size_t group_count = 0;

    // const auto landmarks = m_landmarks.lock();
    LandmarksOrImageSeriesConstLock li_lock = constLock();
    SIGHT_ASSERT(
        "Neither inout '" << s_LANDMARKS_INOUT << "' nor inout '" << s_IMAGE_SERIES_INOUT << "' exists.",
        li_lock.landmarks != nullptr || li_lock.imageSeries != nullptr
    );

    const data::landmarks::GroupNameContainer group_names = get_group_names(li_lock);

    const std::string new_group_name_prefix = m_advancedMode ? "Group_" : "Point_";

    while(std::find(
              group_names.begin(),
              group_names.end(),
              new_group_name_prefix + std::to_string(group_count)
          ) != group_names.end())
    {
        ++group_count;
    }

    return new_group_name_prefix + std::to_string(group_count);
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

    const bool selected_group = (item != nullptr);

    if(selected_group)
    {
        const int top_level_index = m_treeWidget->indexOfTopLevelItem(item);

        if(m_advancedMode && top_level_index == -1)
        {
            item = item->parent();
        }

        _selection = item->text(0).toStdString();
    }

    return selected_group;
}

//------------------------------------------------------------------------------

QColor landmarks::convertToQColor(const data::landmarks::color_t& _color)
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
    const int icon_size = _button->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(icon_size, icon_size);
    pix.fill(_color);

    _button->setIcon(QIcon(pix));
}

//------------------------------------------------------------------------------

QTreeWidgetItem* landmarks::getGroupItem(const std::string& _group_name) const
{
    const auto item_list = m_treeWidget->findItems(QString::fromStdString(_group_name), Qt::MatchExactly);

    SIGHT_ASSERT("Only a single item can be named '" + _group_name + "'", item_list.size() == 1);

    return item_list.at(0);
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
