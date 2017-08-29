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

  motor_client.step(0, 0);
  EXPECT_EQ("", message);

  motor_client.step(1, 0);
  EXPECT_EQ(MotorClient::HORIZONTAL_STEP_CC, message[0]);

  motor_client.step(-1, 0);
  EXPECT_EQ(MotorClient::HORIZONTAL_STEP_CCW, message[0]);

  motor_client.step(0, 1);
  EXPECT_EQ(MotorClient::VERTICAL_STEP_CCW, message[0]);

  motor_client.step(0, -1);
  EXPECT_EQ(MotorClient::VERTICAL_STEP_CC, message[0]);

  motor_client.step(1, 1);
  EXPECT_EQ(MotorClient::HORIZONTAL_STEP_CC | MotorClient::VERTICAL_STEP_CCW,
            message[0]);

  motor_client.step(-1, 1);
  EXPECT_EQ(MotorClient::HORIZONTAL_STEP_CCW | MotorClient::VERTICAL_STEP_CCW,
            message[0]);

  motor_client.step(1, -1);
  EXPECT_EQ(MotorClient::HORIZONTAL_STEP_CC | MotorClient::VERTICAL_STEP_CC,
            message[0]);

  motor_client.step(-1, -1);
  EXPECT_EQ(MotorClient::HORIZONTAL_STEP_CCW | MotorClient::VERTICAL_STEP_CC,
            message[0]);
}
