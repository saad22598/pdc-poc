FROM debian:stable-slim

# Install necessary packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    make \
    openmpi-bin \
    libopenmpi-dev \
    openssh-server && \
    rm -rf /var/lib/apt/lists/*

# Set up SSH for MPI
RUN mkdir -p /var/run/sshd
RUN ssh-keygen -t rsa -f /root/.ssh/id_rsa -N ''
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
RUN chmod 600 /root/.ssh/authorized_keys
RUN echo "Host *\n\tStrictHostKeyChecking no" >> /root/.ssh/config
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# Set working directory
WORKDIR /app

# Copy source files
COPY . /app/

# Compile the MPI program
RUN mpic++ -O2 -std=c++17 -o edit_distance edit_distance_mpi.cpp
RUN chmod +x /app/edit_distance || true
