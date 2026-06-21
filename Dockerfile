FROM gcc:latest

WORKDIR /app

COPY . .

RUN mkdir -p build && \
    gcc src/main.c src/shape.c src/circle.c -I include -o build/demo

CMD ["./build/demo"]
