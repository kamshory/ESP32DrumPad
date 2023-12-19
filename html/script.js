if (!Element.prototype.matches) {
  Element.prototype.matches =
    Element.prototype.msMatchesSelector ||
    Element.prototype.webkitMatchesSelector;
}

if (!Element.prototype.closest) {
  Element.prototype.closest = function (s) {
    let el = this;
    do {
      if (Element.prototype.matches.call(el, s)) 
      {
        return el;
      }
      el = el.parentElement || el.parentNode;
    } while (el !== null && el.nodeType === 1);
    return null;
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
  confirmObject.find(".btn-ok").innerText = btnTextOk;
  confirmObject.find(".btn-cancel").innerText = btnTextCancel;
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
    confirmObject.find(".btn-ok").off("click");
    confirmObject.find(".btn-cancel").off("click");
  } catch (ex) {}
  confirmObject.find(".btn-ok").on("click", function () {
    btnCallbackOk();
    confirmObject.popupHide();
  });
  confirmObject.find(".btn-cancel").on("click", function () {
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

function saveSubData() {
  return (
    ajax.post(
      "save-subscription.html",
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
      !0
    ),
    !1
  );
}
function loadConfWS() {
  ajax.get(
    "subscription-configuration.json",
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
    !0
  );
}
function loadConfAP() {
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
    !0
  );
}
function saveAPData() {
  return (
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
    ),
    false
  );
}
function handleIP(e) {
  e = e.target;
  isValidIP(e.value)
    ? e.classList.remove("invalid-ip")
    : (e.classList.remove("invalid-ip"), e.classList.add("invalid-ip"));
}
function isValidIP(e) {
  if (0 == e.length) return !0;
  let t,
    n = e.split(".");
  if (4 != n.length) return !1;
  for (t in n) {
    if (isNaN(parseInt(n[t]))) return !1;
    if (n[t] < 0 || 255 < n[t]) return !1;
  }
  return !0;
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
  void 0 === asynchronous && (asynchronous = !0);
  let r = ajax.create();
  r.open(type, url, asynchronous);
  r.onreadystatechange = function () {
    if (4 == r.readyState) {
      cbSuccess(r.responseText);
    }
  };

  if ("POST" == type) {
    r.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    r.send(a);
  }
};

ajax.get = function (url, data, cbSuccess, asynchronous) {
  let o,
    r = [];
  for (o in data)
    if (data.hasOwnProperty(o)) {
      r.push(encodeURIComponent(o) + "=" + encodeURIComponent(data[o]));
    }
  ajax.send(
    url + (r.length ? "?" + r.join("&") : ""),
    cbSuccess,
    "GET",
    null,
    asynchronous
  );
};

ajax.post = function (url, data, cbSuccess, asynchronous) {
  let o,
    r = [];
  for (o in data) {
    if (data.hasOwnProperty(o)) {
      r.push(encodeURIComponent(o) + "=" + encodeURIComponent(data[o]));
    }
  }
  ajax.send(url, cbSuccess, "POST", r.join("&"), asynchronous);
};
window.onload = function () {
  let e = window.location.toString();
  if (-1 < e.indexOf("ap-configuration.html")) {
    loadConfAP();
  }
  if (-1 < e.indexOf("subscription-configuration.html")) {
    loadConfWS();
  }
  const t = _sls('input[type="ipaddress"]');
  if (t.length)
    for (let n = 0; n < t.length; n++) {
      t[n].on("keyup", function (e) {
        handleIP(e);
      });
      t[n].on("change", function (e) {
        handleIP(e);
      });
    }
};
