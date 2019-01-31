var fs = require('fs'); 
var express = require('express'); 
var multer = require('multer'); 
var app = express(); 
var config = require(process.cwd()  + '/config.json')
//var upload = multer({ dest: 'upload/' }); 

//定义public路径
var pub = process.cwd() + '/' + config.fileSavePath;

app.use('/downloads',express.static(pub));//设置静态目录为pubic


var storage = multer.diskStorage({
  //设置上传后文件路径，"d:/myapp/public/uploads文件夹"会自动创建。
  destination: function (req, file, cb) {
    cb(null, pub)
  }, 

  //给上传文件重命名，获取添加后缀名
  filename: function (req, file, cb) {
    var fileFormat = (file.originalname).split(".");
    cb(null, file.fieldname + '-' + Date.now() + "." + fileFormat[fileFormat.length - 1]);
  }
});

var upload = multer({ storage: storage })  
var downUrl = config.downServer + 'downloads/'

// 单图上传 
app.post('/upload', upload.single('logo'), function(req, res, next){ 
	var file=req.file;
	console.log("original file name is "+file.originalname);//original file name is 20170615_211619.jpg
	console.log("file name is " + file.filename);//file name is myfile-1511013577361.jpg
	res.send({ret_code: '0'}); 
}); 

// 单图上传 
app.post('/dfs_upload/NubePhotoUpload', upload.single('file'), function(req, res, next){ 
	var file=req.file;
	console.log("original file name is "+file.originalname);//original file name is 20170615_211619.jpg
	console.log("file name is " + file.filename);//file name is myfile-1511013577361.jpg
	var fileurl = downUrl + file.filename;
	res.send({ok: '1', bigImagePath:fileurl, originalImagePath:fileurl, littleImagePath:fileurl}); 
}); 

app.get('/', function(req, res){  
  res.sendFile('test.html', { root: process.cwd() });  
});  

//app.get('public/uploads',function(req, res){  
//  res.sendFile(req.filename, { root: pub });  
//});  

app.get('/form', function(req, res, next){ 
	var form = fs.readFileSync('./form.html', {encoding: 'utf8'}); 
	res.send(form); 
}); 

app.listen(config.serverPort); 
