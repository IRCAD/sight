/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MEMORY_SDUMPPOLICY_HPP__
#define __MEMORY_SDUMPPOLICY_HPP__

#include <fwTools/Failed.hpp>

#include <fwServices/IController.hpp>

#include <fwMemory/IPolicy.hpp>

#include "memory/config.hpp"

namespace memory
{

class MEMORY_CLASS_API SDumpPolicy : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SDumpPolicy)(::fwServices::IController) ) ;

    /**
     * Configuration example :
     * <service ...>
     *   <config>
     *     <policy>ValveDump</policy>
     *     <params>
     *      <min_free_mem>524288000</min_free_mem>
     *      <hysteresis_offet>104857600</hysteresis_offet>
     *     </params>
     *   </config>
     * </service>
     * */
    MEMORY_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides
    MEMORY_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    MEMORY_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    MEMORY_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    MEMORY_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Overrides
    MEMORY_API virtual void swapping() throw ( ::fwTools::Failed );

protected :

    typedef std::vector< std::pair< ::fwMemory::IPolicy::ParamNamesType::value_type, std::string > > ParametersType;

    /// Constructor
    MEMORY_API SDumpPolicy();

    /// Destructor
    MEMORY_API virtual ~SDumpPolicy();

    std::string m_policy;
    ParametersType m_policyParams;
};



} // namespace memory

#endif /*__MEMORY_SDUMPPOLICY_HPP__*/

