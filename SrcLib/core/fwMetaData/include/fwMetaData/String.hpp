#ifndef _FWMETADATA_BASE_STRING_HPP_
#define _FWMETADATA_BASE_STRING_HPP_

#include <string>

#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"

namespace fwMetaData
{

/**
 * @brief Class represented a String
 *
 */
class FWMETADATA_CLASS_API String : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (String)(::fwMetaData::String), (()), new String ) ;

    FWMETADATA_API virtual bool isValue() const {return true;};
    FWMETADATA_API virtual bool isString() const {return true;};
    FWMETADATA_API virtual std::string getString() const {return m_value;};


    /**
     * @brief Construct a new Object represented a string.
     * @param value the internal string.
     *
     */
    FWMETADATA_API static String::sptr New(std::string value);

    FWMETADATA_API virtual Base::sptr clone();
protected:
    String(){m_value = "";};
    String(const std::string& value);
    std::string m_value;
};

}



#endif /* _FWMETADATA_BASE_STRING_HPP_ */
