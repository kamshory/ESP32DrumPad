if (!Element.prototype.matches) {
  Element.prototype.matches =
    Element.prototype.msMatchesSelector ||
    Element.prototype.webkitMatchesSelector;
}
if (!Element.prototype.closest) {
  Element.prototype.closest = function (s) {
    let el = this;
    do {
      if (Element.prototype.matches.call(el, s)) {
        return el;
      }
      el = el.parentElement || el.parentNode;
    } while (el !== null && el.nodeType === 1);
    return null;
  };
}
if (!Element.prototype.remove) {
  Element.prototype.remove = function (s) {
    let el = this;
    el.parentNode.removeChild(el);
  };
}
Element.prototype.popupShow = function () {
  this.style.display = "block";
  this.closest(".popup-shadow").style.display = "block";
};
Element.prototype.attr = function (n, v) {
  if (typeof v != "undefined") {
    return this.setAttribute(n, v);
  } else {
    return this.getAttribute(n);
  }
};
Element.prototype.popupHide = function () {
  this.style.display = "none";
  this.closest(".popup-shadow").style.display = "none";
};
Element.prototype.on = function (a, b, c) {
  return this.addEventListener(a, b, c);
};
Element.prototype.off = function (a, b, c) {
  return this.removeEventListener(a, b, c);
};
Element.prototype.find = function (a) {
  return this.querySelector(a);
};
Element.prototype.findAll = function (a) {
  return this.querySelectorAll(a);
};
function _ce(s) {
  return document.createElement(s);
}
function _nm(s) {
  return document.getElementsByName(s)[0];
}
function _sl(s) {
  return document.querySelector(s);
}
function _sls(s) {
  return document.querySelectorAll(s);
}

let ajax = {};
let cMsg = "Are you sure you want to save the configuration?";
let rMsg = "Are you sure you want to reset the configuration?";
let pSel = ".popup-confirm";
let pTtl = "User Confirmation";

function customConfim(
  selector,
  message,
  title,
  btnTextOk,
  btnTextCancel,
  btnCallbackOk,
  btnCallbackCancel
) {
  message = message || "Are you sure?";
  title = title || "Confirm";
  btnTextOk = btnTextOk || "OK";
  btnTextCancel = btnTextCancel || "Cancel";
  let confirmObject = _sl(selector);
  confirmObject.find(".popup-header").innerText = title;
  confirmObject.find(".popup-body .message").innerText = message;

  confirmObject.find(".btn-ok") && confirmObject.find(".btn-ok").remove();
  confirmObject.find(".btn-cancel") &&
    confirmObject.find(".btn-cancel").remove();

  let btnOk = _ce("button");
  btnOk.attr("class", "btn btn-success btn-ok"); // add classes at once
  btnOk.innerText = btnTextOk;

  let btnCancel = _ce("button");
  btnCancel.attr("class", "btn btn-warning btn-cancel");
  btnCancel.innerText = btnTextCancel;
  let footer = confirmObject.find(".popup-footer");

  footer.appendChild(btnOk);
  footer.appendChild(document.createTextNode(" "));
  footer.appendChild(btnCancel);

  confirmObject.popupShow();
  btnCallbackOk =
    btnCallbackOk ||
    function () {
      confirmObject.popupHide();
    };
  btnCallbackCancel =
    btnCallbackCancel ||
    function () {
      confirmObject.popupHide();
    };
  try {
    btnOk.off("click");
    btnCancel.off("click");
  } catch (ex) {}
  btnOk.on("click", function () {
    btnCallbackOk();
    confirmObject.popupHide();
  });
  btnCancel.on("click", function () {
    btnCallbackCancel();
    confirmObject.popupHide();
  });
}

function saveConfigWS() {
  customConfim(
    pSel,
    cMsg,
    pTtl,
    "Yes",
    "No",
    function () {
      ajax.post(
        "save-ws.html",
        {
          action: "save-ws",
          wifi_enable: _nm("wifi_enable").value,
          ssid_name: _nm("ssid_name").value,
          ssid_password: _nm("ssid_password").value,
          ws_enable: _nm("ws_enable").value,
          ws_host: _nm("ws_host").value,
          ws_port: _nm("ws_port").value,
          ws_path: _nm("ws_path").value,
          ws_username: _nm("ws_username").value,
          ws_password: _nm("ws_password").value,
        },
        function (response, status, statusText) {
          console.log(statusText);
        },
        true
      );
    },
    null
  );
  return false;
}
function loadConfigWS() {
  ajax.get(
    "ws-configuration.json",
    {},
    function (response, status, statusText) {
      try {
        let t = JSON.parse(response);
        _nm("wifi_enable").value = t.wifi_enable;
        _nm("ssid_name").value = t.ssid_name;
        _nm("ssid_password").value = t.ssid_password;
        _nm("ws_enable").value = t.ws_enable;
        _nm("ws_scheme").value = t.ws_scheme;
        _nm("ws_host").value = t.ws_host;
        _nm("ws_port").value = t.ws_port;
        _nm("ws_path").value = t.ws_path;
        _nm("ws_username").value = t.ws_username;
        _nm("ws_password").value = t.ws_password;
      } catch (e) {}
    },
    true
  );
}

