<h1>🔐 Door Locker Security System (Dual ATmega32)</h1>
<p>
  A robust <strong>Door Locker Security System</strong> developed using <strong>two ATmega32 microcontrollers</strong> operating at <strong>8 MHz</strong>.
  The system allows users to unlock a secured door using a password and features a full <strong>layered architecture</strong> with 
  dedicated Human Machine Interface (HMI) and Control ECUs.  
</p>

<hr />

<h2>🚀 Project Overview</h2>
<p>
  This project demonstrates the design and implementation of a <strong>secure embedded access control system</strong>.
  The HMI ECU interacts with the user through a keypad and LCD, while the Control ECU manages decision-making, motor control, EEPROM storage, and alarm activation.
  The system uses <strong>UART communication</strong> between both microcontrollers and ensures secure password management.
</p>

<h3>🎯 Objectives</h3>
<ul>
  <li>Implement a dual-ECU security system (HMI &amp; Control).</li>
  <li>Allow password creation, verification, and storage in external EEPROM.</li>
  <li>Use <strong>UART</strong> for ECU communication and <strong>I2C</strong> for EEPROM interfacing.</li>
  <li>Control a DC motor to lock/unlock the door with timing precision using <strong>Timer1 interrupts</strong>.</li>
  <li>Trigger a <strong>buzzer alarm</strong> on multiple failed password attempts.</li>
  <li>Design based on a clean <strong>layered architecture</strong> (MCAL → HAL → APP).</li>
</ul>

<hr />

<h2>⚙️ System Architecture</h2>
<pre><code>                 ┌─────────────────────────────┐
                 │         HMI_ECU             │
                 │ ─────────────────────────── │
                 │  4x4 Keypad (Input)         │
                 │  2x16 LCD (Display)         │
                 │  UART Communication         │
                 └──────────────┬──────────────┘
                                │ UART Link
                                ▼
                 ┌─────────────────────────────┐
                 │        CONTROL_ECU          │
                 │ ─────────────────────────── │
                 │  DC Motor (Door Control)    │
                 │  EEPROM (I2C Storage)       │
                 │  Buzzer (Alarm)             │
                 │  Timer1 (PWM / Delay)       │
                 └─────────────────────────────┘
</code></pre>

<hr />

<h2>🧩 Features</h2>
<ul>
  <li>Two-microcontroller architecture (HMI &amp; Control).</li>
  <li>Password creation, confirmation, and persistent storage in EEPROM.</li>
  <li>Door open/close sequence with precise timing (Timer1 + PWM).</li>
  <li>Secure UART communication between ECUs.</li>
  <li>Automatic buzzer activation after 3 failed password attempts.</li>
  <li>Blocking mechanism during alarm activation period.</li>
</ul>

<hr />

<h2>🧱 System Workflow</h2>
<ol>
  <li><strong>Step 1 – Create a System Password</strong>
    <ul>
      <li>LCD prompts “Please Enter Password”.</li>
      <li>User enters a 5-digit password; each digit shown as “*”.</li>
      <li>User re-enters password for confirmation.</li>
      <li>If matched, password is stored in <strong>EEPROM</strong>.</li>
      <li>If not matched, system repeats password creation step.</li>
    </ul>
  </li>

  <li><strong>Step 2 – Main Options</strong>
    <ul>
      <li>LCD displays main menu with options:</li>
      <li><strong>(+)</strong> → Open Door</li>
      <li><strong>(-)</strong> → Change Password</li>
    </ul>
  </li>

  <li><strong>Step 3 – Open Door</strong>
    <ul>
      <li>User enters password.</li>
      <li>Control ECU validates it with EEPROM-stored password.</li>
      <li>If correct:
        <ul>
          <li>DC motor rotates <strong>15s CW</strong> (“Door Unlocking”)</li>
          <li>Hold for <strong>3s</strong></li>
          <li>DC motor rotates <strong>15s CCW</strong> (“Door Locking”)</li>
        </ul>
      </li>
      <li>If incorrect: ask again (up to 3 attempts).</li>
    </ul>
  </li>

  <li><strong>Step 4 – Change Password</strong>
    <ul>
      <li>User enters current password for verification.</li>
      <li>If matched → restart Step 1 (create new password).</li>
      <li>If unmatched after 3 attempts:
        <ul>
          <li>Trigger <strong>buzzer alarm for 60 seconds</strong>.</li>
          <li>Lock keypad input during alarm duration.</li>
        </ul>
      </li>
    </ul>
  </li>
</ol>

<hr />

