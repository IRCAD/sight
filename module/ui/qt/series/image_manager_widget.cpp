/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "image_manager_widget.hpp"

#include <core/runtime/path.hpp>

#include <data/image_series.hpp>
#include <data/reconstruction.hpp>

#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QButtonGroup>
#include <QCheckBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>

#include <algorithm>
#include <unordered_set>
#include <utility>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

image_manager_widget::image_manager_widget()
{
    new_signal<signals::image_removed_t>(signals::IMAGE_REMOVED);
    new_signal<signals::model_removed_t>(signals::MODEL_REMOVED);
    new_signal<signals::image_selected_t>(signals::IMAGE_SELECTED);
    new_signal<signals::image_visibility_changed_t>(signals::IMAGE_VISIBILITY_CHANGED);
    new_signal<signals::model_selected_t>(signals::MODEL_SELECTED);
}

//------------------------------------------------------------------------------

void image_manager_widget::configuring()
{
    this->sight::ui::service::initialize();

    if(const auto config = this->get_config().get_child_optional("config.<xmlattr>"); config)
    {
        m_thumbnail_width  = config->get<std::size_t>("thumbnail_width", m_thumbnail_width);
        m_thumbnail_height = config->get<std::size_t>("thumbnail_height", m_thumbnail_height);
        m_thumbnail_color  = config->get<std::string>("thumbnail_color", m_thumbnail_color);
    }
}

//------------------------------------------------------------------------------

void image_manager_widget::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    SIGHT_ASSERT("The image manager parent must be a Qt widget container", qt_container);

    m_series_states.clear();
    m_image_cards.clear();

    m_scroll_area = new QScrollArea();
    m_scroll_area->setWidgetResizable(true);
    m_scroll_area->setFrameShape(QFrame::NoFrame);

    m_cards_container = new QWidget();
    m_image_layout    = new QVBoxLayout(m_cards_container);
    m_image_layout->setContentsMargins(4, 4, 4, 4);
    m_image_layout->setSpacing(6);
    m_image_layout->setAlignment(Qt::AlignTop);
    m_image_selection_group = new QButtonGroup(m_cards_container);
    m_image_selection_group->setExclusive(false);
    m_scroll_area->setWidget(m_cards_container);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_scroll_area);
    qt_container->set_layout(layout);

    this->update_image_widgets();
}

//------------------------------------------------------------------------------

