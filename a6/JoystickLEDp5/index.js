// Christian Kjaer
// HCDE 439 
// A6 Talking to the Web!
// Code Attribution ~ Used the Ball p5 example as a starting framework

const BAUD_RATE = 9600; // Baud rate must match the arduino sketch

// Declare global variables
let port; 
let connectBtn, sendBtn;
let targetSpeed = 500;
let targetBrightness = 128;
let serialBuffer = ""; // Stores incoming serial data

function setup() {
    setupSerial();
    createCanvas(windowWidth, windowHeight); // Create a canvas that is the size of our browser window.
    // windowWidth and windowHeight are p5 variables

    // Creates a button that sends the current speed and brightness values to the Arduino connected with the LED
    sendBtn = createButton("Send to Arduino");
    sendBtn.position(5, 130);
    sendBtn.mouseClicked(sendToArduino);
}

function draw() { // Loops to read joystick values and converts them to appropriate values to control LED behavior.
    const portIsOpen = checkPort(); // Check whether the port is open (see checkPort function below)
    if (!portIsOpen) {
        text("port isn't workign", 20, 50);
        return; 
    }
     // Read data from the serial port, append to buffer. Had to add this as serial data was being sent all at once, breaking the JSON parsing
    let newData = port.read();
    if (newData) {
        serialBuffer += newData; // Append incoming data
    }

  if (serialBuffer.includes("\n")) { // If a new joystick reading is received, process it,
    let lines = serialBuffer.split("\n"); // Split into full lines
    serialBuffer = lines.pop(); // Keep unfinished data for next read
        try { // try/catch added to debug JSON parsing
            let arr = JSON.parse(lines[0]); // Parse first complete JSON line
            if (Array.isArray(arr) && arr.length === 2) { // checks for valid input formatting
                let xVal = arr[0]; // extracts xVal from array
                let yVal = arr[1]; // extracts yVal from array
                targetSpeed = Math.round(map(xVal, 0, 1023, 1000, 10)); // Rounds and maps joystick x value to LED speed, with smaller values (leftward direction of joystick) correlating to a larger delay between blinks
                targetBrightness = Math.round(map(yVal, 0, 1023, 255, 0)); // Rounds and maps Joystick y valueto  LED brightness, with smaller values (upward direction of joystick) correlating to a higher brightness
                background(220);
                textSize(20);
                fill(0);
                // Display current Joystick input values and what they mean for the LED behavior
                text(`Joystick X input: ${xVal} -> LED Blinkspeed: ${targetSpeed} ms`, 20, 65);
                text(`Joystick Y input: ${yVal} -> LED Brightness: ${targetBrightness} / 255`, 20, 95);
            }
        } catch (error) {
            console.error('JSON Parsing Failed: ', error); // added for debugging json parsing
        }
    }
}

function sendToArduino() { // Function to send LED speed & brightness as an array
    let data = [targetSpeed, targetBrightness]; // Create array
    port.write(JSON.stringify(data) + "\n"); // Convert to JSON & send over Serial
}

// Three helper functions for managing the serial connection.
// Serial setup
function setupSerial() {
    port = createSerial();
    // Check to see if there are any ports we have used previously
    let usedPorts = usedSerialPorts();
    if (usedPorts.length > 0) {
      // If there are ports we've used, open the first one
      port.open(usedPorts[0], BAUD_RATE);
    }

    // create a connect button
    connectBtn = createButton("Connect to Arduino");
    connectBtn.position(5, 5); // Position the button in the top left of the screen.
    connectBtn.mouseClicked(onConnectButtonClicked); // When the button is clicked, run the onConnectButtonClicked function
  }
  
  function checkPort() {
    if (!port.opened()) {
      // If the port is not open, change button text
      connectBtn.html("Connect to Arduino");
      // Set background to gray
      background("gray");
      return false;
    } else {
      // Otherwise we are connected
      connectBtn.html("Disconnect");
      return true;
    }
  }
  
  function onConnectButtonClicked() {
    // When the connect button is clicked
    if (!port.opened()) {
      // If the port is not opened, we open it
      port.open(BAUD_RATE);
    } else {
      // Otherwise, we close it!
      port.close();
    }
}
