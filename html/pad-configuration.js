window.onload = function () {
  renderPad(defaultData.row, defaultData.pad, "#container");
  let opt = _nm("instrument");
  let btnhide = _sls(".pupup-hide");
  let btnPad = _sls(".pad");
  for (let i in instCd) {
    opt.appendChild(
      new Option(
        instCd[i][0] + " - " + instCd[i][2] + " (" + instCd[i][1] + ")",
        instCd[i][0]
      )
    );
  }
  for (
    let i = 0;
    i < btnhide.length;
    i++ //NOSONAR
  ) {
    btnhide[i].on("click", function (e) {
      e.target.closest(".popup").popupHide();
    });
  }
  for (
    let i = 0;
    i < btnPad.length;
    i++ //NOSONAR
  ) {
    btnPad[i].on("click", function (e) {
      testDrum(e.target.attr("data-cd"));
    });
  }
  _sl(".test-drum").on("click", function (e) {
    testDrum(e.target.closest(".popup").find('[name="instrument"]').value);
  });
  _sl(".update-data").on(
    "click",
    function (e) {
      setData(_sl("#f" + _nm("number").value), [
        _nm("instrument").value,
        _nm("threshold").value,
        _nm("headroom").value,
        _nm("duration").value,
      ]);
      _sl("#pad-conf").popupHide();
    },
    false
  );
};
let defaultData = {
  row: [
    [9, 10, 11, 12],
    [5, 6, 7, 8],
    [1, 2, 3, 4],
  ],
  pad: {
    f1: [35, 100, 1000, 10000, 20000],
    f2: [36, 100, 1000, 10000, 20000],
    f3: [37, 100, 1000, 10000, 20000],
    f4: [38, 100, 1000, 10000, 20000],
    f5: [39, 100, 1000, 10000, 20000],
    f6: [40, 100, 1000, 10000, 20000],
    f7: [41, 100, 1000, 10000, 20000],
    f8: [42, 100, 1000, 10000, 20000],
    f9: [43, 100, 1000, 10000, 20000],
    f10: [44, 100, 1000, 10000, 20000],
    f11: [45, 100, 1000, 10000, 20000],
    f12: [46, 100, 1000, 10000, 20000],
  },
};
function testDrum(code) {
  console.log("send note", code);
}
function resetToDefault() {
  customConfim(
    pSel,
    rMsg,
    pTtl,
    "Yes",
    "No",
    function () {
      for (let i in defaultData.pad) {
        if (defaultData.pad.hasOwnProperty(i)) {
          setData(_sl("#" + i), defaultData.pad[i]);
        }
      }
    },
    null
  );
}
function renderPad(padArr, config, selector) {
  let sel = _sl(selector);
  for (
    let i = 0;
    i < padArr.length;
    i++ //NOSONAR
  ) {
    let row = padArr[i];
    let rw = _ce("div");
    rw.classList.add("row");
    for (
      let j = 0;
      j < row.length;
      j++ //NOSONAR
    ) {
      let cl = _ce("div");
      cl.classList.add("pad-wrapper");
      cl.attr("data-ch", row[j]);
      cl.appendChild(createButton(row[j], config));
      cl.appendChild(editor(row[j], config));
      rw.appendChild(cl);
    }
    sel.appendChild(rw);
  }
}
function createButton(ch, config) {
  let id = "f" + ch;
  let data = config[id] || [35, 100, 1000, 10000, 20000];

  let bt = _ce("button");
  bt.classList.add("btn");
  bt.classList.add("btn-100");
  bt.classList.add("pad");
  bt.attr("data-number", ch);
  bt.attr("type", "button");
  bt.attr("id", id);
  setData(bt, data);
  return bt;
}

function setData(bt, data) {
  let isnt = data[0];
  bt.innerText = isnt;
  bt.attr("data-tt", instCd["f" + isnt][2]);
  bt.attr("data-cd", isnt);
  bt.attr("data-nt", instCd["f" + isnt][1]);
  bt.attr("data-th", data[1]);
  bt.attr("data-hr", data[2]);
  bt.attr("data-dr", data[3]);
}

