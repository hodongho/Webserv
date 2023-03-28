import socket
import datetime

IP_ADDR = socket.gethostbyname(socket.gethostname())
PORT = 80 # HTTP port

# create server socket
my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
my_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
my_socket.bind((IP_ADDR, PORT))
my_socket.listen(0)
print(f"Started server at {my_socket.getsockname()}")

# wait client access
conn, addr = my_socket.accept()
# request from client
# decode는 무엇을 무엇으로 복호화하는가?
request = conn.recv(1024).decode()
print(request)

# send response
body = ''
with open("example/example_korean.html", "r", encoding="UTF8") as f:
    for line in f.readlines():
        body += line
response = f'HTTP/1.0 200 OK\n\n{body}'
conn.sendall(response.encode())
conn.close()

# clse
my_socket.close()