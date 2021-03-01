/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "atoms/conversion/mapper/Landmarks.hpp"

#include "atoms/conversion/convert.hpp"
#include "atoms/conversion/exception/ConversionNotManaged.hpp"
#include "atoms/conversion/mapper/registry/macros.hpp"

#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <data/Landmarks.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::atoms::conversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( atoms::conversion::mapper::Landmarks, ::sight::data::Landmarks);

//-----------------------------------------------------------------------------

atoms::Object::sptr Landmarks::convert( data::Object::sptr object,
                                        DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    atoms::conversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    atoms::Object::sptr atom = visitor.getAtomObject();

    data::Landmarks::csptr landmarks = data::Landmarks::dynamicCast(object);

    atoms::Map::sptr map = atoms::Map::New();

    data::Landmarks::GroupNameContainer names = landmarks->getGroupNames();
    for (const auto& name: names)
    {
        const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(name);
        atoms::Object::sptr atomGroup                = atoms::Object::New();
        atomGroup->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());

        const std::string colorStr = std::to_string(group.m_color[0]) + ";" +
                                     std::to_string(group.m_color[1]) + ";" +
                                     std::to_string(group.m_color[2]) + ";" +
                                     std::to_string(group.m_color[3]);
        atomGroup->setAttribute("color", atoms::String::New(colorStr));
        atomGroup->setAttribute("size", atoms::Numeric::New(group.m_size));
        const std::string shapeStr = (group.m_shape == data::Landmarks::Shape::SPHERE) ? "SPHERE" : "CUBE";
        atomGroup->setAttribute("shape", atoms::String::New(shapeStr));
        atomGroup->setAttribute("visibility", atoms::Boolean::New(group.m_visibility));

        atoms::Sequence::sptr seq = atoms::Sequence::New();

        for (const auto& point : group.m_points)
        {
            const std::string pointStr = std::to_string(point[0]) + ";" +
                                         std::to_string(point[1]) + ";" +
                                         std::to_string(point[2]);
            seq->push_back(atoms::String::New(pointStr));
        }
        atomGroup->setAttribute("points", seq);
        map->insert(name, atomGroup);
    }
    atom->setAttribute("landmarks", map );

    return atom;
}

//-----------------------------------------------------------------------------

data::Object::sptr Landmarks::convert(  atoms::Object::sptr atom,
                                        AtomVisitor::DataCacheType& cache,
                                        const AtomVisitor::IReadPolicy& uuidPolicy
                                        )
{
    atoms::conversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    data::Object::sptr data         = visitor.getDataObject();
    data::Landmarks::sptr landmarks = data::Landmarks::dynamicCast(data);

    atoms::Map::sptr map = atoms::Map::dynamicCast(atom->getAttribute("landmarks"));

    for (const auto& elt : map->getValue())
    {
        const std::string name = elt.first;
        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                      "sub atoms stored in fwAtom::Map 'landmarks' must be atom objects"),
                                  elt.second->type() != atoms::Base::OBJECT );
        atoms::Object::sptr obj = atoms::Object::dynamicCast(elt.second);

        // get color
        atoms::String::csptr colorObj = atoms::String::dynamicCast(obj->getAttribute("color"));
        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                      "sub atom 'color' stored in fwAtom::Object 'landmarks' must be atoms::String"),
                                  !colorObj );

        const std::string& colorStr = colorObj->getValue();

        std::vector< std::string> result;
        ::boost::split(result, colorStr, ::boost::is_any_of(";"));

        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged("'color' atom must be of type rgba"),
                                  result.size() != 4 );
        const data::Landmarks::ColorType color = {{
                                                      std::stof(result[0]), std::stof(result[1]),
                                                      std::stof(result[2]), std::stof(result[3])
                                                  }};

        // get size
        atoms::Numeric::csptr sizeObj = atoms::Numeric::dynamicCast(obj->getAttribute("size"));
        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                      "sub atom 'size' stored in fwAtom::Object 'landmarks' must be atoms::Numeric"),
                                  !sizeObj );
        const data::Landmarks::SizeType size = sizeObj->getValue< data::Landmarks::SizeType >();

        // get shape
        atoms::String::csptr shapeObj = atoms::String::dynamicCast(obj->getAttribute("shape"));
        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                      "sub atom 'shape' stored in fwAtom::Object 'landmarks' must be atoms::String"),
                                  !shapeObj );

        const std::string& shapeStr = shapeObj->getValue();
        data::Landmarks::Shape shape;
        if (shapeStr == "SPHERE")
        {
            shape = data::Landmarks::Shape::SPHERE;
        }
        else if (shapeStr == "CUBE")
        {
            shape = data::Landmarks::Shape::CUBE;
        }
        else
        {
            SIGHT_THROW_EXCEPTION(exception::ConversionNotManaged("'shape' value '"+ shapeStr +"' is not managed"));
        }

        // get visibility
        atoms::Boolean::csptr visuObj = atoms::Boolean::dynamicCast(obj->getAttribute("visibility"));
        SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                      "sub atom 'visibility' stored in 'landmarks' must be atoms::Boolean"),
                                  !visuObj );
        const bool visibility = visuObj->getValue();

        landmarks->addGroup(name, color, size, shape, visibility);

        // get points
        atoms::Sequence::csptr seq = atoms::Sequence::dynamicCast(obj->getAttribute("points"));
        for (const auto& elt : seq->getValue())
        {
            SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged(
                                          "sub atoms stored in 'points' must be atoms::String"),
                                      elt->type() != atoms::Base::STRING );

            atoms::String::csptr ptStrObj = atoms::String::dynamicCast(elt);
            const std::string& ptStr      = ptStrObj->getValue();

            std::vector< std::string> resultPt;
            ::boost::split(resultPt, ptStr, ::boost::is_any_of(";"));

            SIGHT_THROW_EXCEPTION_IF( exception::ConversionNotManaged("point atom must be of type x;y;z"),
                                      resultPt.size() != 3 );

            data::Landmarks::PointType pt = {{
                                                 std::stod(resultPt[0]), std::stod(resultPt[1]),
                                                 std::stod(resultPt[2])
                                             }};
            landmarks->addPoint(name, pt);
        }
    }

    return landmarks;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace sight::atoms::conversion
