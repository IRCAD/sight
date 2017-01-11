/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwThread/Worker.hpp"

namespace fwThread
{

ThreadIdType getCurrentThreadId()
{
    return ::boost::this_thread::get_id();
}


//SPTR(Worker) Worker::defaultFactory() => WorkerAsio.cpp

} //namespace fwThread
