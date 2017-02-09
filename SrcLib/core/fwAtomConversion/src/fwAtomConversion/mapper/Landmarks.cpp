/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/mapper/Landmarks.hpp"

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/registry/macros.hpp"

#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwData/Landmarks.hpp>

#include <fwTools/UUID.hpp>

#include <boost/algorithm/string.hpp>

namespace fwAtomConversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( ::fwAtomConversion::mapper::Landmarks, ::fwData::Landmarks);

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr Landmarks::convert( ::fwData::Object::sptr object,
                                            DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    ::fwAtomConversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    ::fwAtoms::Object::sptr atom = visitor.getAtomObject();

    ::fwData::Landmarks::csptr landmarks = ::fwData::Landmarks::dynamicCast(object);

    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();

    ::fwData::Landmarks::GroupNameContainer names = landmarks->getGroupNames();
    for (const auto& name: names)
    {
        const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(name);
        ::fwAtoms::Object::sptr atomGroup = ::fwAtoms::Object::New();
        atomGroup->setMetaInfo("ID_METAINFO", ::fwTools::UUID::generateUUID());

        const std::string colorStr = std::to_string(group.m_color[0]) + ";" +
                                     std::to_string(group.m_color[1]) + ";" +
                                     std::to_string(group.m_color[2]) + ";" +
                                     std::to_string(group.m_color[3]);
        atomGroup->setAttribute("color", ::fwAtoms::String::New(colorStr));
        atomGroup->setAttribute("size", ::fwAtoms::Numeric::New(group.m_size));
        const std::string shapeStr = (group.m_shape == ::fwData::Landmarks::Shape::SPHERE) ? "SPHERE" : "CUBE";
        atomGroup->setAttribute("shape", ::fwAtoms::String::New(shapeStr));
        atomGroup->setAttribute("visibility", ::fwAtoms::Boolean::New(group.m_visibility));

        ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();

        for (const auto& point : group.m_points)
        {
            const std::string pointStr = std::to_string(point[0]) + ";" +
                                         std::to_string(point[1]) + ";" +
                                         std::to_string(point[2]);
            seq->push_back(::fwAtoms::String::New(pointStr));
        }
        atomGroup->setAttribute("points", seq);
        map->insert(name, atomGroup);
    }
    atom->setAttribute("landmarks", map );

    return atom;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Landmarks::convert(  ::fwAtoms::Object::sptr atom,
                                            AtomVisitor::DataCacheType& cache,
                                            const AtomVisitor::IReadPolicy& uuidPolicy
                                            )
{
    ::fwAtomConversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    ::fwData::Object::sptr data         = visitor.getDataObject();
    ::fwData::Landmarks::sptr landmarks = ::fwData::Landmarks::dynamicCast(data);

    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::dynamicCast(atom->getAttribute("landmarks"));

    for (const auto& elt : map->getValue())
    {
        const std::string name = elt.first;
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atoms stored in fwAtom::Map 'landmarks' must be atom objects"),
                               elt.second->type() != ::fwAtoms::Base::OBJECT );
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast(elt.second);

        // get color
        ::fwAtoms::String::csptr colorObj = ::fwAtoms::String::dynamicCast(obj->getAttribute("color"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'color' stored in fwAtom::Object 'landmarks' must be ::fwAtoms::String"),
                               !colorObj );

        const std::string& colorStr = colorObj->getValue();

        std::vector< std::string> result;
        ::boost::split(result, colorStr, ::boost::is_any_of(";"));

        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged("'color' atom must be of type rgba"),
                               result.size() != 4 );
        const ::fwData::Landmarks::ColorType color = {{
                                                          std::stof(result[0]), std::stof(result[1]),
                                                          std::stof(result[2]), std::stof(result[3])
                                                      }};

        // get size
        ::fwAtoms::Numeric::csptr sizeObj = ::fwAtoms::Numeric::dynamicCast(obj->getAttribute("size"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'size' stored in fwAtom::Object 'landmarks' must be ::fwAtoms::Numeric"),
                               !sizeObj );
        const ::fwData::Landmarks::SizeType size = sizeObj->getValue< ::fwData::Landmarks::SizeType >();

        // get shape
        ::fwAtoms::String::csptr shapeObj = ::fwAtoms::String::dynamicCast(obj->getAttribute("shape"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'shape' stored in fwAtom::Object 'landmarks' must be ::fwAtoms::String"),
                               !shapeObj );

        const std::string& shapeStr = shapeObj->getValue();
        ::fwData::Landmarks::Shape shape;
        if (shapeStr == "SPHERE")
        {
            shape = ::fwData::Landmarks::Shape::SPHERE;
        }
        else if (shapeStr == "CUBE")
        {
            shape = ::fwData::Landmarks::Shape::CUBE;
        }
        else
        {
            FW_RAISE_EXCEPTION(exception::ConversionNotManaged("'shape' value '"+ shapeStr +"' is not managed"));
        }

        // get visibility
        ::fwAtoms::Boolean::csptr visuObj = ::fwAtoms::Boolean::dynamicCast(obj->getAttribute("visibility"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'visibility' stored in 'landmarks' must be ::fwAtoms::Boolean"),
                               !visuObj );
        const bool visibility = visuObj->getValue();

        landmarks->addGroup(name, color, size, shape, visibility);

        // get points
        ::fwAtoms::Sequence::csptr seq = ::fwAtoms::Sequence::dynamicCast(obj->getAttribute("points"));
        for (const auto& elt : seq->getValue())
        {
            FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                       "sub atoms stored in 'points' must be ::fwAtoms::String"),
                                   elt->type() != ::fwAtoms::Base::STRING );

            ::fwAtoms::String::csptr ptStrObj = ::fwAtoms::String::dynamicCast(elt);
            const std::string& ptStr = ptStrObj->getValue();

            std::vector< std::string> resultPt;
            ::boost::split(resultPt, ptStr, ::boost::is_any_of(";"));

            FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged("point atom must be of type x;y;z"),
                                   resultPt.size() != 3 );

            ::fwData::Landmarks::PointType pt = {{
                                                     std::stof(resultPt[0]), std::stof(resultPt[1]),
                                                     std::stof(resultPt[2])
                                                 }};
            landmarks->addPoint(name, pt);
        }
    }

    return landmarks;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace fwAtomConversion
