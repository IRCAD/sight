/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_INITIALIZER_HPP_
#define FWMEMORY_INITIALIZER_HPP_

#include <fwTools/Factory.hpp>
#include <fwServices/IService.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{
/**
 * @brief install and start memory service, if supported, for a built object
 */
class FWMEMORY_CLASS_API Initializer : public ::fwTools::Factory::Initializer
{

public :
    fwCoreClassDefinitionsWithFactoryMacro((Initializer), (()), new Initializer );

    FWMEMORY_API Initializer() {} ;
    FWMEMORY_API ~Initializer() {} ;

    FWMEMORY_API virtual void init( ::fwTools::Object::sptr _obj);
    FWMEMORY_API virtual void uninit( ::fwCore::LogicStamp::csptr key );

private:

    typedef std::vector< ::fwServices::IService::wptr > SContainer;
    typedef std::map< ::fwCore::LogicStamp::csptr, SContainer> KSContainer;
    KSContainer m_container;
};

}
#endif /* FWMEMORY_INITIALIZER_HPP_ */
