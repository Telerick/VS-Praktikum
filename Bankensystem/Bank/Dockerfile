# Use an official Ubuntu runtime as a parent image
FROM gcc:latest

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Compile the C++ program
RUN g++ bank.cpp -o Bank

# Set the command to run when the container starts
CMD ["./Bank"]