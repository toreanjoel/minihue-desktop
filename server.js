
const express = require('express');
const app = express();
const http = require('http').createServer(app);
const io = require('socket.io')(http);
const wallpaper = require('wallpaper');
const fs = require('fs');
const path = require('path');
const constants = require("./helper/constants");
const helpers = require("./helper/utils");

const { copyFile, setColor, getColor } = helpers;
const { EVENTS, SYSTEM } = constants;

// security is not a concern as it will be a desktop app
app.use('/', express.static(__dirname + SYSTEM.CLIENT_DIR));

/**
 * setup the connection to the server and listen for event stream
 */
io.on(EVENTS.CONNECTION, (socket) => {
  console.info('socket',socket);
  socket.emit(EVENTS.CONNECTION, "hello");
  // client wants the color
  socket.on(EVENTS.GET_AVG_COLOR, (color) => {
    (async () => {
      // get the current wallpaper
      const currentWallp = await wallpaper.get();
      fs.access(SYSTEM.CLIENT_DIR, (err) => {
        if(err) {
          fs.mkdirSync(SYSTEM.CLIENT_DIR);
        }
        copyFile({ fileSource: currentWallp, destination: path.join(SYSTEM.CLIENT_DIR, SYSTEM.CLIENT_IMG), socket });
      });

      getColor({currentWallp, socket});
    })();
  });

  // client sets the color
  socket.on(EVENTS.SET_COLOR, (color) => {
    console.info('setting',color);
    setColor({color, socket});
  });

  socket.on(EVENTS.DISCONNECT, () => {
    console.log('user disconnected');
  });
});

http.listen(SYSTEM.PORT, () => {
  console.log(`connected on port: ${SYSTEM.PORT}`);
});