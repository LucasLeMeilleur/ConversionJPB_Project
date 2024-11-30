const fs = require('fs').promises;
const path = require('path');

const commands = {
    help: () => {
        console.log("Commandes disponibles :");
        console.log("  help        - Affiche cette aide");
        console.log("  vider       - Vide le fichier TempImg");
        console.log("  exit        - Quitte l'application");
    },
    vider: () => {
        viderDossier("TempImg")
    },
    exit: () => {
        console.log("Au revoir !");
        process.exit(0);
    }
};

async function viderDossier(dossier) {
    try {
        // Lister le contenu du dossier
        const fichiers = await fs.readdir(dossier);

        // Parcourir les fichiers et les supprimer
        for (const fichier of fichiers) {
            const chemin = path.join(dossier, fichier);
            const stat = await fs.stat(chemin);

            if (stat.isDirectory()) {
                // Si c'est un dossier, le vider récursivement
                await viderDossier(chemin);
                await fs.rmdir(chemin); // Supprimer le dossier vide
            } else {
                // Si c'est un fichier, le supprimer
                await fs.unlink(chemin);
            }
        }

        console.log(`Le dossier "${dossier}" a été vidé.`);
    } catch (err) {
        console.error(`Erreur lors de la suppression du contenu du dossier "${dossier}" :`, err);
    }
}


module.exports = commands;
