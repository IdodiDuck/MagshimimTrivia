import socket
import struct
import json
from time import sleep

def chooseOperation():
    LOGIN_REQUEST = "login"
    SIGN_UP_REQUEST = "signup"
    LOGIN_REQUEST_CODE = 101
    SIGN_UP_REQUEST_CODE = 100

    OPERATION_CODES = { LOGIN_REQUEST: LOGIN_REQUEST_CODE, SIGN_UP_REQUEST: SIGN_UP_REQUEST_CODE }

    user_choice = input("\nWelcome to Server Testing Script\nPlease choose Login/SignUp: ")

    # Validating user choice (Can be a login request or signup request)
    while (user_choice.lower() not in OPERATION_CODES.keys()):
        print("Error! Please choose a valid operation!\n")
        user_choice = input("\nWelcome to Server Testing Script\nPlease choose Login/Sign-Up: ")

    # Returning the protocol opcode for the desired operation
    return OPERATION_CODES[user_choice.lower()]

def sendRequest(request_code, client_socket):
    LOGIN_REQUEST_CODE = 101
    SIGN_UP_REQUEST_CODE = 100

    if (request_code == LOGIN_REQUEST_CODE):
        return sendLoginRequest(client_socket)

    elif (request_code == SIGN_UP_REQUEST_CODE):
        return sendSignUpRequest(client_socket)
    
    # The request is either login request or signup request (validated user choice before!!)
    
def sendLoginRequest(client_socket):
    USER_NAME_FIELD = "username"
    PASSWORD_FIELD = "password"
    LOGIN_REQUEST_CODE = 101

    print("LOGIN-REQUEST:\n")
    user_name = input("Enter username: ")
    password = input("Enter password: ")

    login_data = {USER_NAME_FIELD: user_name, PASSWORD_FIELD: password}
    login_json = json.dumps(login_data)
    json_length = len(login_json)

    # Packing the Login request by the protocol B -> 1 Byte of LoginRequestCode, I -> Integer (4 Bytes) and then the JSON encoded
    login_request = struct.pack(f'!B I{len(login_json)}s', LOGIN_REQUEST_CODE, json_length, login_json.encode())
    
    print(login_request)
    print(f"Sending to server: {login_request.decode()}\n")
    client_socket.sendall(login_request)

def sendSignUpRequest(client_socket):
    SIGN_UP_REQUEST_CODE = 100
    USER_NAME_FIELD = "username"
    PASSWORD_FIELD = "password"
    EMAIL_FIELD = "email"

    print("SIGNUP-REQUEST:\n")
    user_name = input("Enter username: ")
    password = input("Enter password: ")
    email = input("Enter email: ")

    signup_data = {USER_NAME_FIELD: user_name, PASSWORD_FIELD: password, EMAIL_FIELD: email}
    signup_json = json.dumps(signup_data)
    json_length = len(signup_json)

    # Packing the signup request by the protocol 
    signup_request = struct.pack(f'!BI{json_length}s', SIGN_UP_REQUEST_CODE, json_length, signup_json.encode())

    print(f"Sending to server: {signup_request.decode()}\n")
    client_socket.sendall(signup_request)

def main():
    SERVER_IP = "127.0.0.1"
    SERVER_PORT = 8820
    BUFFER_SIZE = 1024

    try:
        # Create a TCP socket connection to the server -> Close it eventually
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as clientSocket:

            # Connecting to trivia server
            server_address = (SERVER_IP, SERVER_PORT)
            clientSocket.connect(server_address)

            request_code = chooseOperation()
            sendRequest(request_code, clientSocket)

            server_msg = clientSocket.recv(BUFFER_SIZE)
            server_msg = server_msg.decode()
            print("Server response: " + server_msg)
            sleep(0.5)

            clientSocket.close()

    except socket.error:
        print("failed to connect to server")

    print("Exiting client...")

if __name__ == "__main__":
    main()