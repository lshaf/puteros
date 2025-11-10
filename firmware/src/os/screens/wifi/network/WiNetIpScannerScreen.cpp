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
    {"Start IP", std::to_string(startIp)},
    {"End IP", std::to_string(endIp)},
    {"Start Scan"}
  });
}


void WiNetIPScannerScreen::scanIP()
{
  currentState = STATE_SCANNING_IP;
  Template::renderHead("IP Scanner");
  Template::renderStatus("Scanning...");

  IPAddress currentIP = WiFi.localIP();
  if (currentIP[0] == 0 && currentIP[1] == 0 && currentIP[2] == 0 && currentIP[3] == 0)
  {
    setEntries({{"No devices found"}});
    return;
  }

  for (int i = startIp; i < endIp; i++)
  {
    const int percent = static_cast<int>((i - startIp) / static_cast<float>(endIp - startIp) * 100);
    Template::renderProgress(percent, "Scanning...");

    if (i == currentIP[3]) continue;
    IPAddress ip(currentIP[0], currentIP[1], currentIP[2], i);
    if (Ping.ping(ip, 1))
    {
      const auto avg = String(ceil(Ping.averageTime())) + " ms";
      ipList.push_back({ip.toString().c_str(), avg.c_str()});
    }
  }

  if (ipList.empty())
  {
    ipList.push_back({"No devices found"});
  }

  currentState = STATE_RESULT_IP;
  setEntries(ipList);
}

void WiNetIPScannerScreen::scanPort(const std::string& ip)
{
  currentState = STATE_SCANNING_PORT;
  Template::renderHead("Port Scan");
  Template::renderStatus("Start Scanning...");
  const std::vector<std::pair<int, std::string>> commonPorts = {
    {20, "FTP Data"},      {21, "FTP"},            {22, "SSH"},             {23, "Telnet"},
    {25, "SMTP"},          {53, "DNS"},            {67, "DHCP"},            {68, "DHCP"},
    {69, "TFTP"},          {80, "HTTP"},           {110, "POP3"},           {123, "NTP"},
    {135, "MS-RPC"},       {137, "NetBIOS"},       {139, "NetBIOS"},        {143, "IMAP"},
    {161, "SNMP"},         {162, "SNMP Trap"},     {389, "LDAP"},           {443, "HTTPS"},
    {445, "SMB/Microsoft-DS"}, {465, "SMTPS"},     {514, "Syslog"},         {554, "RTSP"},
    {587, "SMTP (submission)"}, {631, "IPP"},      {636, "LDAPS"},          {873, "rsync"},
    {993, "IMAPS"},        {995, "POP3S"},         {1024, "Reserved"},      {1025, "NFS/IIS"},
    {1194, "OpenVPN"},     {1352, "LotusNotes"},   {1433, "MSSQL"},         {1521, "Oracle"},
    {1720, "H.323"},       {1723, "PPTP"},         {2049, "NFS"},           {2181, "Zookeeper"},
    {2222, "SSH Alt"},     {2375, "Docker"},       {2376, "DockerTLS"},     {27017, "MongoDB"},
    {32768, "RPC"},        {3306, "MySQL"},        {3389, "RDP"},           {3690, "SVN"},
    {49152, "WinRPC"},     {49153, "WinRPC"},      {49154, "WinRPC"},       {49155, "WinRPC"},
    {49156, "WinRPC"},     {49157, "WinRPC"},      {5000, "UPnP/Web servers"}, {5060, "SIP/PostgreSQL"},
    {5061, "SIPS (TLS)"},  {5062, "SIP Alt"},      {5080, "SIP Alt"},       {5081, "SIPS Alt"},
    {5160, "SIP (PJSIP)"}, {5432, "PostgreSQL"},   {5555, "ADB"},           {5900, "VNC"},
    {5985, "WinRM HTTP"},  {5986, "WinRM HTTPS"},  {6060, "SIP Alt"},       {6061, "SIPS Alt"},
    {6379, "Redis"},       {8000, "Alternate HTTP"},{8080, "HTTP Proxy/HTTP-alt"}, {8443, "HTTPS Alt/HTTPS-alt"},
    {8888, "Alternate HTTP"}, {9000, "SonarQube"}, {9100, "PJL/JetDirect"}, {9200, "Elastic"},
    {9999, "Urchin"},      {10000, "Webmin"},      {11211, "Memcached"}
  };


  std::vector<ListEntryItem> openPorts;

  const unsigned int portCount = commonPorts.size();
  for (int i = 0; i < portCount; i++)
  {
    const int percent = static_cast<int>(i / static_cast<float>(portCount) * 100);
    Template::renderProgress(percent, "Scanning...");

    WiFiClient client;
    const int port = commonPorts[i].first;
    if (client.connect(ip.c_str(), port)) {
      client.stop();
      openPorts.push_back({ip + ":" + std::to_string(port), commonPorts[i].second});
    }
  }

  if (openPorts.empty()) {
    openPorts.push_back({"No port open"});
  }

  currentState = STATE_RESULT_PORT;
  setEntries(openPorts);
}

void WiNetIPScannerScreen::onEnter(const ListEntryItem entry)
{
  if (currentState == STATE_RESULT_IP)
  {
    if (entry.label == "No devices found")
    {
      onBack();
    } else
    {
      scanPort(entry.label);
    }
  } else if (currentState == STATE_CONFIGURATION)
  {
    if (entry.label == "Start IP")
    {
      startIp = InputNumberScreen::popup("Start IP", startIp, 1, endIp);
      configuration();
    } else if (entry.label == "End IP")
    {
      endIp = InputNumberScreen::popup("End IP", endIp, startIp, 254);
      configuration();
    } else if (entry.label == "Start Scan")
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

void WiNetIPScannerScreen::onEscape()
{
  ipList.clear();
  _global->setScreen(new WifiNetworkScreen());
}