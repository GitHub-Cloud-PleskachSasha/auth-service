FROM ubuntu:22.04 AS builder

WORKDIR /app

COPY . .

RUN apt-get update && \
    apt-get install -y \
        cmake \
        g++ \
        g++-10 \
        git \
        curl \
        libssl-dev \
        libmysqlclient-dev \
        odb \
        libodb-dev \
        libodb-mysql-dev && \
    rm -rf /var/cache/apt && \
    apt-get clean

RUN mkdir -p build/generated
    
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build ./build

FROM ubuntu:22.04 AS runtime

WORKDIR /app

RUN apt-get update && \
    apt-get install -y \
    libssl-dev \
    libmysqlclient-dev \
    libodb-2.4 \
    libodb-mysql-2.4 && \
    rm -rf /var/cache/apt && \
    apt-get clean
    
COPY --from=builder /app/build/AuthService /app/
COPY --from=builder /app/.env/cert.pem /app/
COPY --from=builder /app/.env/key.pem /app/

CMD ["/app/AuthService"]
