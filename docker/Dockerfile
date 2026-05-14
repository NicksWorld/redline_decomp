FROM docker.io/library/debian:stable-slim

RUN dpkg --add-architecture i386
RUN apt-get update -y
RUN apt-get install git wine wine64 wine32 wget unzip -y

RUN export WINEDEBUG=-all

RUN wine wineboot

RUN git clone https://seed.tty.garden/nmcdaniel/MSVC600
RUN mv MSVC600 ~/.wine/drive_c/msvc

RUN wget https://github.com/Kitware/CMake/releases/download/v3.26.6/cmake-3.26.6-windows-i386.zip
RUN unzip cmake-3.26.6-windows-i386.zip -d ~/.wine/drive_c
RUN mv ~/.wine/drive_c/cmake-3.26.6-windows-i386 ~/.wine/drive_c/cmake
RUN rm cmake-3.26.6-windows-i386.zip

COPY entrypoint.sh entrypoint.sh
RUN chmod +x entrypoint.sh
ENTRYPOINT [ "./entrypoint.sh" ]
