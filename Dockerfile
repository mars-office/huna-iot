FROM python:latest AS builder
WORKDIR /app
RUN pip install --upgrade platformio
COPY ./platformio.ini ./platformio.ini
RUN pio lib install
COPY . .
ARG DEPLOYABLE_VERSION
RUN echo '#define VERSION "${DEPLOYABLE_VERSION}"' > ./src/version.h
RUN pio run --target build
RUN echo "${DEPLOYABLE_VERSION}" > ./.pio/build/esp32doit-devkit-v1/VERSION.txt

FROM alpine:latest AS runner
WORKDIR /app
COPY --from=builder /app/.pio /app
WORKDIR /
RUN mkdir -p /firmware

LABEL org.opencontainers.image.source=https://github.com/mars-office/huna-iot