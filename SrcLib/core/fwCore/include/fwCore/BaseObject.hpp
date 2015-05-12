/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_BASEOBJECT_HPP__
#define __FWCORE_BASEOBJECT_HPP__


#include <boost/enable_shared_from_this.hpp>

#include "fwCore/macros.hpp"
#include "fwCore/config.hpp"

namespace fwCore
{

/**
 * @brief Base class for all FW4SPL's classes
 */
class FWCORE_CLASS_API BaseObject : public ::boost::enable_shared_from_this<BaseObject>
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (BaseObject) );

protected:
    virtual ~BaseObject()
    {
    }
    using ::boost::enable_shared_from_this<BaseObject>::shared_from_this;
};

} // namespace fwCore

#endif // __FWCORE_BASEOBJECT_HPP__

