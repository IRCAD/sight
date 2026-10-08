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

#pragma once

#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <QBoxLayout>
#include <QImage>

#include <cstddef>
#include <string>

namespace sight::module::ui::qt::image::thumbnail
{

/// Creates a 2D thumbnail from a 3D mesh using an axonometric projection.
QImage make_mesh_thumbnail(const sight::data::reconstruction& _reconstruction, int _width, int _height);

/// Appends an image to a thumbnail layout.
void append_thumbnail(
    QBoxLayout& _layout,
    const QImage& _image,
    int _width,
    int _height,
    bool _vertical,
    int _content_border
);

/// Appends mesh thumbnails from a model series to a layout.
void append_model_thumbnails(
    const sight::data::model_series& _model_series,
    QBoxLayout& _main_layout,
    std::size_t _thumbnail_width,
    std::size_t _thumbnail_height,
    std::size_t _max_thumbnails,
    bool _vertical,
    const std::string& _reconstruction_id
);

} // namespace sight::module::ui::qt::image::thumbnail
