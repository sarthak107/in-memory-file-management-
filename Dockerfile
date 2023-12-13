FROM gcc:latest

WORKDIR /usr/src/app

COPY . .

RUN g++ -o fs-cli fs-cli.cpp

CMD ["./fs-cli"]


