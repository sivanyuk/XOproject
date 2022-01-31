/*
Created by Sergey Ivanyuk 2022
GitHub: https://github.com/sivanyuk

*/

#pragma once

#include <net_message.h>
#include "scrc_common.h"
#include "scrc_crypto1.h"

namespace scrc
{
	class scrypto : public scrypto_
	{
    //private:
        
	public:

        // prepare new secret keys
        //keys generate and encipher by default secrete keys, then transmit to a client
        //new keys replace current secret keys 
        //answer from the client will be processed with the new keys

		void keys_prep(std::vector<uint8_t> & new_keys )
		{
            //resize vector
            new_keys.resize(sizeof(crt_init_s));

            std::cout << "Keys preparation\n";

            //get random information for new keys
            getRandom((uint8_t*)new_keys.data(), sizeof(crt_init_s)); 

            crt_init_s* server_cr_data = (crt_init_s*)new_keys.data();

            ////for debugging aims
            //server_cr_data->client_cou = 20;
            //server_cr_data->server_cou = 1;
            //server_cr_data->sIVx = 3;


             //write client counter.
            //this counter will be used only for one session during the sending new secure keys from the server to the client
            cou_for_encipher = server_cr_data->client_cou;

            //make a copy of the new keys 
            //this keys will replace the secret keys in current crypto oject
            std::vector<uint8_t> copy_keys = new_keys;

            //encipher message  until crc_data
            data_encipher((uint8_t*)new_keys.data() + sizeof(crt_init_s::client_cou),
                sizeof(crt_init_s) - sizeof(crt_init_s::client_cou));
            
            //as ready (crypted ) data for client so copy new data into a current crypto object
            //copy generated kes and clear source data
            if (!copy_cr_data((scrc::crt_init_s*)copy_keys.data()))
            {
                std::cout << "The data wasn't cleared in " << __LINE__ << "\n";  //for debug
            }
        }

        //the final episode of exchange secret keys
        //server receive encipher key from the client
        bool keys_final(std::vector<uint8_t>& new_keys)
        {
            //check the aray size
            if (new_keys.size() != sizeof(crt_init_s)) return false;
            //std::cout << "Final Key\n";
            //decipher the message
            data_decipher(new_keys.data(), new_keys.size());
            //write checksum into the message and check the message's size
            if (!check_cs(new_keys.data(), new_keys.size())) 

                if (enchipher_cs(new_keys.data(), new_keys.size()))
                {
                    std::cout << "Final Key. Key generation is wrong \n";   //is something went wrong
                    return false;
                }
            crt_init_s* cr_data = (crt_init_s*)new_keys.data();

            //copy from the message into the encipher key
            cou_for_encipher = cr_data->client_cou;
            std::cout << "Final Key. Generation is OK\n";
            return true;
        }

        //client receive keys from server
        //current crypto gets the secret keys and resturn answer for the server
        bool keys_receive(std::vector<uint8_t>& new_keys)
        {
            //check the aray size
            if (new_keys.size() != sizeof(crt_init_s)) return false;
            std::cout << "Key receive\n";

            crt_init_s* client_cr_data = (crt_init_s*)(new_keys.data());


            set_deciper_cou(client_cr_data->client_cou);

            //decipher the message
            data_decipher((uint8_t*)new_keys.data() + sizeof(crt_init_s::client_cou),
                sizeof(crt_init_s) - sizeof(crt_init_s::client_cou));

            copy_cr_data((crt_init_s*)new_keys.data(), false);   //copy data 

            //get random information 
            getRandom((uint8_t*)new_keys.data(), (uint8_t)(new_keys.size() - cs_size));  //for debug
            //client_cr_data->client_cou = 10;

            //read new decipher counter for next sessions
            cou_for_decipher = client_cr_data->client_cou;


            //ensipher the message
            //write checksum into the message and check the message's size
            write_cs((uint8_t*)new_keys.data(), sizeof(crt_init_s));

            //encipher the message
            data_encipher((uint8_t*)new_keys.data(), sizeof(crt_init_s));
            return true;
        }

        //calculat the checksum and encipher msg 
        //msg - message for encipher
        //len - length of the message with the size of the checksum, 
        //maximum length is defined in max_encr_len
        bool enchipher_cs(uint8_t* data, size_t size)
        {
            //write checksum into the message and check the message's size
            if (!write_cs(data, size)) return false;

            //encipher the message
            data_encipher(data, size);
            return true;
        }

        //decipher msg and calculató the checksum 
        //msg - message for encipher
        //len - length of the message with the size of the checksum, 
        //maximum length is defined in max_encr_len
        bool dechipher_cs(uint8_t* data, size_t size)
        {
            //decipher the message
            data_decipher(data, size);
            //write checksum into the message and check the message's size
            check_cs(data, size);
        }


        //calculate a checksum
        //size should include th check summ. It means size = message size + checksum size
        //just a simple cs
        uint16_t get_crc(uint8_t* data, size_t size)
        {
            if (size <= cs_size) return 0;  //check size
            
            uint8_t cs[2] = { 0,0 };
            const uint8_t offset = 11;
            uint8_t current_offset = offset;
            for (size_t i = 0; i < size - cs_size; ++i)
            {
                cs[0] += data[i];
                cs[1] ^= data[i] ^ current_offset;
                current_offset += offset;
            }
            return cs[0] + ((uint16_t)cs[1] << 8);  //return a check summ
        }

