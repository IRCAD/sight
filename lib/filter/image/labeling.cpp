/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include "filter/image/labeling.hpp"

#include "filter/image/detail/filters.hxx"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/tools/dispatcher.hpp>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/string.hpp>

#include <itkLabelImageToShapeLabelMapFilter.h>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

struct labeling_filter
{
    struct parameters
    {
        data::image::sptr m_input_image;
        data::image::sptr m_output_image;
        unsigned int m_num_labels {};
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(parameters& _params)
    {
        using image_t        = itk::Image<PIXELTYPE, 3>;
        using binary_image_t = itk::Image<std::uint8_t, 3>;
        typename image_t::Pointer itk_image;
        itk_image = io::itk::move_to_itk<image_t>(_params.m_input_image);

        binary_image_t::Pointer out;
        out = filter::image::detail::labeling<PIXELTYPE, 3>(itk_image, _params.m_num_labels);

        io::itk::move_from_itk<binary_image_t>(out, *_params.m_output_image);
    }
};

//------------------------------------------------------------------------------

data::image::sptr labeling(data::image::sptr _image, unsigned int _num_labels)
{
    auto do_labeling =
        []<class PIXEL_TYPE>(const data::image& _input_image, data::image& _output_image, unsigned int _num_labels)
        {
            using image_t        = itk::Image<PIXEL_TYPE, 3>;
            using binary_image_t = itk::Image<std::uint8_t, 3>;
            typename image_t::Pointer itk_image;
            itk_image = io::itk::move_to_itk<image_t>(_input_image);

            binary_image_t::Pointer out;
            out = filter::image::detail::labeling<PIXEL_TYPE, 3>(itk_image, _num_labels);

            io::itk::move_from_itk<binary_image_t>(out, _output_image);
        };

    auto output_image = std::make_shared<data::image>();

    const core::type type = _image->type();
    using sight::core::tools::dispatcher;
    using sight::core::tools::integer_types;
    dispatcher<integer_types, decltype(do_labeling)>::invoke(type, *_image, *output_image, _num_labels);

    return output_image;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void compute_centroids(
    data::image::sptr _image,
    std::vector<data::point_list::sptr> _point_list_centroids,
    std::vector<std::vector<std::size_t> > _point_list_labels
)
{
    // Gets centroids of shapes from a labelled image and adds them as a 'landmark' field to it.
    auto do_compute_centroids =
        []<class PIXEL_TYPE>(const data::image::sptr _image,
                             std::vector<data::point_list::sptr> _point_list_centroids,
                             std::vector<std::vector<std::size_t> > _point_list_labels)
        {
            [[maybe_unused]] const unsigned int dimension = 3;
            SIGHT_ASSERT("Only image dimension 3 managed.", _image->num_dimensions() == dimension);
            using input_image_t = typename itk::Image<PIXEL_TYPE, dimension>;
            typename input_image_t::Pointer itk_input_image = io::itk::move_to_itk<input_image_t>(_image);

            using label_t              = PIXEL_TYPE;
            using output_image_t       = itk::Image<label_t, dimension>;
            using shape_label_object_t = itk::ShapeLabelObject<label_t, dimension>;
            using label_map_t          = itk::LabelMap<shape_label_object_t>;

            // Extract shapes
            using i2l_t = typename itk::LabelImageToShapeLabelMapFilter<output_image_t, label_map_t>;

            auto i2l = i2l_t::New();
            i2l->SetInput(itk_input_image);
            i2l->SetComputePerimeter(true);
            i2l->SetBackgroundValue(0);
            i2l->Update();

            // If we have clusters in XML file
            if(!_point_list_centroids.empty() && !_point_list_labels.empty())
            {
                label_map_t* label_map = i2l->GetOutput();
                data::point::sptr new_point;

                for(unsigned int n = 1 ; n <= label_map->GetNumberOfLabelObjects() ; ++n)
                {
                    std::vector<std::size_t> find_planes;
                    std::size_t plane = 0;
                    for(plane = 0 ; plane < _point_list_labels.size() ; ++plane)
                    {
                        std::vector<std::size_t> current_plane = _point_list_labels.at(plane);

                        for(std::size_t label_in_plane : current_plane)
                        {
                            if(label_in_plane == n)
                            {
                                find_planes.push_back(plane);
                            }
                        }
                    }

                    if(!find_planes.empty())
                    {
                        // We need to get the 'n-1'th object because of the '0' background value (1st object = '1' label
                        // value)
                        shape_label_object_t* label_object = label_map->GetNthLabelObject(n - 1);

                        // append to landmark
                        const typename shape_label_object_t::CentroidType centroid = label_object->GetCentroid();

                        new_point = std::make_shared<data::point>(centroid[0], centroid[1], centroid[2]);

                        for(std::size_t find_plane : find_planes)
                        {
                            const data::point_list::sptr& plane_point_list = _point_list_centroids.at(find_plane);

                            // append to point the label
                            std::stringstream label_name;
                            label_name << n;
                            data::string::sptr label = std::make_shared<data::string>(label_name.str());

                            plane_point_list->push_back(new_point);
                        }
                    }
                }
            }
            // Default behaviour (put point in field of the image)
            else
            {
                //get landmarks
                data::point_list::sptr landmarks = data::helper::medical_image::get_landmarks(*_image);

                SIGHT_ASSERT("landmarks not instanced", landmarks);
                landmarks->clear();

                label_map_t* label_map = i2l->GetOutput();
                data::point::sptr new_point;
                for(unsigned int n = 0 ; n < label_map->GetNumberOfLabelObjects() ; ++n)
                {
                    shape_label_object_t* label_object = label_map->GetNthLabelObject(n);

                    // append to landmark
                    const typename shape_label_object_t::CentroidType centroid = label_object->GetCentroid();

                    new_point = std::make_shared<data::point>(centroid[0], centroid[1], centroid[2]);
                    landmarks->push_back(new_point);

                    // append to point the label
                    std::stringstream label_name;
                    label_name << n;
                    new_point->set_label(label_name.str());
                }

                data::helper::medical_image::set_landmarks_visibility(*_image, true);
            }
        };

    // Call the ITK operator
    const core::type type = _image->type();

    using sight::core::tools::dispatcher;
    using sight::core::tools::supported_dispatcher_types;
    dispatcher<supported_dispatcher_types, decltype(do_compute_centroids)>::invoke(
        type,
        _image,
        _point_list_centroids,
        _point_list_labels
    );
}

using function_t = std::function<std::uint8_t(const std::uint8_t&)>;

template<class PIXELTYPE>
class lambda_functor
{
public:

    using function_t = std::function<PIXELTYPE(const PIXELTYPE&)>;

    lambda_functor()
    = default;

    explicit lambda_functor(function_t _f) :
        m_function(std::move(_f))
    {
    }

    //------------------------------------------------------------------------------

    PIXELTYPE operator()(const PIXELTYPE& _in)
    {
        return m_function(_in);
    }

    // Needs to be implemented because it is called by the itkUnaryFunctorImageFilter when setting the functor.
    // Always return true to force-set the functor.
    bool operator!=(const lambda_functor& /*unused*/)
    {
        return true;
    }

private:

    function_t m_function;
};

struct convert_label_image_to_binary_mask_filter
{
    struct parameters
    {
        const data::image& input_image;
        data::image& output_image;
        const std::string& label_field_name;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(parameters& _params)
    {
        using image_t = typename itk::Image<PIXELTYPE, 3>;

        lambda_functor<PIXELTYPE> functor;

        if(not _params.label_field_name.empty())
        {
            data::vector::csptr labels = _params.input_image.get_field<data::vector>(_params.label_field_name);

            if(!labels)
            {
                SIGHT_INFO(
                    "No field named '" + _params.label_field_name
                    + "' in 'labelImage'. No binary mask generated."
                );
                return;
            }

            // Use a more flexible container for different pixel types
            std::set<PIXELTYPE> label_set;

            std::for_each(
                labels->begin(),
                labels->end(),
                [&label_set](data::object::csptr _o)
                {
                    data::integer::csptr int_obj = std::dynamic_pointer_cast<const data::integer>(_o);
                    SIGHT_ASSERT("The label vector should only contain integers.", int_obj);
                    const int val = int(int_obj->value());

                    if constexpr(std::is_same_v<PIXELTYPE, char>)
                    {
                        SIGHT_ASSERT("Integer value outside char range", val >= CHAR_MIN && val <= CHAR_MAX);
                    }
                    else
                    {
                        SIGHT_ASSERT("Integer value outside pixel type range", std::in_range<PIXELTYPE>(val));
                    }

                    label_set.insert(static_cast<PIXELTYPE>(val));
                });

            functor = lambda_functor<PIXELTYPE>(
                function_t(
                    [label_set](const PIXELTYPE& _in) -> PIXELTYPE
                {
                    return label_set.find(_in) != label_set.end()
                           ? std::numeric_limits<PIXELTYPE>::max()
                           : static_cast<PIXELTYPE>(0);
                })
            );
        }
        else
        {
            functor = lambda_functor<PIXELTYPE>(
                function_t(
                    [](const PIXELTYPE& _in) -> PIXELTYPE
                {
                    return _in > static_cast<PIXELTYPE>(0)
                           ? std::numeric_limits<PIXELTYPE>::max()
                           : static_cast<PIXELTYPE>(0);
                })
            );
        }

        auto itk_label_img        = io::itk::move_to_itk<image_t>(_params.input_image);
        auto label_to_mask_filter = itk::UnaryFunctorImageFilter<image_t, image_t, lambda_functor<PIXELTYPE> >::New();

        label_to_mask_filter->SetFunctor(functor);
        label_to_mask_filter->SetInput(itk_label_img);
        label_to_mask_filter->Update();

        typename image_t::Pointer itk_mask_img = label_to_mask_filter->GetOutput();

        io::itk::move_from_itk<image_t>(itk_mask_img, _params.output_image);
    }
};

//------------------------------------------------------------------------------

void convert_label_image_to_binary_mask(
    const data::image& _label_image,
    data::image& _mask_image,
    const std::string& _label_field_name
)
{
    // Preparing the parameters for ITK
    convert_label_image_to_binary_mask_filter::parameters params
    {
        .input_image      = _label_image,
        .output_image     = _mask_image,
        .label_field_name = _label_field_name
    };

    // Call the ITK operator
    const core::type type = _label_image.type();
    core::tools::dispatcher<core::tools::integer_types,
                            convert_label_image_to_binary_mask_filter>::invoke(type, params);
}

} // namespace sight::filter::image.
