/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWJOBS_NAMESPACE_HPP__
#define __FWJOBS_NAMESPACE_HPP__

/**
 * @page Jobs fwJobs library system
 *
 * @tableofcontents
 *
 * fwJobs library is meant to trace progress of tasks.
 *
 * @section JobElements Job elements
 *
 * Each job is composed of several elements in order to be run, to trace its
 * run, and to plan what it could do at different events of its life.
 *
 * Moreover, each job has a name (generally named after the task he traces).
 * Several logs can be added during job's life if the user wants to add details
 * on the progression.
 *
 * @subsection States States
 *
 * A job has several life states:
 *  - Waiting: The job is ready to be used,
 *  - Running: The job is currently running,
 *  - Canceling: The job received a cancel request,
 *  - Canceled: The job is canceled,
 *  - Finished: The job have finished to run his task.
 *
 * \verbatim
 *          +-------------------------------------------
 *          |                                          |
 *          |                                          v
 *          |                      +---------+     +--------+
 *          |               +----->|Canceling|---->|Canceled|
 *          |               |      +---------+     +--------+
 *     +-------+     +-------+                          |
 *     |Waiting|---->|Running|                          |
 *     +-------+     +-------+                          v
 *                         |       +--------+         +--+
 *                         +------>|Finished|-------->|  |
 *                                 +--------+         +--+
 * \endverbatim
 *
 * A job is running until it is finished or canceled.
 * \warning Therefore, a job can only be run once.
 *
 *
 * @subsection WorkUnits Work units
 *
 * A job owns a number of total work units and a number of done work units.
 * Work units are used to trace job's progress. The number of total work units
 * is meant to be set before the job is running. The number of done work units
 * updates while the job is running.
 * Thus, progress is equal to: Done work units / Total work units.
 * When the job is finished, the number of done work units is equal to the
 * number of total work units.
 *
 *
 * @subsection Hooks Hooks
 *
 * After an event (job's cancellation, log added etc.), it can be useful to
 * perform commands.
 *
 * In each job, the user can hook the following events:
 *  - Cancellation,
 *  - State change,
 *  - Done work unit change,
 *  - Total work unit change,
 *  - Log change.
 *
 * There can be several hooks set to one event.
 *
 * \warning Do not get confused by hooks methods (which add callbacks to
 * hooks) and callbacks helpers: methods that returns a callback which can be
 * used to modify the job. For instance progressCallback returns a callback
 * which modify done work units of the job.
 *
 *
 * @section KindsOfJobs Kinds of jobs
 *
 * Three different kinds of job exist to allow the user to trace progress of a
 * task in every situation.
 *
 * @subsection Job Job
 *
 * A Job encapsulates a task and trace its progress.
 *
 * A Job has one important element: a function, in which are instructions to
 * process the task and also instructions for the Job's progress. It can also
 * have an optionnal worker on which the function will be run.
 *
 * When the progress is complete, that is to say the number of done work units
 * has reached the number of total work units, the job is immediately marked as
 * finished.
 *
 * Here is an example of Job use:
 * \code{.cpp}

    ::fwJobs::Job::sptr job = ::fwJobs::Job::New( "Example of Job",
        [=](::fwJobs::Job& runningJob)
        {
            for( std::size_t i = 1; i<= 100; ++i)
            {
                if(runningJob.cancelRequested())
                {
                    break;
                }

                ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(1000) );

                runningJob.doneWork(i);
            }
        },
        worker
        );

    job.run().wait();

   \endcode
 *
 * A Job is initialized with a function taking a job as parameter. This
 * Job is actually the Job itself.
 *
 * In this example, the task is to wait 100 seconds. Every second, the job
 * progress is updated with the method donework(). The method cancelRequested()
 * allows to know inside the lambda if cancel has been requested while running.
 *
 * At the end, the job is run to perform the task.
 *
 * Job's progress and cancellation are feasible by other means:
 * \code{.cpp}

    ::fwJobs::Job::sptr job = ::fwJobs::Job::New( "VTK Mesh reader",
        [=](::fwJobs::Job& runningJob)
        {
            ::fwVtkIO::MeshWriter::sptr writer = ::fwVtkIO::MeshWriter::New();

            runningJob.addSimpleCancelHook( [=]{ writer->cancel(); });

            writer->setProgressCallback( runningJob.progressCallback() );

            writer->setObject(mesh);
            writer->setFile("file.vtk"));
            writer->write();
        },
        m_associatedWorker
        );

    job.run().wait();

   \endcode
 *
 * Here, Job's progress is updated through a done work callback: this function
 * set the number of done work units of the Job. This callback needs only the
 * number of done work units to set. The callback is set to the writer for using
 * it during write() and updates Job's progress.
 *
 * Here, another method is used to cancel the task: addSimpleCancelHook().
 * If the Job is canceling, the lambda inside is run and cancels the write.
 *
 *
 * @subsection Observer Observer
 *
 * An Observer does not encapsulate a task but simply observe its progress.
 *
 * An Observer is run immediately after it is initialized but must be finished
 * by using the method finish().
 *
 * Observer are generally used where progress is already given by an external
 * source.
 * This is shown in the following example:
 * \code{.cpp}

    ::fwJobs::Observer observer =  ::fwJobs::Observer::New("VTK Image reader");

    vtkSmartPointer<vtkLambdaCommand> progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback( [this](vtkObject* caller, long unsigned int , void* )
                                   {
                                       auto filter = static_cast<vtkGenericDataObjectReader*>(caller);
                                       observer->doneWork( filter->GetProgress()*100 );
                                   }
                                 );

    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    observer->addSimpleCancelHook( [&](){
                                    reader->AbortExecuteOn();
                                    } );

    reader->Update();

    observer->finish();

   \endcode
 *
 * Here, the progress is given by the reader (here filter). The method
 * doneWork() is embedded in the reader through a callback. Moreover, the
 * method addSimpleCancelHook() allows to cancel the process.
 *
 * The whole progress process is thus done during Update().
 *
 * @subsection Aggregator Aggregator
 *
 * An Aggregator aggregate IJobs (Jobs, Observers and Aggregator) in order to
 * gather their progress into one.
 *
 * The number of work units owned by a job in an Aggregator is set to 100.
 * For instance, two jobs are present in an Aggregator. Job1 has 3 total work
 * units and job2 has 1000 total work units.
 * Thus the Aggregator's number of total work units is 200. 100 work units for
 * job1 and 100 for jobs2.
 *
 * When adding a job to an Aggregator, an associated weight can be linked to
 * this job. It will determine the number of work units the job takes in the
 * Aggregator by multiplying the weight by 100 (number of work units for each
 * job in an Aggregator). The default weight is 1.
 * For instance, with previous job1 and job2, job1 is added to the Aggregator
 * with a weight of 3. The Aggregator's number ot total work units is 400. 100*3
 * for job1 and 100 for job2.
 * Though, it is not recommended to use a weight lower than 0.01.
 *
 * Moreover, when adding a job to an Aggregator, job's method cancel() is
 * added to the Aggregator through a cancel callback. Thus, when the Aggregator
 * is cancelled, each of its subjobs are cancelled as well.
 *
 * Like a Job, an Aggregator must be run and is automatically marked as finished
 * when its progress is completed. When an Aggregator is run, it calls each
 * subjobs' run(). The Aggregator's progress is updated when a subjob's progress
 * is changed.
 *
 * Below, an example of Aggregator use:
 * \code{.cpp}

    ::fwJobs::Aggregator::sptr = ::fwJobs::Aggregator::New("Example of Aggregator");

    m_job->add(m_subAggregator);
    m_job->add(m_subObserver, 3);
    m_job->add(m_subJob);

    m_job->run().wait();

   \endcode
 *
 * @section ThreadSafe Thread safe
 *
 * fwJobs library is thread safe.
 * A worker can be set to a Job.
 *
 * @section Tips Tips
 *
 * @subsection ProgressionReduced Reducing the progression
 *
 * A progression is not supposed to regress. It can lead to strange behaviors
 * on fwJobs front end.
 * Some misuse of the library can lead to this situation. These misuses are:
 * - Add a job to an Aggregator while it is running,
 * - Change the total of work units while the job is running,
 * - Set a lower number of done work units to a job.
 *
 * It is therefore strongly recommended to be careful when using actions that
 * can possibly change the job's number of total work units before the execution
 * of the process.
 *
 */


/**
 * @brief       This namespace fwJobs provides jobs management.
 * @namespace   fwJobs
 */
namespace fwJobs
{
}
#endif /* __FWJOBS_NAMESPACE_HPP__ */