        //calculate a checksum
        //size should include th check summ. It means size = message size + check summ size
        // the must be  > cs_size  (2 bytes) and  <= max_encr_len 
        //just simple cs
        //result - is check the message size. false if mesage is out range
        bool write_cs(uint8_t* data, size_t size)
        {
            //check cs size 
            if ((size <= cs_size) || (size > max_encr_len))  return false;

            uint16_t cs = get_crc(data, size);
            data[size - cs_size] = (uint8_t)cs;
            data[size - cs_size + 1] = (uint8_t)(cs >> 8);
            return true;
        }

        //check a checksum
        //size should include the checksum. It means size = message size + checksum size
        // the must be  > cs_size  (2 bytes) and  <= max_encr_len 
        // if size < cs_size - checksum isn't compared and result = true
        //just simple cs
        bool check_cs(uint8_t* data, size_t size)
        {
            //check length of data cs size 
            if (size <= cs_size) return true;
            else if (size > max_encr_len)  return false;

            //calculate a checksumm
            uint16_t cs = get_crc(data, size);
            uint16_t cs2 = data[size - cs_size] | (uint16_t)data[size - cs_size + 1] << 8;
            return cs == cs2;
        }

        //encipher a message before sendig;
        void msg_to_out(xop::net::message& msg)
        {
            //get the body size
            size_t size = msg.body.size();
            std::cout << "Message encipher. Size = " << size << std::endl;

            if (!size) return;   //exit if length of message == 0
            
            //write correct size with the checksum
            msg.header.size = (uint32_t)(size + cs_size);

            //write on byte of counter for encipher
            msg.header.encip_cou_1b = (uint8_t)cou_for_encipher;
            
            //calculate checksum for the header
            uint16_t cs = get_crc((uint8_t*)&msg.header, sizeof(xop::net::message::header));
            
            //calculate checksum for te body
            uint16_t cs2 = get_crc(msg.body.data(), size);

            cs += cs2;  //to summ both cs

            //resize the body
            msg.body.resize(size + cs_size);
            
            //write checksumm into the buffer
            uint8_t* data = msg.body.data();
            data[size] = (uint8_t)cs;
            data[size+1] = (uint8_t)(cs >> 8);

            //encipher the message
            data_encipher(data, size + cs_size);
        }

        //decipher a message before receiving;
        bool msg_from_in(xop::net::message& msg)
        {
            //get the body size
            size_t size = msg.body.size()  ;
            std::cout << "Message decipher. Size = " << size << std::endl;
            if (!size)
            {
                //msg.fOK = true;
                return true;   //exit if length of message == 0
            }
            if (size != msg.header.size)    //check real size with value in the haeeader
            {
                //msg.fOK = false;
                return false;   //exit if length of message == 0

            }
            //get 2 lowest bytes of decipher couner
            uint8_t new_decip_cou = msg.header.encip_cou_1b;

            //to avoid country desynchronization. It may be removed in the future
            if (new_decip_cou > (uint8_t)cou_for_decipher)
            {
                std::cout /*<< std::ios::showbase*/ << std::hex << "Crypto adr = " << this 
                    << ". Decipher counter attention 1. Real / expected value = " <<
                    new_decip_cou << " / " << cou_for_decipher << std::noshowbase << std::dec << std::endl;
                cou_for_decipher = (cou_for_decipher & 0xffffff00L) + new_decip_cou;
                
            }
            else if (new_decip_cou < (uint8_t)cou_for_decipher)
            {
                std::cout << std::hex << "Crypto adr = " << this 
                    << ".Decipher counter attention 2. Real/expected value = " <<
                    new_decip_cou << " / " << cou_for_decipher << std::dec << std::endl;
                //possibly bytes 
                cou_for_decipher = (cou_for_decipher & (uint32_t)0xffff0000) + new_decip_cou + (uint32_t)0x100;
            }
            else
            {
                std::cout << std::hex << "Counter is OK, Crypto adr = " << this 
                    << ". Decipher counter = " << cou_for_decipher << std::noshowbase << std::dec <<  std::endl;
            }

            //pointer to the data - 
            uint8_t* data = msg.body.data();

            data_decipher(data, size);

            //write of counter for encipher
            //calculate checksum for the header
            uint16_t cs = get_crc((uint8_t*)&msg.header, sizeof(xop::net::message::header));

            //data size without checksum
            size -= cs_size;
            //calculate checksum for the body
            uint16_t cs2 = get_crc(data, size);

            cs += cs2;  //to summ both cs
            
            //the size of the data is
            if (!((uint8_t)cs == data[size]) && ((uint8_t)(cs >> 8) == data[size + 1]))    //compare checksum
            {
                std::cout << "Checksumm error\n";
                return false;
            }
            //resize the body
            msg.body.resize(size);

            msg.header.size = (uint32_t)size; //size without check summ

            return true;
        }
	};

}


