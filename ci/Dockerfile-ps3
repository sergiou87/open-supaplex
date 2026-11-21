FROM flipacholas/ps3devextra:latest

MAINTAINER Sergio Padrino (@sergiou87)

# Update libvorbis, libogg, and install SDL2, debugnet and SDL2_mixer

RUN cd /tmp \
    && git clone https://github.com/sergiou87/ps3libraries ps3libraries \
    && cd ps3libraries \
    && sh ./libraries.sh 7 8 14 24 \
    && cd .. \
    && rm -Rf ps3libraries \
    && git clone https://github.com/sergiou87/SDL_PSL1GHT_Libs SDL_PSL1GHT_Libs \
    && cd SDL_PSL1GHT_Libs \
    && sh ./make_SDL_Libs.sh 5 \
    && cd .. \
    && rm -Rf SDL_PSL1GHT_Libs

WORKDIR /src
CMD ["/bin/ash"]
