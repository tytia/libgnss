//
// Created by Terry on 5/9/2026.
//

#include "serial.hpp"

namespace libgnss
{

SerialPort::SerialPort()
  : port_(nullptr)
{
}

SerialPort::~SerialPort()
{
  close();
}

bool SerialPort::open(const std::string& device, int baud_rate)
{
  close();
  if (!_open(device, baud_rate))
  {
    close();
    return false;
  }
  return true;
}

bool SerialPort::_open(const std::string& device, int baud_rate)
{
  sp_return ret = sp_get_port_by_name(device.c_str(), &port_);
  if (ret != SP_OK || port_ == nullptr)
  {
    return false;
  }

  ret = sp_open(port_, SP_MODE_READ_WRITE);
  if (ret != SP_OK)
  {
    return false;
  }

  if (
    sp_set_baudrate(port_, baud_rate) != SP_OK ||
    sp_set_bits(port_, 8) != SP_OK ||
    sp_set_parity(port_, SP_PARITY_NONE) != SP_OK ||
    sp_set_stopbits(port_, 1) != SP_OK ||
    sp_set_flowcontrol(port_, SP_FLOWCONTROL_NONE) != SP_OK)
  {
    return false;
  }

  return true;
}

void SerialPort::close()
{
  if (port_ != nullptr)
  {
    sp_close(port_);
    sp_free_port(port_);
    port_ = nullptr;
  }
}

}  // namespace libgnss