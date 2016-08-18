// CNOS Chat Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;

class UDPServer
{
public:
	UDPServer(boost::asio::io_service& io_service, short port)
		: io_service_(io_service),
		socket_(io_service, udp::endpoint(udp::v4(), port))
	{
		socket_.async_receive_from(
			boost::asio::buffer(data_, max_length), sender_endpoint_,
			boost::bind(&UDPServer::handle_receive_from, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive_from(const boost::system::error_code& error,
		size_t bytes_recvd)
	{
		if (!error && bytes_recvd > 0)
		{
			for (int i = 0; i < bytes_recvd; i++) {
				std::cout << data_[i];
			}
			std::cout << std::endl;

			socket_.async_receive_from(
				boost::asio::buffer(data_, max_length), sender_endpoint_,
				boost::bind(&UDPServer::handle_receive_from, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			socket_.async_receive_from(
				boost::asio::buffer(data_, max_length), sender_endpoint_,
				boost::bind(&UDPServer::handle_receive_from, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

private:
	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint sender_endpoint_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class UDPClient
{
public:
	UDPClient(
		boost::asio::io_service& io_service,
		const std::string& host,
		const std::string& port
	) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
		udp::resolver resolver(io_service_);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		endpoint_ = *iter;
	}

	~UDPClient()
	{
		socket_.close();
	}

	void send(const std::string& msg) {
		socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
	}

private:
	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint endpoint_;
};

void client() {
	std::string message;
	std::string ip;
	std::string port;

	std::cout << "Insert IP address: ";
	std::cin >> ip;
	std::cout << "Insert port: ";
	std::cin >> port;

	boost::asio::io_service io_service;
	UDPClient client(io_service, ip, port);

	while (std::cin >> message) {
		client.send(message);
	}
}

void server() {
	boost::asio::io_service io_serviceServer;
	UDPServer s(io_serviceServer, 13);
	io_serviceServer.run();
}

int main()
{
	boost::thread clientThread = boost::thread(client);
	boost::thread serverThread = boost::thread(server);

	clientThread.join();
	serverThread.join();				
	
	return 0;
}



