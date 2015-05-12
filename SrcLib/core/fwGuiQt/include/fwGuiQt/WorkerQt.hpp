/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_WORKERQT_HPP__
#define __FWGUIQT_WORKERQT_HPP__

#include "fwGuiQt/config.hpp"

#include <fwCore/base.hpp>

namespace fwThread
{
class Worker;
} //namespace fwThread



namespace fwGuiQt
{

FWGUIQT_API SPTR(::fwThread::Worker) getQtWorker( int &argc, char **argv );

} //namespace fwGui

#endif /*__FWGUIQT_WORKERQT_HPP__*/

