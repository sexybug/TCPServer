//
// TimeServer.cpp
//
// This sample demonstrates the TCPServer and ServerSocket classes.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>


using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServer;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;

//最大接收数据长度
#define BUFFER_SIZE 3

class TimeServerConnection : public TCPServerConnection
	/// This class handles all client connections.
	///
	/// A string with the current date and time is sent back to the client.
{
public:
	TimeServerConnection(const StreamSocket& s, const std::string& format) :
		TCPServerConnection(s),
		_format(format)
	{
	}

	void run()
	{
		Application& app = Application::instance();
		app.logger().information("Request from " + this->socket().peerAddress().toString());
		try
		{
			char* buffer = new char[BUFFER_SIZE];
			/// Returns the number of bytes received.
			/// A return value of 0 means a graceful shutdown
			/// of the connection from the peer.
			
			int n = socket().receiveBytes(buffer, BUFFER_SIZE);
			std::cout << "Client Message: " << std::endl;
			std::cout << n << std::endl;
			for (int i = 0; i < n; ++i) {
				printf("%02x", buffer[i]);
			}
			printf("\n");
			n = socket().receiveBytes(buffer, BUFFER_SIZE);
			std::cout << "Client Message: " << std::endl;
			std::cout << n << std::endl;
			for (int i = 0; i < n; ++i) {
				printf("%02x", buffer[i]);
			}
			printf("\n");
			n = socket().receiveBytes(buffer, BUFFER_SIZE);
			std::cout << "Client Message: " << std::endl;
			std::cout << n << std::endl;
			for (int i = 0; i < n; ++i) {
				printf("%2x", buffer[i]);
			}
			printf("\n");

			Timestamp now;
			std::string dt(DateTimeFormatter::format(now, _format));
			dt.append("\r\n");
			std::string s2(1024,'a');
			dt.append(s2);
			dt.append("bb\r\n");
			socket().sendBytes(dt.data(), (int)dt.length());
		}
		catch (Poco::Exception& exc)
		{
			app.logger().log(exc);
		}
	}

private:
	std::string _format;
};


class TimeServerConnectionFactory : public TCPServerConnectionFactory
	/// A factory for TimeServerConnection.
{
public:
	TimeServerConnectionFactory(const std::string& format) :
		_format(format)
	{
	}

	TCPServerConnection* createConnection(const StreamSocket& socket)
	{
		return new TimeServerConnection(socket, _format);
	}

private:
	std::string _format;
};


class TimeServer : public Poco::Util::ServerApplication
	/// The main application class.
	///
	/// This class handles command-line arguments and
	/// configuration files.
	/// Start the TimeServer executable with the help
	/// option (/help on Windows, --help on Unix) for
	/// the available command line options.
	///
	/// To use the sample configuration file (TimeServer.properties),
	/// copy the file to the directory where the TimeServer executable
	/// resides. If you start the debug version of the TimeServer
	/// (TimeServerd[.exe]), you must also create a copy of the configuration
	/// file named TimeServerd.properties. In the configuration file, you
	/// can specify the port on which the server is listening (default
	/// 9911) and the format of the date/time string sent back to the client.
	///
	/// To test the TimeServer you can use any telnet client (telnet localhost 9911).
{
public:
	TimeServer() : _helpRequested(false)
	{
	}

	~TimeServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		int a=loadConfiguration(); // load default configuration files, if present
		std::cout << a << " properties file find." << std::endl;
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false));
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("A server application that serves the current date and time.");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		if (_helpRequested)
		{
			displayHelp();
		}
		else
		{
			// get parameters from configuration file
			unsigned short port = (unsigned short)config().getInt("TimeServer.port", 9911);
			std::cout << "server port: " << port << std::endl;
			std::string format(config().getString("TimeServer.format", DateTimeFormat::ISO8601_FORMAT));

			// set-up a server socket
			ServerSocket svs(port);
			// set-up a TCPServer instance
			TCPServer srv(new TimeServerConnectionFactory(format), svs);
			// start the TCPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the TCPServer
			srv.stop();
		}
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};

int main(int argc, char** argv)
{
	TimeServer app;
	return app.run(argc, argv);
}
