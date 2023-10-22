/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "filter/image/filters.hpp"
#include "filter/image/filters.hxx"

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

struct labelingFilter
{
    struct Parameters
    {
        data::image::sptr m_inputImage;
        data::image::sptr m_outputImage;
        unsigned int m_numLabels {};
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& _params)
    {
        using image_t        = itk::Image<PIXELTYPE, 3>;
        using binary_image_t = itk::Image<std::uint8_t, 3>;
        typename image_t::Pointer itk_image;
        itk_image = io::itk::move_to_itk<image_t>(_params.m_inputImage);

        binary_image_t::Pointer out;
        out = filter::image::labeling<PIXELTYPE, 3>(itk_image, _params.m_numLabels);

        io::itk::move_from_itk<binary_image_t>(out, _params.m_outputImage);
    }
};

//------------------------------------------------------------------------------

data::image::sptr labeling(data::image::sptr _image, unsigned int _num_labels)
{
    data::image::sptr output_image = std::make_shared<data::image>();

    labelingFilter::Parameters params;
    params.m_inputImage  = _image;
    params.m_outputImage = output_image;
    params.m_numLabels   = _num_labels;

    const core::type type = _image->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, labelingFilter>::invoke(type, params);

    // Notify image
    data::object::modified_signal_t::sptr sig;
    sig = output_image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();

    return output_image;
}

//------------------------------------------------------------------------------

// Gets centroids of shapes from a labelled image and adds them as a 'landmark' field to it.
struct LabelImageFilter
{
    struct Parameters
    {
        data::image::sptr i_image;
        std::vector<data::point_list::sptr> i_lPointListCentroids;
        std::vector<std::vector<std::size_t> > i_lPointListLabels;
    };
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& _params)
    {
        data::image::sptr image      = _params.i_image;
        const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", image->numDimensions() == dimension);
        using input_image_t = typename itk::Image<PIXELTYPE, dimension>;
        typename input_image_t::Pointer itk_input_image = io::itk::move_to_itk<input_image_t>(image);

        using label_t              = PIXELTYPE;
        using output_image_t       = itk::Image<label_t, dimension>;
        using shape_label_object_t = itk::ShapeLabelObject<label_t, dimension>;
        using label_map_t          = itk::LabelMap<shape_label_object_t>;

        // Extract shapes
        using I2LType = typename itk::LabelImageToShapeLabelMapFilter<output_image_t, label_map_t>;

        typename I2LType::Pointer i2l = I2LType::New();
        i2l->SetInput(itk_input_image);
        i2l->SetComputePerimeter(true);
        i2l->SetBackgroundValue(0);
        i2l->Update();

        // If we have clusters in XML file
        if(!_params.i_lPointListCentroids.empty() && !_params.i_lPointListLabels.empty())
        {
            label_map_t* label_map = i2l->GetOutput();
            data::point::sptr new_point;

            for(unsigned int n = 1 ; n <= label_map->GetNumberOfLabelObjects() ; ++n)
            {
                std::vector<std::size_t> find_planes;
                std::size_t plane = 0;
                for(plane = 0 ; plane < _params.i_lPointListLabels.size() ; ++plane)
                {
                    std::vector<std::size_t> current_plane = _params.i_lPointListLabels.at(plane);

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
                        data::point_list::sptr plane_point_list =
                            _params.i_lPointListCentroids.at(find_plane);

                        // append to point the label
                        std::stringstream label_name;
                        label_name << n;
                        data::string::sptr label = std::make_shared<data::string>(label_name.str());

                        plane_point_list->getPoints().push_back(new_point);
                    }
                }
            }
        }
        // Default behaviour (put point in field of the image)
        else
        {
            //get landmarks
            data::point_list::sptr landmarks = data::helper::medical_image::get_landmarks(*image);

            SIGHT_ASSERT("landmarks not instanced", landmarks);
            landmarks->getPoints().clear();

            label_map_t* label_map = i2l->GetOutput();
            data::point::sptr new_point;
            for(unsigned int n = 0 ; n < label_map->GetNumberOfLabelObjects() ; ++n)
            {
                shape_label_object_t* label_object = label_map->GetNthLabelObject(n);

                // append to landmark
                const typename shape_label_object_t::CentroidType centroid = label_object->GetCentroid();

                new_point = std::make_shared<data::point>(centroid[0], centroid[1], centroid[2]);
                landmarks->getPoints().push_back(new_point);

                // append to point the label
                std::stringstream label_name;
                label_name << n;
                new_point->setLabel(label_name.str());
            }

            data::helper::medical_image::set_landmarks_visibility(*image, true);
        }
    }
};

//------------------------------------------------------------------------------

void compute_centroids(
    data::image::sptr _image,
    std::vector<data::point_list::sptr> _point_list_centroids,
    std::vector<std::vector<std::size_t> > _point_list_labels
)
{
    // Preparing the parameters for ITK
    LabelImageFilter::Parameters params;
    params.i_image = _image;

    if(!_point_list_centroids.empty())
    {
        params.i_lPointListCentroids = _point_list_centroids;
        params.i_lPointListLabels    = _point_list_labels;
    }

    // Call the ITK operator
    const core::type type = _image->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, LabelImageFilter>::invoke(type, params);
}

} // namespace sight::filter::image.
