/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWFWCOMMAND_UNDOREDOMANAGER_HPP_
#define _FWFWCOMMAND_UNDOREDOMANAGER_HPP_

#include <deque>

// See http://www.boost.org/doc/libs/1_47_0/doc/html/signals/s04.html
#ifndef SIGNALSLIB_HPP_INCLUDED
#define SIGNALSLIB_HPP_INCLUDED 

#if defined(signals) && defined(QOBJECTDEFS_H) && \
  !defined(QT_MOC_CPP)
#  undef signals
#  define signals signals
#endif

#include <boost/signal.hpp>
#include <boost/signals/connection.hpp>
namespace boost
{
  namespace signalslib = signals;
}

#if defined(signals) && defined(QOBJECTDEFS_H) && \
  !defined(QT_MOC_CPP)
#  undef signals
// Restore the macro definition of "signals", as it was
// defined by Qt's <qobjectdefs.h>.
#  define signals protected
#endif

#endif

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
class FWCOMMAND_CLASS_API UndoRedoManager : public ::fwTools::Object
{
public:
    typedef ::boost::signal<void (std::string)>  SignalType;
    typedef ::boost::signalslib::connection         ConnectionType;

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
     * @param cmd the command
     * @param execute sets to true if queue must execute the given command, false if not
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
    FWCOMMAND_API const ::boost::uint32_t getMaxUndoLevel();

    /**
     * @brief Returns the maximum memory that could be used by the history.
     */
    FWCOMMAND_API const ::boost::uint32_t getMaxUndoMemory();

    /**
     * @brief Returns the maximum memory that could be used by a single command.
     */
    FWCOMMAND_API const ::boost::uint32_t getMaxCommandMemory();

    /**
     * @briefReturns the number of commands that can be un-done.
     */
    FWCOMMAND_API const ::boost::uint32_t getUndoSize();

    /**
     * @brief Returns the number of commands that can be re-done.
     */
    FWCOMMAND_API const ::boost::uint32_t getRedoSize();

    /**
     * @brief Set the new undo/redo manager
     */
    FWCOMMAND_API void setManager( Manager::sptr currentManager );

    /**
     * @brief Remove the current manager
     */
    FWCOMMAND_API void removeManager();


    FWCOMMAND_API ConnectionType connect(SignalType::slot_function_type subscriber);

    FWCOMMAND_API void disconnect(ConnectionType subscriber);

protected:

    Manager::sptr m_currentManager;

    /**
     * @brief Default constructor.
     */
    FWCOMMAND_API UndoRedoManager();

private :
    SignalType  m_sig;
};


} // namespace fwCommand


#endif //#ifndef _FWCOMMAND_MANAGER_HPP_
