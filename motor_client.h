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
  static constexpr int HORIZONTAL = 0;
  static constexpr int VERTICAL = 1;

  MotorClient() : pi_(std::unique_ptr<PiClient>(new PiClient)) {}
  MotorClient(std::unique_ptr<PiClientInterface> pi) : pi_(std::move(pi)) {}
  virtual ~MotorClient() = default;

  void step(int motor, int value) {
    struct Message {
        int motor;
        int value;
    };
    Message message;
    message.motor = motor;
    message.value = value;
    std::string serialized((char *)(&message), sizeof(Message));
    pi_->snd(serialized, sizeof(Message));
  }

private:
  std::unique_ptr<PiClientInterface> pi_;
};

#endif /* MOTOR_CLIENT_H */
