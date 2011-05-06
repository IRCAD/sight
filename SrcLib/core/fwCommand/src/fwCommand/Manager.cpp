/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <iterator>
#include <limits>
#include <stdexcept>

#include <fwTools/ClassRegistrar.hpp>

#include "fwCommand/Manager.hpp"
#include "fwCommand/Empty.hpp"

namespace fwCommand
{

//-----------------------------------------------------------------------------

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwCommand::Manager, ::fwCommand::Manager );

//-----------------------------------------------------------------------------

Manager::Manager( const ::boost::uint32_t maxUndoLevel, const ::boost::uint32_t maxUndoMemory, const ::boost::uint32_t maxCommandMemory )
: m_maxUndoLevel  ( maxUndoLevel  ),
  m_maxUndoMemory ( maxUndoMemory ),
  m_maxCommandMemory (maxCommandMemory),
  m_usedMemory ( 0 )
{
    m_listCmd.push_back( Empty::New() );
    m_lastCmd = m_listCmd.end();
    --m_lastCmd;
}

//-----------------------------------------------------------------------------

Manager::sptr Manager::ManagerFactory( const ::boost::uint32_t  maxUndoLevel, const ::boost::uint32_t  maxUndoMemory, const ::boost::uint32_t  maxCommandMemory)
{
    ::fwCommand::Manager::NewSptr manager;
    manager->m_maxUndoLevel = maxUndoLevel;
    manager->m_maxUndoMemory = maxUndoMemory;
    manager->m_maxCommandMemory = maxCommandMemory;
    return manager;
}

//-----------------------------------------------------------------------------

Manager::~Manager() throw()
{}

//-----------------------------------------------------------------------------

void Manager::removeFirstCmd()
{
    assert( m_listCmd.size() > 1 );

    CmdList::iterator pos( m_listCmd.begin() + 1 );
    ICommand::sptr pCmd = *pos;
    m_usedMemory -= pCmd->getSize();

    m_listCmd.erase( pos );
}

//-----------------------------------------------------------------------------

void Manager::queue( ICommand::sptr pCmd, const bool execute )
{
    // Erase commands after the current one.
    CmdList::iterator i ( m_listCmd.end()-1 );
    CmdList::iterator iEnd ( m_lastCmd   );

    while( i != iEnd )
    {
        ICommand::sptr pCmd = (*i);
        m_usedMemory -= pCmd->getSize();

        --i;

        m_listCmd.pop_back();
    }

    // Execute command, if needed
    if ( execute ) pCmd->apply();

    // Queue command.
    if ( getMaxUndoLevel() > 0 )
    {
        // Ensure that the command size will not overflow the used memory counter.
        const ::boost::uint32_t cmdSize = pCmd->getSize();

        if( (std::numeric_limits< ::boost::uint32_t >::max() - m_usedMemory) <= cmdSize )
        {
            throw std::overflow_error( "Command size will overflow memory usage count." );
        }

        // Pushes the command
        if ( cmdSize <= getMaxCommandMemory() )
        {
            m_listCmd.push_back( pCmd );
            m_usedMemory += cmdSize;
        }

        // Purges some command in the history.
        while (
                (m_listCmd.size()-1 > getMaxUndoLevel()) ||
                (m_usedMemory - cmdSize > getMaxUndoMemory() )
        )
        {
            removeFirstCmd();
        }

        m_lastCmd = m_listCmd.end();
        --m_lastCmd;
    }
    //else nothing to do
}

//-----------------------------------------------------------------------------

void Manager::forward()
{
    CmdList::iterator lastIter = m_listCmd.end();
    --lastIter;

    if ( m_lastCmd != lastIter )
    {
        ++m_lastCmd;
        (*m_lastCmd)->apply();
    }
    //else nothing to do.
}

//-----------------------------------------------------------------------------

void Manager::backward()
{
    if ( m_lastCmd != m_listCmd.begin() )
    {
        ICommand::sptr command( *m_lastCmd );

        command->unapply();
        --m_lastCmd;
    }
    //else nothing to undo.
}

//-----------------------------------------------------------------------------

void Manager::clear()
{
    CmdList::iterator i( m_listCmd.begin() );
    CmdList::iterator iEnd( m_listCmd.end() );

    // don't remove the first command (Empty).
    ++i;

    while ( i != iEnd )
    {
        CmdList::iterator j( i );

        ++i;

        m_listCmd.erase( j );
    }

    m_usedMemory = 0;

    //
    m_lastCmd = m_listCmd.begin();
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t Manager::getUndoSize()
{
    return static_cast< ::boost::uint32_t> ( std::distance( m_listCmd.begin(), m_lastCmd ) );
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t Manager::getRedoSize()
{
    return static_cast< ::boost::uint32_t >( std::distance( m_lastCmd, m_listCmd.end() ) - 1 );
}

//-----------------------------------------------------------------------------

ICommand::sptr Manager::getFirstUndoableCommand()
{
    return ( m_lastCmd != m_listCmd.begin() ) ? *m_lastCmd : ICommand::sptr();
}

//-----------------------------------------------------------------------------

ICommand::sptr Manager::getFirstRedoableCommand()
{
    return ( m_lastCmd + 1 != m_listCmd.end() ) ? *(m_lastCmd + 1) : ICommand::sptr();
}

//-----------------------------------------------------------------------------

} // namespace fwCommand
