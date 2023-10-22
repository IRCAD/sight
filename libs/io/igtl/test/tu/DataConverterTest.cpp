/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "DataConverterTest.hpp"

#include <core/type.hpp>

#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/line.hpp>
#include <data/matrix4.hpp>
#include <data/mesh.hpp>
#include <data/object.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <io/igtl/detail/converter/CompositeConverter.hpp>
#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/RawMessage.hpp>

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

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::igtl::detail::ut::DataConverterTest);

namespace sight::io::igtl::detail::ut
{

static const double epsilon = 0.00001;

//------------------------------------------------------------------------------

void DataConverterTest::setUp()
{
}

//------------------------------------------------------------------------------

void DataConverterTest::tearDown()
{
}

//------------------------------------------------------------------------------

void DataConverterTest::meshConverterTest()
{
    DataConverter::sptr converter = DataConverter::getInstance();
    data::mesh::sptr mesh         = std::make_shared<data::mesh>();
    const auto lock               = mesh->dump_lock();
    utest_data::generator::mesh::generateMesh(mesh);

    ::igtl::MessageBase::Pointer msg = converter->fromFwObject(mesh);

    CPPUNIT_ASSERT_MESSAGE("Message is null", msg);

    data::object::sptr obj = converter->fromIgtlMessage(msg);

    data::mesh::sptr mesh2 = std::dynamic_pointer_cast<data::mesh>(obj);
    CPPUNIT_ASSERT_MESSAGE("Mesh is null", mesh2);
    const auto lock2 = mesh2->dump_lock();

    CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), mesh2->numPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->numCells(), mesh2->numCells());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellSize(), mesh2->getCellSize());
    CPPUNIT_ASSERT_EQUAL(mesh->getDataSizeInBytes(), mesh2->getDataSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::Attributes::POINT_COLORS>(),
        mesh2->has<data::mesh::Attributes::POINT_COLORS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::Attributes::CELL_COLORS>(),
        mesh2->has<data::mesh::Attributes::CELL_COLORS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::Attributes::POINT_NORMALS>(),
        mesh2->has<data::mesh::Attributes::POINT_NORMALS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::Attributes::CELL_NORMALS>(),
        mesh2->has<data::mesh::Attributes::CELL_NORMALS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::Attributes::POINT_TEX_COORDS>(),
        mesh2->has<data::mesh::Attributes::POINT_TEX_COORDS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::mesh::Attributes::CELL_TEX_COORDS>(),
        mesh2->has<data::mesh::Attributes::CELL_TEX_COORDS>()
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

void DataConverterTest::imageConverterTest()
{
    DataConverter::sptr converter = DataConverter::getInstance();
    data::image::sptr image       = std::make_shared<data::image>();

    core::type type = core::type::INT32;
    utest_data::generator::image::generateRandomImage(image, type);

    ::igtl::MessageBase::Pointer msg = converter->fromFwObject(image);

    CPPUNIT_ASSERT_MESSAGE("Message is null", msg);

    data::object::sptr obj = converter->fromIgtlMessage(msg);

    data::image::sptr image2 = std::dynamic_pointer_cast<data::image>(obj);

    CPPUNIT_ASSERT_MESSAGE("Image is null", image2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[0], image2->getSpacing()[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[1], image2->getSpacing()[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[2], image2->getSpacing()[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getOrigin()[0], image2->getOrigin()[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getOrigin()[1], image2->getOrigin()[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getOrigin()[2], image2->getOrigin()[2], epsilon);

    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());
    image2->setSpacing(image->getSpacing());
    image2->setOrigin(image->getOrigin());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void DataConverterTest::matrixConverterTest()
{
    DataConverter::sptr converter = DataConverter::getInstance();
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
            dynamic_cast< ::igtl::TransformMessage*>(converter->fromFwObject(matrix).
                                                     GetPointer())
        );
    CPPUNIT_ASSERT(converted_matrix);

    converted_matrix->GetMatrix(igtl_matrix);
    for(int i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, matrix->begin() + i * 4LL));
    }

    data::object::sptr dest_obj =
        converter->fromIgtlMessage(::igtl::MessageBase::Pointer(converted_matrix.GetPointer()));
    data::matrix4::sptr matrix2 = std::dynamic_pointer_cast<data::matrix4>(dest_obj);
    for(int i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, matrix2->begin() + i * 4LL));
    }
}

//------------------------------------------------------------------------------

void DataConverterTest::pointListConverterTest()
{
    const std::array points {
        std::array {0.1F, 1.4F, 2.3F},
        std::array {0.4F, 0.5F, 2.0F}
    };
    std::array<::igtl::PointElement::Pointer, 2> igtl_point_element;
    std::remove_const_t<decltype(points)> igtl_points {};
    DataConverter::sptr converter = DataConverter::getInstance();
    data::point_list::sptr point_list;
    ::igtl::PointMessage::Pointer msg;
    data::point::sptr point;

    point_list = std::make_shared<data::point_list>();
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        point = std::make_shared<data::point>();
        std::copy(points[i].begin(), points[i].end(), point->getCoord().begin());
        point_list->getPoints().push_back(point);
    }

    msg =
        ::igtl::PointMessage::Pointer(
            dynamic_cast< ::igtl::PointMessage*>(converter->fromFwObject(
                                                     point_list
            ).GetPointer())
        );
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        msg->GetPointElement(int(i), igtl_point_element[i]);
        igtl_point_element[i]->GetPosition(igtl_points[i].data());
        CPPUNIT_ASSERT(std::equal(points[i].begin(), points[i].end(), igtl_points[i].begin()));
    }

    data::object::sptr dest_obj        = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::point_list::sptr point_list2 = std::dynamic_pointer_cast<data::point_list>(dest_obj);
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        CPPUNIT_ASSERT(
            std::equal(
                point_list2->getPoints()[i]->getCoord().begin(),
                point_list2->getPoints()[i]->getCoord().end(),
                points[i].begin()
            )
        );
    }
}

