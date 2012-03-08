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

protected :

    std::string m_label;

};

}

#endif /* _FWTOOLS_FIELD_HPP_ */
