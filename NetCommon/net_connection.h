/*
	MMO Client/Server Framework using ASIO
	"Happy Birthday Mrs Javidx9!" - javidx9

	Videos:
	Part #1: https://youtu.be/2hNdkYInj4g
	Part #2: https://youtu.be/UbjxGvrDrbw

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2020 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019, 2020

	Changed by Sergey Ivanyuk 2022
	GitHub: https://github.com/sivanyuk


*/

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "..\scrc\scrc_crypto.h"	//s& crypto librarry




namespace xop
{
	namespace net
	{

		// Connection
		// Forward declare
		class server_interface;

		//template<typename T>
		class connection: public std::enable_shared_from_this<connection>
		{
		public:
			// A connection is "owned" by either a server or a client, and its
			// behaviour is slightly different bewteen the two.
			enum class owner
			{
				server,
				client
			};

		public:
			// Constructor: Specify Owner, connect to context, transfer the socket
			//				Provide reference to incoming message queue
			connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message>& qIn)
				: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
			{
				m_nOwnerType = parent;

				// Construct validation check data
				if (m_nOwnerType == owner::server)
				{
					// Connection is Server -> Client, construct random data for the client
					// to transform and send back for validation
					m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

					//// Pre-calculate the result for checking when the client responds
					m_nHandshakeCheck = scramble(m_nHandshakeOut);
				}
				else
				{
					// Connection is Client -> Server, so we have nothing to define, 
					m_nHandshakeIn = 0;
					m_nHandshakeOut = 0;
				}

				//for separate keys for each session //s&
				std::cout << "Here is a new connection \n";
				//crypto_srv = scrc::scrypto();     //create new crypto keys

			}

			virtual ~connection()
			{}

			// This ID is used system wide - its how clients will understand other clients
			// exist across the whole system.
			uint32_t GetID() const
			{
				return id;
			}

