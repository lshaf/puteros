const features = [
  {
    name: 'Wifi',
    desc: 'Wi‑Fi scanning and connection tools.',
    children: [
      {
        name: 'IP Scanner',
        desc: 'Discover devices on the local network and also ports available on every IP.',
      },
      { name: 'Analyzer', desc: 'Inspect packets and signals for debugging.' },
      { name: 'Beacon SPAM', desc: 'Send beacon frames for testing (use responsibly).' },
      { name: 'Deauthenticator', desc: 'Send deauth frames for testing only.' },
      { name: 'Deauth Detector', desc: 'Detect nearby deauthentication events.' },
      { name: 'ESPNOW Chat', desc: 'Peer‑to‑peer chat over ESPNOW.' },
      { name: 'Packet Monitor', desc: 'Live capture and monitor wireless packets.' },
      { name: 'Web File Manager', desc: 'Manage sd card content using web view that connected to network' }
    ],
  },
  {
    name: 'Bluetooth',
    desc: 'Classic Bluetooth tools and utilities.',
    children: [
      { name: 'Beacon SPAM', desc: 'Spamming bluetooth beacon with random string' },
      { name: 'Scanner', desc: 'Scan barcodes and QR codes.' },
    ],
  },
  {
    name: 'Keyboard',
    desc: 'Emulate your ESP32 as keyboard to PC.',
    children: [
      { name: 'DuckyScript', desc: 'Script to run chain of keyboard command in an instance' },
      { name: 'BLE Keyboard', desc: 'Emulate via bluetooth.' },
      { name: 'USB Keyboard', desc: 'Emulate via USB HID.' },
    ],
  },
  {
    name: 'Module',
    desc: 'Manage attached hardware modules.',
    children: [
      { name: 'PN532Killer', desc: 'PN532 RFID testing tools (use responsibly).' },
      { name: 'MFRC522 (M5 RFID 2)', desc: 'Interact with M5 RFID tags.' },
    ],
  },
  {
    name: 'Utility',
    desc: 'Small system utilities and helpers.',
    children: [
      { name: 'I2C Detector', desc: 'Scan I2C bus for connected devices.' },
      { name: 'QRCode Generator', desc: 'Generate QR codes from text.' },
    ],
  },
  {
    name: 'Game',
    desc: 'Small built‑in entertainment games.',
    children: [
      { name: 'HEX Decoder', desc: 'Guessing randomized hex number under 2 minutes.' },
      { name: 'Wordle', desc: 'Wordle game and available in 2 language ID and EN' },
    ],
  },
  { name: 'Setting', desc: 'Adjust device options and preferences.' },
];
const DetailFeature = () => {
  return (
    <div className="section detail-feature">
      {
        features.map((feature, index) => (
          <div className="content" key={index}>
            <div className="title">
              <div className="title_inner">{feature.name}</div>
            </div>
            <div className="desc">
              { feature.desc }
            </div>
            { feature.children && feature.children.length > 0 && (
              <div className="service-items">
                { feature.children.map((child, cIndex) => (
                  <div key={cIndex} className="service-item">
                    <div className="icon">
                      <span className="ion ion-pin" />
                    </div>
                    <div className="name">{ child.name }</div>
                    <p>{ child.desc }</p>
                  </div>))
                }
              </div>)
            }
          </div>
        ))
      }
    </div>
  );
};
export default DetailFeature;