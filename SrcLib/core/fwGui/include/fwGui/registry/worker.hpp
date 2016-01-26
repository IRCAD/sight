/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_REGISTRY_WORKER_HPP__
#define __FWGUI_REGISTRY_WORKER_HPP__

#include "fwGui/config.hpp"

#include <fwThread/Worker.hpp>

namespace fwGui
{

namespace registry
{

namespace worker
{
/// Init current worker with given parameter only if current worker is not defined.
FWGUI_API void init(const ::fwThread::Worker::sptr& _worker);

/// Returns current worker.
FWGUI_API const ::fwThread::Worker::sptr& get();

/// Reset current worker.
FWGUI_API void reset();

} // namespace worker

} // namespace registry
} // namespace fwGui


#endif // __FWGUI_REGISTRY_WORKER_HPP__

