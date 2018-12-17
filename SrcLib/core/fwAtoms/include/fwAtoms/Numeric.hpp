/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWATOMS_NUMERIC_HPP__
#define __FWATOMS_NUMERIC_HPP__

#include "fwAtoms/Base.hpp"
#include "fwAtoms/config.hpp"
#include "fwAtoms/factory/new.hpp"

#include <boost/blank.hpp>
#include <boost/variant/variant.hpp>

#include <string>

namespace fwAtoms
{

/**
 * @brief Storing a  numeric value
 **/
class FWATOMS_CLASS_API Numeric : public Base
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Numeric)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Numeric >);

    typedef enum
    {
        EMPTY = 0,
        INT,
        UINT,
        FLOAT,
        DOUBLE
    } NumericType;

    typedef ::boost::variant< ::boost::blank, std::int64_t, std::uint64_t, float, double > ValueType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Numeric(::fwAtoms::Base::Key key)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Numeric()
    {
    }

    /**
     * @brief Build a new numeric type
     * @param value reference of all numeric type
     * Supported tested type are (int8/16/32/64, uint8/16/32/64, float, double)
     **/
    template< typename T >
    static Numeric::sptr New(T value);

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const override
    {
        return ::fwAtoms::Base::NUMERIC;
    }

    /**
     * @brief Returns currently held numeric type
     */
    NumericType variantType() const
    {
        return static_cast< NumericType >(m_value.which());
    }

    /**
     * @brief Returns const reference to numeric's variant
     *
     * @return
     */
    const ValueType& getVariant() const
    {
        return m_value;
    }

    /**
     * @brief Returns a string representing the currently held numeric value
     */
    FWATOMS_API virtual std::string getString() const override;

    /**
     * @brief Sets Numeric's value from given string, using Numeric::valueFromString
     */
    FWATOMS_API void setFromString(const std::string& s, NumericType type = EMPTY);

    /**
     * @brief Retuns a ValueType from a std::string
     *
     * @param s source string
     * @param type type hint
     *
     * If hint is EMPTY (default), this method tries to find a fitting type ( in
     * order : int64, uint64, double)
     *
     * @throw fwAtoms::Exception if unable to cast givent string
     *
     * @return a ValueType containing the numeric value
     */
    FWATOMS_API static ValueType valueFromString(const std::string& s, NumericType type = EMPTY);

    /**
     * @brief Returns the held value static_casted to T
     *
     * @tparam T wanted type
     *
     * @throw boost::positive_overflow
     * @throw boost::negative_overflow
     */
    template <typename T>
    T getValue() const;

protected:

    ValueType m_value;

};

}

#endif /* __FWATOMS_NUMERIC_HPP__ */

