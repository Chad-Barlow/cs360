var express = require('express');
var fs = require('fs');
var request = require('request');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.sendFile('weather.html', { root: 'public' });
});

router.get('/getcity',function(req,res,next) { 
  console.log("In getcity route"); 
  fs.readFile(__dirname + '/cities.dat.txt',function(err,data) { 
    if(err) throw err; 
    var cities = data.toString().split("\n");
    var myRe = new RegExp("^" + req.query.q); 
    var jsonresult = [];
    for(var i = 0; i < cities.length; i++) {  
      var result = cities[i].search(myRe);
      if(result != -1) {
	console.log(cities[i]);
	jsonresult.push({city:cities[i]});
      }
    }
    console.log(jsonresult);
    res.status(200).json(jsonresult); 
  });
});

router.get('/getpoke',function(req,res,next) {
  console.log("In getpoke route");
 // var r = request.defaults({'proxy':'http://localproxy.com'})
  var url = 'http://pokeapi.co/api/v2/pokemon/' + req.query.q + '.json';
  console.log(url);
  request.get(url).pipe(res);
});


module.exports = router;
