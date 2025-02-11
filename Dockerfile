FROM ubuntu:22.04 AS builder

WORKDIR /app

COPY . .

RUN apt-get update && \
    apt-get install -y cmake g++ git curl libssl-dev libmysqlclient-dev && \
    rm -rf /var/cache/apt && \
    apt-get clean

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build ./build

FROM ubuntu:22.04 AS runtime

WORKDIR /app

COPY --from=builder /app/build/StockExchange /app/

COPY --from=builder /app/cert.pem /app/
COPY --from=builder /app/key.pem /app/

CMD ["/app/StockExchange"]
