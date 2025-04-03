#ifndef _NETREDEFINITIONS_H_
#define _NETREDEFINITIONS_H_

#ifdef ECHO_PLATFORM_NINTENDO_GAMECUBE
#define ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII
#else
#ifdef ECHO_PLATFORM_NINTENDO_WII
#define ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII
#endif
#endif
#ifdef ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII

#define ECHO_WII_NETWORKING
//--------------------------------------------
#include <network.h>
#include <fcntl.h>
#include <unistd.h>
	
namespace Echo
{
typedef int Socket;
typedef sockaddr SocketAddress;
typedef sockaddr_in SocketAddressIn;

#define echo_socket net_socket
#define echo_bind net_bind
#define echo_listen net_listen
#define echo_accept net_accept
#define echo_connect(s,addr,l) (net_connect(s,(struct sockaddr *)addr,l))
//net_write
#define echo_send(s,data,size,flags) (net_send(s,(void*)data,size,flags))
#define echo_sendto(s,data,len,flags,to,tolen) (net_sendto(s,(void*)data,len,flags,(sockaddr*)to,tolen))
#define echo_recv net_recv
#define echo_recvfrom net_recvfrom
//net_read
#define echo_closesocket net_close

// This is the same structure as pollsd that libogc provides but pollsd uses socket instead of fd.
struct pollfd {
	s32 fd;
	u32 events;
	u32 revents;
};
#define echo_net_poll(a,b,c) net_poll(reinterpret_cast<pollsd*>(a),b,c)

// Unfortunately this ugly mess comes from the lack of select support. It is basically a work around
// written as though it was in the SocketNetworkSystem::SocketThreadTask::Update() function. Since
// this is the only known place it is used this works. I can't comment inline very easily due to the
// multi-line macro. Basically this code builds a list of all sockets that we want to poll for a
// second, clears the bits for each in the FD_SET, and sets them again if an event is detected. This
// produces a result that is like a select call.
#define echo_select(a,b,c,d,e) 0;\
do{\
	struct pollsd pollsockets[FD_SETSIZE];\
	s32 i=0;\
	std::list< Socket >::iterator esit = mAllSockets.begin();\
	std::list< Socket >::iterator esitEnd = mAllSockets.end();\
	while(esit != esitEnd && i < FD_SETSIZE)\
	{\
		pollsockets[i].socket = *esit;\
		if(!mSystem->IsSocketConnected(*esit))\
		{\
			pollsockets[i].events = (POLLIN|POLLOUT|POLLHUP);\
		}else\
			pollsockets[i].events = (POLLIN|POLLOUT);\
		pollsockets[i].revents = 0;\
		FD_CLR(*esit, &readSet);\
		FD_CLR(*esit, &writeSet);\
		FD_CLR(*esit, &exceptSet);\
		i++;\
		esit++;\
	}\
	s32 numRead = net_poll(pollsockets,i,1000);\
	if(numRead>0)\
	{\
		i=0;\
		std::list< Socket >::iterator esit = mAllSockets.begin();\
		std::list< Socket >::iterator esitEnd = mAllSockets.end();\
		while(esit != esitEnd)\
		{\
			if(pollsockets[i].revents&POLLIN || pollsockets[i].revents&POLLHUP)\
			{\
				FD_SET(*esit, &readSet);\
			}\
			if(pollsockets[i].revents&POLLOUT)\
			{\
				FD_SET(*esit, &writeSet);\
			}\
			i++;\
			esit++;\
		}\
	}\
	selectReturn = numRead;\
}while(0)
#define echo_setsockopt net_setsockopt
#define echo_gethostname(a,b) -1
#define echo_getsockname(a,socketAddress,addressLength) memset(socketAddress,0,*addressLength)
#define echo_net_ioctl net_ioctrl
#define echo_net_fcntl net_fcntl
#define echo_fstat fstat
#define echo_stat stat

//libogc doesn't signal
const int ECHO_SOCKET_SEND_NO_SIG_FLAGS = 0;
const int ECHO_SOCKET_RECV_NO_SIG_FLAGS = 0;
}

#else

#ifdef WIN32

#define ECHO_WINSOCKS_NETWORKING

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fstat-fstat32-fstat64-fstati64-fstat32i64-fstat64i32
#include <sys/types.h>
#include <sys/stat.h>
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"Ws2_32.lib")
#undef ERROR

namespace Echo
{
typedef int socklen_t;
typedef SOCKET Socket;
typedef SOCKADDR SocketAddress;
typedef sockaddr_in SocketAddressIn;

#define echo_socket socket
#define echo_bind bind
#define echo_listen listen
#define echo_accept accept
#define echo_connect connect
//net_write
#define echo_send send
#define echo_sendto sendto
#define echo_recv recv
#define echo_recvfrom recvfrom
//net_read
#define echo_closesocket closesocket
#define echo_select select
#define echo_setsockopt(a,b,c,d,e) setsockopt(a,b,c,(const char*)d,e)
#define echo_net_ioctl ioctlsocket
#define echo_net_fcntl fcntl
#define echo_gethostname gethostname
#define echo_getsockname getsockname
#define echo_fstat _fstat
#define echo_stat _stat
#define echo_net_poll WSAPoll

//Windows doesn't signal
const int ECHO_SOCKET_SEND_NO_SIG_FLAGS = 0;
const int ECHO_SOCKET_RECV_NO_SIG_FLAGS = 0;
}
#else

#define ECHO_POSIX_NETWORKING
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

namespace Echo
{
typedef int Socket;
typedef sockaddr SocketAddress;
typedef sockaddr_in SocketAddressIn;

#define echo_socket socket
#define echo_bind bind
#define echo_listen listen
#define echo_accept accept
#define echo_connect connect
//net_write
#define echo_send send
#define echo_sendto sendto
#define echo_recv recv
#define echo_recvfrom recvfrom
//net_read
#define echo_closesocket close
#define echo_select select
#define echo_setsockopt setsockopt
#define echo_gethostname gethostname
#define echo_getsockname getsockname
#define echo_net_ioctl ioctl
#define echo_net_fcntl fcntl
#define echo_fstat fstat
#define echo_stat stat
#define echo_net_poll poll

const int ECHO_SOCKET_SEND_NO_SIG_FLAGS = MSG_NOSIGNAL;
const int ECHO_SOCKET_RECV_NO_SIG_FLAGS = MSG_NOSIGNAL;
}
#endif
#endif
#endif

