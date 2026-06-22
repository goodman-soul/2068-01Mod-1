FROM gcc:latest

WORKDIR /app

COPY . .

RUN mkdir -p build && \
    gcc src/main.c src/shape.c src/circle.c src/obj_diag.c src/rectangle.c src/shape_container.c -I include -o build/demo

CMD ["./build/demo"]
