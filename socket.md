# Socket
- 네트워크로 연결된 컴퓨터의 통신 접점에 위치한 통신 객체(추상적 개념)

- **Stream Socket(TCP)**
- **Datagram Socket(UDP)**

- **Socket = IP Address + Port Address**
- **IPC의 일종이다!!**
- 응용 프로그램은 소켓이 제공하는 기능을 사용하기만 하면 된다.

#### Stream(TCP)
![Stream](./img/stream_tcp.gif)
- **연결이 된 후에 보낸다.**
- 서버측
  - `socket()` : 소켓을 생성하여 반환한다.
  - `bind()` : 프로세스가 동작하는 시스템에 **나(=실행할 프로그램)**의 주소를 **등록**시키는 행위.
  - `listen()` :
    - 동시에 모든 클라이언트에 대한 요청을 할 수 없다.
    - 따라서, 순차적으로 처리를 해야 하는데, 이에 대한 처리를 하는 부분이다.
    - 대기열(=Queue)을 만들어서 클라이언트에 대한 요청을 쌓아둠.
    - 대기열의 사이즈를 초과하는 요청은 버려진다.
  - `accept()` : 서버가 클라이언트의 연결 요청을 기다림.
      -  **`blocking` 함수**
  - `read()` : **`blocking` 함수**
  - `write()`
  - `close()`

- 클라이언트 측
  - `socket()`
  - `connect()` : **`blocking` 함수**
  - `write()`
  - `read()` : **`blocking` 함수**
  - `close()`

#### socket()
- 소켓을 생성하여 반환한다.
- 리턴 값.
  - 성공시 : **파일 식별자** (파일 디스크립터 테이블의 index)
  - 실패시 : 실패했을경우에는 -1을 반환하며, 적당한 errno 값을 설정.

#### bind()
- 소켓에 주소를 지정한다.
- 성공할경우 0을 실패했을경우에는 -1을 반환하며, 적당한 errno 값을 설정.

```c
#include <sys/types.h>
#include <sys/socket.h>
int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);

... 다음과 같은 코드 패턴을 가진다.

struct sockaddr_in addr_server;
addr_server.sin_family = AF_INET;
addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
addr_server.sin_port = htons(PORT_ADDR);
len = sizeof(addr_server);
ret = bind(sfd_server, (struct sockaddr *)&addr_server, len);
```

#### listen()
- 연결 요청 대기큐를 생성한다.

```c
#include <sys/socket.h>
int listen(int s, int backlog);
```

#### accept()
- 클라이언트의 연결 요청을 수락하는 `blocking`함수.
- 리턴시
  - **성공 시 클라이언트 소켓의 파일 식별자** 리턴.
  - 실패 시 `errno` 설정 후 -1 리턴.
- 인자
  - `addr` 클라이언트 소켓주소를 넘긴다.(누가 연결했는지 알 수 있다.)

```c
#include <sys/types.h>
#include <sys/socket.h>
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
```

#### connect()
- 소켓에 연결을 시도하는 `blocking` 함수.
- 성공할경우 0을 실패했을경우에는 -1을 반환하며, 적당한 errno 값을 설정.
- 인자
  - `sockfd` : 파일 식별자
  - `serv_addr` : 서버의 소켓 주소.

```c
#include <sys/types.h>
#include <sys/socket.h>

int  connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
```


#### htonl(), htons()
- cpu 아키텍처를 따르는 바이트 order방식의 32비트 혹은 16비트 ip주소를 big endian방식으로 변환하는 함수들이다.
  - Intel 은 Little Endian 방식으로 저장되어 있다.

```c
#include <netinet/in.h>
unsigned short int htons(unsigned short int hostshort);
unsigned long int htonl(unsigned long int hostshort);
```

#### ntohl(), ntohs()
- big endian방식으로 저장되어 있는 32비트 혹은 16비트 ip주소를 cpu 아키텍처를 따르는 바이트 order방식으로 변환하는 함수.

```c
#include <netinet/in.h>
unsigned short int ntohs(unsigned short int hostshort);
unsigned long int ntohl(unsigned long int hostshort);
```

#### 반이중 통신 방식
- [TCP-Client Half-Duplex](./multimedia/EX03-02_tcp_client_pc/tcp_client_pc.c)
- [TCP-Server Half-Duplex](./multimedia/EX03-01_tcp_server_bd/tcp_server_bd.c)

#### 전이중 통신 방식
- [TCP-Client Full-Duplex](./multimedia/EX03-04_tcp_client_pc_th/tcp_client_pc_th.c)
- [TCP-Server Full-Duplex](./multimedia/EX03-03_tcp_server_bd_th/tcp_server_bd_th.c)

## UDP
![Datagram](./img/UDPsockets.jpg)
- 연결이라는 개념이 없다. 따라서 보낼 곳에 대한 정보를 줘야 하는데, `sendto()`에서 이를 설정해야 한다.
  - `socket()`
  - `bind()`
  - `sendto()`
  - `recvfrom()` : **`blocking` 함수**
  - `close()`

