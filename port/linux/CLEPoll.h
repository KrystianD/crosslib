#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <stdio.h>
#include <sys/epoll.h>
#include <vector>
#include <map>

namespace CROSSLIB_NAMESPACE
{
	class EPoll
	{
		int efd;
		std::vector <epoll_event> events;
		std::map<int, int> fdMap;
		int lastCnt;

	public:
		EPoll()
		{
			efd = epoll_create1(0);
		}
		~EPoll()
		{
			::close(efd);
		}

		bool addRead(int fd)
		{
			return addDesc(fd, EPOLLIN);
		}

		bool addWrite(int fd)
		{
			return addDesc(fd, EPOLLOUT);
		}

		bool remove(int fd)
		{
			epoll_event event{};
			event.data.fd = fd;
			int res = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &event);
			return res == 0;
		}

		bool addReadWrite(int fd)
		{
			return addDesc(fd, EPOLLIN | EPOLLOUT);
		}

		int wait(uint32_t timeout = 0xffffffff)
		{
			int nfds = epoll_wait(efd, events.data(), events.size(), timeout == 0xffffffff ? (int)-1 : (int)timeout);
			if (nfds == -1) {
				return -1;
			}
			lastCnt = nfds;
			return nfds;
		}

		bool hasRead(int fd)
		{
			for (int i = 0; i < lastCnt; i++) {
				if (events[i].data.fd == fd) {
					return events[i].events & EPOLLIN;
				}
			}
			return false;
		}

		bool hasWrite(int fd)
		{
			for (int i = 0; i < lastCnt; i++) {
				if (events[i].data.fd == fd) {
					return events[i].events & EPOLLOUT;
				}
			}
			return false;
		}

	private:
		bool addDesc(int fd, int eventsFlags)
		{
			epoll_event event;
			event.data.fd = fd;
			event.events = eventsFlags;
			int res = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
			if (res == 0) {
				fdMap[fd] = events.size();
				events.push_back(epoll_event());
				return true;
			} else {
				return false;
			}
		}
	};
}

#endif
