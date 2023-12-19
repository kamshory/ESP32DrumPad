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

let ajax = {};

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
  confirmObject.find(".btn-cancel") && confirmObject.find(".btn-cancel").remove();
  
  let btnOk = _ce('button');
  btnOk.attr('class', 'btn btn-success btn-ok');
  btnOk.innerText = btnTextOk;
  
  let btnCancel = _ce('button');
  btnCancel.attr('class', 'btn btn-warning btn-cancel');
  btnCancel.innerText = btnTextCancel;  
  let footer = confirmObject.find('.popup-footer');
  
  footer.appendChild(btnOk);
  footer.appendChild(document.createTextNode(' '));
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

function _ce(s) {
  return document.createElement(s);
}
function _sl(s) {
  return document.querySelector(s);
}
function _sls(s) {
  return document.querySelectorAll(s);
}

function saveWSData() {
  customConfim(
    ".popup-confirm",
    "Are you sure you want to save the config?",
    "User Confirmation",
    "Yes",
    "No",
    function () {
      ajax.post(
        "save-ws.html",
        {
          action: "save-subscription",
          ssid_name: _sl("#ssid_name").value,
          ssid_password: _sl("#ssid_password").value,
          ws_host: _sl("#ws_host").value,
          ws_port: _sl("#ws_port").value,
          ws_path: _sl("#ws_path").value,
          ws_username: _sl("#ws_username").value,
          ws_password: _sl("#ws_password").value,
          ws_topic: _sl("#ws_topic").value,
          enable: _sl("#enable").value,
        },
        function (e) {},
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
    function (e) {
      try {
        let t = JSON.parse(e);
        _sl("#wifi_enable").value = t.wifi_enable;
        _sl("#ssid_name").value = t.ssid_name;
        _sl("#ssid_password").value = t.ssid_password;
        _sl("#ws_enable").value = t.ws_enable;
        _sl("#ws_scheme").value = t.ws_scheme;
        _sl("#ws_host").value = t.ws_host;
        _sl("#ws_port").value = t.ws_port;
        _sl("#ws_path").value = t.ws_path;
        _sl("#ws_username").value = t.ws_username;
        _sl("#ws_password").value = t.ws_password;
      } catch (e) {}
    },
    true
  );
}

function loadConfigAP() {
  ajax.get(
    "ap-configuration.json",
    {},
    function (e) {
      try {
        let t = JSON.parse(e);
        _sl("#ssid_name").value = t.ssid_name;
        _sl("#ssid_password").value = t.ssid_password;
        _sl("#ip").value = t.ip;
        _sl("#gateway").value = t.gateway;
        _sl("#subnet").value = t.subnet;
        _sl("#hidden").value = t.hidden;
      } catch (e) {}
    },
    true
  );
}
function saveAPData() {
  customConfim(
    ".popup-confirm",
    "Are you sure you want to save the config?",
    "User Confirmation",
    "Yes",
    "No",
    function () {
      ajax.post(
        "save-ap.html",
        {
          action: "save-ap",
          ssid_name: _sl("#ssid_name").value,
          ssid_password: _sl("#ssid_password").value,
          ip: _sl("#ip").value,
          gateway: _sl("#gateway").value,
          subnet: _sl("#subnet").value,
          hidden: _sl("#hidden").value,
        },
        function (e) {},
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
    function (e) {
      try {
        let t = JSON.parse(e);
        _sl("#channel").value = t.channel;
        _sl("#read_interval").value = t.read_interval;
        _sl("#solo_pad").value = t.solo_pad;
        _sl("#solo_pad_number").value = t.solo_pad_number;
      } catch (e) {}
    },
    true
  );
}
function loadDefaultGeneral() {
  customConfim(
    ".popup-confirm",
    "Are you sure you want to load default config?",
    "User Confirmation",
    "Yes",
    "No",
    function () {
      _sl("#channel").value = 10;
        _sl("#read_interval").value = 1000;
        _sl("#solo_pad").value = 0;
        _sl("#solo_pad_number").value = 1;
    },
    null
  );
}
function saveConfigGeneral() {
  customConfim(
    ".popup-confirm",
    "Are you sure you want to save the config?",
    "User Confirmation",
    "Yes",
    "No",
    function () {
      ajax.post(
        "save-general.html",
        {
          channel: _sl("#channel").value,
          read_interval: _sl("#read_interval").value,
          solo_pad: _sl("#solo_pad").value,
          solo_pad_number: _sl("#solo_pad_number").value
        },
        function (e) {},
        true
      );
    },
    null
  );
  return false;
}
function handleIP(e) {
  e = e.target;
  isValidIP(e.value)
    ? e.classList.remove("invalid-ip")
    : (e.classList.remove("invalid-ip"), e.classList.add("invalid-ip"));
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

ajax.send = function (url, cbSuccess, type, a, asynchronous) {
  asynchronous = asynchronous || false;
  let request = ajax.create();

  request.open(type, url, asynchronous);
  request.onreadystatechange = function () {
    if (4 == request.readyState) {
      cbSuccess(request.responseText);
    }
  };
  if ("POST" == type) {
    request.setRequestHeader(
      "Content-type",
      "application/x-www-form-urlencoded"
    );
  }
  request.send(a);
};

ajax.get = function (url, data, cbSuccess, asynchronous) {
  let i,
    params = [];
  for (i in data)
    if (data.hasOwnProperty(i)) {
      params.push(encodeURIComponent(i) + "=" + encodeURIComponent(data[i]));
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
  let i,
    params = [];
  for (i in data) {
    if (data.hasOwnProperty(i)) {
      params.push(encodeURIComponent(i) + "=" + encodeURIComponent(data[i]));
    }
  }
  ajax.send(url, cbSuccess, "POST", params.join("&"), asynchronous);
};
window.onload = function () {
  let url = window.location.toString();
  if (-1 < url.indexOf("ap-configuration.html")) {
    loadConfigAP();
  }
  if (-1 < url.indexOf("subscription-configuration.html")) {
    loadConfigWS();
  }
  if (-1 < url.indexOf("general-configuration.html")) {
    loadConfigGeneral();
  }
  const elem = _sls('input[type="ipaddress"]');
  if (elem.length)
    for (let i = 0; i < elem.length; i++) {
      elem[i].on("keyup", function (e) {
        handleIP(e);
      });
      elem[i].on("change", function (e) {
        handleIP(e);
      });
    }
};
