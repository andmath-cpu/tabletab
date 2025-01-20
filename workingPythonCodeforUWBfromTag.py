import socket
import cmath

host = "0.0.0.0"
port = 12345

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(1)

a1range = 1.0
a2range = 1.0

print(f"Listening on {host}:{port}...")

def tag_pos(a, b, c):
    # p = (a + b + c) / 2.0
    # s = cmath.sqrt(p * (p - a) * (p - b) * (p - c))
    # y = 2.0 * s / c
    # x = cmath.sqrt(b * b - y * y)
    cos_a = (b * b + c*c - a * a) / (2 * b * c)
    x = b * cos_a
    y = b * cmath.sqrt(1 - cos_a * cos_a)

    return round(x.real, 1), round(y.real, 1)

while True:
    client_socket, addr = server_socket.accept()
    #print(f"Connection from {addr}")
    data = b""
    while True:
        chunk = client_socket.recv(1024)
        if not chunk:
            break
        data += chunk
        
    # Split data by newline and process each message
    messages = data.decode().strip().split("\n")
    for message in messages:
        try:
            #print(f"{message}")
            split_message = message.split(";")
            
            #Extract and clean the data fields
            device_id = split_message[0].strip()
            range_value = float(split_message[1].strip())
            anchor_id = split_message[2].strip()
                    # Print the extracted fields for debugging
            #print(f"Device ID: {device_id}")
            #print(f"Range Value: {range_value}")
            #print(f"Anchor ID: {anchor_id}")
            
            coordinates = tag_pos(a1range, a2range, 1.0)
            print(f"{device_id}" + str(coordinates))
            
            if anchor_id == "6022.00":
                #print(f"anchor2: {range_value}")
                a2range = range_value
            elif anchor_id == "5922.00":
                #print(f"anchor1: {range_value}")
                a1range = range_value
            else:
                print(f"Unknown anchor: {anchor_id}")
                

        except ValueError:
            print(f"Invalid data")
    client_socket.close()
    
    