function editor(id, config) {
  let ed = _ce("div");
  ed.classList.add("editor");
  let a = _ce("a");
  a.classList.add("ctrl");
  a.attr("href", "javascript:");
  a.attr("data-id", id);
  a.on(
    "click",
    function (e) {
      let data = getData(e);
      let pop = _sl("#pad-conf");
      _nm("number").value = e.target.attr("data-id");
      _nm("instrument").value = data["data-cd"];
      _nm("threshold").value = data["data-th"];
      _nm("headroom").value = data["data-hr"];
      _nm("duration").value = data["data-dr"];
      pop.popupShow();
    },
    false
  );
  ed.appendChild(a);
  return ed;
}
function getData(e) {
  let target = e.target.parentNode.parentNode.find(".pad");
  return {
    "data-cd": target.attr("data-cd"),
    "data-th": target.attr("data-th"),
    "data-hr": target.attr("data-hr"),
    "data-dr": target.attr("data-dr"),
  };
}
function getBtnData(container) {
  let cntr = _sl(container);
  let arr = cntr.findAll("button");
  let bld2 = [];
  for (
    let i = 0;
    i < arr.length;
    i++ //NOSONAR
  ) {
    let p = arr[i];
    let bld = [
      p.attr("id") + "=" + p.attr("data-cd"),
      p.attr("data-th"),
      p.attr("data-hr"),
      p.attr("data-dr"),
    ];
    bld2.push(bld.join(","));
  }
  return bld2.join("&");
}

function saveConfigPad() {
  customConfim(
    pSel,
    cMsg,
    pTtl,
    "Yes",
    "No",
    function () {
      let formData = getBtnData("#container");
      ajax.post('save-pad.html', formData, function(response, status, statusText){
      }, true);
    },
    null
  );
}

let instCd = {
  f35: ["35", "B1", "Acoustic Bass Drum"],
  f36: ["36", "C2", "Bass Drum 1"],
  f37: ["37", "C#2", "Side Stick"],
  f38: ["38", "D2", "Acoustic Snare"],
  f39: ["39", "D#2", "Hand Clap"],
  f40: ["40", "E2", "Electric Snare"],
  f41: ["41", "F2", "Low Floor Tom"],
  f42: ["42", "F#2", "Closed Hi Hat"],
  f43: ["43", "G2", "High Floor Tom"],
  f44: ["44", "G#2", "Pedal Hi-Hat"],
  f45: ["45", "A2", "Low Tom"],
  f46: ["46", "A#2", "Open Hi-Hat"],
  f47: ["47", "B2", "Low-Mid Tom"],
  f48: ["48", "C3", "Hi Mid Tom"],
  f49: ["49", "C#3", "Crash Cymbal 1"],
  f50: ["50", "D3", "High Tom"],
  f51: ["51", "D#3", "Ride Cymbal 1"],
  f52: ["52", "E3", "Chinese Cymbal"],
  f53: ["53", "F3", "Ride Bell"],
  f54: ["54", "F#3", "Tambourine"],
  f55: ["55", "G3", "Splash Cymbal"],
  f56: ["56", "G#3", "Cowbell"],
  f57: ["57", "A3", "Crash Cymbal 2"],
  f58: ["58", "A#3", "Vibraslap"],
  f59: ["59", "B3", "Ride Cymbal 2"],
  f60: ["60", "C4", "Hi Bongo"],
  f61: ["61", "C#4", "Low Bongo"],
  f62: ["62", "D4", "Mute Hi Conga"],
  f63: ["63", "D#4", "Open Hi Conga"],
  f64: ["64", "E4", "Low Conga"],
  f65: ["65", "F4", "High Timbale"],
  f66: ["66", "F#4", "Low Timbale"],
  f67: ["67", "G4", "High Agogo"],
  f68: ["68", "G#4", "Low Agogo"],
  f69: ["69", "A4", "Cabasa"],
  f70: ["70", "A#4", "Maracas"],
  f71: ["71", "B4", "Short Whistle"],
  f72: ["72", "C5", "Long Whistle"],
  f73: ["73", "C#5", "Short Guiro"],
  f74: ["74", "D5", "Long Guiro"],
  f75: ["75", "D#5", "Claves"],
  f76: ["76", "E5", "Hi Wood Block"],
  f77: ["77", "F5", "Low Wood Block"],
  f78: ["78", "F#5", "Mute Cuica"],
  f79: ["79", "G5", "Open Cuica"],
  f80: ["80", "G#5", "Mute Triangle"],
  f81: ["81", "A5", "Open Triangle"],
};
