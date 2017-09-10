//#define NOMINMAX
#include "stdafx.h"
#define BOOST_TEST_MODULE ImpExchange test
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <memory>
#include <chrono>
#include "test_client_handler.h"
#include "test_server_handler.h"


		namespace Test {
			/// This class sets up the necessary prerequisites for a unit test
			
			class AsioFixtures
			{
			public:
				AsioFixtures();

				TestClientHandler test_client;
				TestServerHandler test_server;
			};
			AsioFixtures::AsioFixtures()
			{
				
			}
			BOOST_FIXTURE_TEST_SUITE(AsioTests, AsioFixtures)


			BOOST_AUTO_TEST_CASE(TestErrorCode)
			{
				using namespace std;
				auto ts = std::thread([=]() {
					test_server.startListern();
				});
				//let server start
				std::this_thread::sleep_for(0.2s);
				auto tc = std::thread([=]() {
					test_client.StartTest();
				});


			}
			BOOST_AUTO_TEST_SUITE_END()
		}// namespace Test {
	

