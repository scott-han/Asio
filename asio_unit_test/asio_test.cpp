//#define NOMINMAX
#include "stdafx.h"
#define BOOST_TEST_MODULE ImpExchange test
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <memory>
#include <chrono>
#include "test_client_handler.h"
#include "test_server_handler.h"
#include "test_server_mc.h"

namespace Test {
	/// This class sets up the necessary prerequisites for a unit test

	class AsioFixtures
	{
	public:
		AsioFixtures();


		TestSrvMultipleClient test_server;
	};
	AsioFixtures::AsioFixtures()
	{

	}
	BOOST_FIXTURE_TEST_SUITE(AsioTests, AsioFixtures)


	BOOST_AUTO_TEST_CASE(TestSingleServerMultiClient)
	{
		using namespace std;
		auto ts = std::thread([=]() {
			test_server.startListern();
		});
		//let server start
		std::this_thread::sleep_for(0.2s);
		int client_num = 100;
		std::vector<std::shared_ptr<TestClientHandler>> cv;
		boost::asio::io_service client_io_service;

		for (int i = 0; i < client_num; ++i) {
			cv.emplace_back(std::make_shared<TestClientHandler>());
			cv[i]->StartTest(client_io_service);
		}
		auto tc = std::thread([&]() {

			client_io_service.run();
		});
		ts.join();
		tc.join();
		for (auto& c : cv)
		{
			BOOST_CHECK_EQUAL_COLLECTIONS(c->msg_received.begin(), c->msg_received.end(),
				c->msg_to_send.begin(), c->msg_to_send.end());
		}
		
	}
	BOOST_AUTO_TEST_SUITE_END()
}// namespace Test {


