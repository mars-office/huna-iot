FROM python:latest AS builder
WORKDIR /app
RUN pip install --upgrade platformio
COPY ./platformio.ini ./platformio.ini
RUN pio lib install
COPY . .
RUN pio run

FROM alpine:latest AS runner
WORKDIR /app
COPY --from=builder /app/.pio /app
CMD "cp -R /app/* /firmware/"

LABEL org.opencontainers.image.source=https://github.com/mars-office/huna-iot