/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_BOOLEAN_HPP__
#define __FWATOMS_BOOLEAN_HPP__

#include "fwAtoms/Base.hpp"
#include "fwAtoms/config.hpp"
#include "fwAtoms/factory/new.hpp"

#include <string>

namespace fwAtoms
{

/**
 * @brief Represented a Boolean value
 *
 */
class FWATOMS_CLASS_API Boolean : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Boolean)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Boolean > );

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Boolean(::fwAtoms::Base::Key key) :
        m_value(false)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Boolean()
    {
    }

    FWATOMS_API virtual std::string getString() const override;

    FWATOMS_API virtual void setString(const std::string& value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    FWATOMS_API static Boolean::sptr New(std::string value);

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolean value.
     */
    FWATOMS_API static Boolean::sptr New(bool value);

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const override
    {
        return ::fwAtoms::Base::BOOLEAN;
    }

    //------------------------------------------------------------------------------

    bool getValue() const
    {
        return m_value;
    }

protected:

    /**
     * @brief Construct an object storing a bool value
     * @param value the boolena value.
     */
    FWATOMS_API Boolean(bool value);

    /**
     * @brief Construct an object storing a bool value
     * @param value a string representing a boolean.
     *
     * If value = "true" then true is stored otherwise false is stored.
     */
    FWATOMS_API Boolean(std::string& value);

    bool m_value;
};

}

#endif /* __FWATOMS_BOOLEAN_HPP__ */

