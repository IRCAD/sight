/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMMAND_ICOMMAND_H
#define _FWCOMMAND_ICOMMAND_H

#include <exception>
#include <string>
#include <boost/cstdint.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>

#include <fwServices/IService.hpp>

#include "fwCommand/config.hpp"

namespace fwCommand
{


/**
 * @brief The base class for all command.
 *
 * It provides do/undo operations as well as GetDescription operations for the easy
 * tracking of the executed command (quite useful when keeping a menu of last performed
 * operations).
 */
class FWCOMMAND_CLASS_API ICommand : public ::fwTools::Object
{

protected :

    ::fwServices::IService::wptr m_serviceNotifier;

public :

    fwCoreServiceClassDefinitionsMacro ( (ICommand)(::fwTools::Object) ) ;

    /**
     * @brief Virtual destructor.
     */
    FWCOMMAND_API virtual ~ICommand() {};

    /**
     * @briefRetrieves the memory footprint of the command.
     */
    FWCOMMAND_API virtual const boost::uint32_t  getSize() const = 0;

    /**
     * @brief Pure virtual operation that in child classes encapsulates the logic of the change.
     */
    FWCOMMAND_API virtual void apply() /*throw(std::exception)*/ = 0;    // FIXME std::exception to net::cmd::exception

    /**
     * @brief Pure virtual operation that in child classes encapsulates the logic of undoing a change.
     */
    FWCOMMAND_API virtual void unapply() /*throw(std::exception)*/ = 0;

    /**
     * @brief Pure virtual operation that in child classes returns the description of the operation
     *
     * particularly useful for undo/redo lists presented to the user.
     */
    FWCOMMAND_API virtual const std::string getDescription( void ) const = 0;

    FWCOMMAND_API void setNotifier( ::fwServices::IService::sptr serviceNotifier ) { m_serviceNotifier = serviceNotifier; }

    FWCOMMAND_API ::fwServices::IService::sptr getNotifier() { return m_serviceNotifier.lock(); }
};


} // namespace fwCommand


#endif //#ifndef _FWCOMMAND_ICOMMAND_H
