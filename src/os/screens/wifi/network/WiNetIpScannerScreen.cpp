//
// Created by l-sha on 9/24/2025.
//

#include <WiFi.h>
#include <ESPping.h>
#include <WiFiClient.h>
#include <thread>

#include "os/component/InputNumberScreen.h"
#include "os/screens/wifi/network/WiNetIPScannerScreen.h"
#include "os/screens/wifi/WifiNetworkScreen.h"

void WiNetIPScannerScreen::init()
{
  configuration();
}

void WiNetIPScannerScreen::configuration()
{
  currentState = STATE_CONFIGURATION;
  Template::renderHead("IP Scanner");
  setEntries({
    "Start IP: " + std::to_string(startIp),
    "End IP: " + std::to_string(endIp),
  "Start Scan"
  });
}


void WiNetIPScannerScreen::scanIP()
{
  currentState = STATE_SCANNING_IP;
  Template::renderHead("IP Scanner");
  Template::drawStatusBody("Scanning...");

  auto currentIP = WiFi.localIP();
  if (currentIP[0] == 0 && currentIP[1] == 0 && currentIP[2] == 0 && currentIP[3] == 0)
  {
    setEntries({"No devices found"});
    return;
  }

  IPAddress wifiIp = WiFi.localIP();
  for (int i = startIp; i < endIp; i++)
  {
    IPAddress ip(wifiIp[0], wifiIp[1], wifiIp[2], i);
    if (Ping.ping(ip, 1))
    {
      ipList.emplace_back(ip.toString().c_str());
    }

    const int percent = static_cast<int>((i - startIp) / static_cast<float>(endIp - startIp) * 100);
    String progress = "[" + String(percent) + "%] Scanning...";
    Template::drawStatusBody(progress.c_str());
  }

  if (ipList.empty())
  {
    ipList.emplace_back("No devices found");
  }

  currentState = STATE_RESULT_IP;
  setEntries(ipList);
}

void WiNetIPScannerScreen::scanPort(const std::string& ip)
{
  currentState = STATE_SCANNING_PORT;
  Template::renderHead("Port Scan");
  Template::drawStatusBody("Start Scanning...");
  const std::vector<int> commonPorts = {
    21, // FTP
    22, // SSH
    23, // Telnet
    25, // SMTP
    53, // DNS
    80, // HTTP
    110, // POP3
    139, // NetBIOS
    143, // IMAP
    443, // HTTPS
    445, // Microsoft-DS
    465, // SMTPS
    587, // SMTP (submission)
    993, // IMAPS
    995, // POP3S
    1723, // PPTP
    3306, // MySQL
    3389, // RDP
    5900, // VNC
    8080, // HTTP-alt
    8443, // HTTPS-alt
    8888, // Alternate HTTP
    5000, // UPnP/Web servers
    5432, // PostgreSQL
    6379, // Redis
    27017, // MongoDB
    8000, // Alternate HTTP
    10000, // Webmin
  };

  std::vector<std::string> openPorts;

  const int portCount = commonPorts.size();
  for (int i = 0; i < portCount; i++) {
    WiFiClient client;
    const int port = commonPorts[i];
    if (client.connect(ip.c_str(), port)) {
      client.stop();
      openPorts.emplace_back(ip + ":" + std::to_string(port));
    }

    const int percent = static_cast<int>(i / static_cast<float>(portCount) * 100);
    String progress = "[" + String(percent) + "%] Scanning...";
    Template::drawStatusBody(progress.c_str());
  }

  if (openPorts.empty()) {
    openPorts.emplace_back("No port open");
  }

  currentState = STATE_RESULT_PORT;
  setEntries(openPorts);
}

void WiNetIPScannerScreen::onEnter(const std::string& entry)
{
  if (currentState == STATE_RESULT_IP)
  {
    if (entry == "No devices found")
    {
      onBack();
    } else
    {
      scanPort(entry);
    }
  } else if (currentState == STATE_CONFIGURATION)
  {
    if (entry.find("Start IP") != std::string::npos)
    {
      startIp = InputNumberScreen::popup("Start IP", startIp, 1, 254);
      if (startIp > endIp)
        endIp = startIp;

      configuration();
    } else if (entry.find("End IP") != std::string::npos)
    {
      endIp = InputNumberScreen::popup("End IP", endIp, 1, 254);
      if (endIp < startIp)
        startIp = endIp;

      configuration();
    } else if (entry == "Start Scan")
    {
      scanIP();
    }
  }
}

void WiNetIPScannerScreen::onBack()
{
  if (currentState == STATE_RESULT_IP)
  {
    ipList.clear();
    configuration();
  } else if (currentState == STATE_RESULT_PORT)
  {
    currentState = STATE_RESULT_IP;
    Template::renderHead("IP Scanner");
    setEntries(ipList);
  } else if (currentState == STATE_CONFIGURATION)
  {
    _global->setScreen(new WifiNetworkScreen());
  }
}
