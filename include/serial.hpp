//
// Created by Terry on 5/9/2026.
//

#ifndef LIBGNSS_SERIAL_HPP
#define LIBGNSS_SERIAL_HPP

#include <cstdint>
#include <string>
#include <libserialport.h>

namespace libgnss
{

class SerialPort
{
public:
  SerialPort();
  ~SerialPort();

  // disable copying
  SerialPort(const SerialPort&) = delete;
  SerialPort& operator=(const SerialPort&) = delete;

  bool open(const std::string& device, int baud_rate);
  void close();

  [[nodiscard]] bool isOpen() const;

  int read(uint8_t* buffer, size_t size);
  int write(const uint8_t* buffer, size_t size);

private:
  bool _open(const std::string& device, int baud_rate);

  sp_port* port_;
};

}  // namespace libgnss

#endif  // LIBGNSS_SERIAL_HPP
