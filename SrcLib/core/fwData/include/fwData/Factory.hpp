/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_FACTORY_HPP_
#define _FWDATA_FACTORY_HPP_

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{

/**
 * @brief   The purpose of this class is to provide a data Object Factory and  initialized them
 * @class   Factory
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * The purpose of this class is to provide a data Object Factory which produce initialized data.
 *
 * Data can be initialized (typically install default service)
 * by install initializer functors (  specialization Factory::Initializer ). The installation is performed using Factory::addInitializer() method.
 *
 * To produce the data, this factory uses the ClassFactoryRegistry scheme, and needs to declare the data registration with fwDataRegisterMacro macro.
 */
class FWDATA_CLASS_API Factory
{
public :

    /// Builds a object from template CLASSNAME
    template<class CLASSNAME >
    static ::boost::shared_ptr< CLASSNAME > New();

    /// Builds a object from its classname
    FWDATA_API static ::fwData::Object::sptr New(const std::string &classname);

    /// class destructor
    FWDATA_API virtual ~Factory();

protected:

    FWDATA_API Factory();
};

} // namespace fwData

#include "fwData/Factory.hxx"

#endif //_FWDATA_FACTORY_HPP_
