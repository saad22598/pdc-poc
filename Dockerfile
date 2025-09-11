FROM debian:stable-slim

# Install g++, make, OpenMPI, AND the OpenSSH server
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    make \
    openmpi-bin \
    libopenmpi-dev \
    openssh-server && \
    rm -rf /var/lib/apt/lists/*

# Create a directory for the SSH daemon to run (use -p to avoid errors)
RUN mkdir -p /var/run/sshd

# --- SSH Key Generation and Configuration ---
# Generate a passwordless SSH key pair
RUN ssh-keygen -t rsa -f /root/.ssh/id_rsa -N ''

# Add the public key to the list of authorized keys for passwordless login
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
RUN chmod 600 /root/.ssh/authorized_keys

# --- THIS IS THE NEW LINE ---
# Create an SSH config file to disable host key checking
RUN echo "Host *\n\tStrictHostKeyChecking no" >> /root/.ssh/config
# --- END OF NEW LINE ---

# Allow root login via SSH
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

WORKDIR /app

COPY . /app/

# Compile using the MPI C++ compiler wrapper
RUN mpic++ -O2 -std=c++17 -o mpi_task task.cpp