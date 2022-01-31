#pragma once
#include <iostream>
#include < cstdint >

namespace xop
{
	namespace net
	{
		class ÑonnectionInfo
		{
			uint16_t port_ = 60001;
			const char* host_ = "127.0.0.1";
		public:
			uint16_t port() {
				return port_;
			}
			const char* host() { return host_; }
		};
	};
}