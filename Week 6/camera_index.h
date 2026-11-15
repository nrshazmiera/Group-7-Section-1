#ifndef CAMERA_INDEX_H
#define CAMERA_INDEX_H

#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-CAM Face Detection</title>
<style>
body{font-family:Arial,sans-serif;background:#101820;color:#f2f2f2;text-align:center;margin:0;padding:20px}
.card{max-width:700px;margin:auto;background:#1d2b36;padding:18px;border-radius:14px}
h2{margin-top:4px}.video{width:100%;max-width:640px;border-radius:10px;background:#000}
.status{font-size:18px;font-weight:bold;margin:16px;padding:12px;border-radius:8px;background:#34495e}
.note{color:#cbd5df;font-size:14px}
</style>
</head>
<body>
<div class="card">
<h2>ESP32-CAM Face Tracking</h2>
<p class="note">Live camera stream with built-in ESP32 face detection</p>
<img class="video" id="stream" alt="Camera stream">
<div class="status" id="status">Connecting to camera...</div>
<p class="note">When a face is detected, the servo pans. When no face is detected, it returns to the centre.</p>
</div>
<script>
const host=location.hostname;
document.getElementById('stream').src='http://'+host+':81/stream';
async function updateStatus(){
 try{
  const r=await fetch('/status'); const d=await r.json();
  const el=document.getElementById('status');
  el.textContent=d.face_detected?'FACE DETECTED':'NO FACE DETECTED';
  el.style.background=d.face_detected?'#176b45':'#34495e';
 }catch(e){document.getElementById('status').textContent='Waiting for camera...';}
}
setInterval(updateStatus,700); updateStatus();
</script>
</body>
</html>
)rawliteral";

#endif
