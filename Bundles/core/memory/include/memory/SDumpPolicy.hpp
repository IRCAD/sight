/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MEMORY_SDUMPPOLICY_HPP__
#define __MEMORY_SDUMPPOLICY_HPP__

#include "memory/config.hpp"

#include <fwMemory/IPolicy.hpp>

#include <fwServices/IController.hpp>

#include <fwTools/Failed.hpp>

namespace memory
{

class MEMORY_CLASS_API SDumpPolicy : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDumpPolicy)(::fwServices::IController) );

    MEMORY_API SDumpPolicy();

    MEMORY_API virtual ~SDumpPolicy();

    /**
     * Configuration example :
       @code{.xml}
       <service ...>
       <config>
         <policy>ValveDump</policy>
         <params>
          <min_free_mem>524288000</min_free_mem>
          <hysteresis_offet>104857600</hysteresis_offet>
         </params>
       </config>
       </service>
       @endcode
     * */
    MEMORY_API virtual void configuring() override;

    /// Overrides
    MEMORY_API virtual void starting() override;

    /// Overrides
    MEMORY_API virtual void stopping() override;

    /// Overrides
    MEMORY_API virtual void updating() override;

    /// Overrides
    MEMORY_API virtual void swapping() override;

protected:

    typedef std::vector< std::pair< ::fwMemory::IPolicy::ParamNamesType::value_type, std::string > > ParametersType;

    std::string m_policy;
    ParametersType m_policyParams;
};

} // namespace memory

#endif /*__MEMORY_SDUMPPOLICY_HPP__*/

