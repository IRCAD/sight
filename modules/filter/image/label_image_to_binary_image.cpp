/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/filter/image/label_image_to_binary_image.hpp"

#include <core/com/signal.hxx>

#include <data/integer.hpp>
#include <data/vector.hpp>

#include <io/itk/itk.hpp>

#include <service/macros.hpp>

#include <itkImage.h>
#include <itkUnaryFunctorImageFilter.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <utility>

namespace sight::module::filter::image
{

using function_t = std::function<std::uint8_t(const std::uint8_t&)>;

class lambda_functor
{
public:

    lambda_functor()
    = default;

    explicit lambda_functor(function_t _f) :
        m_function(std::move(_f))
    {
    }

    //------------------------------------------------------------------------------

    inline std::uint8_t operator()(const std::uint8_t& _in)
    {
        return m_function(_in);
    }

    // Needs to be implemented because it is called by the itkUnaryFunctorImageFilter when setting the functor.
    // Always return true to force-set the functor.
    inline bool operator!=(const lambda_functor& /*unused*/)
    {
        return true;
    }

private:

    function_t m_function;
};

//------------------------------------------------------------------------------

label_image_to_binary_image::label_image_to_binary_image() :
    filter(m_signals)
{
}

//------------------------------------------------------------------------------

label_image_to_binary_image::~label_image_to_binary_image()
= default;

//------------------------------------------------------------------------------

void label_image_to_binary_image::configuring()
{
    const config_t config = this->get_config();

    m_label_set_field_name = config.get_optional<std::string>("config.<xmlattr>.labelsField");
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::starting()
{
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::updating()
{
    using image_t = typename itk::Image<std::uint8_t, 3>;

    const auto label_image = m_label_image.lock();
    SIGHT_ASSERT("No " << LABEL_IMAGE_INPUT << " input.", label_image);

    const auto mask_image = m_binary_mask.lock();
    SIGHT_ASSERT("No " << BINARY_MASK_INOUT << " inout.", mask_image);

    SIGHT_ASSERT(
        "The label image must be a greyscale image with uint8 values.",
        label_image->type() == core::type::UINT8 && label_image->num_components() == 1
    );

    lambda_functor functor;
    if(m_label_set_field_name)
    {
        data::vector::csptr labels = label_image->get_field<data::vector>(m_label_set_field_name.value());

        if(!labels)
        {
            SIGHT_INFO(
                "No field named '" + m_label_set_field_name.value()
                + "' in 'labelImage'. No binary mask generated."
            );
            return;
        }

        std::bitset<std::numeric_limits<std::uint8_t>::max() + 1> label_set;

        std::for_each(
            labels->begin(),
            labels->end(),
            [&label_set](data::object::csptr _o)
            {
                data::integer::csptr int_obj = std::dynamic_pointer_cast<const data::integer>(_o);
                SIGHT_ASSERT("The label vector should only contain integers.", int_obj);
                const int val = int(int_obj->value());
                SIGHT_ASSERT("The integers in the vector must be in the [0, 255] range.", val >= 0 && val <= 255);
                label_set.set(static_cast<std::uint8_t>(val), true);
            });

        functor = lambda_functor(
            function_t(
                [label_set](const std::uint8_t& _in)
            {
                return label_set[_in] ? 255 : 0;
            })
        );
    }
    else
    {
        functor = lambda_functor(
            function_t(
                [](const std::uint8_t& _in)
            {
                return _in > 0 ? 255 : 0;
            })
        );
    }

    typename image_t::Pointer itk_label_img = io::itk::move_to_itk<image_t>(label_image.get_shared());

    itk::UnaryFunctorImageFilter<image_t, image_t, lambda_functor>::Pointer label_to_mask_filter =
        itk::UnaryFunctorImageFilter<image_t, image_t, lambda_functor>::New();

    label_to_mask_filter->SetFunctor(functor);
    label_to_mask_filter->SetInput(itk_label_img);
    label_to_mask_filter->Update();

    typename image_t::Pointer itk_mask_img = label_to_mask_filter->GetOutput();

    io::itk::move_from_itk<image_t::Pointer>(itk_mask_img, mask_image.get_shared());
    const auto modified_sig = mask_image->signal<data::object::modified_signal_t>(data::image::MODIFIED_SIG);

    modified_sig->async_emit();

    this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t label_image_to_binary_image::auto_connections() const
{
    return {
        {LABEL_IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {LABEL_IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image.
