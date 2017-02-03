/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMMAND_ICOMMAND_HPP__
#define __FWCOMMAND_ICOMMAND_HPP__

#include "fwCommand/config.hpp"

#include <fwServices/IService.hpp>

#include <fwTools/Object.hpp>

#include <boost/cstdint.hpp>

#include <exception>
#include <string>

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

protected:

    ::fwServices::IService::wptr m_serviceNotifier;

public:

    fwCoreServiceClassDefinitionsMacro( (ICommand)(::fwTools::Object) );

    /**
     * @brief Virtual destructor.
     */
    virtual ~ICommand()
    {
    }

    /**
     * @brief Retrieves the memory footprint of the command.
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

    //------------------------------------------------------------------------------

    void setNotifier( ::fwServices::IService::sptr serviceNotifier )
    {
        m_serviceNotifier = serviceNotifier;
    }

    //------------------------------------------------------------------------------

    ::fwServices::IService::sptr getNotifier()
    {
        return m_serviceNotifier.lock();
    }
};

} // namespace fwCommand

#endif // __FWCOMMAND_ICOMMAND_HPP__
