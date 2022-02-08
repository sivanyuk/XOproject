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


namespace xop
{
	namespace net
	{

		struct message_header
		{
			uint32_t size = 0;
			CustMsgTypes  id{};
			
			//the lowest byte of encipher counter 
			uint8_t encip_cou_1b = 0;	//s&
			uint8_t dummy =		0;	//s&

			
		};

		// Message Body contains a header and a std::vector, containing raw bytes
		// of infomation. This way the message can be variable length, but the size
		// in the header must be updated.
		struct message_
		{
			// Header & Body vector
			message_header header{};
			std::vector<uint8_t> body;

			// returns size of entire message packet in bytes
			size_t size() const
			{
				return body.size();
			}

			// Override for std::cout compatibility - produces friendly description of message
			friend std::ostream& operator << (std::ostream& os, const message_& msg)
			{
				std::cout << "Message size = " << msg.header.size << "n";
				os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
				return os;
			}

			// Convenience Operator overloads - These allow us to add and remove stuff from
			// the body vector as if it were a stack, so First in, Last Out. These are a 
			// template in itself, because we dont know what data type the user is pushing or 
			// popping, so lets allow them all. NOTE: It assumes the data type is fundamentally
			// Plain Old Data (POD). TLDR: Serialise & Deserialise into/from a vector

			// Pushes any POD-like data into the message buffer
			template<typename DataType>
			friend message_& operator << (message_& msg, const DataType& data)
			{
				
				//std::cout << " Output Message size size = " << msg.header.size << "n";	//S&
				// Check that the type of the data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache current size of vector, as this will be the point we insert the data
				size_t i = msg.body.size();

				// Resize the vector by the size of the data being pushed
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// Physically copy the data into the newly allocated vector space
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				// Recalculate the message size
				msg.header.size = (uint32_t)msg.size();
				//std::cout << " Output Message size size = " << msg.header.size << "\n";	//S&
				// Return the target message so it can be "chained"
				return msg;
			}

			// Pulls any POD-like data form the message buffer
			template<typename DataType>
			friend message_& operator >> (message_& msg, DataType& data)
			{
				//std::cout << "Message in, header.size = " << msg.header.size << "\n";
				// Check that the type of the data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

				// Cache the location towards the end of the vector where the pulled data starts
				size_t i = msg.body.size() - sizeof(DataType);

				// Physically copy the data from the vector into the user variable
				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				// Shrink the vector to remove read bytes, and reset end position
				msg.body.resize(i);

				// Recalculate the message size
				msg.header.size = (uint32_t)msg.size();

				// Return the target message so it can be "chained"
				return msg;
			}			
		};



		//------changed by s&
		struct message : public message_		//s&
		{
			//flag if received data is OK. Will be used in tests
			bool fOK = false;

			void clear()
			{
				body.resize(0);
				header.size = 0;
				std::cout << "Clear the message\n";	//S&
			}

			//copy data into the body from a buffer
			void copy(uint8_t* data, size_t size)
			{
				body.resize(size);
				std::memcpy(body.data(), data, size);	//copy data into the body
				std::cout << "Data was copied from buffer: " << size << "\n";	//S&
				header.size = (uint32_t)size;

			}

			//copy data into the "body"
			message& operator = (std::vector<uint8_t>& vect)
			{
				body = vect;
				header.size = (uint32_t)body.size();
				return *this;
			}

			//copy assigment
			message& operator = (const message& msg)
			{
				body = msg.body;
				header.size = msg.header.size;
				header.id = msg.header.id;
				return *this;
			}

			//copy a string into a "message" body
			message& operator = (const std::string& s)
			{
				size_t size = s.size();
				header.size = (uint32_t)size + 1;
				//check if nothing to copy
				if (!size)
				{
					return *this;
				}
				body.resize(size + 1);
				std::copy(s.data(), s.data() + size , body.data());
				body.data()[size] = 0;	//add 0 to the end
				return *this;
			}

			//if message is empty
			const char* nothing = "";

			//convert msg to string
			char* to_string()
			{
				size_t size = body.size();
				//hea
				header.size = (uint32_t)size;
				//check if nothing to copy
				if (!size) return (char*)nothing;
				//check the 0 at the end of the strinng
				if (!body.data()[size - 1])
				{
					return (char*)body.data();	//the sting has 0 at the end
				}
				//resize if /0 absent at the end of the string
				body.resize(size + 1);
				body.data()[size] = 0;

				return (char*)body.data();
			}


			bool operator == (message& msg)
			{
				if (!(body == msg.body)) return false;
				else if (header.id != msg.header.id) return false;
				return header.size == msg.body.size();
			}


			//copy vector
			void copy(std::vector<uint8_t>& vect)
			{
				body = vect;
				header.size = (uint32_t)body.size();
			}

			//msg.move 
			//move data from source to vector body
			void move(std::vector<uint8_t>& vect) {
				body = std::move(vect);
				header.size = (uint32_t)body.size();
			}
		};
		//----------------- s&


		// An "owned" message is identical to a regular message, but it is associated with
		// a connection. On a server, the owner would be the client that sent the message, 
		// on a client the owner would be the server.

		// Forward declare the connection
		//template <typename T>
		class connection;

		//template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection> remote = nullptr;
			message msg;

			// Again, a friendly string maker
			friend std::ostream& operator<<(std::ostream& os, const owned_message& msg)
			{
				os << msg.msg;
				return os;
			}
		};		
	}
}

