/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
 * used to modify the job. For instance progress_callback returns a callback
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
 * have an optional worker on which the function will be run.
 *
 * When the progress is complete, that is to say the number of done work units
 * has reached the number of total work units, the job is immediately marked as
 * finished.
 *
 * Here is an example of Job use:
 * \code{.cpp}

    core::jobs::job::sptr job = std::make_shared<core::jobs::job>( "Example of Job",
        [=](core::jobs::job& runningJob)
        {
            for( std::size_t i = 1; i<= 100; ++i)
            {
                if(runningJob.cancel_requested())
                {
                    break;
                }

                boost::this_thread::sleep_for( boost::chrono::milliseconds(1000) );

                runningJob.done_work(i);
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
 * progress is updated with the method donework(). The method cancel_requested()
 * allows to know inside the lambda if cancel has been requested while running.
 *
 * At the end, the job is run to perform the task.
 *
 * Job's progress and cancellation are feasible by other means:
 * \code{.cpp}

    core::jobs::job::sptr job = std::make_shared<core::jobs::job>( "VTK Mesh reader",
        [=](core::jobs::job& runningJob)
        {
            io::vtk::MeshWriter::sptr writer = std::make_shared<io::vtk::MeshWriter>();

            runningJob.add_simple_cancel_hook( [=]{ writer->cancel(); });

            writer->setProgressCallback( runningJob.progress_callback() );

            writer->set_object(mesh);
            writer->set_file("file.vtk"));
            writer->write();
        },
        this->worker()
        );

    job.run().wait();

   \endcode
 *
 * Here, Job's progress is updated through a done work callback: this function
 * set the number of done work units of the Job. This callback needs only the
 * number of done work units to set. The callback is set to the writer for using
 * it during write() and updates Job's progress.
 *
 * Here, another method is used to cancel the task: add_simple_cancel_hook().
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

    core::jobs::observer observer =  std::make_shared<core::jobs::observer>("VTK Image reader");

    vtkSmartPointer<vtk_lambda_command> progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->SetCallback( [this](vtkObject* caller, long unsigned int , void* )
                                   {
                                       auto filter = static_cast<vtkGenericDataObjectReader*>(caller);
                                       observer->done_work( filter->GetProgress()*100 );
                                   }
                                 );

    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    observer->add_simple_cancel_hook( [&](){
                                    reader->AbortExecuteOn();
                                    } );

    reader->Update();

    observer->finish();

   \endcode
 *
 * Here, the progress is given by the reader (here filter). The method
 * done_work() is embedded in the reader through a callback. Moreover, the
 * method add_simple_cancel_hook() allows to cancel the process.
 *
 * The whole progress process is thus done during Update().
 *
 * @subsection aggregator aggregator
 *
 * An aggregator aggregate IJobs (Jobs, Observers and aggregator) in order to
 * gather their progress into one.
 *
 * The number of work units owned by a job in an aggregator is set to 100.
 * For instance, two jobs are present in an aggregator. Job1 has 3 total work
 * units and job2 has 1000 total work units.
 * Thus the aggregator's number of total work units is 200. 100 work units for
 * job1 and 100 for jobs2.
 *
 * When adding a job to an aggregator, an associated weight can be linked to
 * this job. It will determine the number of work units the job takes in the
 * aggregator by multiplying the weight by 100 (number of work units for each
 * job in an aggregator). The default weight is 1.
 * For instance, with previous job1 and job2, job1 is added to the aggregator
 * with a weight of 3. The aggregator's number ot total work units is 400. 100*3
 * for job1 and 100 for job2.
 * Though, it is not recommended to use a weight lower than 0.01.
 *
 * Moreover, when adding a job to an aggregator, job's method cancel() is
 * added to the aggregator through a cancel callback. Thus, when the aggregator
 * is cancelled, each of its subjobs are cancelled as well.
 *
 * Like a Job, an aggregator must be run and is automatically marked as finished
 * when its progress is completed. When an aggregator is run, it calls each
 * subjobs' run(). The aggregator's progress is updated when a subjob's progress
 * is changed.
 *
 * Below, an example of aggregator use:
 * \code{.cpp}

    core::jobs::aggregator::sptr = std::make_shared<core::jobs::aggregator>("Example of aggregator");

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
 * - Add a job to an aggregator while it is running,
 * - Change the total of work units while the job is running,
 * - Set a lower number of done work units to a job.
 *
 * It is therefore strongly recommended to be careful when using actions that
 * can possibly change the job's number of total work units before the execution
 * of the process.
 *
 */

/**
 * @brief       This namespace sight::core::jobs provides jobs management.
 */
#pragma once

namespace sight::core::jobs
{

}
