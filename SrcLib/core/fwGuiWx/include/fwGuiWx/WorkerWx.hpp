/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_WORKERWX_HPP_
#define FWGUIWX_WORKERWX_HPP_

#include "fwGuiWx/config.hpp"

namespace fwThread
{
    class Worker;
} //namespace fwThread



namespace fwGuiWx
{

FWGUIWX_API SPTR(::fwThread::Worker) getWxWorker( int &argc, char **argv );

} //namespace fwGui

#endif /*FWGUIWX_WORKERWX_HPP_*/

