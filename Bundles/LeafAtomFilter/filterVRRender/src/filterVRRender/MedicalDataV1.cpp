/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/helper/Object.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include <fwAtomsFilter/registry/macros.hpp>

#include "filterVRRender/MedicalDataV1.hpp"

namespace filterVRRender
{

fwAtomsFilterRegisterMacro( ::filterVRRender::MedicalDataV1, "VRRenderMedicalDataV1" );

MedicalDataV1::MedicalDataV1(::fwAtomsFilter::IFilter::Key key)
{
}

MedicalDataV1::~MedicalDataV1()
{
}

void clearMapAttribute(const std::string& key, const ::fwAtoms::Map::sptr& values)
{
    if(values->find(key) != values->end())
    {
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast( (*values)[key] );
        SLM_ASSERT("Failed to retrieve key '" + key + "' from atom", obj);

        ::fwAtoms::Map::sptr objValues = obj->getAttribute< ::fwAtoms::Map >("values");
        SLM_ASSERT("Failed to retrieve 'values' attribute as ::fwAtoms::Map", objValues);
        objValues->clear();
    }
}

void MedicalDataV1::apply(const SPTR(::fwAtoms::Object)& atom)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Unable to filter atom : invalid object", atom);

    const std::string& classname = ::fwAtomsPatch::helper::getClassname(atom);
    FW_RAISE_IF("Unable to filter atom of class '" << classname << "'. Expected class is '::fwData::Composite'",
                classname != "::fwData::Composite");

    ::fwAtoms::Map::sptr values = atom->getAttribute< ::fwAtoms::Map >("values");
    SLM_ASSERT("Failed to retrieve 'values' attribute as ::fwAtoms::Map", values);

    clearMapAttribute("processingDB", values);
    clearMapAttribute("planningDB", values);
}

}

