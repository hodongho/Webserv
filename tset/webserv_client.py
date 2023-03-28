import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ("www.example.com", 80)
client_socket.connect(server_address)

# webserver에 HTTP 요청 보내기
request = 'GET /temp HTTP/1.0\r\nHost: www.example.com\r\n\r\n'
client_socket.sendall(request.encode())

response = b''
# received response
while True:
    recv = client_socket.recv(1024)
    if not recv:
        break
    response += recv

# print(response)
with open("example/response_01_error.txt", "w") as f:
    f.write(response.decode())