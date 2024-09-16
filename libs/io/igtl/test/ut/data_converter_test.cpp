/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data_converter_test.hpp"

#include <core/type.hpp>

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/line.hpp>
#include <data/map.hpp>
#include <data/matrix4.hpp>
#include <data/mesh.hpp>
#include <data/object.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <io/igtl/detail/converter/map_converter.hpp>
#include <io/igtl/detail/data_converter.hpp>
#include <io/igtl/detail/raw_message.hpp>

#include <utest_data/generator/image.hpp>
#include <utest_data/generator/mesh.hpp>

#include <igtlImageMessage.h>
#include <igtlPointMessage.h>
#include <igtlPositionMessage.h>
#include <igtlStringMessage.h>
#include <igtlTrackingDataMessage.h>
#include <igtlTransformMessage.h>

#include <algorithm>
#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::igtl::detail::ut::data_converter_test);

namespace sight::io::igtl::detail::ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void data_converter_test::setUp()
{
}

//------------------------------------------------------------------------------

void data_converter_test::tearDown()
{
}

//------------------------------------------------------------------------------

void data_converter_test::mesh_converter_test()
{
    data_converter::sptr converter = data_converter::get_instance();
    data::mesh::sptr mesh          = std::make_shared<data::mesh>();
    const auto lock                = mesh->dump_lock();
    utest_data::generator::mesh::generate_mesh(mesh);

    ::igtl::MessageBase::Pointer msg = converter->from_fw_object(mesh);

    CPPUNIT_ASSERT_MESSAGE("Message is null", msg);

    data::object::sptr obj = converter->from_igtl_message(msg);

    data::mesh::sptr mesh2 = std::dynamic_pointer_cast<data::mesh>(obj);
    CPPUNIT_ASSERT_MESSAGE("Mesh is null", mesh2);
    const auto lock2 = mesh2->dump_lock();

    CPPUNIT_ASSERT_EQUAL(mesh->num_points(), mesh2->num_points());
    CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), mesh2->num_cells());
    CPPUNIT_ASSERT_EQUAL(mesh->cell_size(), mesh2->cell_size());
    CPPUNIT_ASSERT_EQUAL(mesh->size_in_bytes(), mesh2->size_in_bytes());

    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::attribute::point_colors>(),
        mesh2->has<data::mesh::attribute::point_colors>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::attribute::cell_colors>(),
        mesh2->has<data::mesh::attribute::cell_colors>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::attribute::point_normals>(),
        mesh2->has<data::mesh::attribute::point_normals>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::attribute::cell_normals>(),
        mesh2->has<data::mesh::attribute::cell_normals>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::attribute::point_tex_coords>(),
        mesh2->has<data::mesh::attribute::point_tex_coords>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::attribute::cell_tex_coords>(),
        mesh2->has<data::mesh::attribute::cell_tex_coords>()
    );

    const auto dump_lock = mesh->dump_lock();
    namespace point = data::iterator::point;
    namespace cell  = data::iterator::cell;

    const auto range1 = mesh->czip_range<point::xyz, point::rgba, point::nxyz>();
    const auto range2 = mesh2->czip_range<point::xyz, point::rgba, point::nxyz>();

    for(const auto& [orig, cur] : boost::combine(range1, range2))
    {
        const auto& [pt1, c1, n1] = orig;
        const auto& [pt2, c2, n2] = cur;

        CPPUNIT_ASSERT_EQUAL(pt1.x, pt2.x);
        CPPUNIT_ASSERT_EQUAL(pt1.y, pt2.y);
        CPPUNIT_ASSERT_EQUAL(pt1.z, pt2.z);

        CPPUNIT_ASSERT_EQUAL(c1.r, c2.r);
        CPPUNIT_ASSERT_EQUAL(c1.g, c2.g);
        CPPUNIT_ASSERT_EQUAL(c1.b, c2.b);
        CPPUNIT_ASSERT_EQUAL(c1.a, c2.a);

        CPPUNIT_ASSERT_EQUAL(n1.nx, n2.nx);
        CPPUNIT_ASSERT_EQUAL(n1.ny, n2.ny);
        CPPUNIT_ASSERT_EQUAL(n1.nz, n2.nz);
    }

    const auto cell_range1 = mesh->czip_range<cell::triangle, cell::rgba, cell::nxyz>();
    const auto cell_range2 = mesh2->czip_range<cell::triangle, cell::rgba, cell::nxyz>();

    for(const auto& [orig, cur] : boost::combine(cell_range1, cell_range2))
    {
        const auto& [cell1, c1, n1] = orig;
        const auto& [cell2, c2, n2] = cur;

        CPPUNIT_ASSERT_EQUAL(cell1.pt[0], cell2.pt[0]);
        CPPUNIT_ASSERT_EQUAL(cell1.pt[1], cell2.pt[1]);
        CPPUNIT_ASSERT_EQUAL(cell1.pt[2], cell2.pt[2]);

        CPPUNIT_ASSERT_EQUAL(c1.r, c2.r);
        CPPUNIT_ASSERT_EQUAL(c1.g, c2.g);
        CPPUNIT_ASSERT_EQUAL(c1.b, c2.b);
        CPPUNIT_ASSERT_EQUAL(c1.a, c2.a);

        CPPUNIT_ASSERT_EQUAL(n1.nx, n2.nx);
        CPPUNIT_ASSERT_EQUAL(n1.ny, n2.ny);
        CPPUNIT_ASSERT_EQUAL(n1.nz, n2.nz);
    }
}

