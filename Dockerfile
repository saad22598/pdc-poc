FROM debian:stable-slim

# ... (all the apt-get install and ssh setup lines are unchanged) ...
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    make \
    openmpi-bin \
    libopenmpi-dev \
    openssh-server && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p /var/run/sshd
RUN ssh-keygen -t rsa -f /root/.ssh/id_rsa -N ''
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
RUN chmod 600 /root/.ssh/authorized_keys
RUN echo "Host *\n\tStrictHostKeyChecking no" >> /root/.ssh/config
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

WORKDIR /app

COPY . /app/

# Compile ALL the necessary tasks during the build
RUN mpic++ -O2 -std=c++17 -o mpi_task task.cpp
RUN mpic++ -O2 -std=c++17 -o task1 task1.cpp