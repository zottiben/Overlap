Pebble.addEventListener('ready', function(e) {
  console.log('JS component is now READY');
});

Pebble.addEventListener('appmessage', function(msg) {
  console.log('Received Message: ' + JSON.stringify(msg.payload));
});

Pebble.addEventListener('showConfiguration', function(e) {
  var colorConfigURL = 'http://www.benzotti.co/pebbledev/Phaser/phaser_color.html';
  var roundConfigURL = 'http://www.benzotti.co/pebbledev/Phaser/phaser_round.html';
  var bwConfigURL = 'http://www.benzotti.co/pebbledev/Phaser/phaser_bw.html';
  
  var watch;
  
  if(Pebble.getActiveWatchInfo) {
    try {
      watch = Pebble.getActiveWatchInfo();
    } catch(err) {
      watch = {
        platform: "basalt",
      };
    }
  } else {
    watch = {
      platform: "aplite",
    };
  }
  
  if(watch.platform == "aplite") {
    Pebble.openURL(bwConfigURL);
  } else if(watch.platform == "chalk") {
    Pebble.openURL(roundConfigURL);
  } else {
    Pebble.openURL(colorConfigURL);
  }
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = decodeURIComponent(e.response);
  
  if(configData) {
    configData = JSON.parse(decodeURIComponent(e.response));
    
    console.log("Config data received!" + JSON.stringify(configData));
    
    var dict = {};
    
    if(configData.timeColor) {
      dict.KEY_SETTING_TIME_COLOR = parseInt(configData.timeColor, 16);
    }
    
    if(configData.timeBgColor) {
      dict.KEY_SETTING_TIME_BG_COLOR = parseInt(configData.timeBgColor, 16);
    }
    
    if(configData.leadingZero) {
      dict.KEY_SETTING_LEADING_ZERO = 1;
    } else {
      dict.KEY_SETTING_LEADING_ZERO = 0;
    }
    
    if(configData.meridian) {
      dict.KEY_SETTING_MERIDIAN = 1;
    } else {
      dict.KEY_SETTING_MERIDIAN = 0;
    }
    
    if(configData.sizeIncrease) {
      dict.KEY_SETTING_SIZE_INCREASE = 1;
    } else {
      dict.KEY_SETTING_SIZE_INCREASE = 0;
    }
    
    if(configData.blink) {
      dict.KEY_SETTING_BLINK = 1;
    } else {
      dict.KEY_SETTING_BLINK = 0;
    }
    
    
    console.log('Preparing message: ', JSON.stringify(dict));
    
    Pebble.sendAppMessage(dict, function() {
      console.log('Sent config data to pebble');
    }, function() {
      console.log('Failed to send config data!');
    });
  } else {
    console.log('No settings changed');
  }
});