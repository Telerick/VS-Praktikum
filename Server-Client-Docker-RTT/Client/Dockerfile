# Use an official Ubuntu runtime as a parent image
FROM gcc:latest

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Compile the C++ program
RUN g++ Client_IP.cpp -o Client 

# Set the command to run when the container starts
CMD ["./Client"]