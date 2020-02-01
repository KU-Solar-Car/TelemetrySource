#include "OAuth.h"

OAuth::OAuth(FirebaseCert* cert, HttpClient* client) {
  this->_cert = cert;
  this->_client = client;
}

String OAuth::getAccessToken() {
  String jwt = buildJwt();
  return jwt;
  /*String body = "grant_type=urn:ietf:params:oauth:grant-type:jwt-bearer&assertion=" + jwt;
  int result = _client->post(_cert->token_uri, "application/x-www-form-urlencoded", body);
  if (result != 0) {
    return "";
  }

  String response = _client->responseBody();
  MatchState state((char*) response.c_str(), response.length());
  state.Match("^{\r?\n?[ ]*\"access_token\"[ ]*:[ ]*\"([^\"]*)\".*");
  char token[45]; // I think the tokens are 45 characters long
  state.GetCapture(token, 0);

  return String("");*/
}

String OAuth::buildJwt() {
  String header = makeJwtHeader();
  String claimSet = makeJwtClaimSet();
  String signature = makeJwtSignature(header, claimSet);
  return header + "." + claimSet + "." + signature;
}

String OAuth::makeJwtHeader() {
  String header = "{\"alg\":\"RS256\", \"typ\":\"JWT\"}";
  unsigned char base64[int(ceil(header.length() / 3.0) * 4)];
  _base64.encode_base64((unsigned char*) header.c_str(), header.length(), base64);
  return String((char*) base64);
}

String OAuth::makeJwtClaimSet() {
  String claimSet = "{"
                    "\"iss\":\"" + _cert->client_email + "\","
                    "\"scope\":\"https://www.googleapis.com/auth/firebase\","
                    "\"aud\":\"" + _cert->token_uri + "\","
                    "\"exp\":" + (millis() + 1000 * 60 * 30) + "," // 30 minutes after current time
                    "\"iat\":" + millis() + "";
  "}";

  unsigned char base64[int(ceil(claimSet.length() / 3.0) * 4)];
  _base64.encode_base64((unsigned char*) claimSet.c_str(), claimSet.length(), base64);
  return String((char*) base64);
}

String OAuth::makeJwtSignature(String header, String claimSet) {
  SHA256 hasher;
  hasher.update((header + "." + claimSet).c_str(), header.length() + claimSet.length() + 1);

  char hashed[hasher.hashSize()];
  hasher.finalizeHMAC(_cert->private_key.c_str(), _cert->private_key.length(), hashed, hasher.hashSize());
  unsigned char base64[int(ceil(hasher.hashSize() / 3.0) * 4)];
  _base64.encode_base64((unsigned char*) hashed, hasher.hashSize(), base64);
  return String((char*) base64);
}
