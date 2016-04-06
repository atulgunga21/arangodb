////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Dr. Oreste Costa-Panaia
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef LIB_REST_ENDPOINT_IP_H
#define LIB_REST_ENDPOINT_IP_H 1

#include "Endpoint/Endpoint.h"

namespace arangodb {
class EndpointIp : public Endpoint {
 protected:
  EndpointIp(DomainType, EndpointType, TransportType, EncryptionType, int, bool,
             std::string const&, uint16_t const);

 public:
  ~EndpointIp();

 public:
  static uint16_t const _defaultPortHttp;
  static uint16_t const _defaultPortVpp;
  static char const* _defaultHost;

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief connect the socket
  //////////////////////////////////////////////////////////////////////////////

  TRI_socket_t connectSocket(addrinfo const*, double, double);

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief connect the endpoint
  //////////////////////////////////////////////////////////////////////////////

  TRI_socket_t connect(double, double) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief disconnect the endpoint
  //////////////////////////////////////////////////////////////////////////////

  virtual void disconnect() override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief init an incoming connection
  //////////////////////////////////////////////////////////////////////////////

  virtual bool initIncoming(TRI_socket_t) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get port
  //////////////////////////////////////////////////////////////////////////////

  int port() const override { return _port; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get host
  //////////////////////////////////////////////////////////////////////////////

  std::string host() const override { return _host; }

 private:
  std::string const _host;
  uint16_t const _port;
  bool const _reuseAddress;
};
}

#endif
