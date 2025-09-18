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

# ... (all previous lines are the same) ...

WORKDIR /app

COPY . /app/

# Compile ONLY task4 during the build to avoid unrelated compile failures
RUN mpic++ -O2 -std=c++17 -o task4 task4.cpp
RUN chmod +x /app/task4 || true