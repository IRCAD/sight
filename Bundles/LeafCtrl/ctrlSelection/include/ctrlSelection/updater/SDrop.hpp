/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_UPDATER_SDROP_HPP__
#define __CTRLSELECTION_UPDATER_SDROP_HPP__

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @class  SDrop
 * @brief  Drop received object to associated ::fwData::Composite.
 *
 */
class CTRLSELECTION_CLASS_API SDrop : public ::ctrlSelection::IUpdaterSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SDrop)(::ctrlSelection::IUpdaterSrv) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SDrop() noexcept;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SDrop() noexcept;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting();

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping();

    /// Implements configuring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void configuring();

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring();

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating();

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

private:

    /// Slot: add the object defined by uid
    void addObject(std::string uid);
};

} // updater
} // ctrlSelection

#endif // __CTRLSELECTION_UPDATER_SDROP_HPP__
