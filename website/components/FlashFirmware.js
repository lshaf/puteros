"use client";

import {Fragment, useEffect, useRef, useState} from "react";
import { createHash } from 'crypto';

const FlashFirmware = () => {
  const eLog = useRef(null);
  const [deviceLoader, setDeviceLoader] = useState(null);
  const [espStub, setEspStub] = useState(null);
  const [logText, setLogText] = useState("");

  const serialLib = !navigator.serial && navigator.usb ? serial : navigator.serial;
  useEffect(() => {
    const script = document.createElement("script");
    script.type = "module";
    script.textContent = `
       window.esptoolPackage = import("https://unpkg.com/esp-web-flasher@5.1.4/dist/web/index.js?module");
    `;
    document.head.appendChild(script);
  }, []);

  const addLogText = (text) => {
    setLogText(prev => prev + text + '\n');
    eLog.current?.scrollTo({ top: eLog.current.scrollHeight, behavior: 'smooth' });
  }
  const formatMacAddr = (macAddr) => {
    return macAddr.map((value) => value.toString(16).toUpperCase().padStart(2, '0')).join(':')
  }

  const doConnect = async () => {
    try {
      setLogText("");
      let loaderMod = await window.esptoolPackage;
      let eDevice = await serialLib.requestPort({});

      await eDevice.open({baudRate: 921600});
      let eDeviceLoader = new loaderMod.ESPLoader(eDevice, {
        log: (...args) => addLogText(args[0]),
        debug: (...args) => console.debug(...args),
        error: (...args) => console.error(...args),
        baudRate: 921600,
      });

      setDeviceLoader(eDeviceLoader);
      await eDeviceLoader.initialize();
      addLogText(`Connected to ${eDeviceLoader.chipName}`)
      addLogText(`MAC Address: ${formatMacAddr(eDeviceLoader.macAddr())}`)

      const newEspStub = await eDeviceLoader.runStub()
      newEspStub.port.addEventListener('disconnect', async () => {
        setDeviceLoader(null);
        setEspStub(null);

        addLogText("Disconnected");
        addLogText(`------------------------------------------------------------`)
      });

      setEspStub(newEspStub);
    } catch (err) {
      console.error('Error during connection or flashing:', err);
    }
  }

  const doDisconnect = async () => {
    await deviceLoader.disconnect();
    await deviceLoader.port.close();
    setDeviceLoader(null);
    setEspStub(null);

    addLogText("Disconnected");
    addLogText(`------------------------------------------------------------`)
  }

  const doInstall = async () => {
    setLogText(prev => prev + `Download firmware bin...\n`);
    // download puteros-latest.bin
    const firmwareUrl = '/puteros-latest.bin';
    const response = await fetch(firmwareUrl);
    const firmwareData = await response.arrayBuffer();

    setLogText(prev => prev + `Downloaded file size: ${firmwareData.byteLength} bytes\n`);

    try {
      await espStub.flashData(
        firmwareData,
        (bytesWritten, totalBytes) => {
          const progress = (bytesWritten / totalBytes)
          const percentage = Math.floor(progress * 100)

          addLogText(`Flashing... ${percentage}%`)
        }
      )
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
          <a className="btn" href="/puteros-latest.bin">Download</a>
          {
            deviceLoader ? (
                <Fragment>
                  <button className="btn" onClick={doInstall}>Install</button>
                  <button className="btn" onClick={doDisconnect}>Disconnect</button>
                </Fragment>
              ) : (<button className="btn" onClick={doConnect}>Connect</button>)
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