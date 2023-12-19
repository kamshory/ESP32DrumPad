if (!Element.prototype.matches) {
  Element.prototype.matches =
    Element.prototype.msMatchesSelector ||
    Element.prototype.webkitMatchesSelector;
}

if (!Element.prototype.closest) {
  Element.prototype.closest = function (s) {
    let el = this;
    do {
      if (Element.prototype.matches.call(el, s)) return el;
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

let ajax = {};

function customConfim(sl, message, title, tok, tcancel, cbok, cbcancel)
{
  message = message || 'Are you sure?';
  title = title || 'Confirm';
  tok = tok || 'OK';
  tcancel = tcancel || 'Cancel';
  let obj = _sl(sl);
  obj.querySelector('.popup-header').innerText = title;
  obj.querySelector('.popup-body .message').innerText = message;
  obj.querySelector('.btn-ok').innerText = tok;
  obj.querySelector('.btn-cancel').innerText = tcancel;
  obj.popupShow();
  cbok = cbok || function() {
    obj.popupHide();
  };
  cbcancel = cbcancel || function() {
    obj.popupHide();
  };
  try
  {
    obj.querySelector('.btn-ok').removeEventListener('click');
    obj.querySelector('.btn-cancel').removeEventListener('click');
  }
  catch(ex)
  {
    
  }
  obj.querySelector('.btn-ok').addEventListener('click', function(){
    cbok();
    obj.popupHide();
  });
  obj.querySelector('.btn-cancel').addEventListener('click', function(){
    cbcancel();
    obj.popupHide();
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
  let e = _sl("#ssid_name").value,
    t = _sl("#ssid_password").value,
    n = _sl("#ws_host").value,
    a = _sl("#ws_port").value,
    o = _sl("#ws_path").value,
    r = _sl("#ws_username").value,
    u = _sl("#ws_password").value,
    s = _sl("#ws_topic").value,
    c = _sl("#enable").value;
  return (
    ajax.post(
      "save-subscription.html",
      {
        action: "save-subscription",
        ssid_name: e,
        ssid_password: t,
        ws_host: n,
        ws_port: a,
        ws_path: o,
        ws_username: r,
        ws_password: u,
        ws_topic: s,
        enable: c,
      },
      function (e) {},
      !0
    ),
    !1
  );
}
function loadSubData() {
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
function loadAPData() {
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
  let e = _sl("#ssid_name").value,
    t = _sl("#ssid_password").value,
    n = _sl("#ip").value,
    a = _sl("#gateway").value,
    o = _sl("#subnet").value,
    r = _sl("#hidden").value;
  return (
    ajax.post(
      "save-ap.html",
      {
        action: "save-ap",
        ssid_name: e,
        ssid_password: t,
        ip: n,
        gateway: a,
        subnet: o,
        hidden: r,
      },
      function (e) {},
      !0
    ),
    !1
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
(ajax.create = function () {
  if ("undefined" != typeof XMLHttpRequest) return new XMLHttpRequest();
  for (
    let e,
      t = [
        "MSXML2.XmlHttp.6.0",
        "MSXML2.XmlHttp.5.0",
        "MSXML2.XmlHttp.4.0",
        "MSXML2.XmlHttp.3.0",
        "MSXML2.XmlHttp.2.0",
        "Microsoft.XmlHttp",
      ],
      n = 0;
    n < t.length;
    n++
  )
    try {
      e = new ActiveXObject(t[n]);
      break;
    } catch (e) {}
  return e;
}),
  (ajax.send = function (e, t, n, a, o) {
    void 0 === o && (o = !0);
    let r = ajax.create();
    r.open(n, e, o),
      (r.onreadystatechange = function () {
        4 == r.readyState && t(r.responseText);
      }),
      "POST" == n &&
        r.setRequestHeader("Content-type", "application/x-www-form-urlencoded"),
      r.send(a);
  }),
  (ajax.get = function (e, t, n, a) {
    let o,
      r = [];
    for (o in t)
      t.hasOwnProperty(o) &&
        r.push(encodeURIComponent(o) + "=" + encodeURIComponent(t[o]));
    ajax.send(e + (r.length ? "?" + r.join("&") : ""), n, "GET", null, a);
  }),
  (ajax.post = function (e, t, n, a) {
    let o,
      r = [];
    for (o in t)
      t.hasOwnProperty(o) &&
        r.push(encodeURIComponent(o) + "=" + encodeURIComponent(t[o]));
    ajax.send(e, n, "POST", r.join("&"), a);
  }),
  (window.onload = function () {
    let e = window.location.toString();
    -1 < e.indexOf("ap-configuration.html") && loadAPData(),
      -1 < e.indexOf("subscription-configuration.html") && loadSubData();
    const t = _sls('input[type="ipaddress"]');
    if (t.length)
      for (let n = 0; n < t.length; n++)
        t[n].addEventListener("keyup", function (e) {
          handleIP(e);
        }),
          t[n].addEventListener("change", function (e) {
            handleIP(e);
          });
  });
