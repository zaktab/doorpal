var twilio = require('twilio'),
    SerialPort = new require("serialport").SerialPort,
    express = require('express');

var app = express();

// Twilio Credentials 
var accountSid = 'ACCOUNTSID'; 
var authToken = 'AUTHTOKEN'; 
 
//require the Twilio module and create a REST client 
var client = require('twilio')(accountSid, authToken); 

function sendMessage(res, message) {
  var resp = new twilio.TwimlResponse();
  resp.message(message);
  res.type('text/xml');
  res.send(resp.toString());
}

var serialPort = new SerialPort("COM3");

app.use(express.bodyParser());

app.post('/sms', twilio.webhook('AUTHTOKEN', { host:'fooo.ngrok.com', protocol:'http' }), function(req, res){
  if (req.body.From == 'PHONENUMBER' && req.body.Body == "Unlock") {
    console.log("verified number!");

    serialPort.once('data', function(data) {
      if (data.toString().indexOf('U') > -1) { //check if the Arduino returned a U for unlocking
        sendMessage(res, 'Unlocking!');
      }
      else if (data.toString().indexOf('L') > -1) {
        sendMessage(res, 'The door is already open');
      }
      else {
        sendMessage(res, 'ERROR');

      }
    });

    serialPort.write("V", function(err, results) {
      if (err) {
        console.log('err ' + err);
      }
      console.log('results ' + results);
    });

  } else if (req.body.From == '+PHONENUMBER' && req.body.Body == "Lock"){

        console.log("verified number!");

    serialPort.once('data', function(data) {
      if (data.toString().indexOf('U') > -1) { //check if the Arduino returned a U for unlocking
        sendMessage(res, 'Door is already locked');
      }
      else if (data.toString().indexOf('L') > -1) {
        sendMessage(res, 'Locking');
      }
      else {
        sendMessage(res, 'ERROR');

      }
    });

    serialPort.write("V", function(err, results) {
      if (err) {
        console.log('err ' + err);
      }
      console.log('results ' + results);
    });
  }
  else {
    console.log("Access Denied");
  }

});


serialPort.on('open', function(){
// serialPort.open( function () {
  app.listen(3000);
  console.log('Listening on port 3000');




  serialPort.on('data', function(data){

    if (data.toString().indexOf('G')> -1) {
        console.log("Someone at the door");
        client.messages.create({ 
  to: "PHONENUMBER", 
  from: "NUMBER", 
  body: "Someone is at the door",   
}, function(err, message) { 
});
      }

  });
});
