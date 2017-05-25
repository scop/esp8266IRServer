#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
 

WiFiManager wifiManager;


MDNSResponder mdns;
ESP8266WebServer server(80);
 
// set your pin here
IRsend irsend(4);
 
void handleRoot() {
 server.send(200, "text/html", "Please specify command! Form: /ir?code=xxx&bits=xx&protocol=x");
}
 
void handleIr(){
  String codestring=server.arg("code");
  String protocol=server.arg("protocol");
  String bitsstring=server.arg("bits");

  String deviceCode=server.arg("deviceCode");
  String subDeviceCode=server.arg("subDeviceCode");
  String obc=server.arg("obc");

//  String webOutput = "Protocol: "+protocol+"; Code: "+codestring+"; Bits: "+bitsstring + " - ("+deviceCode + subDeviceCode + obc +")";
  String webOutput = "";
  
  Serial.println(webOutput);

  unsigned long code =0;
  
  if ((codestring != "")&&(bitsstring != "")){
    //unsigned long code = codestring.toInt();
    char tarray[15]; 
    codestring.toCharArray(tarray, sizeof(tarray));
    code = strtoul(tarray,NULL,16);  
    // unsigned long code = atol(codestring);
  }
  
  if ((obc != "")&&(deviceCode != "")){
    //convert OBC & deviceCode to hex CodeString


   int iDeviceCode=StrToUL(deviceCode);
   int iDeviceCodeLSB=flipBits(iDeviceCode); 
   int iDeviceCodeLSB_INV=iDeviceCodeLSB ^ 0xFF; 
   dump(iDeviceCode, iDeviceCodeLSB, iDeviceCodeLSB_INV);
   
   int iSubDeviceCode;
   int iSubDeviceCodeLSB;
   int iSubDeviceCodeLSB_INV;
    if ((subDeviceCode == "")){
      iSubDeviceCode=iDeviceCode;
      iSubDeviceCodeLSB=iDeviceCodeLSB; 
      iSubDeviceCodeLSB_INV=iDeviceCodeLSB_INV; 
    } else {
       iSubDeviceCode=StrToUL(subDeviceCode);
       iSubDeviceCodeLSB=flipBits(iSubDeviceCode); 
       iSubDeviceCodeLSB_INV=iSubDeviceCodeLSB ^ 0xFF; 
    }

   int iOBC=StrToUL(obc);
   int iOBCLSB=flipBits(iOBC); 
   int iOBCLSB_INV=iOBCLSB ^ 0xFF; 
   dump(iOBC,iOBCLSB,iOBCLSB_INV);
   
   Serial.println(iDeviceCodeLSB,HEX);
   Serial.println(iOBCLSB,HEX);
   Serial.println("----");
   if (protocol=="Samsung") {
    code=combineBytes(iDeviceCodeLSB,iSubDeviceCodeLSB,iOBCLSB,iOBCLSB_INV);
   } else if (protocol=="NEC") {
    code=combineBytes(iDeviceCodeLSB,iDeviceCodeLSB_INV,iOBCLSB,iOBCLSB_INV);
   } else {
    code=0;
    server.send(404, "text/html", "Protocol not implemented for OBC!");
   }
   
   Serial.println("---");
   Serial.println(code,HEX);
   Serial.println("---");
   
  }
  
  if (code!=0){
    int bits = bitsstring.toInt();
    Serial.println(code,HEX);
    
    if (protocol == "NEC"){
      server.send(200, "text/html", webOutput);
      irsend.sendNEC(code, bits);
    }
    else if (protocol == "Sony"){
      server.send(200, "text/html", webOutput);
      irsend.sendSony(code, bits);
    }
    else if (protocol == "Whynter"){
      server.send(200, "text/html", webOutput);
      irsend.sendWhynter(code, bits);
    }
    else if (protocol == "LG"){
      server.send(200, "text/html", webOutput);
      irsend.sendLG(code, bits);
    }
    else if (protocol == "RC5"){
      server.send(200, "text/html", webOutput);
      irsend.sendRC5(code, bits);
    }
    else if (protocol == "RC6"){
      server.send(200, "text/html", webOutput);
      irsend.sendRC6(code, bits);
    }
    else if (protocol == "DISH"){
      server.send(200, "text/html", webOutput);
      irsend.sendDISH(code, bits);
    }
    else if (protocol == "SharpRaw"){
      server.send(200, "text/html", webOutput);
      irsend.sendSharpRaw(code, bits);
    }
    else if (protocol == "Samsung"){
      server.send(200, "text/html", webOutput);
      irsend.sendSAMSUNG(code, bits);
    }
    else if (protocol == "Gree"){
      server.send(200, "text/html", webOutput);
      // TODO length checks
      unsigned char bytes[8];
      char temp[] = "0x00";
      for (int i = 0; i < codestring.length(); i += 2) {
        temp[2] = codestring.charAt(i);
        temp[3] = codestring.charAt(i+1);
        bytes[i/2] = strtol(temp, NULL, 16);
      }
      irsend.sendGree(bytes);
    }
    else {
      server.send(404, "text/html", "Protocol not implemented!");
    }
  }
  else {
    server.send(404, "text/html", "Missing code or bits!");
  }
}

unsigned long StrToUL (String str){
    char tarray[15]; 
    str.toCharArray(tarray, sizeof(tarray));
    unsigned long code = strtoul(tarray,NULL,10);  
    return (code);
}

unsigned long combineBytes(int a1,int a2, int a3, int a4){
   unsigned long code=0;
   code=code+a1;
   code=(code<<8)+a2;
   code=(code<<8)+a3;
   code=(code<<8)+a4;
   return(code);
}

int flipBits(unsigned char b){
  return ( (b * 0x0202020202ULL & 0x010884422010ULL) % 1023);
}

void dump(int a1,int a2,int a3){
  Serial.print(a1,HEX);
  Serial.print("-");
  Serial.print(a2,HEX);
  Serial.print("-");
  Serial.println(a3,HEX);
}
void handleNotFound(){
  server.send(404, "text/plain", "404");
}

void handleReset(){
  wifiManager.resetSettings();
}

void setup(void){
  irsend.begin();
  Serial.begin(9600);

  
  wifiManager.autoConnect("IRSVR");
  
  if (mdns.begin("irsvr", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/ir", handleIr); 
    server.on("/reset",handleReset);

  server.onNotFound(handleNotFound);
    
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}
