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

#include "thumbnail_mesh_helper.hpp"

#include <data/mesh.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPolygonF>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

namespace sight::module::ui::qt::image::thumbnail
{

//----------------------------------------------------------------------------

QImage make_mesh_thumbnail(const sight::data::reconstruction& _reconstruction, int _width, int _height)
{
    QImage image(_width, _height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    const auto mesh = _reconstruction.get_mesh();
    if(!mesh || mesh->num_points() == 0 || _width <= 0 || _height <= 0)
    {
        return image;
    }

    const auto lock        = mesh->dump_lock();
    const auto point_begin = mesh->cbegin<sight::data::iterator::point::xyz>();

    const auto project = [](const auto& _point)
                         {
                             return QPointF(
                                 static_cast<double>(_point.x) - static_cast<double>(_point.y),
                                 (static_cast<double>(_point.x) + static_cast<double>(_point.y)) * .5
                                 - static_cast<double>(_point.z)
                             );
                         };

    double minimum_x = std::numeric_limits<double>::max();
    double minimum_y = std::numeric_limits<double>::max();
    double maximum_x = std::numeric_limits<double>::lowest();
    double maximum_y = std::numeric_limits<double>::lowest();

    for(const auto& point : mesh->crange<sight::data::iterator::point::xyz>())
    {
        if(!std::isfinite(point.x) || !std::isfinite(point.y) || !std::isfinite(point.z))
        {
            continue;
        }

        const QPointF projected = project(point);
        minimum_x = std::min(minimum_x, projected.x());
        minimum_y = std::min(minimum_y, projected.y());
        maximum_x = std::max(maximum_x, projected.x());
        maximum_y = std::max(maximum_y, projected.y());
    }

    if(minimum_x > maximum_x || minimum_y > maximum_y)
    {
        return image;
    }

    constexpr double padding      = 5.;
    const double projected_width  = maximum_x - minimum_x;
    const double projected_height = maximum_y - minimum_y;
    const double available_width  = std::max(1., static_cast<double>(_width) - (2. * padding));
    const double available_height = std::max(1., static_cast<double>(_height) - (2. * padding));

    const double scale_x = projected_width > 0. ? available_width / projected_width : 1.;
    const double scale_y = projected_height > 0. ? available_height / projected_height : 1.;
    const double scale   = std::min(scale_x, scale_y);

    const double center_x = (minimum_x + maximum_x) / 2.;
    const double center_y = (minimum_y + maximum_y) / 2.;

    const auto to_image = [&](const auto& _point)
                          {
                              const QPointF projected = project(_point);
                              return QPointF(
                                  (static_cast<double>(_width) / 2.) + ((projected.x() - center_x) * scale),
                                  (static_cast<double>(_height) / 2.) + ((projected.y() - center_y) * scale)
                              );
                          };

    const auto valid_point = [&](sight::data::mesh::point_t _index)
                             {
                                 return _index < mesh->num_points()
                                        && std::isfinite((point_begin + _index)->x)
                                        && std::isfinite((point_begin + _index)->y)
                                        && std::isfinite((point_begin + _index)->z);
                             };

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(120, 120, 125), 1.));
    painter.setBrush(QColor(210, 210, 215));

