var mqtt = require('mqtt');
var client  = mqtt.connect('mqtt://mqtt.handler.cloud', {"username":"USERNAME", "password":"PASSWORD"});

client.on('connect', function () {
  client.subscribe('soeren/android/light/led/couch');
  client.subscribe('soeren/android/light/led/bed');
});

client.on('message', function (topic, message) {
    var m = String(message);
    var red = parseInt(m.substring(1,3), 16).toString();
    var green = parseInt(m.substring(3,5), 16).toString();
    var blue = parseInt(m.substring(5,7), 16).toString();
    while(red.length < 3)
    {
        red = "0" + red;
    }
    while(green.length < 3)
    {
        green = "0" + green;
    }
    while(blue.length < 3)
    {
        blue = "0" + blue;
    }
    var msg = red.toString()+green.toString()+blue.toString();
  if(topic=="soeren/android/light/led/couch")
  {
      client.publish("soeren/apartment/light/ledstripe/couch", msg);
  }
  if(topic=="soeren/android/light/led/bed")
  {
      client.publish("soeren/apartment/light/ledstripe/bed", msg);
  }
  // message is Buffer
  console.log(message.toString());
});
