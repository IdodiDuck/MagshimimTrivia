import socket
from time import sleep

def chooseOperation():
    LOGIN_REQUEST = "login"
    SIGN_UP_REQUEST = "signup"
    LOGIN_REQUEST_CODE = 100
    SIGN_UP_REQUEST_CODE = 101

    OPERATION_CODES = { LOGIN_REQUEST: LOGIN_REQUEST_CODE, SIGN_UP_REQUEST: SIGN_UP_REQUEST_CODE }

    user_choice = input("\nWelcome to Server Testing Script\nPlease choose Login/SignUp: ")

    # Validating user choice (Can be a login request or signup request)
    while ((user_choice.lower() != LOGIN_REQUEST) and (user_choice.lower() != "signup")):
        print("Error! Please choose a valid operation!\n")
        user_choice = input("\nWelcome to Server Testing Script\nPlease choose Login/Sign-Up: ").lower()

    # Returning the protocol opcode for the desired operation
    return OPERATION_CODES[user_choice]

def sendRequest(request_code, client_socket):
    LOGIN_REQUEST_CODE = 100
    SIGN_UP_REQUEST_CODE = 101

    if (request_code == LOGIN_REQUEST_CODE):
        return sendLoginRequest(client_socket)

    elif (request_code == SIGN_UP_REQUEST_CODE):
        return sendSignUpRequest(client_socket)
    
    # The request is either login request or signup request (validated user choice before!!)

def sendLoginRequest(client_socket):
    USER_NAME_FIELD = "username"
    PASSWORD_FIELD = "password"
    LOGIN_REQUEST_CODE = 100
    BUFFER_SIZE = 1024

    print("LOGIN-REQUEST:\n")
    user_name = input("Enter username: ")
    password = input("Enter password: ")

    # Constructing the JSON by the entered fields, then constructing the request by the protocol
    login_json = f'{{"{USER_NAME_FIELD}":"{user_name}", "{PASSWORD_FIELD}":"{password}"}}'
    request_string = str(LOGIN_REQUEST_CODE) + str(len(login_json)) + login_json
    print(f"Sending to server: {request_string}\n")

    client_socket.sendall(request_string.encode()) # Sending the login-request as sequence of bytes
    server_msg = client_socket.recv(BUFFER_SIZE)

    print(f"Returned server message: {server_msg}\n")

def sendSignUpRequest(client_socket):
    SIGN_UP_REQUEST_CODE = 101
    USER_NAME_FIELD = "username"
    PASSWORD_FIELD = "password"
    EMAIL_FIELD = "email"
    BUFFER_SIZE = 1024

    print("SIGNUP-REQUEST:\n")
    user_name = input("Enter username: ")
    password = input("Enter password: ")
    email = input("Enter email: ")

    # Constructing the JSON by the entered fields, then constructing the request by the protocol
    signup_json = f'{{"{USER_NAME_FIELD}":"{user_name}", "{PASSWORD_FIELD}":"{password}", "{EMAIL_FIELD}":"{email}"}}'
    request_string = str(SIGN_UP_REQUEST_CODE) + str(len(signup_json)) + signup_json
    print(f"Sending to server: {request_string}\n")

    client_socket.sendall(request_string.encode()) # Sending the signup-request as sequence of bytes
    server_msg = client_socket.recv(BUFFER_SIZE)

    print(f"Returned server message: {server_msg}\n")

def main():
    SERVER_IP = "127.0.0.1"
    SERVER_PORT = 8820
    NULL = '\0'
    BUFFER_SIZE = 1024

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

            request_code = chooseOperation()
            sendRequest(request_code, clientSocket)

    except socket.error:
        print("failed to connect to server")

    print("Exiting client...")

if __name__ == "__main__":
    main()