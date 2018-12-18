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

#ifndef __FWATOMS_SEQUENCE_HPP__
#define __FWATOMS_SEQUENCE_HPP__

#include "fwAtoms/Base.hpp"
#include "fwAtoms/config.hpp"
#include "fwAtoms/factory/new.hpp"

#include <vector>

namespace fwAtoms
{
/**
 * @brief Sequence represented a list of meta object.
 *
 */
class FWATOMS_CLASS_API Sequence : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Sequence)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Sequence > );

    typedef std::vector<Base::sptr> SequenceType;

    typedef SequenceType::value_type ValueType;
    typedef SequenceType::reference ReferenceType;
    typedef SequenceType::const_reference ConstReferenceType;
    typedef SequenceType::iterator IteratorType;
    typedef SequenceType::const_iterator ConstIteratorType;
    typedef SequenceType::reverse_iterator ReverseIteratorType;
    typedef SequenceType::const_reverse_iterator ConstReverseIteratorType;
    typedef SequenceType::size_type SizeType;

    /// boost_foreach/stl compatibility
    /// @{
    typedef SequenceType::value_type value_type;
    typedef SequenceType::reference reference;
    typedef SequenceType::const_reference const_reference;
    typedef SequenceType::iterator iterator;
    typedef SequenceType::const_iterator const_iterator;
    typedef SequenceType::reverse_iterator reverse_iterator;
    typedef SequenceType::const_reverse_iterator const_reverse_iterator;
    typedef SequenceType::size_type size_type;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Sequence(::fwAtoms::Base::Key key)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Sequence()
    {
    }

    /**
     * @brief push an atom in the sequence.
     */
    void push_back(const Base::sptr& value)
    {
        m_value.push_back(value);
    }

    //! Begin of sequence iterator
    IteratorType  begin()
    {
        return m_value.begin();
    }

    //! End of sequence iterator
    IteratorType  end()
    {
        return m_value.end();
    }

    //! Begin of sequence const iterator
    ConstIteratorType begin() const
    {
        return m_value.begin();
    }

    //! End of sequence const iterator
    ConstIteratorType end() const
    {
        return m_value.end();
    }

    //! Returns the sequence size
    size_type size() const
    {
        return m_value.size();
    }

    //! clear the sequence
    void  clear()
    {
        m_value.clear();
    }

    //! Test if the sequence is empty
    bool empty() const
    {
        return m_value.empty();
    }

    //! Returns internal vector
    const SequenceType& getValue() const
    {
        return m_value;
    }

    //! access an element in position index
    Base::sptr& operator[](unsigned int index)
    {
        return m_value[index];
    }
    //------------------------------------------------------------------------------

    const Base::sptr& operator[](unsigned int index) const
    {
        return m_value[index];
    }

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const override
    {
        return ::fwAtoms::Base::SEQUENCE;
    }

protected:
    SequenceType m_value;
};

}
#endif //__FWATOMS_SEQUENCE_HPP__