		public:
			void ConnectToClient(xop::net::server_interface* server, uint32_t uid = 0)
			{
				if (m_nOwnerType == owner::server)
				{
					if (m_socket.is_open())
					{
						id = uid;

						// Was: ReadHeader();

						// A client has attempted to connect to the server, but we wish
						// the client to first validate itself, so first write out the
						// handshake data to be validated
						//WriteValidation();

						//form and send crypto kys to client
						skey_validation();	//s&

						// Next, issue a task to sit and wait asynchronously for precisely
						// the validation data sent back from the client
						//ReadValidation(server);

						//get an answer with encipher counter from a client and check it
						skey_ReadVal();	

					}
				}
			}

			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				// Only clients can connect to servers
				if (m_nOwnerType == owner::client)
				{
					// Request asio attempts to connect to an endpoint
					asio::async_connect(m_socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								// Was: ReadHeader();

								// First thing server will do is send packet to be validated
								// so wait for that and respond
								//ReadValidation();
								
								//get  new crypto keys from server, answer to the server
								skey_ReadVal();	//s&
							}
						});
				}
			}


			void Disconnect()
			{
				if (IsConnected())
					asio::post(m_asioContext, [this]() { m_socket.close(); });
			}

			bool IsConnected() const
			{
				return m_socket.is_open();
			}

			// Prime the connection to wait for incoming messages
			void StartListening()
			{

			}

		public:
			// ASYNC - Send a message, connections are one-to-one so no need to specifiy
			// the target, for a client, the target is the server and vice versa
			//Pay attention the message msg is changed during this function
			//for save msg sending use Send_s function
			void Send(/*const */message& msg)
			{
				crypto_keys.msg_to_out(msg);	//changed by s& -	crypto information before sending 
				asio::post(m_asioContext,
					[this, msg]()
					{
						// If the queue has a message in it, then we must 
						// assume that it is in the process of asynchronously being written.
						// Either way add the message to the queue to be output. If no messages
						// were available to be written, then start the process of writing the
						// message at the front of the queue.
						bool bWritingMessage = !m_qMessagesOut.empty();
						m_qMessagesOut.push_back(msg);
						if (!bWritingMessage)
						{
							WriteHeader();
						}
					});
			}
			
			// Safe send message to thr server
			//msg isn't changed in this function
			// ASYNC - Send a message, connections are one-to-one so no need to specifiy
			// the target, for a client, the target is the server and vice versa
			void Send_s(const message& msg)
			{
				message msg1 = msg;	//copy construcctor
				Send(msg1);
			}


		private:
			const size_t max_payload = scrc::max_encr_len;
			
			// ASYNC - Prime context to write a message header
			void WriteHeader()
			{
				// If this function is called, we know the outgoing message queue must have 
				// at least one message to send. So allocate a transmission buffer to hold
				// the message, and issue the work - asio, send these bytes
				asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header)),
					[this](std::error_code ec, std::size_t length)
					{
						// asio has now sent the bytes - if there was a problem
						// an error would be available...
						if (!ec)
						{
							// ... no error, so check if the message header just sent also
							// has a message body...
							if (m_qMessagesOut.front().body.size() > 0)
							{
								// ...it does, so issue the task to write the body bytes
								WriteBody();
							}
							else
							{
								// ...it didnt, so we are done with this message. Remove it from 
								// the outgoing message queue
								m_qMessagesOut.pop_front();

								// If the queue is not empty, there are more messages to send, so
								// make this happen by issuing the task to send the next header.
								if (!m_qMessagesOut.empty())
								{
									WriteHeader();
								}
							}
						}
						else
						{
							// ...asio failed to write the message, we could analyse why but 
							// for now simply assume the connection has died by closing the
							// socket. When a future attempt to write to this client fails due
							// to the closed socket, it will be tidied up.
							std::cout << "[" << id << "] Write Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC - Prime context to write a message body
			void WriteBody()
			{
				// If this function is called, a header has just been sent, and that header
				// indicated a body existed for this message. Fill a transmission buffer
				// with the body data, and send it!
				asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// Sending was successful, so we are done with the message
							// and remove it from the queue
							m_qMessagesOut.pop_front();

							// If the queue still has messages in it, then issue the task to 
							// send the next messages' header.
							if (!m_qMessagesOut.empty())
							{
								WriteHeader();
							}
						}
						else
						{
							// Sending failed, see WriteHeader() equivalent for description :P
							std::cout << "[" << id << "] Write Body Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC - Prime context ready to read a message header
			void ReadHeader()
			{
				// If this function is called, we are expecting asio to wait until it receives
				// enough bytes to form a header of a message. We know the headers are a fixed
				// size, so allocate a transmission buffer large enough to store it. In fact, 
				// we will construct the message in a "temporary" message object as it's 
				// convenient to work with.
				asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header)),
					[this](std::error_code ec, std::size_t length)
					{
						//m_msgTemporaryIn.fOK = false;	//wrong by default

						if (!ec)
						{
							// A complete message header has been read
							// has a body to follow...

							if (m_msgTemporaryIn.header.size > 0)
							{
								//-------
								//check the message size - added by s&
								if (m_msgTemporaryIn.header.size > max_payload)	
								{
									std::cout << "Wrong message's size. Size should be <= 0x8002 (max_payload) bytes. The connection will be closed \n";
									std::cout << "[" << id << "], message ID = " << (uint32_t)m_msgTemporaryIn.header.id << "\n";
									//close the connection
									m_socket.close();
									//-----------------
								}
								else
								{
									// ...it does, so allocate enough space in the messages' body
									// vector, and issue asio with the task to read the body.
									m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
									ReadBody();
								}
							}
							else
							{
								// it doesn't, so add this bodyless message to the connections
								// incoming message queue
								AddToIncomingMessageQueue();
							}
						}
						else
						{
							// Reading form the client went wrong, most likely a disconnect
							// has occurred. Close the socket and let the system tidy it up later.
							std::cout << "[" << id << "] Read Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC - Prime context ready to read a message body
			void ReadBody()
			{
				// If this function is called, a header has already been read, and that header
				// request we read a body, The space for that body has already been allocated
				// in the temporary message object, so just wait for the bytes to arrive...
				asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// ...and they have! The message is now complete, so add
							// the whole message to incoming queue
							
							// decipher the message
							if (!crypto_keys.msg_from_in(m_msgTemporaryIn))	//changed by s&
							{
								//Do something
							}
							AddToIncomingMessageQueue();
						}
						else
						{
							// As above!
							std::cout << "[" << id << "] Read Body Fail.\n";
							m_socket.close();
						}
					});
			}

			// "Encrypt" data
			uint64_t scramble(uint64_t nInput)
			{
				uint64_t out = nInput ^ 0xDEADBEEFC0DECAFE;
				out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0x0F0F0F0F0F0F0F) << 4;
				return out ^ 0xC0DEFACE12345678;
			}

			// ASYNC - Used by both client and server to write validation packet
			//-----
			void WriteValidation()
			{
				asio::async_write(m_socket, asio::buffer(&m_nHandshakeOut, sizeof(uint64_t)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// Validation data sent, clients should sit and wait
							// for a response (or a closure)
							if (m_nOwnerType == owner::client)
								ReadHeader();
						}
						else
						{
							m_socket.close();
						}
					});
			}

			void ReadValidation(xop::net::server_interface* server = nullptr)
			{
				asio::async_read(m_socket, asio::buffer(&m_nHandshakeIn, sizeof(uint64_t)),
					[this, server](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_nOwnerType == owner::server)
							{
								// Connection is a server, so check response from client

								// Compare sent data to actual solution
								if (m_nHandshakeIn == m_nHandshakeCheck)
								{
									// Client has provided valid solution, so allow it to connect properly
									std::cout << "Client Validated" << std::endl;
									//server->OnClientValidated(this->shared_from_this());

									// Sit waiting to receive data now
									ReadHeader();
								}
								else
								{
									// Client gave incorrect data, so disconnect
									std::cout << "Client Disconnected (Fail Validation)" << std::endl;
									m_socket.close();
								}
							}
							else
							{
								// Connection is a client, so solve puzzle
								m_nHandshakeOut = scramble(m_nHandshakeIn);

								// Write the result
								WriteValidation();
							}
						}
						else
						{
							// Some biggerfailure occured
							std::cout << "Client Disconnected (ReadValidation)" << std::endl;
							m_socket.close();
						}
					});
			}


			//// ASYNC - Used by both client and server to write validation packet
			////----------------------------- changed by s&
			//for for validation and getting new crypto keys 
			//form and send new crypto keys to a client
			virtual void skey_validation()
			{
				//prepare new secret keys for this connection
				crypto_keys.keys_prep(m_msgTemporaryIn.body);	//s&

				asio::async_write(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),	//
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// Validation data sent, clients should sit and wait
							// for a response (or a closure)
							m_bValidHandshake = true;
							if (m_nOwnerType == owner::client)
								ReadHeader();
						}
						else
						{
							m_socket.close();
						}
					});
			}

			//get answer with encipher counter and from client and check it
			void skey_ReadVal(xop::net::server_interface* server = nullptr)
			{
				//resize vector for input data
				m_msgTemporaryIn.body.resize(sizeof(scrc::crt_init_s));
				

				//read the answer from the client
				asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
					[this, server](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_nOwnerType == owner::server)
							{
								// Connection is a server, so check response from client
								if (crypto_keys.keys_final(m_msgTemporaryIn.body))
								{
									// Client has provided valid solution, so allow it to connect properly
									std::cout << "Client Validated" << std::endl;
									//server->OnClientValidated(this->shared_from_this());	//s&

									// Sit waiting to receive data now
									ReadHeader();
								}
								else
								{
									// Client gave incorrect data, so disconnect
									std::cout << "Key generation error\n";
									std::cout << "Client Disconnected (Fail Validation)" << std::endl;
									m_socket.close();
								}
							}
							else
							{
								// Connection is a client, so solve puzzle
								//decipher new keys and put them in crypto_keys
								crypto_keys.keys_receive(m_msgTemporaryIn.body);
								// Write the result
								asio::async_write(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),	//
									[this](std::error_code ec, std::size_t length)
									{
										if (!ec)
										{
											// Validation data sent, clients should sit and wait
											// for a response (or a closure)

											ReadHeader();
											std::cout << "Connection accepted" << std::endl;
										}
										else
										{
											//if error, the close the connection
											m_socket.close();
											std::cout << "Connection closed" << std::endl;
										}
									});


							}
						}
						else
						{
							// Some biggerfailure occured
							std::cout << "Client Disconnected (ReadValidation)" << std::endl;
							m_socket.close();
						}
					});
			
			}

			//--------------- s&



			// Once a full message is received, add it to the incoming queue
			void AddToIncomingMessageQueue()
			{
				// Shove it in queue, converting it to an "owned message", by initialising
				// with the a shared pointer from this connection object
				if (m_nOwnerType == owner::server)
					m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
				else
					m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });

				// We must now prime the asio context to receive the next message. It 
				// wil just sit and wait for bytes to arrive, and the message construction
				// process repeats itself. Clever huh?
				ReadHeader();
			}

		protected:
			// Each connection has a unique socket to a remote 
			asio::ip::tcp::socket m_socket;

			// This context is shared with the whole asio instance
			asio::io_context& m_asioContext;

			//for separate keys for each session //s&
			scrc::scrypto crypto_keys; //= scrc::scrypto();     //s&

			// This queue holds all messages to be sent to the remote side
			// of this connection
			tsqueue<message> m_qMessagesOut;

			// This references the incoming queue of the parent object
			tsqueue<owned_message>& m_qMessagesIn;

			// Incoming messages are constructed asynchronously, so we will
			// store the part assembled message here, until it is ready
			message m_msgTemporaryIn;

			// The "owner" decides how some of the connection behaves
			owner m_nOwnerType = owner::server;


			// Handshake Validation			
			uint64_t m_nHandshakeOut = 0;
			uint64_t m_nHandshakeIn = 0;
			uint64_t m_nHandshakeCheck = 0;

			bool m_bValidHandshake = false;
			bool m_bConnectionEstablished = false;

			uint32_t id = 0;
		};
	}
}

