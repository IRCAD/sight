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

#include "thumbnail_widget.hpp"
#include "thumbnail_mesh_helper.hpp"

#include <core/type.hpp>

#include <data/helper/medical_image.hpp>

#include <ui/qt/container/widget.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPen>
#include <QSizePolicy>
#include <QStringList>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <set>
#include <type_traits>
#include <utility>

namespace sight::module::ui::qt::image
{

constexpr int BORDER = 2;

//------------------------------------------------------------------------------

static std::size_t expected_components(sight::data::image::pixel_format_t _format)
{
    switch(_format)
    {
        case sight::data::image::pixel_format_t::gray_scale:
            return 1;

        case sight::data::image::pixel_format_t::rg:
            return 2;

        case sight::data::image::pixel_format_t::rgb:
        case sight::data::image::pixel_format_t::bgr:
            return 3;

        case sight::data::image::pixel_format_t::rgba:
        case sight::data::image::pixel_format_t::bgra:
            return 4;

        case sight::data::image::pixel_format_t::undefined:
        case sight::data::image::pixel_format_t::count:
            return 0;
    }

    return 0;
}

//------------------------------------------------------------------------------

static void clear_layout(QLayout& _layout)
{
    while(QLayoutItem* child = _layout.takeAt(0))
    {
        if(QLayout* layout = child->layout(); layout != nullptr)
        {
            clear_layout(*layout);
            delete layout;
        }
        else
        {
            delete child->widget();
            delete child;
        }
    }
}

//------------------------------------------------------------------------------

static std::uint8_t normalize(double _value, double _minimum, double _maximum)
{
    if(!std::isfinite(_value))
    {
        return 0;
    }

    if(_maximum <= _minimum)
    {
        return 0;
    }

    const double normalized = std::clamp((_value - _minimum) / (_maximum - _minimum), 0., 1.);
    return static_cast<std::uint8_t>(std::lround(normalized * 255.));
}

//------------------------------------------------------------------------------

template<typename T>
static QImage make_slice(
    const T* _buffer,
    std::size_t _slice_index,
    std::size_t _width,
    std::size_t _height,
    sight::data::image::pixel_format_t _format,
    std::size_t _components,
    double _minimum,
    double _maximum
)
{
    QImage image(static_cast<int>(_width), static_cast<int>(_height), QImage::Format_RGBA8888);
    if(image.isNull())
    {
        return {};
    }

    const std::size_t pixel_count = _width * _height;
    const T* slice                = _buffer + (_slice_index * pixel_count * _components);
    auto* destination             = image.bits();

    const auto component = [ = ](const T* _pixel, std::size_t _index)
                           {
                               if constexpr(std::is_same_v<T, std::uint8_t>)
                               {
                                   return _pixel[_index];
                               }
                               else
                               {
                                   return normalize(static_cast<double>(_pixel[_index]), _minimum, _maximum);
                               }
                           };

    for(std::size_t pixel_index = 0 ; pixel_index < pixel_count ; ++pixel_index)
    {
        const T* source = slice + (pixel_index * _components);
        auto* target    = destination + (pixel_index * 4);

        switch(_format)
        {
            case sight::data::image::pixel_format_t::gray_scale:
            {
                const auto value = normalize(static_cast<double>(source[0]), _minimum, _maximum);
                target[0] = value;
                target[1] = value;
                target[2] = value;
                target[3] = 255;
                break;
            }

            case sight::data::image::pixel_format_t::rgb:
                target[0] = component(source, 0);
                target[1] = component(source, 1);
                target[2] = component(source, 2);
                target[3] = 255;
                break;

            case sight::data::image::pixel_format_t::rgba:
                target[0] = component(source, 0);
                target[1] = component(source, 1);
                target[2] = component(source, 2);
                target[3] = component(source, 3);
                break;

            case sight::data::image::pixel_format_t::bgr:
                target[0] = component(source, 2);
                target[1] = component(source, 1);
                target[2] = component(source, 0);
                target[3] = 255;
                break;

            case sight::data::image::pixel_format_t::bgra:
                target[0] = component(source, 2);
                target[1] = component(source, 1);
                target[2] = component(source, 0);
                target[3] = component(source, 3);
                break;

            case sight::data::image::pixel_format_t::rg:
                target[0] = component(source, 0);
                target[1] = component(source, 1);
                target[2] = 0;
                target[3] = 255;
                break;

            case sight::data::image::pixel_format_t::undefined:
            case sight::data::image::pixel_format_t::count:
                return {};
        }
    }

    return image;
}

//------------------------------------------------------------------------------

template<typename T>
static void append_thumbnails(
    const sight::data::image_series& _image_series,
    QBoxLayout& _main_layout,
    std::size_t _thumbnail_width,
    std::size_t _thumbnail_height,
    std::size_t _max_thumbnails,
    bool _vertical
)
{
    const auto size       = _image_series.size();
    const auto components = _image_series.num_components();
    const auto* buffer    = static_cast<const T*>(_image_series.buffer());
    const auto [minimum, maximum] = sight::data::helper::medical_image::get_min_max<double>(
        _image_series.get_const_sptr()
    );

    const double aspect_ratio = static_cast<double>(size[0]) / static_cast<double>(size[1]);
    int width                 = static_cast<int>(_thumbnail_width);
    int height                = static_cast<int>(_thumbnail_height);

    if(width == 0 && height == 0)
    {
        width = height = 128;
    }
    else if(width == 0)
    {
        width = std::max(1, static_cast<int>(std::lround(static_cast<double>(height) * aspect_ratio)));
    }
    else if(height == 0)
    {
        height = std::max(1, static_cast<int>(std::lround(static_cast<double>(width) / aspect_ratio)));
    }

    const auto fiducials = _image_series.get_fiducials();

    const std::size_t thumbnail_count = _max_thumbnails == 0 ? size[2] : std::min(_max_thumbnails, size[2]);

    for(std::size_t thumbnail_index = 0 ; thumbnail_index < thumbnail_count ; ++thumbnail_index)
    {
        const std::size_t slice_index = thumbnail_count == 1
                                        ? size[2] / 2
                                        : (thumbnail_index * (size[2] - 1)) / (thumbnail_count - 1);

        QImage image = make_slice(
            buffer,
            slice_index,
            size[0],
            size[1],
            _image_series.pixel_format(),
            components,
            minimum,
            maximum
        );

        std::set<std::string> labels;
        if(fiducials)
        {
            QPainter painter(&image);
            const auto query_results = fiducials->query_fiducials(
                [slice_index](const sight::data::fiducials_series::query_result& _result)
                {
                    return _result.m_referenced_frame_number.has_value()
                           && !_result.m_referenced_frame_number->empty()
                           && _result.m_referenced_frame_number->front() > 0
                           && std::cmp_equal(_result.m_referenced_frame_number->front() - 1, slice_index);
                });

            for(const auto& query_result : query_results)
            {
                if(query_result.m_fiducial_description)
                {
                    labels.insert(*query_result.m_fiducial_description);
                }

                if(!query_result.m_graphic_data)
                {
                    continue;
                }

                QColor color = Qt::white;
                if(query_result.m_color)
                {
                    color.setRgbF(
                        (*query_result.m_color)[0],
                        (*query_result.m_color)[1],
                        (*query_result.m_color)[2],
                        1.
                    );
                }

                QPen pen(color, 14, Qt::SolidLine);
                painter.setPen(pen);

                const auto& graphic_data = *query_result.m_graphic_data;
                if(query_result.m_shape == sight::data::fiducials_series::shape::shape && graphic_data.size() > 3)
                {
                    for(std::size_t index = 0 ; index + 3 < graphic_data.size() ; index += 2)
                    {
                        painter.drawLine(
                            QPointF(graphic_data[index], graphic_data[index + 1]),
                            QPointF(graphic_data[index + 2], graphic_data[index + 3])
                        );
                    }
                }
                else if(query_result.m_shape == sight::data::fiducials_series::shape::ruler && graphic_data.size() >= 4)
                {
                    painter.drawLine(
                        QPointF(graphic_data[0], graphic_data[1]),
                        QPointF(graphic_data[2], graphic_data[3])
                    );
                }
            }
        }

        thumbnail::append_thumbnail(_main_layout, image, width, height, _vertical, 2 * BORDER);
    }
}

//------------------------------------------------------------------------------

void thumbnail_widget::configuring()
{
    this->initialize();

    if(const auto config = this->get_config().get_child_optional("config.<xmlattr>"); config)
    {
        m_width             = config->get<std::size_t>("width", m_width);
        m_height            = config->get<std::size_t>("height", m_height);
        m_max_thumbnails    = config->get<std::size_t>("max", m_max_thumbnails);
        m_vertical          = config->get<std::string>("orientation", "vertical") == "vertical";
        m_reconstruction_id = config->get<std::string>("reconstructionId", m_reconstruction_id);

        const std::string color = config->get<std::string>("color", m_color);
        if(const QColor parsed_color(QString::fromStdString(color)); !parsed_color.isValid())
        {
            SIGHT_ERROR("Invalid thumbnail background color: '" << color << "'.");
        }
        else
        {
            m_color = color;
        }
    }
}

//------------------------------------------------------------------------------

void thumbnail_widget::create_widget()
{
    m_thumbnail_widget = new QWidget();
    m_thumbnail_widget->setObjectName(QString::fromStdString(this->base_id()));
    m_thumbnail_widget->setAttribute(Qt::WA_StyledBackground, true);

    m_main_layout = new QBoxLayout(m_vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    m_main_layout->setAlignment(m_vertical ? Qt::AlignTop | Qt::AlignHCenter : Qt::AlignLeft | Qt::AlignVCenter);
    m_main_layout->setContentsMargins(0, 0, 0, 0);
    m_thumbnail_widget->setLayout(m_main_layout);

    if(m_vertical)
    {
        m_thumbnail_widget->setMinimumWidth(static_cast<int>(m_width));
    }
    else
    {
        m_thumbnail_widget->setMinimumHeight(static_cast<int>(m_height));
    }
}

//------------------------------------------------------------------------------

void thumbnail_widget::starting()
{
    this->create();

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    SIGHT_ASSERT("Can't retrieve the Qt container.", qt_container);

    this->create_widget();

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_thumbnail_widget);
    qt_container->set_layout(layout);

    this->updating();
}

//------------------------------------------------------------------------------

sight::service::connections_t thumbnail_widget::auto_connections() const
{
    sight::service::connections_t connections;
    connections.push(IMAGE_SERIES_IN, sight::data::signals::MODIFIED, sight::service::slots::UPDATE);
    connections.push(
        IMAGE_SERIES_IN,
        sight::data::image::signals::BUFFER_MODIFIED,
        sight::service::slots::UPDATE
    );
    connections.push(MODEL_SERIES_IN, sight::data::signals::MODIFIED, sight::service::slots::UPDATE);
    connections.push(
        MODEL_SERIES_IN,
        sight::data::model_series::signals::RECONSTRUCTIONS_ADDED,
        sight::service::slots::UPDATE
    );
    connections.push(
        MODEL_SERIES_IN,
        sight::data::model_series::signals::RECONSTRUCTIONS_REMOVED,
        sight::service::slots::UPDATE
    );
    return connections;
}

//------------------------------------------------------------------------------

void thumbnail_widget::updating()
{
    if(m_main_layout == nullptr || m_thumbnail_widget == nullptr)
    {
        return;
    }

    clear_layout(*m_main_layout);

    m_model_connections.disconnect();

    const auto image_series = m_image_series.lock();
    const auto model_series = m_model_series.lock();

    if(image_series && model_series)
    {
        SIGHT_ERROR("Only one of 'image_series' and 'model_series' can be set on a thumbnails widget.");
        m_thumbnail_widget->setStyleSheet("background-color: transparent;");
        return;
    }

    if(model_series)
    {
        for(const auto& reconstruction : model_series->get_reconstruction_db())
        {
            if(!reconstruction
               || (!m_reconstruction_id.empty() && reconstruction->get_id() != m_reconstruction_id))
            {
                continue;
            }

            m_model_connections.connect(
                reconstruction,
                sight::data::signals::MODIFIED,
                this->get_sptr(),
                sight::service::slots::UPDATE
            );
            m_model_connections.connect(
                reconstruction,
                sight::data::reconstruction::signals::MESH_CHANGED,
                this->get_sptr(),
                sight::service::slots::UPDATE
            );

            if(const auto mesh = reconstruction->get_mesh())
            {
                m_model_connections.connect(
                    mesh,
                    sight::data::signals::MODIFIED,
                    this->get_sptr(),
                    sight::service::slots::UPDATE
                );
                m_model_connections.connect(
                    mesh,
                    sight::data::mesh::signals::VERTEX_MODIFIED,
                    this->get_sptr(),
                    sight::service::slots::UPDATE
                );
            }

            if(const auto material = reconstruction->get_material())
            {
                m_model_connections.connect(
                    material,
                    sight::data::signals::MODIFIED,
                    this->get_sptr(),
                    sight::service::slots::UPDATE
                );
            }
        }

        m_thumbnail_widget->setStyleSheet(
            QString("background-color: %1;").arg(QString::fromStdString(m_color))
        );
        thumbnail::append_model_thumbnails(
            *model_series,
            *m_main_layout,
            m_width,
            m_height,
            m_max_thumbnails,
            m_vertical,
            m_reconstruction_id
        );

        if(QWidget* parent = m_thumbnail_widget->parentWidget(); parent != nullptr)
        {
            parent->adjustSize();
        }

        return;
    }

    if(!image_series)
    {
        m_thumbnail_widget->setStyleSheet("background-color: transparent;");
        return;
    }

    const auto size = image_series->size();
    if(size[0] == 0 || size[1] == 0 || size[2] == 0)
    {
        m_thumbnail_widget->setStyleSheet("background-color: transparent;");
        return;
    }

    constexpr auto max_dimension = static_cast<std::size_t>(std::numeric_limits<int>::max());
    if(size[0] > max_dimension || size[1] > max_dimension)
    {
        SIGHT_ERROR("Cannot create thumbnails: the source image dimensions are too large.");
        return;
    }

    const auto components = expected_components(image_series->pixel_format());
    if(components == 0 || components != image_series->num_components())
    {
        SIGHT_ERROR("Cannot create thumbnails: the source image pixel format is invalid.");
        return;
    }

    const auto lock = image_series->dump_lock();
    if(image_series->buffer() == nullptr)
    {
        m_thumbnail_widget->setStyleSheet("background-color: transparent;");
        return;
    }

    m_thumbnail_widget->setStyleSheet(
        QString("background-color: %1;").arg(QString::fromStdString(m_color))
    );
    switch(image_series->type())
    {
        case core::type::type_t::int_8:
            append_thumbnails<std::int8_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::int_16:
            append_thumbnails<std::int16_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::int_32:
            append_thumbnails<std::int32_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::int_64:
            append_thumbnails<std::int64_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::uint_8:
            append_thumbnails<std::uint8_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::uint_16:
            append_thumbnails<std::uint16_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::uint_32:
            append_thumbnails<std::uint32_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::uint_64:
            append_thumbnails<std::uint64_t>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::float_32:
            append_thumbnails<float>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::float_64:
            append_thumbnails<double>(
                *image_series,
                *m_main_layout,
                m_width,
                m_height,
                m_max_thumbnails,
                m_vertical
            );
            break;

        case core::type::type_t::none:
            SIGHT_ERROR("Cannot create thumbnails from an image series without a pixel type.");
            break;

        default:
            SIGHT_ERROR("Cannot create thumbnails from an unknown image pixel type.");
            break;
    }

    if(QWidget* parent = m_thumbnail_widget->parentWidget(); parent != nullptr)
    {
        parent->adjustSize();
    }
}

//------------------------------------------------------------------------------

void thumbnail_widget::stopping()
{
    m_model_connections.disconnect();
    m_main_layout      = nullptr;
    m_thumbnail_widget = nullptr;
    this->destroy();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
