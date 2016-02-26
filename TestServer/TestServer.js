/**
 * Sample web server based on 
 * https://gist.github.com/ryanflorence/701407
 * and http://blog.kevinchisholm.com/javascript/node-js/making-a-simple-http-server-with-node-js-part-iv/
 *
 * Run in Node and connect to <IP ADDRESS>:8888 with a web browser.
 */

var http = require("http"),
    url = require("url"),
    path = require("path"),
    fs = require("fs")
    port = process.argv[2] || 8888;
    
// Location of page
var pagePath = "/../SD_Vortex/";

// Suppored files
var extensions = {
    ".html" : "text/html",
	".css" : "text/css",
	".js" : "application/javascript",
	".png" : "image/png",
	".gif" : "image/gif",
	".jpg" : "image/jpeg"
};

//helper function handles file verification
function getFile(filePath,res,mimeType){
    
    console.log("Requested: " + filePath);
    
	//does the requested file exist?
	fs.exists(filePath,function(exists){
		//if it does...
		if(exists){
			//read the fiule, run the anonymous function
			fs.readFile(filePath,function(err,contents){
				if(!err){
					//if there was no error
					//send the contents with the default 200/ok header
					res.writeHead(200,{
						"Content-type" : mimeType,
						"Content-Length" : contents.length
					});
					res.end(contents);
				} else {
					//for our own troubleshooting
					console.dir(err);
				};
			});
		} else {

            //send the contents with a 404/not found header 
            res.writeHead(404, {'Content-Type': 'text/html'});
            res.end('404: no page found');
		};
	});
};

http.createServer(function(req, res) {
    
    // Construct file location from request
    var fileName = req.url;
    if (fileName === path.dirname(fileName)) {
       fileName += 'index.html';
    }

    var ext = path.extname(fileName);
	var localFolder = __dirname + pagePath;
 
	// If file isn't supported, send 404
	if(!extensions[ext]){
		res.writeHead(404, {'Content-Type': 'text/html'});
		res.end("&lt;html&gt;&lt;head&gt;&lt;/head&gt;&lt;body&gt;The requested file type is not supported&lt;/body&gt;&lt;/html&gt;");
	};
 
	getFile((localFolder + fileName),res,extensions);
}).listen(parseInt(port, 10));

console.log("Server running at\n  => http://localhost:" + port + 
"/\nCTRL + C to shutdown");