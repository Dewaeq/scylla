import socket
import struct
import threading

LCM_IP = "239.255.76.67"
LCM_PORT = 7667

TCP_PORT = 9000

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
udp_socket.bind(("", 7667))
mreq = socket.inet_aton(LCM_IP) + socket.inet_aton("0.0.0.0")
udp_socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)


host_name = socket.gethostname()
is_pi = host_name == "scylla"
origin_id = 1 if is_pi else 2

if is_pi:
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_socket.connect(("192.168.0.190", TCP_PORT))
else:
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server.bind(("0.0.0.0", TCP_PORT))
    tcp_server.listen(1)
    print(f"[{host_name}] Waiting for connection from RPI")
    tcp_socket, addr = tcp_server.accept()
    print(f"[{host_name}] Connection from {addr}")

socket_lock = threading.Lock()


def tcp_send(origin, payload: bytes | bytearray):
    msg = struct.pack("!IB", len(payload) + 1, origin) + payload
    with socket_lock:
        try:
            tcp_socket.sendall(msg)
        except:
            print("failed to send tcp packet")


def tcp_recv_loop():
    buf = bytes()
    while True:
        try:
            chunk = tcp_socket.recv(4096)
        except:
            print("failed to read from tcp socket")
            continue

        if not chunk:
            break
        buf += chunk
        while len(buf) >= 5:
            length = struct.unpack("!I", chunk[:4])[0]
            if len(buf) < 4 + length:
                break
            origin = buf[4]
            payload = buf[5:4+length]
            buf = buf[4+length:]
            if origin == origin_id:
                continue

            try:
                udp_socket.sendto(payload, (LCM_IP, LCM_PORT))
            except:
                print("failed to send udp request to lcm")


def udp_recv_loop():
    while True:
        data, addr = udp_socket.recvfrom(65535)
        sender_id = data[-1]
        if sender_id == origin_id:
            continue
        if sender_id == 0:
            data = bytearray(data)
            data[-1] = origin_id

        print(f"received udp packet from: {addr}")
        tcp_send(origin_id, data)

        
threading.Thread(target=tcp_recv_loop, daemon=True).start()
threading.Thread(target=udp_recv_loop, daemon=True).start()

print(f"[{host_name}] Relay running: UDP {LCM_IP}:{LCM_PORT} <-> TCP {TCP_PORT}")
threading.Event().wait()

