#!/bin/node
var file_in = "quick-tests.json";
var lineReader = require('readline').createInterface({
    input: require('fs').createReadStream(file_in)
});
var print_test_cases_oses = function(){
  console.log("test_cases:");
  lineReader.on('line', function (line) {
      var o = JSON.parse(line);
      //console.log(o);
      if(o.hasOwnProperty('ua')) {
        console.log("  - user_agent_string: " + "'"+o.string+"'");
        console.log("    family: "      + ( o.ua.hasOwnProperty('family') ? "'" + o.ua.family + "'" : '' ) );
        console.log("    major: "       + ( o.ua.hasOwnProperty('major') ? "'" + o.ua.major+ "'" : '' ) );
        console.log("    minor: "       + ( o.ua.hasOwnProperty('minor') ? "'" + o.ua.minor+ "'" : '' ) );
        console.log("    patch: "       + ( o.ua.hasOwnProperty('patch') ? "'" + o.ua.patch+ "'" : '' ) );
        console.log("    patch_minor: " + ( o.ua.hasOwnProperty('patch_minor') ? "'" + o.ua.patch_minor+ "'": ''));
      }
  });
}
var print_test_cases_devices = function(){
  console.log("test_cases:");
  lineReader.on('line', function (line) {
      var o = JSON.parse(line);
      //console.log(o);
      if(o.hasOwnProperty('device')) {
        console.log("  - user_agent_string: " + "'"+o.string+"'");
        console.log("    family: "      + ( o.device.hasOwnProperty('family') ? "'" + o.device.family + "'" : '' ) );
        console.log("    brand: "       + ( o.device.hasOwnProperty('brand') ? "'" + o.device.brand+ "'" : '' ) );
        console.log("    model: " + ( o.device.hasOwnProperty('model') ? "'" + o.device.model+ "'": ''));
      }
  });
}
/*
test_os.yaml
test_cases:
  - user_agent_string: 'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_3; en-us; Silk/1.1.0-80) AppleWebKit/533.16 (KHTML, like Gecko) Version/5.0 Safari/533.16 Silk-Accelerated=true'
      family: 'Android'
      major:
      minor:
      patch:
      patch_minor:

test_cases:
  - user_agent_string: 'ALCATEL-OT510A/382 ObigoInternetBrowser/Q05A'
    family: 'Alcatel OT510A'
    brand: 'Alcatel'
    model: 'OT510A'

*/
//print_test_cases_oses();
print_test_cases_devices();
