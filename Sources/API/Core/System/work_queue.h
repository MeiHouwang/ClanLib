/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/


#pragma once

#include "../api_core.h"
#include <memory>

namespace clan
{
/// \addtogroup clanCore_System clanCore System
/// \{

/// \brief Interface for executing work on a worker thread
class CL_API_CORE WorkItem
{
public:
	virtual ~WorkItem() { }

	/// \brief Called by a worker thread to process work
	virtual void process_work() = 0;

	/// \brief Called by the WorkQueue thread to complete the work
	virtual void work_completed() { }
};

template<typename ProcessWork>
class WorkItemFunctor : public WorkItem
{
public:
	WorkItemFunctor(const ProcessWork &work) : work(work) { }

	void process_work() { work(); }

private:
	ProcessWork work;
};

class WorkQueue_Impl;

/// \brief Thread pool for worker threads
class CL_API_CORE WorkQueue
{
public:
	/// \brief Constructs a work queue
	/// \param serial_queue If true, executes items in the order they are queued, one at a time
	WorkQueue(bool serial_queue = false);
	~WorkQueue();

	/// \brief Queue some work to be executed on a worker thread
	///
	/// Transfers ownership of the item queued. WorkQueue will delete the item.
	void queue(WorkItem *item);

	/// \brief Queue some work to be executed on a worker thread
	///
	/// This template version takes a lamba functor object to be executed on the worker thread.
	template<typename ProcessWork>
	void queue(const ProcessWork &work)
	{
		queue(new WorkItemFunctor<ProcessWork>(work));
	}

	/// \brief Queue some work to be executed on the main WorkQueue thread
	template<typename WorkCompleted>
	void work_completed(const WorkCompleted &completed)
	{
		work_completed_helper(new WorkItemFunctor<WorkCompleted>(completed));
	}

private:
	void work_completed_helper(WorkItem *item);

	std::shared_ptr<WorkQueue_Impl> impl;
};

}

/// \}
