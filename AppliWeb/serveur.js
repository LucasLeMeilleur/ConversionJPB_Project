const express = require('express');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const port = 3000;

// Middleware pour parser le corps de la requête en JSON
app.use(bodyParser.json());

// Route pour servir le fichier HTML à la racine
app.get('/',(req, res) => {
    res.sendFile(path.join(__dirname, 'index.html'));
});

// Route pour exécuter le code C
app.post('/execute', (req, res) => {
    const code = req.body.code;

    // Créer un fichier temporaire pour le code C
    const filename = 'temp.c';
    fs.writeFileSync(filename, code);

    // Compiler le code C avec gcc
    exec(`gcc ${filename} -o temp.out`, (compileErr, stdout, stderr) => {
        if (compileErr) {
            return res.status(400).json({ error: `Compilation failed: ${stderr}` });
        }

        // Exécuter le fichier compilé
        exec('./temp.out', (execErr, execStdout, execStderr) => {
            if (execErr) {
                return res.status(400).json({ error: `Execution failed: ${execStderr}` });
            }

            // Renvoi de la sortie de l'exécution
            res.json({ output: execStdout });
        });
    });
});

// Serveur d'écoute
app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});