//------------------------------------------------------------------------------

void data_converter_test::image_converter_test()
{
    data_converter::sptr converter = data_converter::get_instance();
    data::image::sptr image        = std::make_shared<data::image>();

    core::type type = core::type::INT32;
    utest_data::generator::image::generate_random_image(image, type);

    ::igtl::MessageBase::Pointer msg = converter->from_fw_object(image);

    CPPUNIT_ASSERT_MESSAGE("Message is null", msg);

    data::object::sptr obj = converter->from_igtl_message(msg);

    data::image::sptr image2 = std::dynamic_pointer_cast<data::image>(obj);

    CPPUNIT_ASSERT_MESSAGE("Image is null", image2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->spacing()[0], image2->spacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->spacing()[1], image2->spacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->spacing()[2], image2->spacing()[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->origin()[0], image2->origin()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->origin()[1], image2->origin()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->origin()[2], image2->origin()[2], EPSILON);

    image2->set_window_center(image->window_center());
    image2->set_window_width(image->window_width());
    image2->set_spacing(image->spacing());
    image2->set_origin(image->origin());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void data_converter_test::matrix_converter_test()
{
    data_converter::sptr converter = data_converter::get_instance();
    ::igtl::TransformMessage::Pointer converted_matrix;
    data::matrix4::sptr matrix;
    ::igtl::Matrix4x4 igtl_matrix;

    matrix = std::make_shared<data::matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*matrix)(i, j) = static_cast<double>(i + j);
        }
    }

    converted_matrix =
        ::igtl::TransformMessage::Pointer(
            dynamic_cast< ::igtl::TransformMessage*>(converter->from_fw_object(matrix).
                                                     GetPointer())
        );
    CPPUNIT_ASSERT(converted_matrix);

    converted_matrix->GetMatrix(igtl_matrix);
    for(int i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, matrix->begin() + i * 4LL));
    }

    data::object::sptr dest_obj =
        converter->from_igtl_message(::igtl::MessageBase::Pointer(converted_matrix.GetPointer()));
    data::matrix4::sptr matrix2 = std::dynamic_pointer_cast<data::matrix4>(dest_obj);
    for(int i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, matrix2->begin() + i * 4LL));
    }
}

//------------------------------------------------------------------------------

