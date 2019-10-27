const fs = require('fs');
const ColorThief = require('colorthief');
const SerialPort = require('serialport');
const constants = require("./constants");
const { EVENTS } = constants;

let port;
// here we are going to make sure the ports array has an arduino - can be changed for other units later
(function() {
  SerialPort.list((err, ports) => {
    if(ports.length > 0 && !err) {
      for(var i = 0; i < ports.length; i++) {
        if (ports[i].manufacturer.indexOf('arduino') !== -1) {
          console.info('currentPort', ports[i].comName);
          port = new SerialPort(ports[i].comName, { baudRate: 9600 });
        }
      }
    }
  });
})();

module.exports = {
  copyFile: function({ fileSource, destination, socket }) {
    let readStream = fs.createReadStream(fileSource);
  
    // error 
    readStream.once('error', (err) => {
      console.log(err);
    });
  
    //success 
    readStream.once('end', () => {
      socket.emit(EVENTS.COPIED, true);
    });
    readStream.pipe(fs.createWriteStream(destination));
  },
  /**
 * get => wallpaper avg
 * set => manual
 */
  setColor: function ({ color, socket }) {
    if(color.length > 0) {
      socket.emit(EVENTS.SET_COLOR, color);
      socket.broadcast.emit(EVENTS.SET_COLOR, color);
      if(port) {
        port.write(color[0] + ',' + color[1] + ',' + color[2])
      }
    }
  },
  getColor: function({ currentWallp, socket }) {
    ColorThief.getColor(currentWallp)
    .then(color => {
      if(color.length > 0) {
        // promie callback to send the color
        socket.emit(EVENTS.GET_AVG_COLOR, color);
        socket.broadcast.emit(EVENTS.GET_AVG_COLOR, color);
        if (port) {
          port.write(color[0] + ',' + color[1] + ',' + color[2])
        }
      }
    })
    .catch(err => { console.log(err) });
  }
}

