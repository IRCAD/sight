/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWATOMS_BASE_SEQUENCE_HPP_
#define _FWATOMS_BASE_SEQUENCE_HPP_

#include <vector>

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"
#include "fwAtoms/factory/new.hpp"

fwCampAutoDeclareMacro((fwAtoms)(Sequence), FWATOMS_API);

namespace fwAtoms
{
/**
 * @brief Sequence represented a list of meta object.
 *
 */
class FWATOMS_CLASS_API Sequence : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Sequence)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Sequence > ) ;

    typedef std::vector<Base::sptr> SequenceType;
    typedef SequenceType::iterator Iterator;
    typedef SequenceType::const_iterator cIterator;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Sequence(::fwAtoms::Base::Key key)
    {}

    /**
     * @brief   Destructor
     */
    virtual ~Sequence()
    {}

    /**
     * @brief Append a base in back of the sequence.
     * @param value the value to push in the sequence (the value is not copy)
     */
    FWATOMS_API void append(Base::sptr value);

    /**
     * @brief set a value in the sequence.
     * @param pos the position is the sequence.
     * @param value the new value.
     */
    FWATOMS_API void set(unsigned int pos, Base::sptr value);

    //! Begin of sequence iterator
    FWATOMS_API Iterator  begin();

    //! End of sequence iterator
    FWATOMS_API Iterator  end();

    //! Begin of sequence const iterator
    FWATOMS_API cIterator cBegin() const;

    //! End of sequence const iterator
    FWATOMS_API cIterator cEnd() const;


    //! Test if the sequence is empty
    FWATOMS_API bool isEmpty() const;


    //! Retrieve internal vector
    FWATOMS_API const SequenceType& getValue() const;

    //! Retrieve internal vector
    FWATOMS_API SequenceType& getValue();


    //! access an element in position index
    FWATOMS_API Base::sptr operator[](unsigned int index);

    virtual bool isSequence() const {return true;};

    FWATOMS_API virtual Base::sptr clone();

protected:
    SequenceType m_value;
};

}
#endif
