var express = require("express");
var app = express();
app.use(express.logger());
var redis = require('redis-url').connect(process.env.REDISTOGO_URL);

app.use(function(req, res, next) {
    var data = '';
    req.setEncoding('utf8');
    req.on('data', function(chunk) { 
        data += chunk;
    });
    req.on('end', function() {
        req.rawBody = data;
        next();
    });
});

app.get('/', function(request, response) {
    response.send('Welcome to Thermoculus!');
});

app.get('/:key', function(request, response) {
    redis.lpop(request.params.key, function (error, item) {
	console.log("lpop starting");
	var body = "";
	if (error) {
	    console.log("Error!");
	    console.log(error);
	    //throw error;
	    body = error;
	} else {
	    console.log("no error");
	    if (item) body = item;
	}
	console.log("Body: " + body);
        response.setHeader('Content-Type', 'text/plain');
        response.setHeader('Content-Length', body.length);
        response.end(body);
    });

});

app.post('/:key', function(request, response) {
    redis.rpush(request.params.key,request.rawBody);
    response.send("Pushed " + request.rawBody + " for " + request.params.key);
});

var port = process.env.PORT || 5000;
app.listen(port, function() {
    console.log("Listening on " + port);
});
