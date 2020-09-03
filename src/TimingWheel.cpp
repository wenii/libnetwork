#include "TimingWheel.h"
#include "Timer.h"
#include <new>
#include "Log.h"
#include <time.h>
using namespace libnetwork;

TimingWheel* TimingWheel::create()
{
	return new(std::nothrow) TimingWheel();
}

void TimingWheel::destory()
{
	delete this;
}

TimingWheel::TimingWheel()
	: _seconds(0)
	, _needAddTimers(nullptr)
{
	for (int i = 0; i < SECONDS_WHEEL; ++i)
	{
		_wheel[i] = nullptr;
	}
}

TimingWheel::~TimingWheel()
{
	for (int i = 0; i < SECONDS_WHEEL; ++i)
	{
		Timer* head = _wheel[i];
		while (head)
		{
			Timer* next = head->_next;
			delete head;
			head = next;
		}
	}
}

void TimingWheel::tick()
{
	_seconds++;
}

void TimingWheel::addToIndex(Timer* timer, int index)
{
	Timer* teHead = _wheel[index];
	Timer* prev = nullptr;

	while (teHead && teHead->_triggerTime < timer->_triggerTime)
	{
		prev = teHead;
		teHead = teHead->_next;
	}

	if (prev)
	{
		prev->_next = timer;
		timer->_next = teHead;
	}
	else
	{
		timer->_next = _wheel[index];
		_wheel[index] = timer;
	}
}

void TimingWheel::processTimerEvent()
{
	const int index = _seconds % SECONDS_WHEEL;
	const long curTime = time(nullptr);

	// 添加定时器
	Timer* head = _needAddTimers;
	while (head)
	{
		Timer* next = head->_next;
		int dt = head->_triggerTime - curTime;
		dt = dt > 0 ? dt : 0;
		const int index = (_seconds + dt) % SECONDS_WHEEL;
		addToIndex(head, index);
		head = next;
	}
	_needAddTimers = nullptr;

	// 处理定时器事件
	Timer* timer = _wheel[index];
	while (timer && curTime >= timer->_triggerTime)
	{
		if (timer->_remove)
		{
			// 删除时间事件
			_wheel[index] = timer->_next;

			// 执行定时器终止函数
			timer->onTimerFinalize(curTime);
		
			delete timer;
			timer = _wheel[index];
		}
		else
		{
			Log::info("TimingWheel: curTime:%d _triggerTime:%d wheelIndex:%d", curTime, timer->_triggerTime, index);
			
			// 执行定时器函数
			timer->onTimer(curTime);

			if (!timer->_remove)
			{
				long interval = timer->getInterval();
				interval = interval > 0 ? interval : 1;
				timer->_triggerTime = curTime + interval;
				const int nextIndex = (index + interval) % SECONDS_WHEEL;
				if (nextIndex != index)
				{
					// 从本index移除
					_wheel[index] = timer->_next;
				}

				// 添加到nextIndex上
				addToIndex(timer, nextIndex);
				timer = _wheel[index];
			}
		}
	}
}

void TimingWheel::addTimer(Timer* timer)
{
	timer->_next = _needAddTimers;
	_needAddTimers = timer;
}
