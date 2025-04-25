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

#include "module/filter/image/label_geometry_image.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <filter/image/labeling.hpp>

#include <boost/tokenizer.hpp>

namespace sight::module::filter::image
{

const core::com::slots::key_t UPDATE_SELECTED_POINT_LIST = "updateSelectedPointList";

//-----------------------------------------------------------------------------

label_geometry_image::label_geometry_image() :
    filter(m_signals)
{
    new_slot(UPDATE_SELECTED_POINT_LIST, &label_geometry_image::update_selected_point_list, this);
}

//-----------------------------------------------------------------------------

label_geometry_image::~label_geometry_image()
= default;

//-----------------------------------------------------------------------------

void label_geometry_image::configuring()
{
    const auto& config = this->get_config();

    const auto& clusters = config.get_child("clusters");
    for(const auto& elt : boost::make_iterator_range(clusters.equal_range("cluster")))
    {
        const auto cluster_str = elt.second.get_value<std::string>();
        std::vector<std::size_t> cluster_labels;
        const boost::char_separator<char> separator(",");
        const boost::tokenizer<boost::char_separator<char> > tok {cluster_str, separator};

        for(const auto& t : tok)
        {
            cluster_labels.push_back(std::stoul(t));
        }

        m_l_point_list_labels.push_back(cluster_labels);
        m_l_point_list_centroids.push_back(std::make_shared<data::point_list>());
    }
}

//-----------------------------------------------------------------------------

void label_geometry_image::starting()
{
}

//-----------------------------------------------------------------------------

void label_geometry_image::updating()
{
    const auto image = m_image.lock();

    // Call the ITK operator
    sight::filter::image::compute_centroids(image.get_shared(), m_l_point_list_centroids, m_l_point_list_labels);

    if(m_l_point_list_centroids.empty())
    {
        data::point_list::sptr landmarks = data::helper::medical_image::get_landmarks(*image);

        SIGHT_ASSERT("landmarks not instanced", landmarks);

        for(const auto& point : landmarks->get_points())
        {
            auto sig = image->signal<data::image::landmark_added_signal_t>(data::image::LANDMARK_ADDED_SIG);
            sig->async_emit(point);
        }
    }
    else
    {
        this->update_selected_point_list("1", "");
    }

    this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
}

//-----------------------------------------------------------------------------

void label_geometry_image::stopping()
{
}

//-----------------------------------------------------------------------------

void label_geometry_image::update_selected_point_list(std::string _value, std::string /*key*/)
{
    SIGHT_ASSERT(
        "value: " << _value << "should end by a number between 0 and 9",
        _value.back() >= '0' && _value.back() <= '9'
    );
    std::size_t index_plane = std::stoul(_value);
    // if the XML enum is between 1 and n, instead of 0 and n-1
    if(index_plane > 0)
    {
        index_plane--;
    }

    data::point_list::sptr selected_point_list = m_l_point_list_centroids.at(index_plane);

    for(std::size_t id_point = 0 ; id_point < selected_point_list->get_points().size() ; ++id_point)
    {
        selected_point_list->get_points().at(id_point)->set_label(std::to_string(id_point));
    }

    this->set_output(m_l_point_list_centroids.at(index_plane), "pointList");
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image.
