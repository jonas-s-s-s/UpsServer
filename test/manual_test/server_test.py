import socket

host = input("Enter the host (press Enter for 'localhost'): ")
port_str = input("Enter the port: ")

host = host if host else "localhost"
port = int(port_str) if port_str else 3001

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

print('\n')

while True:
    data = s.recv(1024)
    print('Server response:', repr(data))

    message = input("Enter request: ")

    if message.lower() == 'exit':
        break

    message = message.encode().decode('unicode_escape')
    s.sendall(message.encode('utf-8'))
    print('')

s.close()