//------------------------------------------------------------------------------

void DataConverterTest::stringConverterTest()
{
    std::string const& sample     = "Hello world";
    DataConverter::sptr converter = DataConverter::getInstance();
    data::string::sptr str;
    ::igtl::StringMessage::Pointer str_msg;

    str = std::make_shared<data::string>();
    str->setValue(sample);
    str_msg =
        ::igtl::StringMessage::Pointer(
            dynamic_cast< ::igtl::StringMessage*>(converter->fromFwObject(
                                                      str
            ).GetPointer())
        );
    CPPUNIT_ASSERT(std::string(str_msg->GetString()) == sample);

    data::object::sptr dest_obj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(str_msg.GetPointer()));
    data::string::sptr str2     = std::dynamic_pointer_cast<data::string>(dest_obj);
    CPPUNIT_ASSERT(str2->getValue() == sample);
}

//------------------------------------------------------------------------------

void DataConverterTest::lineConverterTest()
{
    std::array position {0.0F, 1.0F, 2.0F};
    // While we only use the first three elements of direction, igtl::PositionMessage::GetQuaternion requires a
    // four-element array.
    std::array<float, 4> direction {0.0F, 1.0F, 2.0F};

    data::point::sptr point;
    data::line::sptr line;
    DataConverter::sptr converter = DataConverter::getInstance();
    ::igtl::PositionMessage::Pointer line_msg;

    line = std::make_shared<data::line>();
    line->setPosition(std::make_shared<data::point>());
    line->setDirection(std::make_shared<data::point>());
    std::copy(direction.begin(), direction.begin() + 3, line->getDirection()->getCoord().begin());
    std::copy(position.begin(), position.end(), line->getPosition()->getCoord().begin());
    line_msg =
        ::igtl::PositionMessage::Pointer(
            dynamic_cast< ::igtl::PositionMessage*>(converter->fromFwObject(
                                                        line
            ).GetPointer())
        );
    line_msg->GetPosition(position.data());
    line_msg->GetQuaternion(direction.data());
    CPPUNIT_ASSERT(
        std::equal(
            line->getPosition()->getCoord().begin(),
            line->getPosition()->getCoord().end(),
            position.begin()
        )
    );
    CPPUNIT_ASSERT(
        std::equal(
            line->getDirection()->getCoord().begin(),
            line->getDirection()->getCoord().end(),
            direction.begin()
        )
    );

    data::object::sptr dest_obj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(line_msg.GetPointer()));
    data::line::sptr line2      = std::dynamic_pointer_cast<data::line>(dest_obj);
    CPPUNIT_ASSERT(
        std::equal(
            line2->getPosition()->getCoord().begin(),
            line2->getPosition()->getCoord().end(),
            position.begin()
        )
    );
    CPPUNIT_ASSERT(
        std::equal(
            line2->getDirection()->getCoord().begin(),
            line2->getDirection()->getCoord().end(),
            direction.begin()
        )
    );
}

