#ifndef _FWMETADATA_BASE_NUMERIC_HPP_
#define _FWMETADATA_BASE_NUMERIC_HPP_


#include <string>

#include <fwCamp/Mapper/ValueMapper.hpp>
#include <fwCamp/macros.hpp>

#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"

fwCampAutoDeclareMacro((fwMetaData)(Numeric), FWMETADATA_API);

namespace fwMetaData
{

/**
* @brief Storing a  numeric value
**/
class FWMETADATA_CLASS_API Numeric : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Numeric)(::fwMetaData::Numeric), (()), new Numeric) ;

    FWMETADATA_API virtual bool isNumeric() const {return true;};
    FWMETADATA_API virtual bool isValue() const {return true;};
    FWMETADATA_API virtual bool isSigned();
    FWMETADATA_API virtual bool isReal();
    FWMETADATA_API virtual std::string getString() const {return m_value;};

    FWMETADATA_API virtual void setString(std::string value) { m_value = value;};

    /**
     * @brief Constructed a new numeric type
     * @param value reference of all numeric type
     * Supported tested type are (int8/16/32/64, uint8/16/32/64, float, double)
     **/
    template< typename T >
    static Numeric::sptr New(T value);

    /**
     * @brief Storing a numeric represented by a string
     * @param value this string have to be lexical castable.
     */
    FWMETADATA_API static Numeric::sptr New(std::string value);


    /**
     *@brief test if two numerics are equal
     */
    FWMETADATA_API bool isEqual(Numeric::sptr obj);


    /**
     * @brief test if this numeric is higher than obj
     */
    FWMETADATA_API bool isHigher(Numeric::sptr obj);

    /**
     * @brief test if this numeric is lower than obj
     */
    FWMETADATA_API bool isLower(Numeric::sptr obj);

    FWMETADATA_API virtual Base::sptr clone();
protected:
    Numeric(){}
    template< typename T >

    Numeric(T value);

    Numeric(std::string& value);

    std::string m_value;
};

}



#include "fwMetaData/Numeric.hxx"
#endif /* _FWMETADATA_BASE_NUMERIC_HPP_ */
