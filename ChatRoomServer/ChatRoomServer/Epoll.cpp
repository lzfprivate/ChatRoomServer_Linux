#include "Epoll.h"
#include <string.h>
#include <unistd.h>
#include <cstdio>

#define EPOLLSIZE 2048

CEpoll_Data::CEpoll_Data()
{
	memset(&m_data, 0, sizeof(m_data));
}

CEpoll_Data::CEpoll_Data(const CEpoll_Data& data)
{
	m_data = data;
}

CEpoll_Data::CEpoll_Data(int fd)
{
	m_data.fd = fd;
}

CEpoll_Data::CEpoll_Data(uint64_t data)
{
	m_data.u64 = data;
}



CEpoll_Data::CEpoll_Data(uint32_t data)
{
	m_data.u32 = data;
}

CEpoll_Data::CEpoll_Data(void* ptr)
{
	m_data.ptr = ptr;
}

CEpoll_Data& CEpoll_Data::operator=(const CEpoll_Data& data)
{
	// TODO: 在此处插入 return 语句
	if (&data != this)
	{
		m_data.u64 = data.m_data.u64;
	}
	return *this;
}

CEpoll_Data::~CEpoll_Data()
{
}

CEpoll::CEpoll():m_epoll(-1)
{
}

CEpoll::~CEpoll()
{
}

int CEpoll::Create(unsigned int nSize)
{
	//如果已经创建
	if (m_epoll != -1) return -1;
	m_epoll = epoll_create(nSize);
	//如果创建失败
	if (m_epoll == -1)	return -2;
	return 0;
}

int CEpoll::Add(int fd, const CEpoll_Data& data, unsigned op)
{
	if (m_epoll == -1) return -1;
	epoll_event ep_event{ op ,data };
	int ret = epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &ep_event);
	return ret;
}

int CEpoll::Modify(int fd, const CEpoll_Data& data, unsigned op)
{
	epoll_event ep_event{ op ,data };
	if (-1 == epoll_ctl(m_epoll, EPOLL_CTL_MOD, fd, &ep_event)) 
		return -1;
	return 0;
}

int CEpoll::Delete(int fd)
{
	if (-1 == epoll_ctl(m_epoll, EPOLL_CTL_DEL, fd, nullptr))
		return -1;
	return 0;
}

int CEpoll::WaitEvents(EPEVENTS& epEvents, int timeout)
{
	EPEVENTS events;
	events.resize(EPOLLSIZE);
	ssize_t iRet = epoll_wait(m_epoll, events.data(), (int)events.size(), timeout);
	if (-1 == iRet)
		return -1;
	if (events.size() > 0 ) {
		epEvents.resize(iRet);
	}
	memcpy((void*)epEvents.data(), events.data(), epEvents.size() * sizeof(epoll_event));
	return iRet;
}

int CEpoll::Close()
{
	if (m_epoll != -1)
	{
		int fd = m_epoll;
		m_epoll = -1;
		close(fd);
	}
	return 0;
}

CEpoll::operator int() const
{
	return m_epoll;
}
