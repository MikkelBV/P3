const express = require('express');
const expressFileupload = require('express-fileupload');
const bodyParser = require('body-parser');
const fs = require('fs');
const {execFile} = require('child_process');

const PORT = process.env.PORT || 3000;
const app = express();

app.listen(PORT, () => {
    console.log("Server started successfully on port " + PORT);
    console.log("-----------------------------------------");
}); 
// app.use((req, res, next) => {
//     console.log(req.url)
//     next();
// });
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());
app.use(express.static('public'));
app.use(expressFileupload());

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/public/index.html')
})

app.post('/calculatespeed', (req, res) => {
    if (!req.files) {
        console.log("No files in this request");
    } else {
        req.files.file.mv('temp.mp4', (err) => {
            if (err) 
                console.log('error movin file');

            let params = req.body;
            console.log(params);
            
            execFile('P3', ['temp.mp4', params.method, params.reps, params.resize === 'on' ? 1 : 0, params.skips], (err, stdout, stderr) => {
                console.log('Processing done...');

                fs.readFile('output.txt', 'utf8', (err, data) => {
                    let speeds = data.split('/');
                    fs.rename('output.mp4', __dirname + '/public/output.mp4', (err) => {
                        if (err) 
                            console.log(err);
                    })

                    const jsonResponse = {
                        avg: speeds[0],
                        min: speeds[1],
                        max: speeds[2]
                    }

                    console.log(jsonResponse);

                    fs.unlink('temp.mp4', (err, stats) => {
                        if (err)
                            console.log(err);
                    })
                    
                    res.json(jsonResponse);
                })
            })
        });
    }
})