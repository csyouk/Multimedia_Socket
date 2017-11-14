# Socket
- 네트워크로 연결된 컴퓨터의 통신 접점에 위치한 통신 객체(추상적 개념)

- **Stream Socket(TCP)**
- **Datagram Socket(UDP)**

- **Socket = IP Address + Port Address**
- **IPC의 일종이다!!**
- 응용 프로그램은 소켓이 제공하는 기능을 사용하기만 하면 된다.

#### Stream(TCP)
![Stream](./img/stream_tcp.gif)
- 연결이 된 후에 보낸다.
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

- [TCP-Client Half-Duplex](./multimedia/EX03-01_tcp_server_bd/tcp_server_bd.c)
- [TCP-Server Half-Duplex](./multimedia/EX03-02_tcp_client_pc/tcp_client_pc.c)

- [TCP-Client Full-Duplex](./multimedia/EX03-03_tcp_server_bd_th/tcp_server_bd_th.c)
- [TCP-Server Full-Duplex](./multimedia/EX03-04_tcp_client_pc_th/tcp_client_pc_th.c)

## UDP
![Datagram](./img/UDPsockets.jpg)
- 연결이라는 개념이 없다. 따라서 보낼 곳에 대한 정보를 줘야 하는데, `sendto()`에서 이를 설정해야 한다.
  - `socket()`
  - `bind()`
  - `sendto()`
  - `recvfrom()` : **`blocking` 함수**
  - `close()`