    switch(mesh->cell_type())
    {
        case sight::data::mesh::cell_type_t::point:
        {
            const auto cells = mesh->cbegin<sight::data::iterator::cell::point>();
            for(std::size_t index = 0 ; index < mesh->num_cells() ; ++index)
            {
                const auto point_index = (cells + static_cast<std::ptrdiff_t>(index))->pt;
                if(valid_point(point_index))
                {
                    painter.drawEllipse(to_image(*(point_begin + point_index)), 2., 2.);
                }
            }

            break;
        }

        case sight::data::mesh::cell_type_t::line:
        {
            const auto cells = mesh->cbegin<sight::data::iterator::cell::line>();
            for(std::size_t index = 0 ; index < mesh->num_cells() ; ++index)
            {
                const auto& cell = *(cells + static_cast<std::ptrdiff_t>(index));
                if(valid_point(cell.pt[0]) && valid_point(cell.pt[1]))
                {
                    painter.drawLine(
                        to_image(*(point_begin + cell.pt[0])),
                        to_image(*(point_begin + cell.pt[1]))
                    );
                }
            }

            break;
        }

        case sight::data::mesh::cell_type_t::triangle:
        {
            const auto cells = mesh->cbegin<sight::data::iterator::cell::triangle>();
            for(std::size_t index = 0 ; index < mesh->num_cells() ; ++index)
            {
                const auto& cell = *(cells + static_cast<std::ptrdiff_t>(index));
                if(valid_point(cell.pt[0]) && valid_point(cell.pt[1]) && valid_point(cell.pt[2]))
                {
                    QPolygonF polygon;
                    polygon << to_image(*(point_begin + cell.pt[0]))
                    << to_image(*(point_begin + cell.pt[1]))
                    << to_image(*(point_begin + cell.pt[2]));
                    painter.drawPolygon(polygon);
                }
            }

            break;
        }

        case sight::data::mesh::cell_type_t::quad:
        {
            const auto cells = mesh->cbegin<sight::data::iterator::cell::quad>();
            for(std::size_t index = 0 ; index < mesh->num_cells() ; ++index)
            {
                const auto& cell = *(cells + static_cast<std::ptrdiff_t>(index));
                if(valid_point(cell.pt[0]) && valid_point(cell.pt[1])
                   && valid_point(cell.pt[2]) && valid_point(cell.pt[3]))
                {
                    QPolygonF polygon;
                    polygon << to_image(*(point_begin + cell.pt[0]))
                    << to_image(*(point_begin + cell.pt[1]))
                    << to_image(*(point_begin + cell.pt[2]))
                    << to_image(*(point_begin + cell.pt[3]));
                    painter.drawPolygon(polygon);
                }
            }

            break;
        }

        case sight::data::mesh::cell_type_t::tetra:
        case sight::data::mesh::cell_type_t::size:
            break;
    }

    return image;
}

//----------------------------------------------------------------------------

void append_thumbnail(
    QBoxLayout& _layout,
    const QImage& _image,
    int _width,
    int _height,
    bool _vertical,
    int _content_border
)
{
    _layout.addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    auto* box        = new QWidget();
    auto* box_layout = new QBoxLayout(QBoxLayout::TopToBottom, box);
    box_layout->setAlignment(Qt::AlignCenter);
    box_layout->setContentsMargins(0, 0, 0, 0);

    auto* image_layout = new QHBoxLayout();
    image_layout->setContentsMargins(0, 0, 0, 0);

    auto* image_label = new QLabel();
    image_label->setPixmap(
        QPixmap::fromImage(
            _image.scaled(
                QSize(std::max(1, _width - _content_border), std::max(1, _height - _content_border)),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            )
        )
    );
    image_label->setFixedSize(_width, _height);
    image_label->setAlignment(Qt::AlignCenter);
    image_label->setSizePolicy(
        _vertical ? QSizePolicy::MinimumExpanding : QSizePolicy::Preferred,
        _vertical ? QSizePolicy::Preferred : QSizePolicy::MinimumExpanding
    );
    image_label->setStyleSheet("QLabel { border: 2px solid rgb(86, 90, 94); }");
    image_layout->addWidget(image_label);
    image_layout->addStretch();
    box_layout->addLayout(image_layout);
    _layout.addWidget(box);
}

//----------------------------------------------------------------------------

void append_model_thumbnails(
    const sight::data::model_series& _model_series,
    QBoxLayout& _main_layout,
    std::size_t _thumbnail_width,
    std::size_t _thumbnail_height,
    std::size_t _max_thumbnails,
    bool _vertical,
    const std::string& _reconstruction_id
)
{
    std::vector<sight::data::reconstruction::csptr> reconstructions;
    for(const auto& reconstruction : _model_series.get_reconstruction_db())
    {
        if(reconstruction && reconstruction->get_mesh()
           && (_reconstruction_id.empty() || reconstruction->get_id() == _reconstruction_id))
        {
            reconstructions.push_back(reconstruction);
        }
    }

    const std::size_t thumbnail_count = _max_thumbnails == 0
                                        ? reconstructions.size()
                                        : std::min(_max_thumbnails, reconstructions.size());
    const int width  = static_cast<int>(_thumbnail_width == 0 ? 128 : _thumbnail_width);
    const int height = static_cast<int>(_thumbnail_height == 0 ? 128 : _thumbnail_height);

    for(std::size_t thumbnail_index = 0 ; thumbnail_index < thumbnail_count ; ++thumbnail_index)
    {
        const std::size_t reconstruction_index = thumbnail_count == 1
                                                 ? reconstructions.size() / 2
                                                 : (thumbnail_index * (reconstructions.size() - 1))
                                                 / (thumbnail_count - 1);
        const auto& reconstruction = reconstructions[reconstruction_index];

        append_thumbnail(
            _main_layout,
            make_mesh_thumbnail(*reconstruction, width, height),
            width,
            height,
            _vertical,
            2
        );
    }
}

} // namespace sight::module::ui::qt::image::thumbnail
