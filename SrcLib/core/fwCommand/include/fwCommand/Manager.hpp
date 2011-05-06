/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWFWCOMMAND_MANAGER_HPP_
#define _FWFWCOMMAND_MANAGER_HPP_

#include <deque>
#include <boost/cstdint.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include "fwCommand/config.hpp"
#include "fwCommand/ICommand.hpp"

namespace fwCommand
{

/**
 * @brief Keep tracking an history of commands.
 *
 * @todo macro command.
 */
struct FWCOMMAND_CLASS_API Manager : public ::fwTools::Object
{
    fwCoreClassDefinitionsWithNFactoriesMacro( (Manager)(::fwTools::Object),
           ((::fwTools::Factory::New< Manager > ,() ))
           ((ManagerFactory ,((::boost::uint32_t)) ((::boost::uint32_t)) ((::boost::uint32_t))  ))
           );

    /**
     * @brief Default constructor.
     *
     * @param maxUndoLevelmaximum number of command that could be stored in the history
     * @param maxUndoMemorymaximum number of byte that could be stored in the history (but the latest queued command is ignored by the memory usage counter. See below for more explanation)
     * @param maxCommandMemorymaximum number of byte that could be used by a single command
     *
     * If the size of a single command is greater than maxCommandMemory, then it would not be stored in the history.
     *
     * @remarks The latest queued command is ignored by the memory usage counter in order to ensure that the latest command could always being stored in the history.
     * @remarks The maximum memory used by the history is maxUndoMemory + maxCommandMemory.
     */
    FWCOMMAND_API Manager( const ::boost::uint32_t  maxUndoLevel = 0, const ::boost::uint32_t  maxUndoMemory = 0, const ::boost::uint32_t  maxCommandMemory = 1024*1024*32 /* 32 MB */);

    FWCOMMAND_API static sptr ManagerFactory( const ::boost::uint32_t  maxUndoLevel, const ::boost::uint32_t  maxUndoMemory , const ::boost::uint32_t  maxCommandMemory);

    /**
     * @brief Destructor.
     */
    FWCOMMAND_API virtual ~Manager() throw();


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
    FWCOMMAND_API const ::boost::uint32_t  getMaxUndoLevel() const { return m_maxUndoLevel; }

    /**
     * @brief Returns the maximum memory that could be used by the history.
     */
    FWCOMMAND_API const ::boost::uint32_t  getMaxUndoMemory() const { return m_maxUndoMemory; }

    /**
     * @brief Returns the maximum memory that could be used by a single command.
     */
    FWCOMMAND_API const ::boost::uint32_t  getMaxCommandMemory() const { return m_maxCommandMemory; }

    /**
     * @briefReturns the number of commands that can be un-done.
     */
    FWCOMMAND_API const ::boost::uint32_t  getUndoSize();

    /**
     * @briefReturns the number of commans that can be re-done.
     */
    FWCOMMAND_API const ::boost::uint32_t  getRedoSize();



protected:

    /**
     * @brief Typedef for list of commands.
     */
    typedef std::deque< ICommand::sptr > CmdList;

    /**
     * @brief History
     */
    CmdList m_listCmd;

    /**
     * @brief  Maximum number of undo that could be stored in the history.
     */
    ::boost::uint32_t  m_maxUndoLevel;

    /**
     * @brief Maximum number of bytes that could be used by the history.
     */
    ::boost::uint32_t m_maxUndoMemory;

    /**
     * @brief Maximum number of bytes that could be used by a single command.
     */
    ::boost::uint32_t m_maxCommandMemory;

    /**
     * @brief Memory used by the history.
     */
    ::boost::uint32_t m_usedMemory;

    /**
     * @brief Iterator on the last command applied.
     */
    CmdList::iterator m_lastCmd;


    /**
     * @brief Remove first command in m_listCmd and dessalocate this command.
     *
     * @pre m_listCmd not empty.
     */
    void removeFirstCmd();

};


} // namespace fwCommand


#endif //#ifndef _FWCOMMAND_MANAGER_HPP_