void data_converter_test::point_list_converter_test()
{
    const std::array points {
        std::array {0.1F, 1.4F, 2.3F},
        std::array {0.4F, 0.5F, 2.0F}
    };
    std::array<::igtl::PointElement::Pointer, 2> igtl_point_element;
    std::remove_const_t<decltype(points)> igtl_points {};
    data_converter::sptr converter = data_converter::get_instance();
    data::point_list::sptr point_list;
    ::igtl::PointMessage::Pointer msg;
    data::point::sptr point;

    point_list = std::make_shared<data::point_list>();
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        point = std::make_shared<data::point>();
        std::copy(points[i].begin(), points[i].end(), point->get_coord().begin());
        point_list->get_points().push_back(point);
    }

    msg =
        ::igtl::PointMessage::Pointer(
            dynamic_cast< ::igtl::PointMessage*>(converter->from_fw_object(
                                                     point_list
            ).GetPointer())
        );
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        msg->GetPointElement(int(i), igtl_point_element[i]);
        igtl_point_element[i]->GetPosition(igtl_points[i].data());
        CPPUNIT_ASSERT(std::equal(points[i].begin(), points[i].end(), igtl_points[i].begin()));
    }

    data::object::sptr dest_obj        = converter->from_igtl_message(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::point_list::sptr point_list2 = std::dynamic_pointer_cast<data::point_list>(dest_obj);
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        CPPUNIT_ASSERT(
            std::equal(
                point_list2->get_points()[i]->get_coord().begin(),
                point_list2->get_points()[i]->get_coord().end(),
                points[i].begin()
            )
        );
    }
}

//------------------------------------------------------------------------------

void data_converter_test::string_converter_test()
{
    std::string const& sample      = "Hello world";
    data_converter::sptr converter = data_converter::get_instance();
    data::string::sptr str;
    ::igtl::StringMessage::Pointer str_msg;

    str = std::make_shared<data::string>();
    str->set_value(sample);
    str_msg =
        ::igtl::StringMessage::Pointer(
            dynamic_cast< ::igtl::StringMessage*>(converter->from_fw_object(
                                                      str
            ).GetPointer())
        );
    CPPUNIT_ASSERT(std::string(str_msg->GetString()) == sample);

    data::object::sptr dest_obj = converter->from_igtl_message(::igtl::MessageBase::Pointer(str_msg.GetPointer()));
    data::string::sptr str2     = std::dynamic_pointer_cast<data::string>(dest_obj);
    CPPUNIT_ASSERT(str2->get_value() == sample);
}

//------------------------------------------------------------------------------

void data_converter_test::line_converter_test()
{
    std::array position {0.0F, 1.0F, 2.0F};
    // While we only use the first three elements of direction, igtl::PositionMessage::GetQuaternion requires a
    // four-element array.
    std::array<float, 4> direction {0.0F, 1.0F, 2.0F};

    data::point::sptr point;
    data::line::sptr line;
    data_converter::sptr converter = data_converter::get_instance();
    ::igtl::PositionMessage::Pointer line_msg;

    line = std::make_shared<data::line>();
    line->set_position(std::make_shared<data::point>());
    line->set_direction(std::make_shared<data::point>());
    std::copy(direction.begin(), direction.begin() + 3, line->get_direction()->get_coord().begin());
    std::copy(position.begin(), position.end(), line->get_position()->get_coord().begin());
    line_msg =
        ::igtl::PositionMessage::Pointer(
            dynamic_cast< ::igtl::PositionMessage*>(converter->from_fw_object(
                                                        line
            ).GetPointer())
        );
    line_msg->GetPosition(position.data());
    line_msg->GetQuaternion(direction.data());
    CPPUNIT_ASSERT(
        std::equal(
            line->get_position()->get_coord().begin(),
            line->get_position()->get_coord().end(),
            position.begin()
        )
    );
    CPPUNIT_ASSERT(
        std::equal(
            line->get_direction()->get_coord().begin(),
            line->get_direction()->get_coord().end(),
            direction.begin()
        )
    );

    data::object::sptr dest_obj = converter->from_igtl_message(::igtl::MessageBase::Pointer(line_msg.GetPointer()));
    data::line::sptr line2      = std::dynamic_pointer_cast<data::line>(dest_obj);
    CPPUNIT_ASSERT(
        std::equal(
            line2->get_position()->get_coord().begin(),
            line2->get_position()->get_coord().end(),
            position.begin()
        )
    );
    CPPUNIT_ASSERT(
        std::equal(
            line2->get_direction()->get_coord().begin(),
            line2->get_direction()->get_coord().end(),
            direction.begin()
        )
    );
}

