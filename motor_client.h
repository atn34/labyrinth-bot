#ifndef MOTOR_CLIENT_H
#define MOTOR_CLIENT_H

#include <cassert>
#include <memory>

#include "libsocket/inetclientstream.hpp"

class PiClientInterface {
public:
  virtual ~PiClientInterface() {}
  virtual void snd(std::string buffer, size_t len) = 0;
};

class PiClient : public PiClientInterface {
public:
  PiClient() : pi_("bigapple", "10000", LIBSOCKET_IPv4) {}

  void snd(std::string buffer, size_t len) override {
    pi_.snd(buffer.data(), len);
  }

private:
  libsocket::inet_stream pi_;
};

class MotorClient {
public:
  static constexpr char HORIZONTAL_STEP_CC = 0b00000001;
  static constexpr char HORIZONTAL_STEP_CCW = 0b00000010;
  static constexpr char VERTICAL_STEP_CC = 0b00000100;
  static constexpr char VERTICAL_STEP_CCW = 0b00001000;

  MotorClient(std::unique_ptr<PiClientInterface> pi) : pi_(std::move(pi)) {}
  virtual ~MotorClient() = default;

  void step(int horizontal, int vertical) {
    assert(std::abs(horizontal) <= 16);
    assert(std::abs(vertical) <= 16);
    std::string message(16, 0);
    if (horizontal < 0) {
      for (int i = 0; i < -horizontal; ++i) {
        message[i] |= HORIZONTAL_STEP_CCW;
      }
    } else {
      for (int i = 0; i < horizontal; ++i) {
        message[i] |= HORIZONTAL_STEP_CC;
      }
    }
    if (vertical < 0) {
      for (int i = 0; i < -vertical; ++i) {
        message[i] |= VERTICAL_STEP_CCW;
      }
    } else {
      for (int i = 0; i < vertical; ++i) {
        message[i] |= VERTICAL_STEP_CC;
      }
    }
    pi_->snd(message, 16);
  }

private:
  std::unique_ptr<PiClientInterface> pi_;
};

#endif /* MOTOR_CLIENT_H */
