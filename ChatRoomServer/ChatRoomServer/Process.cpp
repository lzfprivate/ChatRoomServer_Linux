#include "Process.h"
#include <string.h>

CProcess::CProcess()
{
	m_func = NULL;
	memset(m_pipes, 0, sizeof(m_pipes));
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
	if (m_func == NULL)return -1;
	int ret = socketpair(AF_LOCAL,SOCK_STREAM, 0, m_pipes);
	if (ret == -1)return -2;
	pid_t pid = fork();
	if (pid == -1)return -3;
	if (pid == 0) {
		//子进程
		close(m_pipes[1]);//关闭掉写
		m_pipes[1] = 0;
		ret = (*m_func)();
		exit(ret);
	}
	//主进程关闭读功能
	close(m_pipes[0]);
	m_pipes[0] = 0;
	m_pid = pid;
	return 0;
}

//在主进程中调用
int CProcess::SendFD(int fd)
{
	struct msghdr msg;
	iovec iov[2];
	char buf[2][10] = { "edoyun","jueding" };
	iov[0].iov_base = buf[0];
	iov[0].iov_len = sizeof(buf[0]);
	iov[1].iov_base = buf[1];
	iov[1].iov_len = sizeof(buf[1]);
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	// 下面的数据，才是我们需要传递的。
	cmsghdr* cmsg = (cmsghdr*)calloc(1,
		CMSG_LEN(sizeof(int)));
	if (cmsg == NULL)return -1;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg) = fd;
	msg.msg_control = cmsg;
	msg.msg_controllen = cmsg->cmsg_len;
	ssize_t ret = sendmsg(m_pipes[1], &msg,0);
	if(cmsg) free(cmsg);
	if (ret == -1) {
		return -2;
	}
	return 0;
}

//在子线程中调用
int CProcess::RecvFD(int& fd)
{
	msghdr msg;
	iovec iov[2];
	char buf[2][10] = { "","" };
	iov[0].iov_base = buf[0];
	iov[0].iov_len = sizeof(buf[0]);
	iov[1].iov_base = buf[1];
	iov[1].iov_len = sizeof(buf[1]);
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
	if (cmsg == NULL)return -1;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control = cmsg;
	msg.msg_controllen = CMSG_LEN(sizeof(int));
		
	ssize_t ret = recvmsg(m_pipes[0], &msg, 0);
	if (ret == -1) {
		free(cmsg);
		return -2;
	}
	fd = *(int*)CMSG_DATA(cmsg);
	if(cmsg) free(cmsg);
	return 0;
}

int CProcess::SendSocket(int fd, sockaddr* addr)
{
	return 0;
}

int CProcess::RecvSocket(int& fd, sockaddr* addr)
{
	return 0;
}
