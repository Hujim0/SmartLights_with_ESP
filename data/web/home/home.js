var gateway = "ws:" + window.location.host + "/ws";
var websocket = new WebSocket(gateway);

window.addEventListener('load', BeginWebSocket);

function BeginWebSocket() {
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;

    websocket.onerror = function (error) {
        //alert("Ws Error " + JSON.stringify(error));
    };
}

console.log('Trying to open a WebSocket connection...');

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    current_mode = null;
    console.log("Lost connection!");
    lostConnection = true;
    location.reload();
}

var current_mode = null;

var sentStreamEvent = false;
var lostConnection = true;
var gotPreferences = false;

const ON_GET_ELEMENTS_EVENT = "onElementsMode"
const ON_GET_PREFERENCES_EVENT = "onGetPreferences"

function onMessage(event) {
    if (!gotPreferences) {
        lostConnection = false;

        window.dispatchEvent(new CustomEvent(ON_GET_PREFERENCES_EVENT, {
            detail: JSON.parse(event.data)
        }));
        gotPreferences = true;

        var epoch_time = new Date();

        var evnt = new espEvent("epoch_time",
            (epoch_time.getHours() * 3600) +
            (epoch_time.getMinutes() * 60) +
            epoch_time.getSeconds());

        evnt.dayOfTheWeek = epoch_time.getDay();

        SendJson(evnt);

        console.log("[WS] - got preferences!");
        return;
    }

    obj = JSON.parse(event.data);

    if (obj.elems == null) {
        current_mode = obj;
        console.log("[WS] - got args");
        return;
    }

    console.log("[WS] - got elements");

    window.dispatchEvent(new CustomEvent(ON_GET_ELEMENTS_EVENT, {
        detail: obj
    }));
}

function EndStream() {
    if (lostConnection) return;
    websocket.send("]");
    console.log("closing stream");
    sentStreamEvent = false;
}

function SendJson(object) {
    if (lostConnection) return;
    websocket.send(JSON.stringify(object));
}

class espEvent {
    event;
    value;

    constructor(event, value) {
        this.event = event;
        this.value = value;
    }
}

const OPEN_STREAM = "open_stream";

const LIGHT_SWITCH = "light_switch";
const BRIGHTNESS = "brightness";
const MODE_SWITCH = "mode_switch";
const REQUEST_ARGS = "mode_args_request";


//---------------------------------------------------------------------------

window.addEventListener(ON_GET_PREFERENCES_EVENT, (event) => {
    var checkbox = document.getElementById("light_switch_checkbox");

    checkbox.checked = event.detail[LIGHT_SWITCH];

    checkbox.addEventListener("click", (checkbox_event) => {
        SendJson(new espEvent(LIGHT_SWITCH, checkbox_event.target.checked))
    });
});

//---------------------------------------------------------------------------

var modes = document.getElementById("mode_list").children;
for (var i = 0; i < modes.length; i++) {
    modes[i].addEventListener("input", (event) => {
        SendModeSwitchEvent(event.target.getAttribute("mode-id"));
    });
}


window.addEventListener(ON_GET_PREFERENCES_EVENT, (event) => {
    var mode_id = event.detail.mode;

    modes[mode_id].children[0].checked = true;
    SendRequestArgsEvent(mode_id);
});

//---------------------------------------------------------------------------

window.addEventListener(ON_GET_PREFERENCES_EVENT, (event) => {
    var brigthness_slider = document.getElementById("brightness_slider");

    brigthness_slider.value = event.detail.brightness;

    brigthness_slider.addEventListener("change", EndStream);
    brigthness_slider.addEventListener("change", (brigthness_slider_event) => {
        SendJson(new espEvent(BRIGHTNESS, brigthness_slider_event.target.value))
    });
    brigthness_slider.addEventListener("input", handleBrigthnessInput);
});

function handleBrigthnessInput() {
    if (!sentStreamEvent) {
        console.log("starting slider event");
        SendJson(new espEvent(OPEN_STREAM, BRIGHTNESS));
        sentStreamEvent = true;
    }

    console.log(event.target.value);
    websocket.send(event.target.value);
}



//---------------------------------------------------------------------------

var list_container = document.getElementById("list-containter");
var list_temlate = document.getElementById("list-template");
var skeleton_template = document.getElementById("skeleton-template");
//send update func
function SendModeSwitchEvent(id) {
    window.addEventListener(ON_GET_ELEMENTS_EVENT, OnGetArgs);
    CreateSkeleton();

    SendJson(new espEvent(MODE_SWITCH, id));
}

function SendRequestArgsEvent(id) {

    window.addEventListener(ON_GET_ELEMENTS_EVENT, OnGetArgs);
    CreateSkeleton();

    SendJson(new espEvent(REQUEST_ARGS, id));
}

function OnGetArgs() {
    CreateModeElements(event.detail.elems);

    window.removeEventListener(ON_GET_ELEMENTS_EVENT, OnGetArgs);
}

function SendCurrentArgs() {
    console.log(JSON.stringify(current_mode));
    SendJson(current_mode);
}

function CreateSkeleton() {
    list_container.innerHTML = "";
    for (let i = 0; i < 2; i++) {
        list_container.appendChild(skeleton_template.content.cloneNode(true));
    }
}

var current_info_inputs = [];

function CreateModeElements(_elements) {
    list_container.innerHTML = "";

    current_info_inputs = [];

    for (var i = 0; i < _elements.length; i++) {
        var child_element = list_temlate.content.cloneNode(true).children[0];

        child_element.children[0].innerText = _elements[i]["name"];

        child_element.children[1] = SetUpInputChild(
            child_element.children[1],
            _elements[i]);

        if (_elements[i].desc == null) {
            child_element.children[2].hidden = true;
            list_container.appendChild(child_element);
            return;
        }
        child_element.children[2].children[0]["arg_id"] = i;
        child_element.children[2].children[0].addEventListener("input", (event) => {
            // window.dispatchEvent(new CustomEvent("ARG_INFO_ID_UPDATE", {
            //     detail: {
            //         state: event.target.checked,
            //         arg_info_id: event.target["arg_id"]
            //     }
            // }));

            current_info_inputs[event.target["arg_id"]].classList.toggle("active");
        });

        current_info_inputs.push(child_element.children[3]);

        child_element.children[3].innerText = _elements[i].desc;
        list_container.appendChild(child_element);
    }
}

function SetUpInputChild(input_element, _element) {
    for (var x = 0; x < _element.input_props.length; x++) {

        var attribute = _element.input_props[x].split("=");

        input_element[attribute[0]] = attribute[1];
    }

    input_element.mode_arg_name = _element.arg_name;

    var input_type_getter = "value";
    if (input_element.type == "checkbox") input_type_getter = "checked";

    input_element.input_type_getter = input_type_getter;

    input_element[input_type_getter] = current_mode[_element.arg_name];

    if (input_element.type == "color" || input_element.type == "range") {
        input_element.addEventListener("input", (event) => {
            if (!sentStreamEvent) {
                console.log("starting stream event");

                SendJson(new espEvent(OPEN_STREAM, event.target.mode_arg_name));

                sentStreamEvent = true;
            }

            console.log(event.target.value);
            websocket.send(event.target.value);
        });

        input_element.addEventListener("change", EndStream);
    }


    input_element.addEventListener("change", (event) => {
        current_mode[event.target.mode_arg_name] = event.target[event.target.input_type_getter];

        SendCurrentArgs();
    });


    return input_element;
}