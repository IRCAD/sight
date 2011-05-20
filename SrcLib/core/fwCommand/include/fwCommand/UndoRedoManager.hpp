/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWFWCOMMAND_UNDOREDOMANAGER_HPP_
#define _FWFWCOMMAND_UNDOREDOMANAGER_HPP_

#include <deque>
#include <boost/cstdint.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>


#include "fwCommand/config.hpp"
#include "fwCommand/ICommand.hpp"
#include "fwCommand/Manager.hpp"

namespace fwCommand
{

/**
 * @brief Keep tracking an history of commands.
 *
 * @todo macro command.
 */
struct FWCOMMAND_CLASS_API UndoRedoManager : public ::fwTools::Object
{
    fwCoreClassDefinitionsWithFactoryMacro( (UndoRedoManager)(::fwTools::Object), (( )), ::fwTools::Factory::New< UndoRedoManager > );

    /// Return the unique Instance, create it if required at first access
    FWCOMMAND_API static UndoRedoManager::sptr getDefault();

    /**
     * @brief Destructor.
     */
    FWCOMMAND_API virtual ~UndoRedoManager() throw();


    /**
     * @name History management methods.
     */
    //@{

    /**
     * @brief Execute the command and push it in the history.
     *
     * @param cmdthe command
     * @param executesets to true if queue must execute the given command, false if not
     *
     * @todo Remove use of size on list (OPT)
     */
    FWCOMMAND_API void queue( ICommand::sptr cmd, const bool execute = false );

    /**
     * @brief Execute the next command if any.
     */
    FWCOMMAND_API void forward();

    /**
     * @brief Execute the previous command if any.
     */
    FWCOMMAND_API void backward();

    /**
     * @brief Remove all commands in history.
     */
    FWCOMMAND_API void clear();
    //@}


    /**
     * @briefRetrieves the first undoable command.
     *
     * @returna shared pointer to the first undoable command, can be empty
     */
    FWCOMMAND_API ICommand::sptr getFirstUndoableCommand();

    /**
     * @briefRetrieves the first redoable command.
     *
     * @returna shared pointer to the first undoable command, can be empty
     */
    FWCOMMAND_API ICommand::sptr getFirstRedoableCommand();

    /**
     * @brief Returns the maximum number of undo that could be stored in the history.
     */
    FWCOMMAND_API const boost::uint32_t getMaxUndoLevel();

    /**
     * @brief Returns the maximum memory that could be used by the history.
     */
    FWCOMMAND_API const boost::uint32_t getMaxUndoMemory();

    /**
     * @brief Returns the maximum memory that could be used by a single command.
     */
    FWCOMMAND_API const boost::uint32_t getMaxCommandMemory();

    /**
     * @briefReturns the number of commands that can be un-done.
     */
    FWCOMMAND_API const boost::uint32_t getUndoSize();

    /**
     * @brief Returns the number of commans that can be re-done.
     */
    FWCOMMAND_API const boost::uint32_t getRedoSize();

    /**
     * @brief Set the new undo/redo manager
     */
    FWCOMMAND_API void setManager( Manager::sptr currentManager );

    /**
     * @brief Remove the current manager
     */
    FWCOMMAND_API void removeManager();

protected:

    Manager::sptr m_currentManager;

    /**
     * @brief Default constructor.
     */
    FWCOMMAND_API UndoRedoManager();


};


} // namespace fwCommand


#endif //#ifndef _FWCOMMAND_MANAGER_HPP_
