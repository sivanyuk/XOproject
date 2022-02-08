
/*
Created by Sergey Ivanyuk 2022
GitHub: https://github.com/sivanyuk

*/



#include "test.h"



int main()
{
    scrc::scrypto crypto_srv = scrc::scrypto();     //
    scrc::scrypto crypto_clnt = scrc::scrypto();    //client crypto

    std::vector<uint8_t> vec = std::vector<uint8_t>();

    int err = 0;
    std::cout << "Key generation Test\n";
    //create crypto keys
    crypto_srv.keys_prep(vec);
    //send to client
    crypto_clnt.keys_receive(vec);
    //answer from client with new decipher key
    if (!crypto_srv.keys_final(vec))
    {
        err++;
        std::cout << "Key generation error\n";
    }
    else
    {
        std::cout << "Key generation OK\n";
    }

    xop::net::message msg, msg2;
    msg.header.id = xop::net::CustMsgTypes::ServerPing;
    //copy vector into a mesage
    msg = vec;

    msg2 = msg;

    std::cout << "Encription message test\n";
    //encrypt message
    crypto_srv.msg_to_out(msg);

    //decrypt message
    if (!crypto_clnt.msg_from_in(msg))
    {
        err++;
        std::cout << "Encryption message cs error\n";
    }
    else if (!(msg == msg2))
    {
        err++;
        std::cout << "Encryption message error\n";
    }
    else
    {
        std::cout << "Encryption message OK\n";
    }

    std::cout << "Erros during the test: " << err << "\n";

    //simple test of the ramdom generator
    int i;
    for (i = 0; i < 0x10; ++i)
    {
        uint32_t rand = scrc::rand32();
        std::cout << std::hex << "A random number is " << rand << std::endl;
    }
    return 0;
}
