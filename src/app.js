var ws;// = new WebSocket('ws://192.168.11.4:20080');
var ready;

var xhrRequest = function (url, type, callback) {
  console.log("xhrRequest(js)");
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function sendDengeki()
{
  console.log("sendDengeki(js)");  
  var url = "http://192.168.11.4:8090";
  xhrRequest(url, 'GET',
             function(responseText) {
               var dictionary = {
                 "DENGEKI": 0,
               };
               Pebble.sendAppMessage(dictionary);
             }
            );
}

function sendSound()
{
  console.log("sendSound(js)");  
  var url = "http://192.168.11.4:8090";
  xhrRequest(url, 'GET',
             function(responseText) {
               var dictionary = {
                 "SOUND": 1,
               };
               Pebble.sendAppMessage(dictionary);
             }
            );
}

function openWS() {
	ws = new WebSocket('ws://192.168.11.4:20080','vsido-cmd');
	ws.onerror = function (error) {
		console.error(error);
	};
	// Log messages from the server
	ws.onmessage = function (evt) {
    console.log("on message");
//		var remoteMsg = JSON.parse( evt.data );
//		var responseDate = new Date();
//		var timeResponse = responseDate.getTime() - self.requestDate.getTime();
//		if(self.cb){
//			self.cb(remoteMsg,timeResponse);
//		}
	};
	ws.onopen = function (evt) {
    console.log("on open");
		ready = true;
	};
	ws.onclose = function(evt){
    console.log("on close");
//		var remoteMsg = JSON.parse( evt.data );
		ready = false;
	};
}



function sendWalk()
{
  console.log("Ready state : " + ready);
  if (ready === true)
  {
    console.log("send json");
		var jsonMsg = {
			"cmd": "walk",
			"forward":50,/* -100 ~ 100 percentage */
			"turn":0/* -100 ~ 100 percentage */
		};
    ws.send(JSON.stringify(jsonMsg));
  }
  else
  {
    openWS();
  }
}

Pebble.addEventListener("ready",
                      function(e) {
                        console.log("Ready");
                      }
                     );

Pebble.addEventListener("appmessage",
                        function(e) {
                          console.log("Received status : " + e.payload.status);
//                          console.log("Received message : " + e.payload.message);
                          if (e.payload.status === 0)
                          {
                            sendDengeki();
                          }
                          else if (e.payload.status == 1)
                          {
                            sendSound();
                          }
                          else if (e.payload.status == 2)
                          {
                            sendWalk();
                          }
                        }
                       );
