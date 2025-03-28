import socket
from time import sleep

def main():
    SERVER_IP = "127.0.0.1"
    SERVER_PORT = 8820
    BUFFER_SIZE = 1024
    NULL = '\0'

    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as clientSocket:
            # Connecting to trivia server
            server_address = (SERVER_IP, SERVER_PORT)
            clientSocket.connect(server_address)
            
            server_msg = clientSocket.recv(BUFFER_SIZE)
            server_msg = server_msg.decode().rstrip(NULL)
            print("Server message: " + server_msg)
            sleep(0.5)
            
            if server_msg == "Hello":
                clientSocket.sendall("Hello".encode())
               
    except socket.error:
        print("failed to connect to server")
    
    print("Exiting client...")

if __name__ == "__main__":
    main()
