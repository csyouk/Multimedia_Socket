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
  - `socket()`
  - `bind()` : 프로세스가 동작하는 시스템에 **나(=실행할 프로그램)**의 주소를 **등록**시키는 행위.
  - `listen()` :
    - 동시에 모든 클라이언트에 대한 요청을 할 수 없다.
    - 따라서, 순차적으로 처리를 해야 하는데, 이에 대한 처리를 하는 부분이다.
    - 대기열(=Queue)을 만들어서 클라이언트에 대한 요청을 쌓아둠.
    - 대기열의 사이즈를 초과하는 요청은 버려진다.
  - `accept()` : -서버가 클라이언트의 연결 요청을 기다림.
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