//------------------------------------------------------------------------------

void data_converter_test::scalar_converter_test()
{
    data_converter::sptr converter = data_converter::get_instance();
    raw_message::Pointer msg;
    data::object::sptr dest_obj;

    const int value_int              = 10;
    data::integer::sptr data_integer = std::make_shared<data::integer>(value_int);

    msg = raw_message::Pointer(dynamic_cast<raw_message*>(converter->from_fw_object(data_integer).GetPointer()));
    CPPUNIT_ASSERT(msg->get_message().size() == sizeof(int));

    dest_obj = converter->from_igtl_message(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::integer::sptr new_data_integer = std::dynamic_pointer_cast<data::integer>(dest_obj);
    CPPUNIT_ASSERT(new_data_integer->get_value() == value_int);

    const float value_float     = 16.64F;
    data::real::sptr data_float = std::make_shared<data::real>(value_float);

    msg = raw_message::Pointer(dynamic_cast<raw_message*>(converter->from_fw_object(data_float).GetPointer()));
    CPPUNIT_ASSERT(msg->get_message().size() == sizeof(float));

    dest_obj = converter->from_igtl_message(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::real::sptr new_data_float = std::dynamic_pointer_cast<data::real>(dest_obj);
    CPPUNIT_ASSERT(new_data_float->get_value() == value_float);
}

//------------------------------------------------------------------------------

void data_converter_test::map_converter_test()
{
    //FIXME : there is 3 converter that can convert a data::map (aka MapConverter, TrackingStartConverter
    ///and TrackingStopConverter). To avoid asserts MapConverter should be called explicitly.
    io::igtl::detail::converter::map_converter::sptr converter =
        io::igtl::detail::converter::map_converter::New();

    ::igtl::TrackingDataMessage::Pointer tracking_msg;

    data::matrix4::sptr matrix = std::make_shared<data::matrix4>();
    data::map::sptr map        = std::make_shared<data::map>();
    (*map)["H_marker1_2_polaris"] = matrix;

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*matrix)(i, j) = static_cast<double>(i + j);
        }
    }

    tracking_msg =
        ::igtl::TrackingDataMessage::Pointer(
            dynamic_cast< ::igtl::TrackingDataMessage*>(converter->from_fw_data_object(
                                                            map
            ).
                                                        GetPointer())
        );
    CPPUNIT_ASSERT(tracking_msg);
    const int nb_tracking_element = tracking_msg->GetNumberOfTrackingDataElements();
    CPPUNIT_ASSERT_EQUAL(1, nb_tracking_element);

    ::igtl::TrackingDataElement::Pointer track_element = ::igtl::TrackingDataElement::New();
    tracking_msg->GetTrackingDataElement(0, track_element);
    const std::string name = track_element->GetName();

    CPPUNIT_ASSERT_EQUAL(std::string("H_marker1_2_polaris"), name);

    ::igtl::Matrix4x4 igtl_matrix;
    track_element->GetMatrix(igtl_matrix);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, matrix->begin() + i * 4));
    }

    data::object::sptr dest_object =
        converter->from_igtl_message(::igtl::MessageBase::Pointer(tracking_msg.GetPointer()));
    data::map::sptr dest_map = std::dynamic_pointer_cast<data::map>(dest_object);

    auto iter = dest_map->find("H_marker1_2_polaris");
    CPPUNIT_ASSERT(iter != dest_map->end());

    data::matrix4::sptr dest_matrix = std::dynamic_pointer_cast<data::matrix4>(iter->second);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, dest_matrix->begin() + i * 4));
    }
}

} // namespace sight::io::igtl::detail::ut
