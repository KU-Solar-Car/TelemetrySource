#ifndef OAUTH_H
#define OAUTH_H

#include "base64.h"
#include "FirebaseCert.h"
#include <Arduino.h>
#include <SHA256.h>
#include <ArduinoHttpClient.h>
#include <Regexp.h>

class OAuth {
  public:
    OAuth(FirebaseCert* cert, HttpClient* client);
    String getAccessToken();
  private:
    FirebaseCert* _cert;
    HttpClient* _client;
    Base64 _base64;
    String buildJwt();
    String makeJwtHeader();
    String makeJwtClaimSet();
    String makeJwtSignature(String header, String claimSet);
};

#endif