- UDP는 **연결**에 대한 코드를 작성할 일이 없다. 따라서 TCP보다 좀 더 간단하다.
- UDP는 Client/Server 둘다 **bind()**행위가 필요하다.
  - **왜 이런 구조를 가지게 되었는지에 대해 생각해보자.**
  - TCP는 **accepct()**라는 행위가 있다.


#### UDP 전이중 통신 방식
- [UDP Client / Full Duplex](./multimedia/EX03-04_tcp_client_pc_th/tcp_client_pc_th.c)
- [UDP Server / Full Duplex](./multimedia/EX03-05_udp_server_bd_th/udp_server_bd_th.c)

- [강의 참고](http://forum.falinux.com/zbxe/index.php?document_srl=406058&mid=network_programming)

## 서버데몬
- **-d** surfix로 끝나는 실행프로그램들을 **데몬**이라고 한다.
- `ftpd`, `telnetd`등을 **서버 데몬**이라 한다.
- 기본적으로 제공해주는 프로그램이다. 파일전송, 원격로그인등에 편리한 프로그램이다.


- *데몬이란?*
> 멀티태스킹 운영 체제에서 **데몬**은 **사용자가 직접적으로 제어하지 않고, 백그라운드에서 돌면서 여러 작업을 하는 프로그램을 말한다.** 시스템 로그를 남기는 syslogd처럼 보통 데몬을 뜻하는 ‘d’를 이름 끝에 달고 있으며, 일반적으로 프로세스로 실행된다.
>
> **데몬은 대개 부모 프로세스를 갖지 않으며, 즉 PPID가 1이며, 따라서 프로세스 트리에서 init 바로 아래에 위치한다.** 데몬이 되는 방법은 일반적으로 자식 프로세스를 포크(fork)하여 생성하고 자식을 분기한 자신을 죽이면서 init이 고아가 된 자식 프로세스를 자기 밑으로 데려가도록 하는 방식이다. 이러한 방법을 ‘fork off and die’라 부르기도 한다.
>
> 출처 : [Wikipedia](https://ko.wikipedia.org/wiki/%EB%8D%B0%EB%AA%AC_(%EC%BB%B4%ED%93%A8%ED%8C%85))



## Multi-Client
- 구현방식에는 여러가지 방식이 있다.
  - `multi-process` 방식.
  - `multi-thread` 방식(`apache`)
  - `multiplexing` 방식(`nginx`)
    - 다중 파일 식별자 모니터링 방식 이라고도 한다.

- 책에는 **multiplexing**방식을 중점적으로 살핀다.
  - 이 방식을 이해하기 위해서는 **`select()`** 함수를 잘 이해해야 한다.

- [입출력 다중화 모델](https://www.joinc.co.kr/w/Site/system_programing/File/select)

#### select()

```c
int select (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
- 기능 : 다중 파일 식별자 모니터링.
- 리턴 :
  - 성공시 : **I/O**동작(읽기, 쓰기, 예외)이 발생한 **파일 식별자의 개수** 리턴.
  - 실패시 : 실패했을경우에는 -1을 반환하며, 적당한 errno 값을 설정.
- 인자
  - `nfds` : 관리하는 파일의 개수를 등록. 파일의 개수는 최대 파일 지정 번호 + 1로 지정하면 된다.
  - *참고 fd_set : 관리하는 파일의 지정번호가 등록되어 있는 비트 배열 구조체*
  - `readfds` : 읽을 데이터가 있는지 검사하기 위한 파일 목록
  - `writefds` : 쓰여진 데이터가 있는지 검사하기 위한 파일 목록
  - `exceptfds` : 파일에 예외 사항들이 있는지 검사하기 위한 파일 목록
  - `timeout` : **select함수는 fd_set에 등록된 파일들에 데이터 변경이 있는지를 timeout동안 기다린다**. **만약 timeout시간동안 변경이 없다면 select()는 0을 반환** 한다. **timeout을 NULL로 하면, 데이터가 있을 때까지 무한정 기다리고**, **멤버 값이 모두 0이면 즉시 반환한다.**

- **관련 함수**
  - `void FD_CLR(int fd, fd_set *set);`
    - fd비트를 0으로 set한다.
  - `void FD_SET(int fd, fd_set *set);`
    - fd비트를 1로 set
  - `void FD_ZERO(int fd, fd_set *set);`
    - 모든 비트를 0으로
  - `void FD_ISSET(int fd, fd_set *set);`
    - fd비트 값을 리턴.

- 다음은 `fd_set` 구조체의 선언부이다.
```c
typedef struct fd_set {
  u_int  fd_count;
  SOCKET fd_array[FD_SETSIZE];
} fd_set;
```

- [다중 클라이언트 지원 서버 - timeout 구현](./multimedia/EX03-07_tcp_server_bd_timeout/tcp_server_bd_timeout.c)
- [다중 클라이언트 지원 서버](./multimedia/EX03-08_tcp_server_bd_multi/tcp_server_bd_multi.c)
- [다중 클라이언트 지원 서버(채팅방) - 모든 연결 클라이언트에게 메세지 전달](./multimedia/EX03-09_tcp_server_bd_chatting/tcp_server_bd_chatting.c)
