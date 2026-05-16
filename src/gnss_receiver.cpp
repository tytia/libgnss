//
// Created by Terry on 5/7/2026.
//

#include <utility>

#include "libgnss/gnss_receiver.hpp"

namespace libgnss
{

GNSSReceiver::GNSSReceiver(std::string port_name, const uint16_t baud_rate)
  : port_name_(std::move(port_name))
  , baud_rate_(baud_rate)
{
}

}  // namespace libgnss