/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__
#define  __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include <fwData/Object.hpp>

#include <fwAtoms/Object.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"

namespace fwAtomConversion
{

/// ???
class FWATOMCONVERSION_CLASS_API AtomToDataMappingVisitor : public ::camp::ExtendedClassVisitor
{

public:

    /// ???
    FWATOMCONVERSION_API AtomToDataMappingVisitor(
            ::fwData::Object::sptr dataObj,
            ::fwAtoms::Object::sptr atomObj,
            AtomVisitor::DataCacheType & cache );

    /// ???
    FWATOMCONVERSION_API virtual ~AtomToDataMappingVisitor();

    /// ???
    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::MapProperty& property);

    /// ???
    FWATOMCONVERSION_API ::fwAtoms::Object::sptr getAtomObject() const;

private:

    /// ???
    ::fwData::Object::sptr m_dataObj;

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// ???
    ::fwAtoms::Object::sptr m_atomObj;

    /// ???
    AtomVisitor::DataCacheType & m_cache;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__
