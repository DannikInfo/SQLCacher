FROM mtd/builder:noDNN as builder

WORKDIR /builder/

RUN git clone git@github.com:MetidaAI/Core.git
RUN cd Core &&\
      cmake . -DBUILD_WITH_REDIS=ON && \
      make install -j"$(nproc)"


COPY . .
RUN cmake . && make -j4


FROM ubuntu:latest

RUN apt update -y && apt install -y libmysqlcppconn7v5

WORKDIR app

COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /builder/SQLCache .
RUN ldconfig

CMD ["./SQLCache"]