/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMDSemanticPatch/V10/V09ALA/fwData/Image.hpp"

#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include <fwTools/UUID.hpp>

#include <boost/algorithm/string.hpp>

namespace fwMDSemanticPatch
{
namespace V10
{
namespace V09ALA
{
namespace fwData
{

Image::Image() :
    ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Image";
    m_originVersion   = "2";
    this->addContext("MedicalData", "V10", "V09ALA");
}

// ----------------------------------------------------------------------------

Image::~Image()
{
}

// ----------------------------------------------------------------------------

Image::Image( const Image& cpy ) :
    ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Image::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    ::fwAtoms::Map::csptr previousFieldMap = ::fwAtoms::Map::dynamicCast(previous->getAttribute("fields"));
    SLM_ASSERT("Image does not have field map", previousFieldMap);

    const auto& iter = previousFieldMap->find("m_landmarksId");
    if (iter != previousFieldMap->end())
    {
        // create new Landmarks structure
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();

        ::fwAtoms::Object::sptr currentPL = creators->create( "::fwData::PointList", "1");
        ::fwAtomsPatch::helper::Object helperPL( currentPL );

        ::fwAtoms::Map::sptr currentFieldMap = ::fwAtoms::Map::dynamicCast(current->getAttribute("fields"));
        currentFieldMap->insert("m_imageLandmarksId", currentPL);

        ::fwAtoms::Sequence::sptr plSeq = ::fwAtoms::Sequence::dynamicCast(currentPL->getAttribute("points"));

        // Convert previous Landmarks
        ::fwAtoms::Object::sptr previousLandmarks = ::fwAtoms::Object::dynamicCast(iter->second);

        ::fwAtoms::Map::sptr previousLandmarksMap =
            ::fwAtoms::Map::dynamicCast(previousLandmarks->getAttribute("landmarks"));

        for (const auto& elt : previousLandmarksMap->getValue())
        {
            ::fwAtoms::Object::csptr atomGroup = ::fwAtoms::Object::dynamicCast(elt.second);
            ::fwAtoms::Sequence::csptr points  = ::fwAtoms::Sequence::dynamicCast(atomGroup->getAttribute("points"));

            size_t count = 0;
            for (const auto& ptObj : points->getValue())
            {
                ::fwAtoms::String::csptr previousPt = ::fwAtoms::String::dynamicCast(ptObj);
                std::vector<std::string> res;
                const std::string coords = previousPt->getValue();
                ::boost::split(res, coords, ::boost::is_any_of(";"));

                ::fwAtoms::Object::sptr point = ::fwAtoms::Object::New();
                ::fwAtomsPatch::helper::setClassname(point, "::fwData::Point");
                ::fwAtomsPatch::helper::setVersion(point, "1");
                ::fwAtomsPatch::helper::generateID(point);

                plSeq->push_back(point);

                ::fwAtomsPatch::helper::Object helperPt( point );
                ::fwAtoms::Sequence::sptr pointCoord = ::fwAtoms::Sequence::New();
                helperPt.addAttribute("coord", pointCoord);
                pointCoord->push_back(::fwAtoms::Numeric::New(std::stod(res[0])));
                pointCoord->push_back(::fwAtoms::Numeric::New(std::stod(res[1])));
                pointCoord->push_back(::fwAtoms::Numeric::New(std::stod(res[2])));

                // set point label
                ::fwAtoms::Map::sptr pointFieldMap = ::fwAtoms::Map::New();
                helperPt.addAttribute("fields", pointFieldMap);

                const std::string label = elt.first + "_" + std::to_string(count);

                pointFieldMap->insert("m_labelId", ::fwAtoms::String::New(label));
                ++count;
            }
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V09ALA
} // namespace V10
} // namespace fwMDSemanticPatch

