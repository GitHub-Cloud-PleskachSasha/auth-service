FROM ubuntu:22.04 AS builder

WORKDIR /app

COPY . .

RUN apt-get update
RUN apt-get install -y cmake g++ git curl

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build ./build

FROM ubuntu:22.04 AS runtime

WORKDIR /app

COPY --from=builder /app/build/StockExchange /app/

CMD ["/app/StockExchange"]
