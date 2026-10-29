# Same dated public toolchain image as the pinned librpxloader revision.
FROM ghcr.io/wiiu-env/devkitppc:20260225
RUN apt-get update && apt-get install -y --no-install-recommends nodejs \
    && rm -rf /var/lib/apt/lists/*
RUN git clone https://github.com/wiiu-env/librpxloader.git /opt/hbl-librpxloader \
    && git -C /opt/hbl-librpxloader checkout --detach 7b055f15f2c0e31af5b0a4ceda44ecb1ec39cc34 \
    && make -C /opt/hbl-librpxloader -j2
WORKDIR /project
CMD ["make", "-C", "aroma", "RPXLOADER=/opt/hbl-librpxloader", "-j2"]