service::connections_t image_manager_widget::auto_connections() const
{
    connections_t connections;
    connections.push("series_set", data::series_set::signals::ADDED_OBJECTS, service::slots::UPDATE);
    connections.push("series_set", data::series_set::signals::REMOVED_OBJECTS, service::slots::UPDATE);
    connections.push("model_series", data::model_series::signals::RECONSTRUCTIONS_ADDED, service::slots::UPDATE);
    connections.push("model_series", data::model_series::signals::RECONSTRUCTIONS_REMOVED, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------
inline static std::string format_date(const std::string& _date)
{
    if(_date.size() < 8)
    {
        return _date;
    }

    return _date.substr(4, 2) + "/" + _date.substr(6, 2) + "/" + _date.substr(0, 4);
}

//------------------------------------------------------------------------------
inline static std::string image_extension(const data::image_series& _image)
{
    if(_image.num_instances() > 0)
    {
        const auto extension = _image.get_file().extension().string();
        if(!extension.empty())
        {
            return extension;
        }
    }

    return {};
}

//------------------------------------------------------------------------------
inline static std::string image_dimensions(const data::image_series& _image)
{
    const auto size = _image.size();
    return std::to_string(size[0]) + " x " + std::to_string(size[1]) + " x " + std::to_string(size[2]);
}

//------------------------------------------------------------------------------
inline static std::string mesh_extension(const data::model_series& _model_series)
{
    if(_model_series.num_instances() > 0)
    {
        return _model_series.get_file().extension().string();
    }

    return {};
}

//------------------------------------------------------------------------------
void image_manager_widget::update_image_widgets()
{
    if(m_image_layout == nullptr)
    {
        return;
    }

    const auto series_set         = m_series_set.lock();
    const auto input_model_series = m_model_series.lock();

    if(series_set && !m_selected_image_id.empty())
    {
        const bool selection_exists = std::ranges::any_of(
            *series_set,
            [this](const data::series::csptr& _series)
            {
                return std::dynamic_pointer_cast<const data::image_series>(_series)
                       && _series->get_id() == m_selected_image_id;
            });

        if(!selection_exists)
        {
            m_selected_image_id.clear();
        }
    }

    std::unordered_set<std::string> active_ids;

    const auto add_image = [this, &active_ids](
        const data::image_series::sptr& _image,
        std::string _label,
        std::string _date
                            )
                           {
                               if(!_image)
                               {
                                   return;
                               }

                               const std::string id = _image->get_id();
                               if(id.empty())
                               {
                                   return;
                               }

                               active_ids.insert(id);

                               auto& state = m_series_states[id];
                               if(state.closed || m_image_cards.contains(id))
                               {
                                   return;
                               }

                               if(state.alias.empty())
                               {
                                   if(!_label.empty())
                                   {
                                       state.alias = std::move(_label);
                                   }
                                   else if(const auto& series_instance_uid = _image->get_series_instance_uid();
                                           !series_instance_uid.empty())
                                   {
                                       state.alias = series_instance_uid;
                                   }
                                   else
                                   {
                                       state.alias = "Image " + std::to_string(m_image_cards.size() + 1);
                                   }
                               }

                               const bool select_image = m_selected_image_id.empty();
                               if(select_image)
                               {
                                   m_selected_image_id = id;
                               }

                               this->add_card(id, state.alias, _date, _image);

                               if(select_image)
                               {
                                   this->async_emit(signals::IMAGE_SELECTED, id);
                               }
                           };

    const auto add_model = [this, &active_ids](const data::model_series::csptr& _model_series)
                           {
                               if(!_model_series)
                               {
                                   return;
                               }

                               for(const auto& reconstruction : _model_series->get_reconstruction_db())
                               {
                                   if(!reconstruction)
                                   {
                                       continue;
                                   }

                                   const std::string id = reconstruction->get_id();
                                   if(id.empty())
                                   {
                                       continue;
                                   }

                                   active_ids.insert(id);

                                   auto& state = m_series_states[id];
                                   if(state.closed || m_image_cards.contains(id))
                                   {
                                       continue;
                                   }

                                   state.alias = reconstruction->get_organ_name().empty()
                                                 ? "Mesh " + std::to_string(m_image_cards.size() + 1)
                                                 : reconstruction->get_organ_name();

                                   this->add_card(
                                       id,
                                       state.alias,
                                       _model_series->get_series_date(),
                                       nullptr,
                                       _model_series
                                   );
                               }
                           };

    if(series_set)
    {
        for(const auto& series : *series_set)
        {
            if(const auto image_series = std::dynamic_pointer_cast<data::image_series>(series))
            {
                add_image(
                    image_series,
                    image_series->get_series_description(),
                    image_series->get_series_date()
                );
            }
            else if(const auto model_series = std::dynamic_pointer_cast<data::model_series>(series))
            {
                add_model(model_series);
            }
        }
    }

    add_model(input_model_series.get_shared());

    for(auto it = m_image_cards.begin() ; it != m_image_cards.end() ; )
    {
        if(active_ids.contains(it->first))
        {
            ++it;
            continue;
        }

        const std::string id    = it->first;
        const bool was_selected = m_selected_image_id == id;
        if(was_selected)
        {
            m_selected_image_id.clear();
            this->async_emit(signals::IMAGE_VISIBILITY_CHANGED, id, false);
        }

        this->remove_card(id);
        m_series_states.erase(id);
        it = m_image_cards.begin();
    }
}

//------------------------------------------------------------------------------

void image_manager_widget::add_card(
    const std::string& _id,
    const std::string& _label,
    const std::string& _date,
    const data::image_series::sptr& _image_series,
    const data::model_series::csptr& _model_series
)
{
    auto* card_widget = new QFrame(m_cards_container);
    card_widget->setObjectName(QString("image_card_%1").arg(QString::fromStdString(_id)));
    card_widget->setProperty("sightCardType", QStringLiteral("image_card"));
    card_widget->setMinimumHeight(180);
    card_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    card_widget->setFrameShape(QFrame::StyledPanel);
    card_widget->setFrameShadow(QFrame::Raised);

    auto* card_layout = new QVBoxLayout(card_widget);
    card_layout->setContentsMargins(8, 8, 8, 8);

    auto* close_button    = new QPushButton(card_widget);
    const auto close_icon = core::runtime::get_resource_file_path("sight::module::ui::icons/cross.svg");
    close_button->setIcon(QIcon(QString::fromStdString(close_icon.string())));
    close_button->setIconSize(QSize(16, 16));
    close_button->setFixedSize(24, 24);
    close_button->setToolTip("Close");

    auto* controls_layout = new QHBoxLayout();
    controls_layout->setContentsMargins(0, 0, 0, 0);
    controls_layout->setSpacing(16);
    controls_layout->addWidget(close_button, 0, Qt::AlignVCenter);
    controls_layout->addStretch();
    card_layout->addLayout(controls_layout);

    image_card card;
    card.widget = card_widget;

    QVBoxLayout* details_layout = nullptr;

    if(_image_series || _model_series)
    {
        auto* content_widget = new QWidget(card_widget);
        auto* content_layout = new QHBoxLayout(content_widget);
        content_layout->setContentsMargins(0, 0, 0, 0);
        content_layout->setSpacing(12);
        card_layout->addWidget(content_widget);

        auto* thumbnail_host = new QWidget(content_widget);
        thumbnail_host->setObjectName(
            QString("thumbnail_%1").arg(QString::fromStdString(_id))
        );
        thumbnail_host->setFixedSize(
            static_cast<int>(m_thumbnail_width),
            static_cast<int>(m_thumbnail_height)
        );
        content_layout->addWidget(thumbnail_host, 0, Qt::AlignTop);

        auto* details_widget = new QWidget(content_widget);
        details_layout = new QVBoxLayout(details_widget);
        details_layout->setContentsMargins(0, 0, 0, 0);
        details_layout->setSpacing(6);
        content_layout->addWidget(details_widget, 1, Qt::AlignTop);

        card.thumbnail_service_id = this->get_id() + "_thumbnail_" + _id;
        const auto thumbnail_container = sight::ui::qt::container::widget::make();
        thumbnail_container->set_qt_container(thumbnail_host);
        card.thumbnail_container = thumbnail_container;
        sight::ui::registry::register_sid_container(card.thumbnail_service_id, card.thumbnail_container);

        try
        {
            card.thumbnail_service = sight::service::add(
                "sight::module::ui::qt::image::thumbnail_widget",
                card.thumbnail_service_id
            );
            if(_image_series)
            {
                card.thumbnail_service->set_input(_image_series, "image_series");
            }
            else
            {
                card.thumbnail_service->set_input(_model_series, "model_series");
            }

            service::config_t thumbnail_config;
            thumbnail_config.put("config.<xmlattr>.width", m_thumbnail_width);
            thumbnail_config.put("config.<xmlattr>.height", m_thumbnail_height);
            thumbnail_config.put("config.<xmlattr>.orientation", "vertical");
            thumbnail_config.put("config.<xmlattr>.color", m_thumbnail_color);
            thumbnail_config.put("config.<xmlattr>.max", 1);
            if(_model_series)
            {
                thumbnail_config.put("config.<xmlattr>.reconstructionId", _id);
            }

            card.thumbnail_service->configure(thumbnail_config);
            card.thumbnail_service->start().get();
        }
        catch(...)
        {
            if(card.thumbnail_service)
            {
                card.thumbnail_service->stop().get();
                sight::service::remove(card.thumbnail_service);
            }

            sight::ui::registry::unregister_sid_container(card.thumbnail_service_id);
            card.thumbnail_container->destroy_container();
            delete card_widget;
            throw;
        }
    }

    auto* alias_row = new QHBoxLayout();
    alias_row->setContentsMargins(0, 0, 0, 0);
    alias_row->setSpacing(3);

    if(_image_series)
    {
        auto* visibility = new QCheckBox(card_widget);
        visibility->setObjectName(QString("image_visibility_%1").arg(QString::fromStdString(_id)));
        visibility->setChecked(m_selected_image_id == _id);
        visibility->setToolTip(QString("Display this image in the scenes."));
        m_image_selection_group->addButton(visibility);
        controls_layout->addWidget(visibility, 0, Qt::AlignVCenter);

        QObject::connect(
            visibility,
            &QCheckBox::toggled,
            this,
            [this, _id](bool _visible)
            {
                if(_visible)
                {
                    m_selected_image_id = _id;
                    this->async_emit(signals::IMAGE_SELECTED, _id);

                    QMetaObject::invokeMethod(
                        this,
                        [this, _id]()
                    {
                        this->async_emit(
                            signals::IMAGE_VISIBILITY_CHANGED,
                            _id,
                            true
                        );
                    },
                        Qt::QueuedConnection
                    );
                }
                else
                {
                    const bool was_selected = m_selected_image_id == _id;
                    this->async_emit(signals::IMAGE_VISIBILITY_CHANGED, _id, false);

                    if(was_selected)
                    {
                        m_selected_image_id.clear();
                        for(const auto& [other_id, other_card] : m_image_cards)
                        {
                            if(other_id == _id || !other_card.widget)
                            {
                                continue;
                            }

                            const auto object_name = QString("image_visibility_%1").arg(
                                QString::fromStdString(other_id)
                            );
                            auto* checkbox = other_card.widget->findChild<QCheckBox*>(object_name);
                            if(checkbox != nullptr && checkbox->isChecked())
                            {
                                m_selected_image_id = other_id;
                                this->async_emit(signals::IMAGE_SELECTED, other_id);
                                this->async_emit(signals::IMAGE_VISIBILITY_CHANGED, other_id, true);
                                break;
                            }
                        }
                    }
                }
            });
    }

    if(_model_series)
    {
        auto* visibility = new QCheckBox(card_widget);
        visibility->setObjectName(QString("mesh_visibility_%1").arg(QString::fromStdString(_id)));
        visibility->setToolTip(QString("Display this mesh in the scene."));

        const auto& reconstructions = _model_series->get_reconstruction_db();
        const auto reconstruction   = std::ranges::find_if(
            reconstructions,
            [&_id](const data::reconstruction::csptr& _reconstruction)
            {
                return _reconstruction && _reconstruction->get_id() == _id;
            });
        visibility->setChecked(
            reconstruction != reconstructions.cend() && (*reconstruction)->get_is_visible()
        );
        controls_layout->addWidget(visibility, 0, Qt::AlignVCenter);
        QObject::connect(
            visibility,
            &QCheckBox::toggled,
            this,
            [this, _id](bool _visible)
            {
                this->async_emit(signals::MODEL_SELECTED, _id, _visible);
            });
    }

    auto* alias = new QLineEdit(QString::fromStdString(_label), card_widget);
    alias->setObjectName(QString("image_alias_%1").arg(QString::fromStdString(_id)));
    alias->setProperty("sightCardField", QStringLiteral("true"));
    alias->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    alias->setCursorPosition(0);
    alias->setToolTip(QString("Rename this item"));
    alias->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    const int field_height = alias->sizeHint().height();
    alias->setFixedHeight(field_height);
    alias_row->addWidget(alias, 1);

    QObject::connect(
        alias,
        &QLineEdit::textEdited,
        this,
        [this, _id](const QString& _text)
        {
            if(const auto state = m_series_states.find(_id); state != m_series_states.end())
            {
                state->second.alias = _text.toStdString();
            }
        });

    const auto add_label = [card_widget, details_layout, card_layout, field_height](const std::string& _text)
                           {
                               auto* field = new QFrame(card_widget);
                               field->setProperty("sightCardField", QStringLiteral("true"));
                               field->setFrameShape(QFrame::NoFrame);
                               field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
                               field->setFixedHeight(field_height);

                               auto* field_layout = new QHBoxLayout(field);
                               field_layout->setContentsMargins(3, 3, 3, 3);

                               auto* label = new QLabel(QString::fromStdString(_text), field);
                               label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                               label->setWordWrap(true);
                               label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
                               field_layout->addWidget(label);

                               (details_layout != nullptr ? details_layout : card_layout)->addWidget(field);
                           };

    if(details_layout != nullptr)
    {
        details_layout->addLayout(alias_row);
    }
    else
    {
        card_layout->addLayout(alias_row);
    }

    if(_image_series)
    {
        const std::string modality   = _image_series->get_modality_string();
        const std::string extension  = image_extension(*_image_series);
        const std::string dimensions = image_dimensions(*_image_series);
        const std::string date       = format_date(_date);

        const std::string image_info = modality
                                       + (extension.empty() ? "" : " " + extension)
                                       + " - " + dimensions;
        add_label(image_info);
        add_label(date);
    }
    else
    {
        std::string mesh_info = "Mesh";
        if(_model_series)
        {
            const auto extension = mesh_extension(*_model_series);
            if(!extension.empty())
            {
                mesh_info += " " + extension;
            }

            for(const auto& reconstruction : _model_series->get_reconstruction_db())
            {
                if(reconstruction && reconstruction->get_id() == _id && reconstruction->get_mesh())
                {
                    const auto mesh = reconstruction->get_mesh();
                    mesh_info += " - " + std::to_string(mesh->num_points()) + " points, "
                                 + std::to_string(mesh->num_cells()) + " cells";
                    break;
                }
            }
        }

        add_label(mesh_info);

        const std::string date = format_date(_date);
        if(!date.empty())
        {
            add_label(date);
        }
    }

    QObject::connect(
        close_button,
        &QPushButton::clicked,
        this,
        [this, _id, is_image = _image_series != nullptr, is_model_series = _model_series != nullptr]
        {
            m_series_states.at(_id).closed = true;
            if(is_image && m_selected_image_id == _id)
            {
                m_selected_image_id.clear();
                this->async_emit(signals::IMAGE_VISIBILITY_CHANGED, _id, false);

                for(const auto& [other_id, other_card] : m_image_cards)
                {
                    if(other_id != _id && other_card.widget)
                    {
                        const auto object_name = QString("image_visibility_%1").arg(
                            QString::fromStdString(other_id)
                        );
                        if(auto* checkbox = other_card.widget->findChild<QCheckBox*>(object_name); checkbox != nullptr)
                        {
                            checkbox->setChecked(true);
                            break;
                        }
                    }
                }
            }

            this->remove_card(_id);

            if(is_image)
            {
                this->async_emit(signals::IMAGE_REMOVED, _id);
            }
            else if(is_model_series)
            {
                this->async_emit(signals::MODEL_REMOVED, _id);
            }
        });

    m_image_layout->addWidget(card_widget);
    m_image_cards.emplace(_id, std::move(card));
}

//------------------------------------------------------------------------------
void image_manager_widget::remove_card(const std::string& _id)
{
    auto node = m_image_cards.extract(_id);
    if(node.empty())
    {
        return;
    }

    auto& card = node.mapped();
    if(card.thumbnail_service)
    {
        card.thumbnail_service->stop().get();
        sight::ui::registry::unregister_sid_container(card.thumbnail_service_id);
        sight::service::remove(card.thumbnail_service);
        card.thumbnail_service.reset();
    }

    if(card.thumbnail_container)
    {
        card.thumbnail_container->destroy_container();
        card.thumbnail_container.reset();
    }

    if(card.widget != nullptr)
    {
        m_image_layout->removeWidget(card.widget);
        card.widget->deleteLater();
    }
}

//------------------------------------------------------------------------------

void image_manager_widget::clear_cards()
{
    while(!m_image_cards.empty())
    {
        this->remove_card(m_image_cards.begin()->first);
    }
}

//------------------------------------------------------------------------------

void image_manager_widget::updating()
{
    this->update_image_widgets();
}

//------------------------------------------------------------------------------

void image_manager_widget::stopping()
{
    this->clear_cards();
    m_series_states.clear();
    m_selected_image_id.clear();
    m_image_selection_group = nullptr;
    this->destroy();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
