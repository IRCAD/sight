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

#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Line.hpp>
#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>
#include <data/Object.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

#include <io/igtl/detail/converter/CompositeConverter.hpp>
#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/RawMessage.hpp>

#include <utestData/generator/Image.hpp>
#include <utestData/generator/Mesh.hpp>

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
    data::Mesh::sptr mesh         = std::make_shared<data::Mesh>();
    const auto lock               = mesh->dump_lock();
    utestData::generator::Mesh::generateMesh(mesh);

    ::igtl::MessageBase::Pointer msg = converter->fromFwObject(mesh);

    CPPUNIT_ASSERT_MESSAGE("Message is null", msg);

    data::Object::sptr obj = converter->fromIgtlMessage(msg);

    data::Mesh::sptr mesh2 = std::dynamic_pointer_cast<data::Mesh>(obj);
    CPPUNIT_ASSERT_MESSAGE("Mesh is null", mesh2);
    const auto lock2 = mesh2->dump_lock();

    CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), mesh2->numPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->numCells(), mesh2->numCells());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellSize(), mesh2->getCellSize());
    CPPUNIT_ASSERT_EQUAL(mesh->getDataSizeInBytes(), mesh2->getDataSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::Mesh::Attributes::POINT_COLORS>(),
        mesh2->has<data::Mesh::Attributes::POINT_COLORS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::Mesh::Attributes::CELL_COLORS>(),
        mesh2->has<data::Mesh::Attributes::CELL_COLORS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::Mesh::Attributes::POINT_NORMALS>(),
        mesh2->has<data::Mesh::Attributes::POINT_NORMALS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::Mesh::Attributes::CELL_NORMALS>(),
        mesh2->has<data::Mesh::Attributes::CELL_NORMALS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>(),
        mesh2->has<data::Mesh::Attributes::POINT_TEX_COORDS>()
    );
    CPPUNIT_ASSERT_EQUAL(
        mesh->has<data::Mesh::Attributes::CELL_TEX_COORDS>(),
        mesh2->has<data::Mesh::Attributes::CELL_TEX_COORDS>()
    );

    const auto dumpLock = mesh->dump_lock();
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

    const auto cellRange1 = mesh->czip_range<cell::triangle, cell::rgba, cell::nxyz>();
    const auto cellRange2 = mesh2->czip_range<cell::triangle, cell::rgba, cell::nxyz>();

    for(const auto& [orig, cur] : boost::combine(cellRange1, cellRange2))
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
    data::Image::sptr image       = std::make_shared<data::Image>();

    core::type type = core::type::INT32;
    utestData::generator::Image::generateRandomImage(image, type);

    ::igtl::MessageBase::Pointer msg = converter->fromFwObject(image);

    CPPUNIT_ASSERT_MESSAGE("Message is null", msg);

    data::Object::sptr obj = converter->fromIgtlMessage(msg);

    data::Image::sptr image2 = std::dynamic_pointer_cast<data::Image>(obj);

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
    ::igtl::TransformMessage::Pointer convertedMatrix;
    data::Matrix4::sptr matrix;
    ::igtl::Matrix4x4 igtlMatrix;

    matrix = std::make_shared<data::Matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*matrix)(i, j) = static_cast<double>(i + j);
        }
    }

    convertedMatrix =
        ::igtl::TransformMessage::Pointer(
            dynamic_cast< ::igtl::TransformMessage*>(converter->fromFwObject(matrix).
                                                     GetPointer())
        );
    CPPUNIT_ASSERT(convertedMatrix);

    convertedMatrix->GetMatrix(igtlMatrix);
    for(int i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtlMatrix[i], igtlMatrix[i] + 4, matrix->begin() + i * 4LL));
    }

    data::Object::sptr destObj =
        converter->fromIgtlMessage(::igtl::MessageBase::Pointer(convertedMatrix.GetPointer()));
    data::Matrix4::sptr matrix2 = std::dynamic_pointer_cast<data::Matrix4>(destObj);
    for(int i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtlMatrix[i], igtlMatrix[i] + 4, matrix2->begin() + i * 4LL));
    }
}

//------------------------------------------------------------------------------