function loadConfigAP() {
  ajax.get(
    "ap-configuration.json",
    {},
    function (response, status, statusText) {
      try {
        let t = JSON.parse(response);
        _nm("ssid_name").value = t.ssid_name;
        _nm("ssid_password").value = t.ssid_password;
        _nm("ip").value = t.ip;
        _nm("gateway").value = t.gateway;
        _nm("subnet").value = t.subnet;
        _nm("hidden").value = t.hidden;
      } catch (e) {}
    },
    true
  );
}
function saveConfigAP() {
  customConfim(
    pSel,
    cMsg,
    pTtl,
    "Yes",
    "No",
    function () {
      ajax.post(
        "save-ap.html",
        {
          action: "save-ap",
          ssid_name: _nm("ssid_name").value,
          ssid_password: _nm("ssid_password").value,
          ip: _nm("ip").value,
          gateway: _nm("gateway").value,
          subnet: _nm("subnet").value,
          hidden: _nm("hidden").value,
        },
        function (response, status, statusText) {},
        true
      );
    },
    null
  );
  return false;
}
function loadConfigGeneral() {
  ajax.get(
    "general-configuration.json",
    {},
    function (response, status, statusText) {
      try {
        let t = JSON.parse(response);
        _nm("channel").value = t.channel;
        _nm("read_interval").value = t.read_interval;
        _nm("solo_pad").value = t.solo_pad;
        _nm("solo_pad_number").value = t.solo_pad_number;
      } catch (e) {}
    },
    true
  );
}
function loadDefaultGeneral() {
  customConfim(
    pSel,
    rMsg,
    pTtl,
    "Yes",
    "No",
    function () {
      _nm("channel").value = 10;
      _nm("read_interval").value = 1000;
      _nm("solo_pad").value = 0;
      _nm("solo_pad_number").value = 1;
    },
    null
  );
}
function saveConfigGeneral() {
  customConfim(
    pSel,
    cMsg,
    pTtl,
    "Yes",
    "No",
    function () {
      ajax.post(
        "save-general.html",
        {
          channel: _nm("channel").value,
          read_interval: _nm("read_interval").value,
          solo_pad: _nm("solo_pad").value,
          solo_pad_number: _nm("solo_pad_number").value,
        },
        function (response, status, statusText) {},
        true
      );
    },
    null
  );
  return false;
}
function handleIP(e) {
  let el = e.target;
  isValidIP(el.value)
    ? el.classList.remove("invalid-ip")
    : (el.classList.remove("invalid-ip"), el.classList.add("invalid-ip"));
}
function initIP() {
  let el = _sls('input[type="ipaddress"]');
  if (el != null && el.length)
    for (let i = 0; i < el.length; i++) {
      el[i].on("keyup", function (e) {
        handleIP(e);
      });
      el[i].on("change", function (e) {
        handleIP(e);
      });
    }
}
function isValidIP(e) {
  if (0 == e.length) return true;
  let t,
    n = e.split(".");
  if (4 != n.length) return false;
  for (t in n) {
    if (isNaN(parseInt(n[t]))) return false;
    if (n[t] < 0 || 255 < n[t]) return false;
  }
  return true;
}

ajax.create = function () {
  if ("undefined" != typeof XMLHttpRequest) return new XMLHttpRequest();
  for (
    let httpRequest,
      obj = [
        "MSXML2.XmlHttp.6.0",
        "MSXML2.XmlHttp.5.0",
        "MSXML2.XmlHttp.4.0",
        "MSXML2.XmlHttp.3.0",
        "MSXML2.XmlHttp.2.0",
        "Microsoft.XmlHttp",
      ],
      n = 0;
    n < obj.length;
    n++
  )
    try {
      httpRequest = new ActiveXObject(obj[n]);
      break;
    } catch (ex) {}
  return httpRequest;
};

ajax.send = function (url, cbSuccess, type, data, asynchronous) {
  if (typeof asynchronous == "undefined") {
    asynchronous = true;
  }
  let request = ajax.create();

  request.open(type, url, asynchronous);
  request.onreadystatechange = function () {
    if (
      4 == request.readyState &&
      cbSuccess != null &&
      typeof cbSuccess == "function"
    ) {
      let len = cbSuccess.length;
      if (len == 1) {
        cbSuccess(request.responseText);
      } else if (len == 2) {
        cbSuccess(request.responseText, request.status);
      } else if (len == 3) {
        cbSuccess(request.responseText, request.status, request.statusText);
      }
    }
  };
  if ("POST" == type) {
    request.setRequestHeader(
      "Content-type",
      "application/x-www-form-urlencoded"
    );
  }
  request.send(data);
};

ajax.get = function (url, data, cbSuccess, asynchronous) {
  let i;
  let params = [];
  if (typeof data != "undefined") {
    for (i in data)
      if (data.hasOwnProperty(i)) {
        params.push(encodeURIComponent(i) + "=" + encodeURIComponent(data[i]));
      }
  }
  ajax.send(
    url + (params.length ? "?" + params.join("&") : ""),
    cbSuccess,
    "GET",
    null,
    asynchronous
  );
};

ajax.post = function (url, data, cbSuccess, asynchronous) {
  let i;
  let params = [];
  if (typeof data != "undefined") {
    for (i in data) {
      if (data.hasOwnProperty(i)) {
        params.push(encodeURIComponent(i) + "=" + encodeURIComponent(data[i]));
      }
    }
  }
  ajax.send(url, cbSuccess, "POST", params.join("&"), asynchronous);
};
