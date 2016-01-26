/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__
#define  __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"

namespace fwAtoms
{
class Object;
}
namespace fwData
{
class Object;
}

namespace fwAtomConversion
{

/**
 * @brief This visitor visits data object to fill it with associated atom object attributes.
 * This class is used by AtomVisitor to convert an fwData::Object.
 * @class AtomToDataMappingVisitor
 * @date 2013
 */
class FWATOMCONVERSION_CLASS_API AtomToDataMappingVisitor : public ::camp::ExtendedClassVisitor
{

public:

    /// Constructor. Initializes visitor.
    FWATOMCONVERSION_API AtomToDataMappingVisitor(
        SPTR(::fwData::Object)dataObj,
        SPTR(::fwAtoms::Object)atomObj,
        AtomVisitor::DataCacheType & cache,
        const AtomVisitor::IReadPolicy &uuidPolicy
        );

    /// Destructor. Does nothing.
    FWATOMCONVERSION_API virtual ~AtomToDataMappingVisitor();

    /// Visits data object simple property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);

    /// Visits data object enum property and fill it with associated atom attribute.
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);

    /**
     * @brief Visits data object user property and fill it with associated atom attribute.
     * Manages null fwAtoms::Base::sptr by inserting a null fwData::Object::sptr.
     */
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);

    /**
     * @brief Visits data object array property and fill it with associated atom attribute.
     * Manages null fwAtoms::Base::sptr by inserting a null fwData::Object::sptr.
     *
     * Only array that contains ::fwAtoms::Base::BOOLEAN, ::fwAtoms::Base::NUMERIC, ::fwAtoms::Base::STRING and
     * ::fwAtoms::Base::OBJECT are managed.
     */
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);

    /**
     * @brief Visits data object map property and fill it with associated atom attribute.
     * Manages null fwAtoms::Base::sptr by inserting a null fwData::Object::sptr.
     *
     * Only map that contains ::fwAtoms::Base::BOOLEAN, ::fwAtoms::Base::NUMERIC, ::fwAtoms::Base::STRING and
     * ::fwAtoms::Base::OBJECT are managed.
     */
    FWATOMCONVERSION_API void visit(const camp::MapProperty& property);

private:

    /// Converted data object
    SPTR(::fwData::Object) m_dataObj;

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// Atom object to convert
    SPTR(::fwAtoms::Object) m_atomObj;

    /// Cache to register the atoms already converted, used when an atom is referenced multiple times.
    AtomVisitor::DataCacheType & m_cache;

    /// Atom visitor uuids policy
    const AtomVisitor::IReadPolicy & m_uuidPolicy;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_ATOMTODATAMAPPINGVISITOR_HPP__
