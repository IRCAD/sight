/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
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

/**
 * @brief This visitor visits data object to fill it with associated atom object attributes.
 * @class AtomToDataMappingVisitor
 * @date 2013
 */
class FWATOMCONVERSION_CLASS_API AtomToDataMappingVisitor : public ::camp::ExtendedClassVisitor
{

public:

    /// Constructor. Initializes visitor.
    FWATOMCONVERSION_API AtomToDataMappingVisitor(
            ::fwData::Object::sptr dataObj,
            ::fwAtoms::Object::sptr atomObj,
            AtomVisitor::DataCacheType & cache );

    /// Destructor. Does nothing.
    FWATOMCONVERSION_API virtual ~AtomToDataMappingVisitor();

    /// Visits data object simple property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);

    /// Visits data object enum property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);

    /// Visits data object user property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);

    /// Visits data object array property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);

    /// Visits data object map property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::MapProperty& property);

private:

    /// Converted data object
    ::fwData::Object::sptr m_dataObj;

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// Atom object to convert
    ::fwAtoms::Object::sptr m_atomObj;

    /// Cache to register the atoms already converted, used when an atom is referenced multiple times.
    AtomVisitor::DataCacheType & m_cache;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__
