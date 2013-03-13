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
#include "fwAtoms/factory/new.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (Object)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Object >) ;

    typedef std::map<std::string, Base::sptr> Attributes;
    typedef std::map<std::string, std::string> MetaInfos;

    //--------------------------------------------------------------------------

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Object(::fwAtoms::Base::Key key);

    /**
     * @brief   Destructor
     */
    virtual ~Object()
    {}

    //! Set the internal attributs map
    FWATOMS_API void setAttributs(Attributes& attrs);

    //! add a atrtribut, old value is erased
    FWATOMS_API void addAttribut(const std::string& key, Base::sptr);

    //! Retrieve the internal map
    Attributes& getAttributes() {return m_attrs;};

    //! Retrieve the internal map
    const Attributes& getAttributes() const {return m_attrs;};

    //! Retrieve one Attribut
    Base::sptr& getAttribut(const std::string& key) {return m_attrs[key];};

    //! Remove an attributs
    FWATOMS_API void removeAttribut(const std::string& key);

    //--------------------------------------------------------------------------

    //! Retrieve object type ; ex : "::fwData::Object"
    FWATOMS_API std::string getType() const;

    //! Set internal object type
    FWATOMS_API void setType(const std::string& type);

    //! Retrieve object id
    std::string getId() const {return m_id;};

    //! Set current id
    void setId(std::string id){m_id = id;};

    //--------------------------------------------------------------------------

    /**
     * @brief Add a metainfo in the object
     * MetaInfo coud be anything limited to string type
     */
    FWATOMS_API void addMetaInfo(const std::string& key, const std::string& value);

    //!Replace metaInfos.
    FWATOMS_API void setMetaInfos(const MetaInfos& metaInfos);

    //! Retrieve one meta information
    FWATOMS_API std::string getMetaInfo(const std::string& key) const;

    //! Get internal metaInfo mapping
    FWATOMS_API MetaInfos& getMetaInfos();

    //! Get internal metaInfo mapping
    FWATOMS_API const MetaInfos& getMetaInfos() const;


    //--------------------------------------------------------------------------

    virtual bool isObject() const {return true;};
    virtual std::string getString() const {return "Object : ";};
    FWATOMS_API virtual Base::sptr clone();

private:

    MetaInfos m_metaInfos;
    Attributes m_attrs;
    std::string m_id;
};

}

#endif /*  _FWATOMS_BASE_OBJECT_HPP_ */
