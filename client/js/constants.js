const EVENTS = {
  CONNECTION: 'connection',
  DISCONNECT: 'disconnect',
  SET_COLOR: 'setColor',
  GET_AVG_COLOR: 'getAvgColor',
  COPIED: 'copied',
  GET_AVG_PALETTE: 'getAvgColorpalette',
};

const DOM = {
  MAIN_WRAPPER: document.querySelector("#main-wrapper"),
  GET_COLOR: document.querySelector("#getColor"),
  SET_COLOR: document.querySelector("#setColor"),
  CURRENT_COLOR_TEXT: document.querySelector(".current-color"),
}