#ifndef _FWMETADATA_BASE_SEQUENCE_HPP_
#define _FWMETADATA_BASE_SEQUENCE_HPP_

#include <string>
#include <vector>

#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"



namespace fwMetaData
{
/**
 * @brief Sequence represented a list of meta object.
 *
 */
class FWMETADATA_CLASS_API Sequence : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Sequence)(::fwMetaData::Sequence), (()), new Sequence ) ;
    typedef std::vector<Base::sptr> SequenceType;
    typedef SequenceType::iterator Iterator;
    typedef SequenceType::const_iterator cIterator;

    /**
     * @brief Append a base in back of the sequence.
     * @param value the value to push in the sequence (the value is not copy)
     */
    FWMETADATA_API void append(Base::sptr value);

    /**
     * @brief set a value in the sequence.
     * @param pos the position is the sequence.
     * @param value the new value.
     */
    FWMETADATA_API void set(unsigned int pos, Base::sptr value);

    //! Begin of sequence iterator
    FWMETADATA_API Iterator  begin();

    //! End of sequence iterator
    FWMETADATA_API Iterator  end();

    //! Begin of sequence const iterator
    FWMETADATA_API cIterator cBegin() const;

    //! End of sequence const iterator
    FWMETADATA_API cIterator cEnd() const;


    //! Test if the sequence is empty
    FWMETADATA_API bool isEmpty() const;


    //! Retrieve internal vector
    FWMETADATA_API const SequenceType& getValue();


    //! access an element in position index
    FWMETADATA_API Base::sptr operator[](unsigned int index);


    FWMETADATA_API virtual bool isSequence() const {return true;};
    FWMETADATA_API virtual Base::sptr clone();
protected:
    Sequence(){};
    SequenceType m_value;
};

}
#endif
