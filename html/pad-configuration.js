window.onload = function () {
    renderPad(defaultConfigData.row, defaultConfigData.pad, '#container');
    let opt = _sl('#instrument');
    for(let i in instrumentCode)
    {
        opt.appendChild(new Option(instrumentCode[i][0]+' - '+instrumentCode[i][2], instrumentCode[i][0]));
    }
    let btnhide = _sls('.pupup-hide');
    for(let i = 0; i<btnhide.length; i++) //NOSONAR
    {
        btnhide[i].addEventListener('click', function(e){
            e.target.closest('.popup').popupHide();
        });
    }
    let btnPad = _sls('.pad');
    for(let i = 0; i<btnPad.length; i++) //NOSONAR
    {
        btnPad[i].addEventListener('click', function(e){
            testDrum(e.target.attr('data-code'));
        });
    }
    _sl('.test-drum').addEventListener('click', function(e){
        testDrum(e.target.closest('.popup').querySelector('#instrument').value)
    });
    _sl('.update-data').addEventListener('click', function(e){
        let number = _sl('#number').value;
        let isntrument = _sl('#instrument').value;
        let threshold = _sl('#threshold').value;
        let headroom = _sl('#headroom').value;
        let duration = _sl('#duration').value;
        let bt = _sl('#f'+number);
        setData(bt, [isntrument, threshold, headroom, duration]);
        _sl('#popup-pad-setting').popupHide();
    }, false);
    
}
let defaultConfigData = {
    "row": [
        [9, 10, 11, 12],
        [5, 6, 7, 8],
        [1, 2, 3, 4]
    ],
    "pad": {
        "f1": [35, 100, 1000, 10000, 20000],
        "f2": [36, 100, 1000, 10000, 20000],
        "f3": [37, 100, 1000, 10000, 20000],
        "f4": [38, 100, 1000, 10000, 20000],
        "f5": [39, 100, 1000, 10000, 20000],
        "f6": [40, 100, 1000, 10000, 20000],
        "f7": [41, 100, 1000, 10000, 20000],
        "f8": [42, 100, 1000, 10000, 20000],
        "f9": [43, 100, 1000, 10000, 20000],
        "f10": [44, 100, 1000, 10000, 20000],
        "f11": [45, 100, 1000, 10000, 20000],
        "f12": [46, 100, 1000, 10000, 20000],
    }
};
function testDrum(code)
{
    console.log('send note', code)
}
function resetToDefault()
{
    if(confirm('Are you sure you want to reset the config?'))
    {
        for(let i in defaultConfigData.pad)
        {
            if(defaultConfigData.pad.hasOwnProperty(i))
            {
                let bt = _sl('#'+i);
                let data = defaultConfigData.pad[i];
                setData(bt, data);
            }
        }
    }
}
function renderPad(padArr, config, selector) {
    let sel = _sl(selector);
    for (let i = 0; i < padArr.length; i++) //NOSONAR
    {
        let row = padArr[i];
        let rw = document.createElement('div');
        rw.classList.add('row');
        for (let j = 0; j < row.length; j++) //NOSONAR
        {
            let cl = document.createElement('div');
            cl.classList.add('pad-wrapper');
            cl.attr('data-ch', row[j]);
            cl.appendChild(createButton(row[j], config));
            cl.appendChild(editor(row[j], config));
            rw.appendChild(cl);
        }
        sel.appendChild(rw);
    }
}
function createButton(ch, config) {
    let id = 'f' + ch;
    let data = config[id] || [35, 100, 1000, 10000, 20000];

    let bt = document.createElement('button');
    bt.classList.add('btn');
    bt.classList.add('btn-100');
    bt.classList.add('pad');
    bt.attr('data-number', ch);
    bt.attr('type', 'button');
    bt.attr('id', id);
    setData(bt, data);
    return bt;
}

function setData(bt, data) {
    let isnt = data[0];
    bt.innerText = isnt;
    bt.attr('title', instrumentCode['f' + isnt][2]);
    bt.attr('data-code', isnt);
    bt.attr('data-threshold', data[1]);
    bt.attr('data-headroom', data[2]);
    bt.attr('data-duration', data[3]);
}

