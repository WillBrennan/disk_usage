FROM    ubuntu:21.10

ENV     DEBIAN_FRONTEND="noninteractive"
RUN     apt-get update -yq && \
        apt-get install -y      g++ build-essential mesa-common-dev libssl-dev \ 
                                wget python3 pip lsb libgl1-mesa-dev \
                                libxkbcommon-x11-0 libpulse-dev

RUN     mkdir -p /app/disker
WORKDIR /app/disker
RUN     pip install yaqti && \
        python3 -m yaqti install --version 6.2.0 --platform desktop --os linux --modules qtcharts

RUN     mkdir cmake && cd cmake && \
        wget https://github.com/Kitware/CMake/releases/download/v3.21.0/cmake-3.21.0-linux-x86_64.sh && \
        sh ./cmake-3.21.0-linux-x86_64.sh --skip-license
ENV     PATH="${PATH}:/app/disker/cmake/bin"

COPY    . .

RUN     mkdir build && cd build && \
        cmake .. -DCMAKE_PREFIX_PATH=/app/disker/qt/6.2.0/gcc_64 && make -j4