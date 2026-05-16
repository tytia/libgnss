//
// Created by Terry on 5/7/2026.
//

#ifndef LIBGNSS_GNSS_RECEIVER_HPP
#define LIBGNSS_GNSS_RECEIVER_HPP

#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE 4096
#endif

#include <array>
#include <string>

#include "libgnss/serial.hpp"
#include "libgnss/nmea_reader.hpp"
#include "ringbuffer/ringbuffer.hpp"

namespace libgnss
{

class GNSSReceiver
{
public:
  GNSSReceiver(std::string port_name, uint16_t baud_rate);
  ~GNSSReceiver();

  // disable copying
  GNSSReceiver(const GNSSReceiver&) = delete;
  GNSSReceiver& operator=(const GNSSReceiver&) = delete;

  void start();
  void stop();
  void write(const std::string& data);
  void configurePort(const std::string& port_name, int baud_rate);
  void setRTCMCallback(utils::Callback<std::vector<uint8_t>> callback);

  // addParser
  // addOneTimeParser

  template <typename TSentence>
  void setSentenceCallback(utils::Callback<TSentence> callback, const bool async = false)
  {
    nmea_reader_.setCustomCallback<TSentence>(std::move(callback), async);
  }

private:
  SerialPort serial_port_;
  nmea::NMEAReader nmea_reader_;
  jnk0le::Ringbuffer<uint8_t, SERIAL_RX_BUFFER_SIZE> read_buffer_;
  utils::Callback<std::vector<uint8_t>> rtcm_callback_;

  std::string port_name_;
  int baud_rate_;
};

}  // namespace libgnss

#endif  // LIBGNSS_GNSS_RECEIVER_HPP
