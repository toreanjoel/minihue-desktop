const EVENTS = {
  CONNECTION: 'connection',
  DISCONNECT: 'disconnect',
  SET_COLOR: 'setColor',
  GET_AVG_COLOR: 'getAvgColor',
  COPIED: 'copied',
  GET_AVG_PALETTE: 'getAvgColorpalette',
};

const TEXT = {
  CUSTOM_COLOR: 'Custom Color',
  DESKTOP_THEME: 'Desktop Theme'
}

const DOM = {
  MAIN_WRAPPER: document.querySelector("#main-wrapper"),
  DESKTOP_TOGGLE: document.querySelector("#desktop-toggle"),
  GET_COLOR: document.querySelector("#getColor"),
  SET_COLOR: document.querySelector("#setColor"),
  CURRENT_COLOR_TEXT: document.querySelector(".current-color"),
  FOOTER_SECTION_TEXT: document.querySelector(".footer-section"),
}