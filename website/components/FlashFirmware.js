"use client";

import {Fragment, useRef, useState} from "react";
import {ESPLoader, Transport} from "esptool-js";

const usbPortFilters = [
  { usbVendorId: 0x10c4, usbProductId: 0xea60 }, /* CP2102/CP2102N */
  { usbVendorId: 0x0403, usbProductId: 0x6010 }, /* FT2232H */
  { usbVendorId: 0x303a, usbProductId: 0x1001 }, /* Espressif USB_SERIAL_JTAG */
  { usbVendorId: 0x303a, usbProductId: 0x1002 }, /* Espressif esp-usb-bridge firmware */
  { usbVendorId: 0x303a, usbProductId: 0x0002 }, /* ESP32-S2 USB_CDC */
  { usbVendorId: 0x303a, usbProductId: 0x0009 }, /* ESP32-S3 USB_CDC */
  { usbVendorId: 0x1a86, usbProductId: 0x55d4 }, /* CH9102F */
  { usbVendorId: 0x1a86, usbProductId: 0x7523 }, /* CH340T */
  { usbVendorId: 0x0403, usbProductId: 0x6001 }, /* FT232R */
];

const FlashFirmware = () => {
  const eLog = useRef(null);
  const [logText, setLogText] = useState("");
  const [espTransport, setEspTransport] = useState(null);
  const [deviceLoader, setDeviceLoader] = useState(null);

  const serialLib = !navigator.serial && navigator.usb ? serial : navigator.serial;

  let espLoaderTerminal = {
    clean() {
      setLogText("")
    },
    writeLine(data) {
      setLogText(prev => prev + data + '\n');
      setTimeout(() => eLog.current?.scrollTo({ top: eLog.current.scrollHeight, behavior: 'smooth' }), 100);
    },
    write(data) {
      setLogText(prev => prev + data);
    }
  }

  const doConnect = async () => {
    try {
      espLoaderTerminal.clean();
      let device = await serialLib.requestPort({ filters: usbPortFilters });
      let transport = new Transport(device);

      const loaderOptions = {
        transport,
        baudrate: 921600,
        terminal: espLoaderTerminal
      };
      let esploader = new ESPLoader(loaderOptions);
      await esploader.main();
      await esploader.flashId();

      setEspTransport(transport);
      setDeviceLoader(esploader);
    } catch (err) {
      console.error('Error during connection or flashing:', err);
    }
  }

  const doDisconnect = async () => {
    if (espTransport !== null)
      espTransport.disconnect();

    setEspTransport(null);
    setDeviceLoader(null);
    espLoaderTerminal.writeLine("Disconnected");
    espLoaderTerminal.writeLine(`------------------------------------------------------------`)
  }

  function arrayBufferToBinaryString(buffer) {
    const bytes = new Uint8Array(buffer);
    const CHUNK = 0x8000; // safe chunk size
    let str = "";
    for (let i = 0; i < bytes.length; i += CHUNK) {
      const slice = bytes.subarray(i, Math.min(i + CHUNK, bytes.length));
      for (let j = 0; j < slice.length; j++) str += String.fromCharCode(slice[j]);
    }
    return str;
  }

  const doInstall = async () => {
    espLoaderTerminal.writeLine(`Download firmware bin...`);
    // download puteros-latest.bin
    const firmwareUrl = '/puteros-latest.bin';
    const response = await fetch(firmwareUrl);
    const firmwareBuffer = await response.arrayBuffer();
    const firmwareString = arrayBufferToBinaryString(firmwareBuffer);

    espLoaderTerminal.writeLine(`Downloaded file size: ${firmwareString.length} bytes`);

    try {
      await deviceLoader.writeFlash({
        fileArray : [{
          address: 0,
          data: firmwareString,
        }],
        flashSize: "keep",
        flashMode: undefined,
        flashFreq: undefined,
        eraseAll: false,
        compress: true,
      });
    } catch (e) {
      console.error(e);
    }
  }

  return (
    <div className="section installation">
      <div className="content">
        <div className="title">
          <div className="title_inner">Installation</div>
        </div>
        <p>This firmware only support Cardputer / Cardputer ADV for now</p>
        <div className="action">
          {
            deviceLoader ? (
              <Fragment>
                <button className="btn" onClick={doInstall}>Install</button>
                <button className="btn" onClick={doDisconnect}>Disconnect</button>
              </Fragment>
            ) : (
              <Fragment>
                <button className="btn" onClick={doConnect}>Connect</button>
                <a className="btn" href="/puteros-latest.bin">Download</a>
              </Fragment>
            )
          }
        </div>
        {
          deviceLoader && (
            <div className="device-info" ref={eLog}>
              {logText}
            </div>
          )
        }
      </div>
    </div>
  );
};

export default FlashFirmware;