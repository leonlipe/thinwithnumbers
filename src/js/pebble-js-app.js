

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


function locationSuccess(pos) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
  pos.coords.latitude + '&lon=' + pos.coords.longitude;
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log('Temperature is ' + temperature);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log('Conditions are ' + conditions);

      console.log('Humidity ' +  json.main.humidity);
     // console.log('Wind ' +  (json.wind.speed / 0.62137));

     var wind_speed = Math.floor((json.wind.speed / 0.62137) * 100) / 100  ;
     var sunrise = new Date(json.sys.sunrise*1000);
     var sunset = new Date(json.sys.sunset*1000);

      // Assemble dictionary using our keys
		var dictionary = {
		  'KEY_TEMPERATURE': temperature,
		  'KEY_CONDITIONS': conditions,
      'KEY_HUMIDITY': json.main.humidity,
      'KEY_WIND': wind_speed.toString(),
      'KEY_SUNRISE': sunrise.getHours()+":"+sunrise.getMinutes(),
      'KEY_SUNSET': sunset.getHours()+":"+sunset.getMinutes()
		};

		// Send to Pebble
		Pebble.sendAppMessage(dictionary,
		  function(e) {
		    console.log('Weather info sent to Pebble successfully!');
		  },
		  function(e) {
		    console.log('Error sending weather info to Pebble!');
		  }
		);
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    // Get the initial weather
  //  getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
	getWeather();

  }                     
);

