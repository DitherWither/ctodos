FROM alpine:latest as builder

RUN apk update && apk add --no-cache build-base musl-dev clang autoconf automake
WORKDIR /app
COPY . /app
RUN autoreconf --install .
RUN ./configure
RUN make all LDFLAGS=-static
RUN mkdir /out && cp ./src/ctodos /out/ && cp ./template.html /out/

FROM scratch

COPY --from=builder /out/ /

ENTRYPOINT [ "/ctodos" ]