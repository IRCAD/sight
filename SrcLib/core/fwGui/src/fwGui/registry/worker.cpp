/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/registry/worker.hpp"

namespace fwGui
{

namespace registry
{

namespace worker
{

static ::fwThread::Worker::sptr s_worker;

//-----------------------------------------------------------------------------

void init(const ::fwThread::Worker::sptr& _worker)
{
    if(!s_worker)
    {
        s_worker = _worker;
    }
    else
    {
        SLM_WARN("Main worker has already been initialized");
    }
}

//-----------------------------------------------------------------------------

const ::fwThread::Worker::sptr& get()
{
    SLM_WARN_IF("Main worker not yet initialized", !s_worker);
    return s_worker;
}

//-----------------------------------------------------------------------------

void reset()
{
    s_worker.reset();
}

//-----------------------------------------------------------------------------

} // namespace worker

} // namespace registry
} // namespace fwGui



