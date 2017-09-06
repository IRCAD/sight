/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_OBJECT_HPP__
#define __FWTOOLS_OBJECT_HPP__

#include "fwTools/config.hpp"
#include "fwTools/fwID.hpp"

#include <fwCore/base.hpp>

namespace fwTools
{

class UUID;

/**
 * @brief   Define Base class for FW4SPL objects and services
 */
class FWTOOLS_CLASS_API Object  : public ::fwCore::BaseObject,
                                  public ::boost::noncopyable,
                                  protected ::fwTools::fwID
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (Object)(::fwCore::BaseObject) );
    fwCoreAllowSharedFromThis();

    friend class ::fwTools::UUID;

    // expose API for ID management
    using ::fwTools::fwID::hasID;
    using ::fwTools::fwID::getID;
    using ::fwTools::fwID::setID;
    using ::fwTools::fwID::resetID;

    FWTOOLS_API Object();
    virtual ~Object();

protected:

    /// UUID used to identify the object, notably for serialization
    SPTR(::fwTools::UUID) m_uuid;
};

//-----------------------------------------------------------------------------

inline Object::~Object()
{
}

//-----------------------------------------------------------------------------

}

#endif /* __FWTOOLS_OBJECT_HPP__ */
