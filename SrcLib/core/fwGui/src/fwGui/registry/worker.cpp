#include "fwGui/registry/worker.hpp"

namespace fwGui
{

namespace registry
{

namespace worker
{

static ::fwThread::Worker::sptr s_worker;

void init(const ::fwThread::Worker::sptr& _worker)
{
    if(!s_worker)
    {
        s_worker = _worker;
    }
}

const ::fwThread::Worker::sptr& get()
{
    return s_worker;
}

} // namespace worker

} // namespace registry
} // namespace fwGui



