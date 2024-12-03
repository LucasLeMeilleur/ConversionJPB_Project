const express = require('express');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const multer = require('multer');
const session = require('express-session');
const commands = require("./commands.js");
const readline = require("readline");


const uploadDir = path.join(__dirname, 'TempImg'); 
const exePath = './Convert';
const app = express();
const port = 3000;
let clientsConnectes = 0;



const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    prompt: "> "
});

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use(session({
    secret: 'votre_clé_secrète',  
    resave: false,
    saveUninitialized: true,
    cookie: { secure: false } 
}));


app.use((req, res, next) => {    
    if (!req.session.clientId) {        
        req.session.clientId = Date.now(); 
        clientsConnectes++; 
        console.log(`Nouveau client connecté. Nombre de clients: ${clientsConnectes}`);
    }
    console.log(`Client ${req.session.clientId} est connecté. Nombre de clients: ${clientsConnectes}`);
    next();
});


app.use(express.static(path.join(__dirname, 'public')));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});


const storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, uploadDir);
    },
    filename: function (req, file, cb){        
        cb(null, "Input-"+req.session.clientId + path.extname(file.originalname));
    }
});

function supprimerImagesParNom(dossier, prefixe) {
    fs.readdir(dossier, (err, fichiers) => {
        if (err) {
            return console.error(`Erreur lors de la lecture du dossier : ${err.message}`);
        }
        fichiers.forEach((fichier) => {
            if (fichier.startsWith(prefixe)) {
                const cheminFichier = path.join(dossier, fichier);
                fs.unlink(cheminFichier, (err) => {
                    if (err) {
                        console.error(`Erreur lors de la suppression de ${cheminFichier} : ${err.message}`);
                    } else {
                        console.log(`Fichier supprimé : ${cheminFichier}`);
                    }
                });
            }
        });
    });
}


const upload = multer({ storage: storage });


app.post('/upload', upload.single('image'), (req, res) => {
    supprimerImagesParNom("TempImg", `Output-${req.session.clientId}`)
    const format = req.body.format; 
    const fileExtension = path.extname(req.file.originalname).substring(1);
    if (!req.file) {
        return res.status(400).json({ error: 'Aucune image téléchargée' });
    }
    const filePath = path.join(__dirname, 'TempImg', req.file.filename);
    console.log(fileExtension);
    console.log(format);
    const argsExe = [fileExtension, format];
    var args = [fileExtension, format];

    console.log(`${exePath} ${argsExe.join(' ')}`);

    exec(`${exePath} ${argsExe.join(' ')} ${req.session.clientId}`, args, (error, stdout, stderr) => {
        if(error){
            console.error(`Erreur d'exécution : ${error.message}`);
            return res.status(400).json({erreur: error.message });
        }
        if(stderr){
            console.error(`Erreur (stderr) : ${stderr}`);
            return res.status(400).json({ erreur: stderr });
        }
        console.log(`Sortie : ${stdout}`);
        return res.status(200).json({message : "Fichier converti !"});
    });    
});

app.get('/get', (req, res) => {
    var fileName, extensionTemp;
    var trouver = false;
    const extension = ["png", "bmp", "jpg"];
    for (let i = 0; i < 3; i++) {
        fileName = `Output-${req.session.clientId}.${extension[i]}`
        if (fs.existsSync(path.join(__dirname, 'TempImg', fileName))) {
            trouver = true;
            extensionTemp = extension[i];
            break;
        } 
    }
    if(!trouver){
        return res.status(400).json({message: "Image non trouvé"});
    } 
    const filePath = path.join(__dirname, 'TempImg', fileName);
    res.download(filePath, `Output.${extensionTemp}` ,  (err) => {
        if (err) {
            return res.status(400).json({message: 'Erreur lors du téléchargement.'});
        }
    });
});


app.listen(port, '0.0.0.0', () => {
    console.log(`Serveur en cours d'exécution sur http://0.0.0.0:${port}`);
});

console.log("Bienvenue dans la conso    le Node.js !");
console.log("Tapez 'help' pour voir les commandes disponibles.");
rl.prompt();

rl.on("line", (line) => {
    const args = line.trim().split(" "); // Divise la commande et ses arguments
    const command = args.shift();       // Récupère la commande
    const handler = commands[command];  // Trouve la commande dans commands.js

    if (handler) {
        handler(...args); // Exécute la commande avec les arguments
    } else {
        console.log(`Commande inconnue : '${command}'. Tapez 'help' pour voir les commandes disponibles.`);
    }

    rl.prompt(); // Affiche de nouveau le prompt
}).on("close", () => {
    console.log("Console terminée.");
    process.exit(0);
});