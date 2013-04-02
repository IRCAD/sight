/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMS_BASE_OBJECT_HPP__
#define  __FWATOMS_BASE_OBJECT_HPP__

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"
#include "fwAtoms/factory/new.hpp"

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

    typedef std::map<std::string, Base::sptr> AttributesType;
    typedef std::map<std::string, std::string> MetaInfosType;

    //--------------------------------------------------------------------------

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWATOMS_API Object(::fwAtoms::Base::Key key);

    /**
     * @brief   Destructor
     */
    virtual ~Object()
    {}

    //! Sets the attributes map
    FWATOMS_API void setAttributes(const AttributesType& attrs);

    //! Returns the internal map
    const AttributesType& getAttributes() const {return m_attributes;}


    //! Returns requested attribute if exists, empty sptr else.
    FWATOMS_API Base::sptr getAttribute(const std::string& key) const;

    //! add a atrtribut, old value is erased
    FWATOMS_API void setAttribute(const std::string& key, const Base::sptr&);


    //! Remove an attributes
    FWATOMS_API void removeAttribute(const std::string& key);

    //--------------------------------------------------------------------------

    //! Returns object id
    const std::string& getId() const {return m_id;};

    //! Set current id
    void setId(const std::string &id){m_id = id;};

    //--------------------------------------------------------------------------

    /**
     * @brief Add a metainfo in the object
     * MetaInfo coud be anything limited to string type
     */
    FWATOMS_API void setMetaInfo(const std::string& key, const std::string& value);

    //!Replace metaInfos.
    FWATOMS_API void setMetaInfos(const MetaInfosType& metaInfos);

    //! Returns one meta information
    FWATOMS_API std::string getMetaInfo(const std::string& key) const;

    //! Get internal metaInfo mapping
    const MetaInfosType& getMetaInfos() const { return m_metaInfos; }

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const {return ::fwAtoms::Base::OBJECT;};


private:

    MetaInfosType  m_metaInfos;
    AttributesType m_attributes;
    std::string    m_id;

};

}

#endif /*  _FWATOMS_BASE_OBJECT_HPP_ */

