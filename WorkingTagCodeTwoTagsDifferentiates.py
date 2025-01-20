import socket
import cmath
import turtle

host = "0.0.0.0"
port = 12345

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(1)

a1tag1range = 1.0
a2tag1range = 1.0
a1tag2range = 1.0
a2tag2range = 1.0

print(f"Listening on {host}:{port}...")

# Initialize turtle screen
screen = turtle.Screen()
screen.title("Tag Coordinates")
screen.setup(width=600, height=400)
screen.tracer(0)  # Disable automatic updates for smoother animation

# Draw the rectangle plotting area
border = turtle.Turtle()
border.hideturtle()
border.penup()
border.goto(-250, -150)
border.pendown()
for _ in range(2):
    border.forward(500)
    border.left(90)
    border.forward(300)
    border.left(90)

# Create turtle objects for tag1 and tag2
tag1_turtle = turtle.Turtle()
tag1_turtle.shape("circle")
tag1_turtle.color("blue")
tag1_turtle.penup()

tag2_turtle = turtle.Turtle()
tag2_turtle.shape("circle")
tag2_turtle.color("red")
tag2_turtle.penup()

def tag_pos(a, b, c):
    cos_a = (b * b + c * c - a * a) / (2 * b * c)
    x = b * cos_a
    y = b * cmath.sqrt(1 - cos_a * cos_a)
    return round(x.real, 1), round(y.real, 1)

def is_within_bounds(x, y, x_min=-10, x_max=10, y_min=-10, y_max=10):
    """Check if the coordinates are within the defined bounds."""
    return x_min <= x <= x_max and y_min <= y <= y_max

while True:
    client_socket, addr = server_socket.accept()
    data = b""
    while True:
        chunk = client_socket.recv(1024)
        if not chunk:
            break
        data += chunk

    messages = data.decode().strip().split("\n")
    for message in messages:
        try:
            split_message = message.split(";")
            device_id = split_message[0].strip()
            range_value = float(split_message[1].strip())
            anchor_id = split_message[2].strip()

            # Update ranges based on the incoming data
            if anchor_id == "6022.00" and device_id == "tracker1":
                a2tag1range = range_value
            elif anchor_id == "5922.00" and device_id == "tracker1":
                a1tag1range = range_value
            elif anchor_id == "6022.00" and device_id == "tracker2":
                a2tag2range = range_value
            elif anchor_id == "5922.00" and device_id == "tracker2":
                a1tag2range = range_value
            else:
                print(f"Unknown anchor: {anchor_id}")

            # Calculate and plot tag1 and tag2 coordinates
            tag1coordinates = tag_pos(a1tag1range, a2tag1range, 1.0)
            tag2coordinates = tag_pos(a1tag2range, a2tag2range, 1.0)

            print("tag1", tag1coordinates)
            print("tag2", tag2coordinates)

            # Check if coordinates are within bounds
            tag1_x, tag1_y = tag1coordinates
            tag2_x, tag2_y = tag2coordinates

            if is_within_bounds(tag1_x, tag1_y):
                tag1_turtle.goto(tag1_x * 50 - 250, tag1_y * 50 - 150)
            else:
                print(f"tag1 outlier: {tag1coordinates}")

            if is_within_bounds(tag2_x, tag2_y):
                tag2_turtle.goto(tag2_x * 50 - 250, tag2_y * 50 - 150)
            else:
                print(f"tag2 outlier: {tag2coordinates}")

            screen.update()

        except ValueError:
            print(f"Invalid data")

    client_socket.close()

