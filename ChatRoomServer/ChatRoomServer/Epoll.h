#pragma once
#include <sys/epoll.h>
#include <vector>
#include <cstdint>
using EPEVENTS = std::vector<epoll_event>;

class CEpoll_Data 
{
public:
	CEpoll_Data();
	explicit CEpoll_Data(const CEpoll_Data& data);
	//����4�����캯������epoll_data_t�Ķ����������д��
	explicit CEpoll_Data(int fd);
	explicit CEpoll_Data(uint64_t data);
	explicit CEpoll_Data(uint32_t data);
	explicit CEpoll_Data(void* ptr);
	CEpoll_Data& operator=(const CEpoll_Data& data);
	~CEpoll_Data();

public:
	operator epoll_data_t() {
		return m_data;
	}
	operator epoll_data_t() const {
		return m_data;
	}
	operator epoll_data_t* () {
		return &m_data;
	}
	operator const epoll_data_t* () const
	{
		return &m_data;
	}
private:
	epoll_data_t m_data;
};

class CEpoll
{
public:
	CEpoll();
	~CEpoll();
	//����
	int Create(unsigned int nSize);
	//���
	int Add(int fd, const CEpoll_Data& data = CEpoll_Data(nullptr), unsigned op = EPOLLIN);
	//�޸�
	int Modify(int fd, const CEpoll_Data& data = CEpoll_Data(nullptr), unsigned op = EPOLLIN);
	//ɾ��
	int Delete(int fd);
	//�ȴ�
	int WaitEvents(EPEVENTS& epEvents,int timeout = 10);
	//�ر�
	int Close();

private:
	int m_epoll;						//epoll�ļ�������



};

