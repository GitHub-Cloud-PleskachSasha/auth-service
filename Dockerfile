    # syntax=docker/dockerfile:1.4
    FROM ubuntu:22.04 AS builder

    # Встановлення необхідних пакетів
    RUN apt-get update && apt-get install -y \
        cmake g++ git curl \
        libmysqlclient-dev libboost-all-dev \
        && rm -rf /var/lib/apt/lists/*

    WORKDIR /app

    # Завантаження та встановлення cpp-httplib
    RUN git clone --depth=1 https://github.com/yhirose/cpp-httplib.git \
        && mv cpp-httplib /usr/local/include/httplib \
        && rm -rf cpp-httplib

    # Завантаження та встановлення jwt-cpp
    RUN git clone --depth=1 https://github.com/Thalhammer/jwt-cpp.git \
        && mv jwt-cpp/include /usr/local/include/jwt-cpp \
        && rm -rf jwt-cpp

    # Завантаження та встановлення mysql_orm
    RUN git clone --depth=1 https://github.com/Ethiraric/mysql_orm.git \
        && mv mysql_orm/include /usr/local/include/mysql_orm \
        && rm -rf mysql_orm

    # Копіюємо вихідний код у контейнер
    COPY . .

    RUN rm -rf build \
        && cmake -B build -DCMAKE_BUILD_TYPE=Release \
        && cmake --build build --target StockExchange -j$(nproc) \
        && ls -la build


    # Фінальний образ
    FROM ubuntu:22.04

    WORKDIR /app
    COPY --from=builder /app/build/StockExchange /app/StockExchange

    RUN chmod +x /app/StockExchange

    CMD ["bash", "-c", "ls -l /app]
