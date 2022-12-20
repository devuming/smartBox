// this method makes a HTTP connection to the server
void httpRequest(String url)
{
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect("15.164.111.44", 80)) {
    Serial.println("Connecting...");
    
    // send the HTTP PUT request
    client.println("GET " + url + " HTTP/1.1");
    client.println(F("Host: 15.164.111.44"));
    client.println("Connection: keep-Alive");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    delay(1000);
  }
}