function editor(id, config) {
    let ed = document.createElement('div');
    ed.classList.add('editor');
    let a = document.createElement('a');
    a.classList.add('ctrl');
    a.attr('href', 'javascript:');
    a.attr('data-id', id);
    a.addEventListener('click', function (e) {
        let data = getPadData(e);
        let pop = _sl('#popup-pad-setting');
        _sl('#number').value = e.target.attr('data-id');
        _sl('#instrument').value = data['data-code'];
        _sl('#threshold').value = data['data-threshold'];
        _sl('#headroom').value = data['data-headroom'];
        _sl('#duration').value = data['data-duration'];
        pop.popupShow();
    }, false);
    ed.appendChild(a);
    return ed;
}
function getPadData(e)
{
    let target = e.target.parentNode.parentNode.querySelector('.pad');
    return {
        'data-code': target.attr('data-code'),
        'data-threshold': target.attr('data-threshold'),
        'data-headroom': target.attr('data-headroom'),
        'data-duration': target.attr('data-duration')
    };
}
function getFormData(container) {
    let cntr = _sl(container);
    let arr = cntr.querySelectorAll('button');
    let bld2 = [];
    for (let i = 0; i < arr.length; i++) //NOSONAR
    {

        let bld = [];
        bld.push(arr[i].attr('id') + '=' + arr[i].attr('data-code'));
        bld.push(arr[i].attr('data-threshold'));
        bld.push(arr[i].attr('data-headroom'));
        bld.push(arr[i].attr('data-duration'));
        bld2.push(bld.join(','));
    }
    return bld2.join('&');
}

function savePadConfig() {
    if(confirm('Are you sure you want to save the config?'))
    {
        let fomData = getFormData('#container');
        console.log(fomData);
    }
}

let instrumentCode = {
    "f35": ["35", "B1", "Acoustic Bass Drum"],
    "f36": ["36", "C2", "Bass Drum 1"],
    "f37": ["37", "C#2", "Side Stick"],
    "f38": ["38", "D2", "Acoustic Snare"],
    "f39": ["39", "D#2", "Hand Clap"],
    "f40": ["40", "E2", "Electric Snare"],
    "f41": ["41", "F2", "Low Floor Tom"],
    "f42": ["42", "F#2", "Closed Hi Hat"],
    "f43": ["43", "G2", "High Floor Tom"],
    "f44": ["44", "G#2", "Pedal Hi-Hat"],
    "f45": ["45", "A2", "Low Tom"],
    "f46": ["46", "A#2", "Open Hi-Hat"],
    "f47": ["47", "B2", "Low-Mid Tom"],
    "f48": ["48", "C3", "Hi Mid Tom"],
    "f49": ["49", "C#3", "Crash Cymbal 1"],
    "f50": ["50", "D3", "High Tom"],
    "f51": ["51", "D#3", "Ride Cymbal 1"],
    "f52": ["52", "E3", "Chinese Cymbal"],
    "f53": ["53", "F3", "Ride Bell"],
    "f54": ["54", "F#3", "Tambourine"],
    "f55": ["55", "G3", "Splash Cymbal"],
    "f56": ["56", "G#3", "Cowbell"],
    "f57": ["57", "A3", "Crash Cymbal 2"],
    "f58": ["58", "A#3", "Vibraslap"],
    "f59": ["59", "B3", "Ride Cymbal 2"],
    "f60": ["60", "C4", "Hi Bongo"],
    "f61": ["61", "C#4", "Low Bongo"],
    "f62": ["62", "D4", "Mute Hi Conga"],
    "f63": ["63", "D#4", "Open Hi Conga"],
    "f64": ["64", "E4", "Low Conga"],
    "f65": ["65", "F4", "High Timbale"],
    "f66": ["66", "F#4", "Low Timbale"],
    "f67": ["67", "G4", "High Agogo"],
    "f68": ["68", "G#4", "Low Agogo"],
    "f69": ["69", "A4", "Cabasa"],
    "f70": ["70", "A#4", "Maracas"],
    "f71": ["71", "B4", "Short Whistle"],
    "f72": ["72", "C5", "Long Whistle"],
    "f73": ["73", "C#5", "Short Guiro"],
    "f74": ["74", "D5", "Long Guiro"],
    "f75": ["75", "D#5", "Claves"],
    "f76": ["76", "E5", "Hi Wood Block"],
    "f77": ["77", "F5", "Low Wood Block"],
    "f78": ["78", "F#5", "Mute Cuica"],
    "f79": ["79", "G5", "Open Cuica"],
    "f80": ["80", "G#5", "Mute Triangle"],
    "f81": ["81", "A5", "Open Triangle"]
}