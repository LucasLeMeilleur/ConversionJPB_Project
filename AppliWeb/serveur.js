const express = require('express');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const port = 3000;

// Middleware pour parser les requêtes JSON
app.use(bodyParser.json());

// Route pour servir le fichier HTML
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Route pour exécuter le code C
app.post('/execute', (req, res) => {
    const code = req.body.code;

    // Création de noms de fichiers temporaires uniques
    const timestamp = Date.now();
    const filename = `temp_${timestamp}.c`;
    const outputFilename = `temp_${timestamp}.out`;

    try {
        // Écrire le code C dans un fichier temporaire
        fs.writeFileSync(filename, code);

        // Compiler le fichier C
        exec(`gcc ${filename} -o ${outputFilename}`, (compileErr, stdout, stderr) => {
            if (compileErr) {
                // Supprimer le fichier temporaire s'il existe
                fs.unlinkSync(filename);
                return res.status(400).json({ error: `Compilation failed: ${stderr}` });
            }

            // Exécuter le programme compilé
            exec(`./${outputFilename}`, (execErr, execStdout, execStderr) => {
                // Nettoyer les fichiers temporaires
                if (fs.existsSync(filename)) fs.unlinkSync(filename);
                if (fs.existsSync(outputFilename)) fs.unlinkSync(outputFilename);

                if (execErr) {
                    return res.status(400).json({ error: `Execution failed: ${execStderr}` });
                }

                // Envoyer le résultat au client
                res.json({ output: execStdout });
            });
        });
    } catch (err) {
        res.status(500).json({ error: 'Une erreur interne est survenue.' });
    }
});

// Lancer le serveur
app.listen(port, '0.0.0.0', () => {
    console.log(`Serveur en cours d'exécution sur http://0.0.0.0:${port}`);
});
