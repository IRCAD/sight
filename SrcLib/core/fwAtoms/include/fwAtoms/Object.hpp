/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMS_BASE_OBJECT_HPP__
#define  __FWATOMS_BASE_OBJECT_HPP__

#include <fwCamp/macros.hpp>

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"

fwCampAutoDeclareMacro((fwAtoms)(Object), FWATOMS_API);

namespace fwAtoms
{

/**
 * @brief Class represented a fwData::Object
 *
 */
class FWATOMS_CLASS_API Object : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Object)(::fwAtoms::Object), (()), new Object) ;
    typedef std::map<std::string, Base::sptr> Attributes;
    typedef std::map<std::string, std::string> MetaInfos;

    //--------------------------------------------------------------------------

    //! Set the internal attributs map
    FWATOMS_API void setAttributs(Attributes& attrs);

    //! add a atrtribut, old value is erased
    FWATOMS_API void addAttribut(const std::string& key, Base::sptr);

    //! Retrieve the internal map
    FWATOMS_API  Attributes& getAttributes() {return m_attrs;};

    //! Retrieve one Attribut
    FWATOMS_API  Base::sptr& getAttribut(const std::string& key) {return m_attrs[key];};

    //! Remove an attributs
    FWATOMS_API void removeAttribut(const std::string& key);

    //--------------------------------------------------------------------------

    //! Retrieve object type ; ex : "::fwData::Object"
    FWATOMS_API std::string getType();

    //! Set internal object type
    FWATOMS_API void setType(const std::string& type);

    //! Retrieve object id
    FWATOMS_API std::string getId(){return m_id;};

    //! Set current id
    FWATOMS_API void setId(std::string id){m_id = id;};

    //--------------------------------------------------------------------------

    /**
     * @brief Add a metainfo in the object
     * MetaInfo coud be anything limited to string type
     */
    FWATOMS_API void addMetaInfo(const std::string& key, const std::string& value);

    //!Replace metaInfos.
    FWATOMS_API void setMetaInfos(const MetaInfos& metaInfos);

    //! Retrieve one meta information
    FWATOMS_API std::string getMetaInfo(const std::string& key);

    //! Get internal metaInfo mapping
    FWATOMS_API MetaInfos& getMetaInfos()  ;


    //--------------------------------------------------------------------------

    FWATOMS_API virtual bool isObject() const {return true;};
    FWATOMS_API virtual std::string getString() const {return "Object : ";};
    FWATOMS_API virtual Base::sptr clone();

private:
    FWATOMS_API Object();

    MetaInfos m_metaInfos;
    Attributes m_attrs;
    std::string m_id;
};

}

#endif /*  _FWATOMS_BASE_OBJECT_HPP_ */
