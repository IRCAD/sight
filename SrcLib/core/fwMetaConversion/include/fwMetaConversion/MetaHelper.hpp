/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETACONVERSION_METAHELPER_HPP__
#define __FWMETACONVERSION_METAHELPER_HPP__

#include <fwData/Object.hpp>
#include <fwMetaData/Object.hpp>

#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{

/**
 * @brief Class helper to convert fwData to metaData and metaData to fwData
 */
class FWMETACONVERSION_CLASS_API MetaHelper
{
public:
    /**
     * Helper could not be instantiated
     */
    FWMETACONVERSION_API MetaHelper();
    FWMETACONVERSION_API virtual ~MetaHelper();
    typedef std::map<std::string, ::fwData::Object::sptr> Cache;
    typedef std::map<std::string, ::fwMetaData::Object::sptr> MetaCache;

    /**
     * @brief Convert a fwData to a MetaData and clear all caches
     * @param a fwData::Object sptr
     * @return a MetaObject sptr
     */
    FWMETACONVERSION_API ::fwMetaData::Object::sptr fromFwData(::fwData::Object::sptr object);

    /**
     * @brief Convert a MetaData to a fwData and clear all caches
     * @param object the fwData::Object
     * @return a fwData::Object sptr.
     */
    FWMETACONVERSION_API ::fwData::Object::sptr toFwData(::fwMetaData::Object::sptr object);


    /**
     * @brief Convert a fwData to a MetaData
     * @param a fwData::Object sptr
     * @return a MetaObject sptr
     */
    FWMETACONVERSION_API ::fwMetaData::Object::sptr dataToMeta(::fwData::Object::sptr object);

    /**
     * @brief Convert a MetaData to a fwData
     * @param object the fwData::Object
     * @return a fwData::Object sptr.
     */
    FWMETACONVERSION_API ::fwData::Object::sptr metaToData(::fwMetaData::Object::sptr object);

    /**
     * Clean the object cache.
     */
    FWMETACONVERSION_API  void clearCache();

    /**
     * Clean the metaobject cache.
     */
    FWMETACONVERSION_API  void clearMetaCache();


    /**
     * return the number of processed object
     */
    FWMETACONVERSION_API unsigned int getNbProcessObject();

private:
    ///Cache of fwData
    Cache m_cache;

    ///Cache of metaData
    MetaCache m_metaCache;

    unsigned int m_nbProcessObject;
};

} // namespace fwMetaConversion
#endif /* __FWMETACONVERSION_METAHELPER_HPP__ */