//------------------------------------------------------------------------------

void DataConverterTest::scalarConverterTest()
{
    DataConverter::sptr converter = DataConverter::getInstance();
    RawMessage::Pointer msg;
    data::object::sptr dest_obj;

    const int value_int              = 10;
    data::integer::sptr data_integer = std::make_shared<data::integer>(value_int);

    msg = RawMessage::Pointer(dynamic_cast<RawMessage*>(converter->fromFwObject(data_integer).GetPointer()));
    CPPUNIT_ASSERT(msg->getMessage().size() == sizeof(int));

    dest_obj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::integer::sptr new_data_integer = std::dynamic_pointer_cast<data::integer>(dest_obj);
    CPPUNIT_ASSERT(new_data_integer->getValue() == value_int);

    const float value_float     = 16.64F;
    data::real::sptr data_float = std::make_shared<data::real>(value_float);

    msg = RawMessage::Pointer(dynamic_cast<RawMessage*>(converter->fromFwObject(data_float).GetPointer()));
    CPPUNIT_ASSERT(msg->getMessage().size() == sizeof(float));

    dest_obj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::real::sptr new_data_float = std::dynamic_pointer_cast<data::real>(dest_obj);
    CPPUNIT_ASSERT(new_data_float->getValue() == value_float);
}

//------------------------------------------------------------------------------

void DataConverterTest::compositeConverterTest()
{
    //FIXME : there is 3 converter that can convert a data::composite (aka CompositeConverter, TrackingStartConverter
    ///and TrackingStopConverter). To avoid asserts CompositeConverter should be called explicitly.
    io::igtl::detail::converter::CompositeConverter::sptr converter =
        io::igtl::detail::converter::CompositeConverter::New();

    ::igtl::TrackingDataMessage::Pointer tracking_msg;

    data::matrix4::sptr matrix      = std::make_shared<data::matrix4>();
    data::composite::sptr composite = std::make_shared<data::composite>();
    (*composite)["H_marker1_2_polaris"] = matrix;

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*matrix)(i, j) = static_cast<double>(i + j);
        }
    }

    tracking_msg =
        ::igtl::TrackingDataMessage::Pointer(
            dynamic_cast< ::igtl::TrackingDataMessage*>(converter->fromFwDataObject(
                                                            composite
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
        converter->fromIgtlMessage(::igtl::MessageBase::Pointer(tracking_msg.GetPointer()));
    data::composite::sptr dest_composite = std::dynamic_pointer_cast<data::composite>(dest_object);

    auto iter = dest_composite->find("H_marker1_2_polaris");
    CPPUNIT_ASSERT(iter != dest_composite->end());

    data::matrix4::sptr dest_matrix = std::dynamic_pointer_cast<data::matrix4>(iter->second);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtl_matrix[i], igtl_matrix[i] + 4, dest_matrix->begin() + i * 4));
    }
}

} // namespace sight::io::igtl::detail::ut
