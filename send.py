# open a simple udp connection to localhost on port 6767
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(b'aaaaaaaaaaaaaaaa', ('127.0.0.1', 6767))