/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIQT_WORKERQT_HPP_
#define FWGUIQT_WORKERQT_HPP_

#include "fwGuiQt/config.hpp"

namespace fwThread
{
    class Worker;
} //namespace fwThread



namespace fwGuiQt
{

FWGUIQT_API SPTR(::fwThread::Worker) getQtWorker( int &argc, char **argv );

} //namespace fwGui

#endif /*FWGUIQT_WORKERQT_HPP_*/

