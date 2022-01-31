#pragma once
#include "scrc_common.h"

namespace scrc
{

	const std::size_t max_encr_len = 0x8002;

	static const std::uint32_t smpl_keysize_bytes = 16;
	static const std::uint32_t smpl_keysize_longs = 4;

	/******************************************************************************
	 * typedefs
	 */
	 /* union used to access key as both a string and as unsigned longs */
	union key_t
	{
		std::uint8_t  keys[smpl_keysize_bytes];
		std::uint32_t keyl[smpl_keysize_longs];
	};

	union sMesU
	{
		std::uint8_t  datas[8];
		std::uint32_t datal[2];
	};

	//structure for creation new encipher key
	struct crt_init_s
	{
		//temporary counter for sending data to client counter as a decipher counter, 
		//only for a session of first secured keys transmission, 
		//this counter will be sent without enciphering
		uint32_t client_cou;	
			
		//following data should be enciphered deuring the sending to the client

		uint32_t server_cou;	//the new counter for for the getting data from a client. Client for cipher, serves for decipher
		uint32_t sIVx;	
		key_t sKeyx;
	};


	class scrypto_ {

	private:

		//default key
		key_t baseKey = { 0x40,0x6E,0x17,0x3D,0x9B,0x18,0xFA,0x6D,0xAB,0xC9,0x9A,0x8B,0xFD,0x43,0x6B,0x05 };

		//32-bit initialization vector 
		static const uint32_t secIV = 0x74db2eb8;

		key_t sKey;
		uint32_t sIV;

		//xtea encipher
		void xtea_encipher(uint32_t* v, uint32_t const* k, uint32_t num_rounds = 32) 
		{
			unsigned int i;
			uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9e3779b9;
			for (i = 0; i < num_rounds; i++) {
				v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
				sum += delta;
				v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
			}
			v[0] = v0; v[1] = v1;
		}


		//message to encipher / decipher
		//data - message for encipher
		//len - length of message, size of maximum length defined in max_encr_len
		//receive_flag - true for receive, false for send
		//return value true for correct data during receive, for send is no matter		
		void data_cipher(uint8_t* data, size_t len, uint32_t& counter)
		{
			//maxium length for ecryption
			//check the message legth
			if (len > max_encr_len)
			{
				std::cout << "The size for enciphering is too big = " << len << ". The size was reduced to "<< max_encr_len << std::endl;
				len = max_encr_len;
			}
			//check if input data is  valid
			if ((NULL == data) || !len)
			{
				return;
			}

			uint32_t  i, idx = 0;
			sMesU sMsg = {};

			//exclude check summ

			uint8_t cs[2] = {0, 0};

			while (true)
			{
				// Set block to be enciphered. 1st 32 bits are the IV. The second  32 bits are the current CTR value.
				sMsg.datal[0] = sIV;
				sMsg.datal[1] = counter;

				xtea_encipher(&sMsg.datal[0], &sKey.keyl[0]);

				// increment counter for each time. 
				counter++;

				// XOR ciphered block with message to be sent. 
				//the cipher block length is (64 bits == 8 bytes).
				for (i = 0; i < sizeof(sMsg) && idx < len; ++i, ++idx)
				{
					data[idx] ^= sMsg.datas[i];	
				}
				//check the finish of the cipher
				if (idx >= len)
				{
					break;
				}
			}
		}
	protected: 
		//checksum size
		static const  size_t cs_size = sizeof(uint16_t);   //size of check sum = 2 bytes
		
		//counter for decipher
		// created by this device
		//send to remoute for encipher, decipher data from remote device
		std::uint32_t cou_for_decipher;

		//counter for encipher
		//received from remote, ecncipher data for remote device
		std::uint32_t cou_for_encipher;

	public:

		//constructor with defaul values
		//copy base key and secure initial vector
		scrypto_() : sIV(secIV), sKey(), cou_for_decipher(0), cou_for_encipher(0)
		{
			//copy secure key
			for (int i = 0; i < smpl_keysize_longs; i++) sKey.keyl[i] = baseKey.keyl[i];
		}


		//get random data into a buffer, 
		//the size of the data is limited by max_encr_len size !!!
		static void getRandom(uint8_t* buffer, size_t size )
		{
			//check if corect input parameters
			if ((buffer == nullptr) || !size) return;
			if (size > max_encr_len - cs_size)
			{
				std::cout << "Size of random generator buffer is overised\n";
				return;
			}

			//get random number
			std::uniform_int_distribution<int> range_distribution(1, 0xff);
			uint64_t time = uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());

			std::mt19937 random_number_engine((uint32_t)(time | (time >> 32)) ); // pseudorandom number generator
			auto get_number = std::bind(range_distribution, random_number_engine);
			int random_roll = get_number();
			uint32_t size1 = (uint32_t)size, i;

			for (i = 0; i < size1; ++i)
			{
				buffer[i] = (uint8_t)get_number();
			}
		}

		//set decipher by value;
		void set_deciper_cou(uint32_t cou)
		{
			cou_for_decipher = cou;
		}

		//get set decipher by the random generator
		void set_deciper_cou()
		{
			getRandom((uint8_t*)&cou_for_decipher, sizeof(cou_for_decipher));	//generate internal counter
		}

		//set counter for remote devive
		void set_encipher_cou(uint32_t cou)
		{
			cou_for_encipher = cou;
		}

		//encipher data 
		//data - message for encipher
		//len - length of the message with the size of the checksumm, 
		//maximum length is defined in max_encr_len
		void data_encipher(uint8_t* msg, size_t len)
		{
			data_cipher(msg, len,  cou_for_encipher);
		}

		//decipher data 
		//data - message for decipher
		//len - length of the message with the size of the checksum, 
		//size of maximum length defined in max_encr_len
		void data_decipher(uint8_t* msg, size_t len)
		{
			//use cou_from_rem for the cipher
			data_cipher(msg, len, cou_for_decipher);
		}

		//clear memory and check to avoid of optimiization
		bool mem_clr(uint8_t * arr, size_t size)
		{
			memset(arr, 0, size); //clear the memory

			//check if memory == 0
			for (size_t i = 0; i < size; i++)
			{
				if (arr[i]) return false;	// false if any byte !=0
			}
			return true;
		}

		//Write secure keys for server
		// f_server: for server = true, for client = false
		//clear memory and check the clearance (for avoid of optimzation)
		bool copy_cr_data(crt_init_s* data, bool f_server = true)
		{
			if (f_server)
			{
				//copy counter for server
				cou_for_decipher = data->server_cou; //server will use this counter for decipher data from client
			}
			else
			{ 
				cou_for_encipher = data->server_cou;	//client will use this counter for encipher data for server
			}
		
			sIV = data->sIVx;
			//erace for security aims
			for (int i = 0; i < smpl_keysize_longs; ++i)
			{
				//copy uint32 data
				sKey.keyl[i] = data->sKeyx.keyl[i];
				//erase for security aims
			}
			//clear the memory and check for == 0
			return mem_clr((uint8_t*)data, sizeof(crt_init_s));
		}
	};
};
