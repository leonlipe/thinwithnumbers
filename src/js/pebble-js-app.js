

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

      try {
          // responseText contains a JSON object with weather info
          var json = JSON.parse(responseText);

          // Temperature in Kelvin requires adjustment
          console.log("Temp:"+json.main.temp);
          var temperature = Math.round(json.main.temp - 273.15);
          var temperatureF = Math.round( ((json.main.temp - 273.15) * 1.8) +32 );
          console.log('Temperature is ' + temperature);

          // Conditions
          var conditions = json.weather[0].main;     
          var conditions_code = json.weather[0].id;   
          var conditions_icon = json.weather[0].icon;   
          console.log('Conditions are ' + conditions);
          console.log('Code are ' + conditions_code);
          console.log('Icon are ' + conditions_icon);
          console.log('Humidity ' +  json.main.humidity);
         // console.log('Wind ' +  (json.wind.speed / 0.62137));

         var wind_speed = Math.floor((json.wind.speed / 0.62137) * 100) / 100  ;
         var sunrise = new Date(json.sys.sunrise*1000);
         var sunset = new Date(json.sys.sunset*1000);
         var now = new Date();
         console.log("Now:"+(now.toString()));
         console.log("Sunrise:"+(sunrise.toString()));
         console.log("Sunset:"+(sunset.toString()));
         console.log("Date res:"+(now.getTime() > sunset.getTime()));

          // Assemble dictionary using our keys
      		var dictionary = {
      		  'KEY_TEMPERATURE': temperature,
            'KEY_TEMPERATUREF': temperatureF,
      		  'KEY_CONDITIONS': conditions,
            'KEY_HUMIDITY': json.main.humidity,
            'KEY_WIND': wind_speed.toString(),
            'KEY_SUNRISE': (sunrise.getHours()>12?sunrise.getHours()-12:sunrise.getHours() )+":"+sunrise.getMinutes()+ (sunrise.getHours()>12?"pm":"am")+"|"+sunrise.getHours()+":"+sunrise.getMinutes(),
            'KEY_SUNSET': (sunset.getHours()>12? sunset.getHours()-12:sunset.getHours() )+":"+sunset.getMinutes() +(sunset.getHours()>12?"pm":"am")+"|"+sunset.getHours()+":"+sunset.getMinutes(),
            'KEY_CONDITIONS_ID': conditions_code,
            'KEY_CONDITIONS_ICON': conditions_icon
      		};
          console.log('Data to be sent: ' +  JSON.stringify(dictionary));


    		// Send to Pebble if everithing is ok
    		Pebble.sendAppMessage(dictionary,
    		  function(e) {
    		    console.log('Weather info sent to Pebble successfully!');
    		  },
    		  function(e) {
    		    console.log('Error sending weather info to Pebble!');
    		  }
    		);
    }
    catch(err) {
      console.log("Error when getting weather");
    }
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

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  console.log("Show config"+JSON.stringify(e));

  Pebble.openURL('https://dl.dropboxusercontent.com/u/318065/analogy_config.html');
});

Pebble.addEventListener('webviewclosed',
  function(e) {
     var configuration = JSON.parse(decodeURIComponent(e.response));
    //console.log('Configuration window returned:' ,JSON.stringify(configuration));

     
    var configs = {
      'KEY_DATE': parseInt(configuration.date),
      'KEY_DAY': parseInt(configuration.dow),
      'KEY_BT': parseInt(configuration.bt),
      'KEY_BATTERY': parseInt(configuration.bat),
      'KEY_SECOND_HAND': parseInt(configuration.showSec),
      'BACKTYPE': 0,
      'MARGIN': 6,
      'HAND_LENGTH_SEC': 65,
      'HAND_LENGTH_SEC_INVERSE': 15,
      'HAND_LENGTH_MIN': 65,
      'HAND_LENGTH_HOUR': 45,
      'HAND_TYPE': 2,
      'TEMPERATURE': parseInt(configuration.temperature),
      'CONDITIONS': 1,
      'HUMIDITY': parseInt(configuration.humidity),
      'WIND': 0,
      'SUNRISE': parseInt(configuration.sunrise),
      'SUNSET': parseInt(configuration.sunset),
      'DIGITALTIME': 0,
      'INVERTED': parseInt(configuration.inverted),
      'NUMBERS': 1,
      'CELCIUS': parseInt(configuration.celcius),
      'POLL_TIME': parseInt(configuration.time_poll)

      
    };

        //console.log('I sent:' ,JSON.stringify(configs));



    Pebble.sendAppMessage(
      configs,
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!"+JSON.stringify(e));
      }
    );
  }
);
