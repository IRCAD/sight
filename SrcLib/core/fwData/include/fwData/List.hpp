/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_LIST_HPP_
#define _FWDATA_LIST_HPP_

#include <map>
#include <boost/shared_ptr.hpp>
#include "fwData/config.hpp"

#include <fwData/Object.hpp>

namespace fwData
{
/**
 * @class   List
 * @brief   This class defines a vector of objects.
 *
 * List contains a map of ::fwData::Object.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API List : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (List)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< List >) ;

    typedef std::list< Object::sptr > Container;

    /// Constructor
    FWDATA_API List();

    /// Destructor
    FWDATA_API virtual ~List();

    /// @brief get the container of ::fwData::Object
    FWDATA_API Container &getRefContainer();

    /// @brief get the container of ::fwData::Object
    FWDATA_API Container const &getRefContainer() const;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( List::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( List::csptr _source );

protected:

    /// container of ::fwData::Object
    Container m_container;
};
}

#endif /* _FWDATA_LIST_HPP_ */

