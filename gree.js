function sendGree()
{
    var fanSpeed = parseInt($("#fanspeed").val(), 16);
    var operatingMode = parseInt($("#operatingmode").val(), 16);
    var powerMode = parseInt($("input[name=powermode]:checked").val(), 16);
    var sleepMode = parseInt($("input[name=sleepmode]:checked").val(), 16);
    var swingMode = parseInt($("input[name=swingmode]:checked").val(), 16);
    var temperature = parseInt($("#temperature").val());

    var data = [0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00];
    data[0] = fanSpeed | operatingMode | powerMode | sleepMode | swingMode;
    data[1] = temperature;
    data[7] = (
        (data[0] & 0x0F) +
        (data[1] & 0x0F) +
        (data[2] & 0x0F) +
        (data[3] & 0x0F) +
        (data[5] & 0xF0) >> 4 +
        (data[6] & 0xF0) >> 4 +
        (data[7] & 0xF0) >> 4 +
        0x0A) & 0xF0;

    var bits = 64;

    for (var i = 0; i < data.length; i++) {
        data[i] = data[i].toString(16);
        if (data[i].length === 1)
            data[i] = "0" + data[i];
    }

    var url = "http://.../ir";

    $.post(url, {
        code: data.join(""),
        protocol: "Gree",
        bits: bits
    })
        .done(function(res) {
            alert("Success: " + res);
        })
        .fail(function(jqXHR, textStatus, errorThrown) {
            alert("Fail: " + (errorThrown || textStatus));
        });
}
