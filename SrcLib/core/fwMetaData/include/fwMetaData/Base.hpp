#ifndef __FWMETADATA_BASE_BASE_HPP__
#define __FWMETADATA_BASE_BASE_HPP__


#include <map>
#include <string>

#include <fwCore/base.hpp>
#include <fwCamp/macros.hpp>
#include <fwCamp/camp/customtype.hpp>
#include <fwCamp/camp/MapProperty.hpp>
#include <fwCamp/camp/MapMapper.hpp>
#include <fwCamp/camp/MapValueMapper.hpp>
#include <fwCamp/camp/detail/MapPropertyImpl.hpp>
#include <fwCamp/camp/ExtendedClassVisitor.hpp>


#include "fwMetaData/config.hpp"

fwCampAutoDeclareMacro((fwMetaData)(Base), FWMETADATA_API);

namespace fwMetaData
{

/**
 * @brief Base class for all metadata classes.
 */
class FWMETADATA_CLASS_API Base : public ::fwCore::BaseObject
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Base), (()), new Base );
    fwCoreAllowSharedFromThis();

    /// return the sub class classname : an alias of this->getClassname
    std::string className() const { return this->getClassname(); };

    /**
     * @brief clone a data.
     * @return a clone of the current MetaData
     */
    FWMETADATA_API virtual Base::sptr clone(){ return Base::New();};

    /**
     *@brief Test if the current base is a value or not (Value = {string;numeric;boolean})
     *@return the test result
     */
    FWMETADATA_API virtual bool isValue() const {return false;};

    /**
     *@brief Test if the current base is a boolean
     *@return the test result
     */
    FWMETADATA_API virtual bool isBoolean() const {return false;};

    /**
     *@brief Test if the current base is a Numeric (real or integer)
     *@return the test result
     */
    FWMETADATA_API virtual bool isNumeric() const {return false;};

    /**
     *@brief Test if the current base is a string
     *@return the test result
     */
    FWMETADATA_API virtual bool isString() const {return false;};

    /**
     *@brief Test if the current base is a sequence
     *@return the test result
     */
    FWMETADATA_API virtual bool isSequence() const {return false;};

    /**
     *@brief Test if the current base is a mapping
     *@return the test result
     */
    FWMETADATA_API virtual bool isMapping() const {return false;};

    /**
     *@brief Test if the current base is a blob (a object with a buffer)
     *@return the test result
     */
    FWMETADATA_API virtual bool isBlob() const {return false;};

    /**
     *@brief Test if the current base is an object (representation of fwData)
     *@return the test result
     */
    FWMETADATA_API virtual bool isObject() const {return false;};

    /**
     * @brief Return the string value of the current base.
     * @return the string representation. Default value = "Unknown"
     */
    FWMETADATA_API virtual std::string getString() const {return "Unknown";};

};

}

#endif /* __FWMETADATA_BASE_BASE_HPP__ */
