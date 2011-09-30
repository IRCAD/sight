/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_FIELD_HPP_
#define _FWTOOLS_FIELD_HPP_

#include <string>

#include <fwCore/base.hpp>

#include "fwTools/config.hpp"

namespace fwTools
{

class Object;
class UUID;

/**
 * @brief   Class for objects fields.
 * @class   Field.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWTOOLS_CLASS_API Field :  public ::fwTools::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Field)(::fwTools::Object), (()) ( ((const std::string &)) ) , new Field );

    /// default constructor label is empty
    FWTOOLS_API Field();

    /// constructor with a given name
    FWTOOLS_API Field(const std::string &newlabel);

    /// retreive read only reference on label for the current class
    FWTOOLS_API const std::string &label() const;

    /// retreive r/w reference on label for the current class
    FWTOOLS_API std::string & label();

    FWTOOLS_API virtual ~Field();

    /**
     * @brief A shallow copy (also called "bitwise copy") simply copies chunks of memory from one location to another.
     * @param[in] _source source of the copy.
     */
    FWTOOLS_API void shallowCopy( ::fwTools::Field::csptr _source );
    FWTOOLS_API void shallowCopy( ::fwTools::Field::sptr _source );

    /**
     * @brief A deep copy clone all source object parameters (sub-ojects are duplcated in memory). For a sptr on sub-object, method allocates a new object.
     * @param[in] _source source of the copy.
     */
    FWTOOLS_API void deepCopy( ::fwTools::Field::csptr _source );
    FWTOOLS_API void deepCopy( ::fwTools::Field::sptr _source );

    FWTOOLS_API void shallowCopy( ::fwTools::Object::csptr _source );

    FWTOOLS_API void deepCopy( ::fwTools::Object::csptr _source );

protected :

    std::string m_label;

};

}

#endif /* _FWTOOLS_FIELD_HPP_ */