<h2>🧰 Software Layers</h2>
<ul>
  <li><strong>MCAL (Microcontroller Abstraction Layer):</strong> GPIO, Timer1, UART, I2C/TWI.</li>
  <li><strong>HAL (Hardware Abstraction Layer):</strong> LCD, Keypad, DC Motor, Buzzer, EEPROM.</li>
  <li><strong>APP (Application Layer):</strong> System control logic, password validation, communication handler.</li>
</ul>

<hr />

<h2>🔧 Drivers Implemented</h2>
<table>
  <thead>
    <tr><th>Driver</th><th>Description</th><th>Connected To</th></tr>
  </thead>
  <tbody>
    <tr><td><strong>GPIO</strong></td><td>General I/O driver for pin control</td><td>Both ECUs</td></tr>
    <tr><td><strong>LCD</strong></td><td>2x16 LCD control (4-bit/8-bit mode)</td><td>HMI ECU</td></tr>
    <tr><td><strong>Keypad</strong></td><td>4x4 matrix keypad scanning</td><td>HMI ECU</td></tr>
    <tr><td><strong>UART</strong></td><td>Full-duplex asynchronous serial communication</td><td>Both ECUs</td></tr>
    <tr><td><strong>I2C (TWI)</strong></td><td>Communication with external EEPROM</td><td>Control ECU</td></tr>
    <tr><td><strong>EEPROM</strong></td><td>External memory for password storage</td><td>Control ECU</td></tr>
    <tr><td><strong>Timer1</strong></td><td>Used for timing and motor delay</td><td>Both ECUs</td></tr>
    <tr><td><strong>DC Motor</strong></td><td>Door lock/unlock via PWM</td><td>Control ECU</td></tr>
    <tr><td><strong>Buzzer</strong></td><td>Alarm alert for wrong attempts</td><td>Control ECU</td></tr>
  </tbody>
</table>

<hr />

<h2>🧪 Testing &amp; Validation</h2>
<ul>
  <li>Verified UART communication stability between ECUs.</li>
  <li>EEPROM tested for reliable data persistence and read/write accuracy.</li>
  <li>Timer1 validated for precise 15s / 3s timing cycles.</li>
  <li>Buzzer activation tested after consecutive failed entries.</li>
  <li>LCD and keypad inputs tested for responsiveness and clarity.</li>
</ul>

<hr />

<h2>🧰 Build &amp; Run Instructions</h2>

<h3>Requirements</h3>
<ul>
  <li>Atmel Studio / Microchip Studio</li>
  <li>Two ATmega32 Development Boards</li>
  <li>STK or USBasp Programmer</li>
  <li>Proteus (for simulation)</li>
</ul>

<h3>Steps</h3>
<ol>
  <li>Load <strong>HMI_ECU</strong> project and build it.</li>
  <li>Load <strong>Control_ECU</strong> project and build it.</li>
  <li>Flash each HEX file to its corresponding MCU.</li>
  <li>Connect UART lines: TX ↔ RX, RX ↔ TX, and GND shared.</li>
  <li>Connect peripherals (LCD, Keypad, Motor, EEPROM, Buzzer) as per design.</li>
  <li>Power the system and follow LCD instructions.</li>
</ol>

<hr />

<h2>📁 Example LCD Display Flow</h2>
<pre><code>[System Boot]
Please Enter Password:
*****
Please Re-enter Password:
*****
Password Saved!

[Main Menu]
+ : Open Door
- : Change Pass

[Door Opening]
Door is Unlocking...
Door is Locking...
</code></pre>

<hr />

<h2>🚀 Future Enhancements</h2>
<ul>
  <li>Add <strong>RFID/NFC authentication</strong> alongside password input.</li>
  <li>Implement <strong>Bluetooth or Wi-Fi</strong> remote control.</li>
  <li>Include <strong>RTC-based lock scheduling</strong>.</li>
  <li>Enhance security with <strong>multi-factor access</strong> (password + token).</li>
</ul>

<hr />

<h2>📚 References</h2>
<ul>
  <li>ATmega32 Datasheet – Microchip Technology</li>
  <li>Embedded Systems Course by Eng. Mohamed Tarek</li>
  <li>UART, I2C, Timer1 AVR Driver Implementations</li>
  <li>Proteus Simulation Documentation</li>
</ul>

<hr />

<h2>👨‍💻 Author</h2>
<p>
  <strong>Mohaned Hossam</strong><br />
  Embedded Software Engineer<br />
  📧 <a href="mailto:mohanedtohamy444@gmail.com">mohanedtohamy444@gmail.com</a><br />
  🔗 <a href="https://www.linkedin.com/in/mohaned-hossam-8593041b3">LinkedIn</a><br />
  💻 <a href="https://github.com/MOHANED01">GitHub</a>
</p>

<hr />

<p><strong>⭐ “Secure embedded design begins with layered logic and precise communication.”</strong></p>
