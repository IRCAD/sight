/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwAtoms/Base.hpp"
#include "fwAtoms/config.hpp"
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
    fwCoreClassMacro(Object, ::fwAtoms::Base, ::fwAtoms::factory::New< Object >);

    typedef std::map<std::string, Base::sptr> AttributesType;
    typedef std::map<std::string, std::string> MetaInfosType;

    //--------------------------------------------------------------------------

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Object(::fwAtoms::Base::Key)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Object()
    {
    }

    //! Sets the attributes map
    FWATOMS_API void setAttributes(const AttributesType& attrs);

    //! Returns the internal map
    const AttributesType& getAttributes() const
    {
        return m_attributes;
    }

    //! Returns requested attribute if exists, empty sptr else.
    FWATOMS_API Base::sptr getAttribute(const std::string& key) const;

    //! Returns requested attribute casted in OBJECT_TYPE::sptr if exists, empty sptr else.
    template < typename OBJECT_TYPE >
    SPTR(OBJECT_TYPE) getAttribute(const std::string& key) const;

    //! add a atrtribut, old value is erased
    FWATOMS_API void setAttribute(const std::string& key, const Base::sptr&);

    /**
     * @brief Removes an attributes
     *
     * @param key Key of the element to be removed
     *
     * @return returns the number of elements erased
     */
    FWATOMS_API AttributesType::size_type eraseAttribute(const std::string& key);

    //! clear attributes
    FWATOMS_API void clearAttribute();

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
    const MetaInfosType& getMetaInfos() const
    {
        return m_metaInfos;
    }

    /**
     * @brief Removes a MetaInfo
     *
     * @param key Key of the element to be removed
     *
     * @return returns the number of elements erased
     */
    FWATOMS_API MetaInfosType::size_type eraseMetaInfo(const std::string& key);

    //! clear MetaInfos
    FWATOMS_API void clearMetaInfo();

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const override
    {
        return ::fwAtoms::Base::OBJECT;
    }

private:

    MetaInfosType m_metaInfos;
    AttributesType m_attributes;

};

}
