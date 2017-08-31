#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "motor_client.h"

using ::testing::_;
using ::testing::SaveArg;

class MockPiClient : public PiClientInterface {
public:
  MOCK_METHOD2(snd, void(std::string buffer, size_t len));
};

TEST(MotorClient, Test) {
  auto *mock_pi_client = new MockPiClient;
  std::unique_ptr<PiClientInterface> pi_client(mock_pi_client);

  MotorClient motor_client(std::move(pi_client));

  std::string message;
  EXPECT_CALL(*mock_pi_client, snd(_, _)).WillRepeatedly(SaveArg<0>(&message));

  motor_client.step_to(1, 0);
  EXPECT_EQ(std::string("\x01\x00\x00\x00\x00\x00\x00\x00", 8), message);
  motor_client.step_to(1, 8);
  EXPECT_EQ(std::string("\x01\x00\x00\x00\x08\x00\x00\x00", 8), message);
  motor_client.step_to(0, 0);
  EXPECT_EQ(std::string("\x00\x00\x00\x00\x00\x00\x00\x00", 8), message);
  motor_client.step_to(0, 8);
  EXPECT_EQ(std::string("\x00\x00\x00\x00\x08\x00\x00\x00", 8), message);
}
