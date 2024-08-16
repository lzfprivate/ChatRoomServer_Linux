#include "Process.h"
#include <string.h>

CProcess::CProcess()
{
	m_func = nullptr;
	m_pid = 0;
	m_pipes[0] = 0;
	m_pipes[1] = 0;
}

CProcess::~CProcess()
{
	if (m_func) {
		delete m_func;
	}
	m_func = nullptr;
}

int CProcess::CreateSubProcess()
{
	if (!m_func) return -1;
	
	int pid = fork();
	if (pid == -1) return -2;
	int ret = -1;
	//创建子线程前分离
	ret = socketpair(AF_LOCAL,SOCK_STREAM ,0, m_pipes);
	if (ret == -1) {
		printf("<%s> [%d] (%s)  errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return -1;
	}
	if (pid == 0)
	{
		close(m_pipes[1]);
		m_pipes[1] = 0;
		//子进程
		ret = (*m_func)();
		exit(ret);
	}
	else {
		close(m_pipes[0]);
		m_pipes[0] = 0;
		m_pid = pid;
	}

	return 0;
}

int CProcess::SendFD(int fd)
{
	if (!m_func) return -1;
	struct msghdr msg;

	iovec iov[2];
	char buf[2][10] = { "12345","12324" };
	iov[0].iov_base = buf[0];
	iov[0].iov_len = sizeof(buf[0]);

	iov[1].iov_base = buf[1];
	iov[1].iov_len = sizeof(buf[1]);

	msg.msg_iov = iov;
	msg.msg_iovlen = 2;

	cmsghdr* pMsg = (cmsghdr*)calloc(1,CMSG_LEN(sizeof(int)));
	if (!pMsg) return -2;
	pMsg->cmsg_len = CMSG_LEN(sizeof(int));
	pMsg->cmsg_level = SOL_SOCKET;
	pMsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(pMsg) = fd;
	msg.msg_control = pMsg;
	msg.msg_controllen = pMsg->cmsg_len;

	printf("<%s> [%d] (%s)  write pipe value:%d send file fd:%d\n", __FILE__, __LINE__, __FUNCTION__, m_pipes[1], fd);
	printf("<%s> [%d] (%s)  read pipe value:%d send file fd:%d\n", __FILE__, __LINE__, __FUNCTION__, m_pipes[0], fd);
	ssize_t iRet = sendmsg(m_pipes[1], &msg, 0);
	free(pMsg);
	if (-1 == iRet)
	{
		printf("<%s> [%d] (%s)  errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return -2;
	}
	printf("<%s> [%d] (%s)  write pipe value:%d send file fd:%d\n", __FILE__, __LINE__, __FUNCTION__, m_pipes[1], fd);
	return 0;
	
}

int CProcess::RecvFD(int& fd)
{
	if (!m_func) return -1;
	msghdr msg;
	cmsghdr* pMsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
	if (!pMsg) return -2;
	pMsg->cmsg_len = CMSG_LEN(sizeof(int));
	pMsg->cmsg_level = SOL_SOCKET;
	pMsg->cmsg_type = SCM_RIGHTS;

	msg.msg_control = pMsg;
	msg.msg_controllen = pMsg->cmsg_len;

	ssize_t iRet = recvmsg(m_pipes[0], &msg, 0);
	free(pMsg);
	if (-1 == iRet)
	{
		printf("<%s> [%d] (%s)  read pipe value:%d recv file fd:%d\n", __FILE__, __LINE__, __FUNCTION__, m_pipes[0], fd);
		printf("<%s> [%d] (%s)  errno:%d errmsg:%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
		return -2;
	}
	fd = *(int*)CMSG_DATA(pMsg);
	return 0;
}