void DataConverterTest::pointListConverterTest()
{
    const std::array points {
        std::array {0.1F, 1.4F, 2.3F},
        std::array {0.4F, 0.5F, 2.0F}
    };
    std::array<::igtl::PointElement::Pointer, 2> igtlPointElement;
    std::remove_const_t<decltype(points)> igtlPoints {};
    DataConverter::sptr converter = DataConverter::getInstance();
    data::PointList::sptr pointList;
    ::igtl::PointMessage::Pointer msg;
    data::Point::sptr point;

    pointList = std::make_shared<data::PointList>();
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        point = std::make_shared<data::Point>();
        std::copy(points[i].begin(), points[i].end(), point->getCoord().begin());
        pointList->getPoints().push_back(point);
    }

    msg =
        ::igtl::PointMessage::Pointer(
            dynamic_cast< ::igtl::PointMessage*>(converter->fromFwObject(
                                                     pointList
            ).GetPointer())
        );
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        msg->GetPointElement(int(i), igtlPointElement[i]);
        igtlPointElement[i]->GetPosition(igtlPoints[i].data());
        CPPUNIT_ASSERT(std::equal(points[i].begin(), points[i].end(), igtlPoints[i].begin()));
    }

    data::Object::sptr destObj       = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::PointList::sptr pointList2 = std::dynamic_pointer_cast<data::PointList>(destObj);
    for(std::size_t i = 0 ; i < 2 ; ++i)
    {
        CPPUNIT_ASSERT(
            std::equal(
                pointList2->getPoints()[i]->getCoord().begin(),
                pointList2->getPoints()[i]->getCoord().end(),
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
    data::String::sptr str;
    ::igtl::StringMessage::Pointer strMsg;

    str = std::make_shared<data::String>();
    str->setValue(sample);
    strMsg =
        ::igtl::StringMessage::Pointer(
            dynamic_cast< ::igtl::StringMessage*>(converter->fromFwObject(
                                                      str
            ).GetPointer())
        );
    CPPUNIT_ASSERT(std::string(strMsg->GetString()) == sample);

    data::Object::sptr destObj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(strMsg.GetPointer()));
    data::String::sptr str2    = std::dynamic_pointer_cast<data::String>(destObj);
    CPPUNIT_ASSERT(str2->getValue() == sample);
}

//------------------------------------------------------------------------------

void DataConverterTest::lineConverterTest()
{
    std::array position {0.0F, 1.0F, 2.0F};
    // While we only use the first three elements of direction, igtl::PositionMessage::GetQuaternion requires a
    // four-element array.
    std::array<float, 4> direction {0.0F, 1.0F, 2.0F};

    data::Point::sptr point;
    data::Line::sptr line;
    DataConverter::sptr converter = DataConverter::getInstance();
    ::igtl::PositionMessage::Pointer lineMsg;

    line = std::make_shared<data::Line>();
    line->setPosition(std::make_shared<data::Point>());
    line->setDirection(std::make_shared<data::Point>());
    std::copy(direction.begin(), direction.begin() + 3, line->getDirection()->getCoord().begin());
    std::copy(position.begin(), position.end(), line->getPosition()->getCoord().begin());
    lineMsg =
        ::igtl::PositionMessage::Pointer(
            dynamic_cast< ::igtl::PositionMessage*>(converter->fromFwObject(
                                                        line
            ).GetPointer())
        );
    lineMsg->GetPosition(position.data());
    lineMsg->GetQuaternion(direction.data());
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

    data::Object::sptr destObj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(lineMsg.GetPointer()));
    data::Line::sptr line2     = std::dynamic_pointer_cast<data::Line>(destObj);
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
    data::Object::sptr destObj;

    const int valueInt              = 10;
    data::Integer::sptr dataInteger = std::make_shared<data::Integer>(valueInt);

    msg = RawMessage::Pointer(dynamic_cast<RawMessage*>(converter->fromFwObject(dataInteger).GetPointer()));
    CPPUNIT_ASSERT(msg->getMessage().size() == sizeof(int));

    destObj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::Integer::sptr newDataInteger = std::dynamic_pointer_cast<data::Integer>(destObj);
    CPPUNIT_ASSERT(newDataInteger->getValue() == valueInt);

    const float valueFloat      = 16.64F;
    data::Float::sptr dataFloat = std::make_shared<data::Float>(valueFloat);

    msg = RawMessage::Pointer(dynamic_cast<RawMessage*>(converter->fromFwObject(dataFloat).GetPointer()));
    CPPUNIT_ASSERT(msg->getMessage().size() == sizeof(float));

    destObj = converter->fromIgtlMessage(::igtl::MessageBase::Pointer(msg.GetPointer()));
    data::Float::sptr newDataFloat = std::dynamic_pointer_cast<data::Float>(destObj);
    CPPUNIT_ASSERT(newDataFloat->getValue() == valueFloat);
}

//------------------------------------------------------------------------------

void DataConverterTest::compositeConverterTest()
{
    //FIXME : there is 3 converter that can convert a data::Composite (aka CompositeConverter, TrackingStartConverter
    ///and TrackingStopConverter). To avoid asserts CompositeConverter should be called explicitly.
    io::igtl::detail::converter::CompositeConverter::sptr converter =
        io::igtl::detail::converter::CompositeConverter::New();

    ::igtl::TrackingDataMessage::Pointer trackingMsg;

    data::Matrix4::sptr matrix      = std::make_shared<data::Matrix4>();
    data::Composite::sptr composite = std::make_shared<data::Composite>();
    (*composite)["H_marker1_2_polaris"] = matrix;

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*matrix)(i, j) = static_cast<double>(i + j);
        }
    }

    trackingMsg =
        ::igtl::TrackingDataMessage::Pointer(
            dynamic_cast< ::igtl::TrackingDataMessage*>(converter->fromFwDataObject(
                                                            composite
            ).
                                                        GetPointer())
        );
    CPPUNIT_ASSERT(trackingMsg);
    const int nbTrackingElement = trackingMsg->GetNumberOfTrackingDataElements();
    CPPUNIT_ASSERT_EQUAL(1, nbTrackingElement);

    ::igtl::TrackingDataElement::Pointer trackElement = ::igtl::TrackingDataElement::New();
    trackingMsg->GetTrackingDataElement(0, trackElement);
    const std::string name = trackElement->GetName();

    CPPUNIT_ASSERT_EQUAL(std::string("H_marker1_2_polaris"), name);

    ::igtl::Matrix4x4 igtlMatrix;
    trackElement->GetMatrix(igtlMatrix);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtlMatrix[i], igtlMatrix[i] + 4, matrix->begin() + i * 4));
    }

    data::Object::sptr destObject =
        converter->fromIgtlMessage(::igtl::MessageBase::Pointer(trackingMsg.GetPointer()));
    data::Composite::sptr destComposite = std::dynamic_pointer_cast<data::Composite>(destObject);

    auto iter = destComposite->find("H_marker1_2_polaris");
    CPPUNIT_ASSERT(iter != destComposite->end());

    data::Matrix4::sptr destMatrix = std::dynamic_pointer_cast<data::Matrix4>(iter->second);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        CPPUNIT_ASSERT(std::equal(igtlMatrix[i], igtlMatrix[i] + 4, destMatrix->begin() + i * 4));
    }
}

} // namespace sight::io::igtl::detail::ut
