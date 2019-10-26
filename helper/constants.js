const path = require('path');
module.exports = {
  EVENTS: {
    CONNECTION: 'connection',
    DISCONNECT: 'disconnect',
    SET_COLOR: 'setColor',
    GET_AVG_COLOR: 'getAvgColor',
    COPIED: 'copied',
    GET_AVG_PALETTE: 'getAvgColorpalette',
  },
  
  SYSTEM: {
    PORT: 8080,
    CLIENT_DIR: '/client',
    CLIENT_IMG_DIR: path.join(__dirname, '/client/images'),
    CLIENT_IMG: 'img.jpg',
    PALETTE_AMMOUNT: 5,
  }
};