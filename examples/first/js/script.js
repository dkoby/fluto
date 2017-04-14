
var FIRST_REFRESH_TIMEOUT   = 500;
var SUCCESS_REFRESH_TIMEOUT = 1000;
var ERROR_REFRESH_TIMEOUT   = 5000;
var REQUEST_TIMEOUT         = 5000;

var errorOutput = document.getElementById("requestError");

function getData(dataControl) {
    var reqListener = function() {
        if (this.readyState == 4) {
            if (this.status == 200)
            {
                var data = this.response;
                this.dataOutput.innerHTML  = data.result;
                this.errorOutput.innerHTML = "";
                getNextData(SUCCESS_REFRESH_TIMEOUT);
            } else {
                this.errorOutput.innerHTML = "Request error";
                getNextData(ERROR_REFRESH_TIMEOUT);
            }
        }
    }
    var request = new XMLHttpRequest();
    request.errorOutput  = errorOutput;
    request.dataOutput   = dataControl;
    request.timeout      = REQUEST_TIMEOUT;
    request.responseType = "json";
    request.ontimeout = function() {
        this.errorOutput.innerHTML = "Request timeout";
        getNextData(ERROR_REFRESH_TIMEOUT);
    }
    request.addEventListener("load", reqListener);
    request.open("GET", "request1");
    request.send();
}
function _getData() {
    getData(document.getElementById("jsCounter"));
}
var timeoutID;
function getNextData(timeout) {
    if (timeoutID)
        window.clearTimeout(timeoutID);
    timeoutID = window.setTimeout(_getData, timeout);
}

getNextData(FIRST_REFRESH_TIMEOUT);

