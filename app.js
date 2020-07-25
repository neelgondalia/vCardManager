'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app = express();
const path = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/public/index.html'));
});

// Send Style, do not change
app.get('/style.css', function (req, res) {
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname + '/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js', function (req, res) {
  fs.readFile(path.join(__dirname + '/public/index.js'), 'utf8', function (err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {
      compact: true,
      controlFlowFlattening: true
    });
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function (req, res) {
  if (!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function (err) {
    if (err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function (req, res) {
  fs.stat('uploads/' + req.params.name, function (err, stat) {
    console.log(err);
    if (err == null) {
      console.log('app.get(): req.param.name = ' + req.param.name);
      res.sendFile(path.join(__dirname + '/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let vCardParserLib = ffi.Library('./vCardParser.so', {
  "createFileLogJSONString": ["string", ["string"]],
  "createCardViewJSONString": ["string", ["string"]]
});


app.get('/assets/:images', function (req, res) {
  fs.stat('public/assets/' + req.params.images, function (err, stat) {
    console.log(err);
    if (err == null) {
      res.sendFile(path.join(__dirname + '/public/assets/' + req.params.images));
    } else {
      res.send('');
    }
  });
});

app.get('/fileLogs', function (req, res) {

  let listOfFileNames = getFileNamesForFileLog();
  let fileLog = [];

  for (let i = 0; i < listOfFileNames.length; i++) {
    let tempFileLogObj = [];
    let fileDir = "./uploads/" + listOfFileNames[i];
    let JSONString = vCardParserLib.createFileLogJSONString(fileDir);

    tempFileLogObj = JSON.parse(JSONString);
    fileLog.push(tempFileLogObj);
  }
  res.send(fileLog);
});

app.get('/getcard', function (req, res) {
  let filename = req.query.file;
  let fileDir = "./uploads/" + filename;
  let JSONstring = vCardParserLib.createCardViewJSONString(fileDir);
  let JSONobj = JSON.parse(JSONstring);
  res.send(JSONobj);
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

///////////////////////// MY FUNCTIONS ///////////////////////////////////////////////

function getFileNamesForFileLog() {
  let dir = './uploads';
  let filenamesArr = [];
  filenamesArr = fs.readdirSync(dir);
  return filenamesArr;
}