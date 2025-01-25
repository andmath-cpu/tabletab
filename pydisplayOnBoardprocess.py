import socket
import turtle

host = "0.0.0.0"
port = 12345

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(1)

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

# Store tag data and turtles in dictionaries
tags = {}  # Stores coordinates for each tag
turtles = {}  # Turtle objects for each tag
labels = {}  # Store text labels (device IDs) for each tag

def create_turtle(color):
    """Create and return a new turtle with the specified color."""
    t = turtle.Turtle()
    t.shape("circle")
    t.color(color)
    t.penup()
    return t

def create_label(device_id):
    """Create a new turtle for displaying the device ID below the tag."""
    label = turtle.Turtle()
    label.hideturtle()
    label.penup()
    label.color("black")
    label.goto(0, -50)  # Temporary position, will be updated below the tag
    return label

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
            device_id = split_message[0].strip()  # e.g., tracker1, tracker2
            x = float(split_message[1].strip())  # x-coordinate
            y = float(split_message[2].strip())  # y-coordinate

            # Initialize tag data if not already present
            if device_id not in tags:
                tags[device_id] = {"x": x, "y": y}
                turtles[device_id] = create_turtle("blue" if len(turtles) % 2 == 0 else "red")
                labels[device_id] = create_label(device_id)

            # Update coordinates for the current tag
            tags[device_id]["x"] = x
            tags[device_id]["y"] = y

            # Print the updated data
            print(f"Updated live: Received data: {device_id}; {x}; {y}")

            # Check if coordinates are within bounds and plot them
            if is_within_bounds(x, y):
                # Move the tag turtle
                turtles[device_id].goto(x * 50 - 250, y * 50 - 150)
                # Update the position of the label below the tag
                labels[device_id].goto(x * 50 - 250, y * 50 - 180)
                # Display the device ID below the tag
                labels[device_id].clear()
                labels[device_id].write(device_id, align="center", font=("Arial", 10, "normal"))
            else:
                print(f"{device_id} outlier: ({x}, {y})")

            screen.update()

        except ValueError:
            print(f"Invalid data: {message}")

    client_socket.close()

