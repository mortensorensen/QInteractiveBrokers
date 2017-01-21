# Dockerfile for QInteractiveBrokers
#
# To build:
#		docker build --build-arg KDB_URL=https://kx.com/<YOUR KEY>/<VERSION>/linuxx86.zip -t qib/qib .
#
# To run:
#		docker run -ti qib/qib

FROM ubuntu:16.04

MAINTAINER Morten Sorensen <https://github.com/mortensorensen/QInteractiveBrokers>

ENV QHOME=/opt/q \
		QPATH=/opt/q \
		QIB=/qib

RUN mkdir -p $QIB \
	&& apt-get update && apt-get install -y \
	wget \
	unzip \
	
	build-essential \
	gcc-multilib \
	g++-multilib \

	# For CMake
	software-properties-common \
	&& add-apt-repository ppa:george-edison55/cmake-3.x \
	&& apt-get install -y cmake \
	
	&& rm -rf /var/lib/apt/lists/*

# Add QIB to installation
COPY . $QIB/

ARG KDB_URL

# Install kdb+
RUN wget -P /tmp ${KDB_URL} \
	&& unzip /tmp/$(basename ${KDB_URL}) -d $(dirname $QHOME) \
	&& ln -s $QHOME/l32/q /usr/bin/q \
	&& rm /tmp/*

WORKDIR $QHOME

# Install qutil and qspec
RUN wget -O qutil.zip https://github.com/nugend/qutil/archive/master.zip \
	&& wget -O qspec.zip https://github.com/nugend/qspec/archive/master.zip \
	&& unzip '*.zip' \
	&& mkdir qpackages \
	&& mv qutil-master qpackages/qutil \
	&& mv qspec-master qpackages/qspec \
	&& rm *.zip \
	&& ln -s $QHOME/qpackages/qutil/lib/bootstrap.q bootstrap.q \
	&& ln -s $QHOME/qpackages/qutil/lib qutil \
	&& ln -s $QHOME/qpackages/qspec/lib qspec \
	&& cp -f qpackages/qutil/q_q.sample q.q

WORKDIR $QIB

CMD ["bash"]
