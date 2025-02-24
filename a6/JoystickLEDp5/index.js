const BAUD_RATE = 9600;
let port;
let connectBtn, sendBtn;
let targetSpeed = 500;
let targetBrightness = 128;
let serialBuffer = ""; // Store incoming serial data

function setup() {
  setupSerial();
  createCanvas(windowWidth, windowHeight);

  sendBtn = createButton("Send to Arduino");
  sendBtn.position(5, 35);
  sendBtn.mouseClicked(sendToArduino);
}

function draw() {
  const portIsOpen = checkPort();
  if (!portIsOpen) return;

  // Read data from the serial port, append to buffer
  let newData = port.read();
  if (newData) {
    serialBuffer += newData; // Append incoming data
  }

  // If a full message (newline) is received, process it
  if (serialBuffer.includes("\n")) {
    let lines = serialBuffer.split("\n"); // Split into full lines
    serialBuffer = lines.pop(); // Keep unfinished data for next read

    try {
      let arr = JSON.parse(lines[0]); // Parse first complete JSON line
      if (Array.isArray(arr) && arr.length === 2) {
        let xVal = arr[0];
        let yVal = arr[1];

        // Map joystick values to LED speed & brightness
        targetSpeed = Math.round(map(xVal, 0, 1023, 1000, 10));
        targetBrightness = Math.round(map(yVal, 0, 1023, 255, 0));

        background(220);
        textSize(20);
        fill(0);
        text(`\n Joystick X input: ${xVal} → LED Blinkspeed: ${targetSpeed} ms`, 20, 50);
        text(`\n Joystick Y input: ${yVal} → LED Brightness: ${targetBrightness} / 255`, 20, 80);
      }
    } catch (e) {
      console.error("Invalid JSON received:", lines[0]);
    }
  }
}

// Function to send LED speed & brightness as an array
function sendToArduino() {
  let data = [targetSpeed, targetBrightness]; // Create array
  port.write(JSON.stringify(data) + "\n"); // Convert to JSON & send over Serial
}

// Serial setup
function setupSerial() {
  port = createSerial();
  let usedPorts = usedSerialPorts();
  if (usedPorts.length > 0) {
    port.open(usedPorts[0], BAUD_RATE);
  }

  connectBtn = createButton("Connect to Arduino");
  connectBtn.position(5, 5);
  connectBtn.mouseClicked(onConnectButtonClicked);
}

// Check if serial port is open
function checkPort() {
  if (!port.opened()) {
    connectBtn.html("Connect to Arduino");
    background("gray");
    return false;
  } else {
    connectBtn.html("Disconnect");
    return true;
  }
}

// Handle connect/disconnect button click
function onConnectButtonClicked() {
  if (!port.opened()) {
    port.open(BAUD_RATE);
  } else {
    port.close();
  }
}
