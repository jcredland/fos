all: test

test: pcap_runner.cpp pcap.h
#	clang -v -stdlib=libc++ -std=c++11 -o test pcap_runner.cpp
	clang -std=c++11 -lc++ -stdlib=libc++ -mmacosx-version-min=10.8 -o test pcap_runner.cpp
