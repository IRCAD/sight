/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/ui/qt/calibration/images_selector.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/id.hpp>

#include <data/image.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::calibration
{

const core::com::slots::key_t images_selector::ADD_SLOT    = "add";
const core::com::slots::key_t images_selector::REMOVE_SLOT = "remove";
const core::com::slots::key_t images_selector::RESET_SLOT  = "reset";

//------------------------------------------------------------------------------
images_selector::images_selector() noexcept
{
    new_slot(ADD_SLOT, &images_selector::add, this);
    new_slot(REMOVE_SLOT, &images_selector::remove, this);
    new_slot(RESET_SLOT, &images_selector::reset, this);
}

//------------------------------------------------------------------------------

images_selector::~images_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void images_selector::configuring()
{
    sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void images_selector::starting()
{
    const auto frame_tl = m_frame_tl.lock();
    SIGHT_ASSERT("Frame timeline is not found.", frame_tl);

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    // Main container, VBox
    auto* v_layout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    auto* nb_items_h_box = new QHBoxLayout();

    //     Fill the nbItemsHBox
    auto* label = new QLabel("nb captures:");
    nb_items_h_box->addWidget(label);

    m_nb_captures_label = new QLabel("0");
    nb_items_h_box->addWidget(m_nb_captures_label);
    nb_items_h_box->addStretch();

    //   The ListWidget
    m_captures_list_widget = new QListWidget();

    // Fill the main VBox
    v_layout->addLayout(nb_items_h_box);
    v_layout->addWidget(m_captures_list_widget);

    qt_container->set_layout(v_layout);

    this->updating();
}

//------------------------------------------------------------------------------

void images_selector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void images_selector::updating()
{
    const auto vector = m_selected_image.lock();

    m_captures_list_widget->clear();
    unsigned int capture_idx = 0;
    for(const data::object::sptr& obj : *vector)
    {
        data::image::sptr image = std::dynamic_pointer_cast<data::image>(obj);
        if(image)
        {
            QString count_string;

            count_string = QString("%1. %2").arg(capture_idx).arg(QString::fromStdString(image->get_id()));
            m_captures_list_widget->addItem(count_string);
            ++capture_idx;
        }
    }

    m_nb_captures_label->setText(QString("%1").arg(capture_idx));
}

// ----------------------------------------------------------------------------

void images_selector::remove()
{
    int idx = m_captures_list_widget->currentRow();

    if(idx >= 0)
    {
        const auto vector      = m_selected_image.lock();
        data::object::sptr obj = (*vector)[std::size_t(idx)];

        const auto scoped_emitter = vector->scoped_emit();
        vector->remove(obj);

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void images_selector::reset()
{
    const auto vector = m_selected_image.lock();

    const auto scoped_emitter = vector->scoped_emit();
    vector->clear();

    m_captures_list_widget->clear();
    m_nb_captures_label->setText(QString("0"));
}

//------------------------------------------------------------------------------

void images_selector::add(core::clock::type _timestamp)
{
    const auto frame_tl = m_frame_tl.lock();
    CSPTR(data::frame_tl::buffer_t) buffer = frame_tl->get_closest_buffer(_timestamp);

    if(!buffer)
    {
        SIGHT_INFO("Buffer not found with timestamp " << _timestamp);
        return;
    }

    data::image::sptr image = std::make_shared<data::image>();

    data::image::size_t size;
    size[0] = frame_tl->get_width();
    size[1] = frame_tl->get_height();
    size[2] = 1;

    enum data::image::pixel_format_t format
    {
        data::image::pixel_format_t::undefined
    };
    // FIXME since frameTL does not have format information, we assume that image are Grayscale, RGB or RGBA according
    // to the number of components.
    switch(frame_tl->num_components())
    {
        case 1:
            format = data::image::gray_scale;
            break;

        case 3:
            format = data::image::rgb;
            break;

        case 4:
            format = data::image::rgba;
            break;

        default:
            format = data::image::undefined;
    }

    image->resize(size, frame_tl->type(), format);
    const data::image::origin_t origin = {0., 0., 0.};
    image->set_origin(origin);
    const data::image::spacing_t spacing = {1., 1., 1.};
    image->set_spacing(spacing);
    image->set_window_width({100});
    image->set_window_center({0});

    const auto dump_lock = image->dump_lock();

    const std::uint8_t* frame_buff = &buffer->get_element(0);
    auto* img_buffer               = static_cast<std::uint8_t*>(image->buffer());
    std::copy(frame_buff, frame_buff + buffer->size(), img_buffer);

    const auto vector = m_selected_image.lock();

    const auto scoped_emitter = vector->scoped_emit();
    vector->push_back(image);

    this->updating();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
