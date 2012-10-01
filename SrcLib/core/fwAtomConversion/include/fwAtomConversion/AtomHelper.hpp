/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_METAHELPER_HPP__
#define __FWATOMCONVERSION_METAHELPER_HPP__

#include <fwData/Object.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

/**
 * @brief Class helper to convert fwData to metaData and metaData to fwData
 */
class FWATOMCONVERSION_CLASS_API AtomHelper
{
public:
    /**
     * Helper could not be instantiated
     */
    FWATOMCONVERSION_API AtomHelper();
    FWATOMCONVERSION_API virtual ~AtomHelper();
    typedef std::map<std::string, ::fwData::Object::sptr> Cache;
    typedef std::map<std::string, ::fwAtoms::Object::sptr> MetaCache;

    /**
     * @brief Convert a fwData to a MetaData and clear all caches
     * @param a fwData::Object sptr
     * @return a Atom::Object sptr
     */
    FWATOMCONVERSION_API ::fwAtoms::Object::sptr fromFwData(::fwData::Object::sptr object);

    /**
     * @brief Convert a MetaData to a fwData and clear all caches
     * @param object the fwData::Object
     * @return a fwData::Object sptr.
     */
    FWATOMCONVERSION_API ::fwData::Object::sptr toFwData(::fwAtoms::Object::sptr object);


    /**
     * @brief Convert a fwData to a MetaData
     * @param a fwData::Object sptr
     * @return a Atom::Object sptr
     */
    FWATOMCONVERSION_API ::fwAtoms::Object::sptr dataToMeta(::fwData::Object::sptr object);

    /**
     * @brief Convert a MetaData to a fwData
     * @param object the fwData::Object
     * @return a fwData::Object sptr.
     */
    FWATOMCONVERSION_API ::fwData::Object::sptr metaToData(::fwAtoms::Object::sptr object);

    /**
     * Clean the object cache.
     */
    FWATOMCONVERSION_API  void clearCache();

    /**
     * Clean the metaobject cache.
     */
    FWATOMCONVERSION_API  void clearMetaCache();


    /**
     * return the number of processed object
     */
    FWATOMCONVERSION_API unsigned int getNbProcessObject();

private:
    ///Cache of fwData
    Cache m_cache;

    ///Cache of metaData
    MetaCache m_metaCache;

    unsigned int m_nbProcessObject;
};

} // namespace fwAtomConversion
#endif /* __FWATOMCONVERSION_METAHELPER_HPP__ */
